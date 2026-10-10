#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "tr2/application/modbus_system_server.h"
#include "tr2/modbus/b4_configuration_codec.h"
#include "tr2/persistence/campaign_data_store_persistent_composition.h"
#include "tr2/platform_host/host_platform.h"

typedef struct {
    SerialTransportEvent events[MODBUS_RTU_ADU_MAX_SIZE + 4u];
    size_t count;
    size_t index;
    uint8_t response[MODBUS_RTU_ADU_MAX_SIZE];
    size_t response_length;
    unsigned tx_calls;
    unsigned start_calls;
    Tr2Result tx_result;
    Tr2Result start_result;
    bool poll_failure;
} TestSerial;

static Tr2Result start(void *context)
{
    TestSerial *serial = context;
    serial->start_calls++;
    return serial->start_result;
}

static Tr2Result transmit(void *context, const uint8_t *data, size_t length)
{
    TestSerial *serial = context;
    serial->tx_calls++;
    if (serial->tx_result == TR2_OK) {
        assert(length <= sizeof(serial->response));
        memcpy(serial->response, data, length);
        serial->response_length = length;
    }
    return serial->tx_result;
}

static bool poll(void *context, SerialTransportEvent *event)
{
    TestSerial *serial = context;
    if (serial->poll_failure) return false;
    *event = (SerialTransportEvent){SERIAL_TRANSPORT_EVENT_NONE, 0u,
                                   SERIAL_TRANSPORT_ERROR_UNSPECIFIED};
    if (serial->index < serial->count) *event = serial->events[serial->index++];
    return true;
}

static void load(TestSerial *serial, uint8_t address, const uint8_t *pdu,
                 size_t length, bool corrupt)
{
    uint8_t adu[MODBUS_RTU_ADU_MAX_SIZE];
    size_t adu_length;
    assert(modbus_rtu_adu_encode(address, pdu, length, adu, sizeof(adu),
                                 &adu_length) == MODBUS_RTU_CODEC_OK);
    if (corrupt) adu[adu_length - 1u] ^= 1u;
    serial->count = serial->index = 0u;
    for (size_t i = 0u; i < adu_length; i++) {
        serial->events[serial->count++] = (SerialTransportEvent){
            SERIAL_TRANSPORT_EVENT_BYTE, adu[i], SERIAL_TRANSPORT_ERROR_UNSPECIFIED};
    }
    serial->events[serial->count++] = (SerialTransportEvent){
        SERIAL_TRANSPORT_EVENT_SILENCE_T1_5, 0u, SERIAL_TRANSPORT_ERROR_UNSPECIFIED};
    serial->events[serial->count++] = (SerialTransportEvent){
        SERIAL_TRANSPORT_EVENT_SILENCE_T3_5, 0u, SERIAL_TRANSPORT_ERROR_UNSPECIFIED};
}

static Tr2Result run(ModbusSystemServer *server, TestSerial *serial)
{
    Tr2Result result = TR2_OK;
    while (serial->index < serial->count) {
        result = modbus_system_server_poll_once(server);
        if (result != TR2_OK) return result;
    }
    return result;
}

static ModbusRtuAduView response(TestSerial *serial)
{
    ModbusRtuAduView view;
    assert(modbus_rtu_adu_decode(serial->response, serial->response_length, &view)
           == MODBUS_RTU_CODEC_OK);
    assert(view.unit_id == 17u);
    return view;
}

static void exception(TestSerial *serial, uint8_t function, uint8_t code)
{
    ModbusRtuAduView view = response(serial);
    assert(view.pdu_length == 2u && view.pdu[0] == (uint8_t)(function | 0x80u));
    if (view.pdu[1] != code) {
        fprintf(stderr, "FC %u: exception %u, expected %u\n", function, view.pdu[1], code);
    }
    assert(view.pdu[1] == code);
}

/* Activation deliberately unavailable in this host fixture. */
static Tr2Result test_commit(void *context, const ValidatedConfiguration *validated,
                             ActiveConfigurationSnapshot *snapshot)
{
    (void)context;
    (void)validated;
    (void)snapshot;
    return TR2_ERROR_NOT_AVAILABLE;
}

typedef struct {
    SystemRuntime *runtime;
    unsigned calls;
    CommandMailboxSubmitResult last_submit;
    CommandRequest last_request;
} TestCommandHandler;

static Tr2Result submit(void *context, CommandMailboxSubmitResult kind,
                        const CommandRequest *request)
{
    TestCommandHandler *handler = context;
    CommandTerminalTimestamp timestamp = {false, 0u};
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    handler->calls++;
    handler->last_submit = kind;
    handler->last_request = *request;
    if (kind != COMMAND_MAILBOX_SUBMISSION_CAPTURED) return TR2_ERROR_NOT_AVAILABLE;
    /* Reuse the production command path; no mocked business execution. */
    return system_runtime_execute_p9_command(handler->runtime, request, &timestamp,
                                              &admission, &entry);
}

int main(void)
{
    static HostPlatform platform;
    static SystemRuntime runtime;
    static CampaignDataStorePersistentComposition historical;
    ModbusSystemServer server = {0};
    TestSerial serial = {0};
    SerialTransport transport = {&serial, start, transmit, poll};
    ConfigurationValidationEnvironment environment = {true, 4096u};
    SystemRuntimeDependencies deps = {0};
    ConfigurationStagingService staging;
    ConfigurationWorkflow workflow;
    TestCommandHandler handler = {0};
    ModbusSystemServerBinding binding = {0};
    ModbusBlock0Image identity = {{0u}, 0u};
    const uint8_t read_b1[] = {3u, 0x03u, 0xE8u, 0u, 1u};
    const uint8_t read_b0[] = {3u, 0u, 0u, 0u, 2u};
    const uint8_t unsupported[] = {6u, 0u, 0u, 0u, 1u};
    const uint8_t bad_address[] = {3u, 0x03u, 0xE7u, 0u, 1u};
    const uint8_t write_b4[] = {16u, 0x0Fu, 0xA2u, 0u, 2u, 4u, 0x12u, 0x34u, 0x56u, 0x78u};
    const uint8_t write_b2[] = {16u, 0x07u, 0xD8u, 0u, 2u, 4u, 0x12u, 0x34u, 0x56u, 0x78u};
    const uint8_t read_b6[] = {3u, 0x17u, 0x70u, 0u, 5u};
    const uint8_t write_b6[] = {16u, 0x17u, 0x73u, 0u, 1u, 2u, 0u, 0u};
    const uint8_t read_only_write[] = {16u, 0x03u, 0xE8u, 0u, 1u, 2u, 0u, 1u};
    const uint8_t read_b4[] = {3u, 0x0Fu, 0xA2u, 0u, 2u};
    uint8_t command[] = {16u, 0x13u, 0x88u, 0u, 8u, 16u,
                         0u, 7u, 0u, 1u, 0u, 0u, 0u, 0u,
                         0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u};
    unsigned before;
    ModbusRtuAduView view;
    PreparedConfiguration prepared;
    bool prepared_time_available;
    Tr2CivilTimestamp prepared_time;

    host_platform_init(&platform);
    MonotonicClock monotonic = host_platform_monotonic_clock(&platform);
    WallClock wall = host_platform_wall_clock(&platform);
    ResetCauseProvider reset = host_platform_reset_cause_provider(&platform);
    TimeContinuityEvidenceProvider continuity = host_platform_time_continuity_evidence_provider(&platform);
    PersistentMedia media = host_platform_persistent_media(&platform);
    VibrationSource vibration = host_platform_vibration_source(&platform);
    assert(campaign_data_store_persistent_composition_init(&historical, &media) == TR2_OK);
    deps.monotonic_clock = &monotonic;
    deps.wall_clock = &wall;
    deps.reset_cause_provider = &reset;
    deps.time_continuity_evidence_provider = &continuity;
    deps.persistent_media = &media;
    deps.configuration_validation_environment = &environment;
    deps.vibration_source = &vibration;
    deps.campaign_data_store = campaign_data_store_persistent_interface(&historical.store);
    assert(system_runtime_init(&runtime, &deps) == TR2_OK);
    binding.runtime = &runtime;
    binding.transport = &transport;
    binding.unit_id = 17u;
    assert(modbus_system_server_init(&server, &binding) == TR2_ERROR_NOT_AVAILABLE);
    assert(system_runtime_boot(&runtime) == TR2_OK);
    configuration_staging_init(&staging);
    assert(configuration_workflow_init(&workflow, &staging,
        (ConfigurationIntegrityPort){tr2_b4_prepared_payload_crc}, (ActivationCommitPort){NULL, test_commit}) == TR2_OK);
    binding.configuration_workflow = &workflow;
    assert(modbus_system_server_init(&server, &binding) == TR2_OK);
    assert(modbus_system_server_poll_once(&server) == TR2_ERROR_INVALID_STATE);
    serial.start_result = TR2_ERROR_UNAVAILABLE;
    assert(modbus_system_server_start(&server) == TR2_ERROR_UNAVAILABLE);
    serial.start_result = TR2_OK;
    assert(modbus_system_server_start(&server) == TR2_OK);
    assert(modbus_system_server_start(&server) == TR2_ERROR_INVALID_STATE);

    load(&serial, 17u, read_b1, sizeof(read_b1), false);
    assert(run(&server, &serial) == TR2_OK);
    view = response(&serial);
    assert(view.pdu_length == 4u && view.pdu[0] == 3u && view.pdu[1] == 2u);
    assert(((uint16_t)view.pdu[2] << 8u | view.pdu[3]) == runtime.b1_image.registers[0]);
    /* Service updates must not leave a permanently copied boot image. */
    runtime.b1_image.registers[0] = 4u;
    load(&serial, 17u, read_b1, sizeof(read_b1), false);
    assert(run(&server, &serial) == TR2_OK);
    view = response(&serial);
    assert(view.pdu[2] == 0u && view.pdu[3] == 4u);

    before = serial.tx_calls;
    load(&serial, 18u, write_b4, sizeof(write_b4), false);
    assert(run(&server, &serial) == TR2_OK);
    assert(serial.tx_calls == before && !configuration_staging_has_prepared(&staging));
    load(&serial, 17u, write_b4, sizeof(write_b4), true);
    assert(run(&server, &serial) == TR2_OK);
    assert(serial.tx_calls == before && !configuration_staging_has_prepared(&staging));
    load(&serial, 0u, read_b1, sizeof(read_b1), false);
    assert(run(&server, &serial) == TR2_OK && serial.tx_calls == before);

    load(&serial, 17u, unsupported, sizeof(unsupported), false);
    assert(run(&server, &serial) == TR2_OK);
    exception(&serial, 6u, MODBUS_PDU_EXCEPTION_ILLEGAL_FUNCTION);
    load(&serial, 17u, bad_address, sizeof(bad_address), false);
    assert(run(&server, &serial) == TR2_OK);
    exception(&serial, 3u, MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_ADDRESS);
    load(&serial, 17u, read_b0, sizeof(read_b0), false);
    assert(run(&server, &serial) == TR2_OK);
    exception(&serial, 3u, MODBUS_PDU_EXCEPTION_SLAVE_DEVICE_FAILURE);

    workflow.has_validated = true;
    workflow.state = CONFIGURATION_STATE_VALID;
    load(&serial, 0u, write_b4, sizeof(write_b4), false);
    before = serial.tx_calls;
    assert(run(&server, &serial) == TR2_OK && serial.tx_calls == before);
    assert(configuration_staging_snapshot(&staging, &prepared));
    assert(prepared.config_id == 0x12345678u);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_DRAFT);
    assert(!workflow.has_validated);
    load(&serial, 17u, read_b4, sizeof(read_b4), false);
    assert(run(&server, &serial) == TR2_OK);
    view = response(&serial);
    assert(view.pdu_length == 6u && view.pdu[2] == 0x12u && view.pdu[5] == 0x78u);

    load(&serial, 17u, write_b2, sizeof(write_b2), false);
    assert(run(&server, &serial) == TR2_OK);
    assert(time_service_get_prepared_time(&runtime.time_service, &prepared_time_available,
                                          &prepared_time) == TR2_OK);
    assert(prepared_time_available && prepared_time == 0x12345678u);
    load(&serial, 17u, read_b6, sizeof(read_b6), false);
    assert(run(&server, &serial) == TR2_OK);
    view = response(&serial);
    assert(view.pdu_length == 12u && view.pdu[0] == 3u);
    load(&serial, 17u, write_b6, sizeof(write_b6), false);
    assert(run(&server, &serial) == TR2_OK);
    assert(response(&serial).pdu[0] == 16u);
    load(&serial, 17u, read_only_write, sizeof(read_only_write), false);
    assert(run(&server, &serial) == TR2_OK);
    exception(&serial, 16u, MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_ADDRESS);

    /* An absent command handler must fail before altering the mailbox. */
    load(&serial, 17u, command, sizeof(command), false);
    assert(run(&server, &serial) == TR2_OK);
    exception(&serial, 16u, MODBUS_PDU_EXCEPTION_SLAVE_DEVICE_FAILURE);
    assert(runtime.command_mailbox.transaction_id == 0u);

    identity.registers[0] = 0x1234u;
    identity.registers[1] = 0x5678u;
    binding.identity = &identity;
    handler.runtime = &runtime;
    binding.command_submit = submit;
    binding.command_submit_context = &handler;
    assert(modbus_system_server_init(&server, &binding) == TR2_OK);
    assert(modbus_system_server_start(&server) == TR2_OK);
    load(&serial, 17u, read_b0, sizeof(read_b0), false);
    assert(run(&server, &serial) == TR2_OK);
    view = response(&serial);
    assert(view.pdu_length == 6u && view.pdu[2] == 0x12u && view.pdu[5] == 0x78u);
    load(&serial, 17u, command, sizeof(command), false);
    assert(run(&server, &serial) == TR2_OK);
    assert(handler.calls == 1u && handler.last_submit == COMMAND_MAILBOX_SUBMISSION_CAPTURED);
    assert(runtime.command_snapshot.last.present);
    assert(runtime.command_snapshot.last.transaction_id == 1u);
    assert(runtime.command_snapshot.last.command_code == COMMAND_CODE_REFRESH_INDICATORS);
    assert((runtime.command_mailbox.control & COMMAND_REQUEST_CONTROL_SUBMIT) == 0u);

    /* Failed TX never causes an automatic replay of the application callback. */
    serial.tx_result = TR2_ERROR_UNAVAILABLE;
    command[9] = 2u;
    load(&serial, 17u, command, sizeof(command), false);
    assert(run(&server, &serial) == TR2_ERROR_UNAVAILABLE);
    assert(handler.calls == 2u);
    assert(modbus_system_server_poll_once(&server) == TR2_OK && handler.calls == 2u);
    serial.tx_result = TR2_OK;

    command[9] = 0u;
    load(&serial, 17u, command, sizeof(command), false);
    assert(run(&server, &serial) == TR2_OK);
    assert(handler.calls == 3u && handler.last_submit == COMMAND_MAILBOX_SUBMISSION_INVALID_TRANSACTION_ID);
    assert(handler.last_request.transaction_id == 0u && handler.last_request.identity.command_code == 7u);
    exception(&serial, 16u, MODBUS_PDU_EXCEPTION_SLAVE_DEVICE_FAILURE);

    command[7] = 0u;
    command[9] = 3u;
    load(&serial, 17u, command, sizeof(command), false);
    assert(run(&server, &serial) == TR2_OK);
    assert(handler.calls == 4u && handler.last_submit == COMMAND_MAILBOX_SUBMISSION_INVALID_CODE);
    assert(handler.last_request.identity.command_code == 0u && handler.last_request.transaction_id == 3u);
    exception(&serial, 16u, MODBUS_PDU_EXCEPTION_SLAVE_DEVICE_FAILURE);

    command[7] = 7u;
    command[9] = 4u;
    load(&serial, 0u, command, sizeof(command), false);
    before = serial.tx_calls;
    assert(run(&server, &serial) == TR2_OK && serial.tx_calls == before);
    assert(handler.calls == 5u && runtime.command_snapshot.last.transaction_id == 4u);

    /* Reserved control must fail atomically, without calling application code. */
    command[20] = 0x80u;
    command[9] = 5u;
    load(&serial, 17u, command, sizeof(command), false);
    assert(run(&server, &serial) == TR2_OK);
    exception(&serial, 16u, MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_ADDRESS);
    assert(handler.calls == 5u && runtime.command_mailbox.transaction_id == 4u);
    command[20] = 0u;

    serial.poll_failure = true;
    assert(modbus_system_server_poll_once(&server) == TR2_ERROR_UNAVAILABLE);
    assert(server.rtu.receiver.state == MODBUS_RTU_RECEIVER_IDLE);
    serial.poll_failure = false;
    load(&serial, 17u, read_b1, sizeof(read_b1), false);
    serial.events[2].type = SERIAL_TRANSPORT_EVENT_ERROR;
    before = serial.tx_calls;
    assert(run(&server, &serial) == TR2_OK && serial.tx_calls == before);
    load(&serial, 17u, read_b1, sizeof(read_b1), false);
    assert(run(&server, &serial) == TR2_OK && serial.tx_calls == before + 1u);

    load(&serial, 17u, command, sizeof(command), false);
    assert(modbus_system_server_poll_once(&server) == TR2_OK);
    runtime.system_ready_for_modbus = false;
    before = serial.tx_calls;
    assert(modbus_system_server_poll_once(&server) == TR2_ERROR_NOT_AVAILABLE);
    assert(!server.started && server.rtu.receiver.length == 0u && serial.tx_calls == before);
    assert(modbus_system_server_poll_once(&server) == TR2_ERROR_INVALID_STATE);

    binding.unit_id = 0u;
    assert(modbus_system_server_init(&server, &binding) == TR2_ERROR_INVALID_ARGUMENT);
    binding.unit_id = 248u;
    assert(modbus_system_server_init(&server, &binding) == TR2_ERROR_INVALID_ARGUMENT);
    assert(modbus_system_server_init(NULL, &binding) == TR2_ERROR_INVALID_ARGUMENT);
    return 0;
}
