#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "tr2/application/production_application.h"
#include "tr2/application/configuration_activation_adapter.h"
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

static ConfigurationPayload valid_payload(void)
{
    ConfigurationPayload payload = { 0 };

    payload.sampling_frequency_hz = UINT16_C(26667);
    payload.axes_enable_mask = UINT16_C(7);
    payload.full_scale_code = UINT16_C(2);
    payload.acquisition_mode = UINT16_C(1);
    payload.window_size_samples = UINT16_C(32768);
    payload.indicator_period_ms = UINT16_C(5000);
    payload.campaign_duration_s = UINT32_C(3600);
    payload.storage_mode = UINT16_C(1);
    payload.storage_limit_mb = UINT32_C(512);
    payload.supervision_enable_mask = UINT16_C(1);
    payload.rms_warn_threshold_mg = UINT16_C(100);
    payload.rms_alarm_threshold_mg = UINT16_C(200);
    payload.peak_warn_threshold_mg = UINT16_C(300);
    payload.peak_alarm_threshold_mg = UINT16_C(400);
    payload.threshold_hysteresis_mg = UINT16_C(20);
    payload.alarm_hold_time_ms = UINT16_C(500);
    payload.campaign_context_id = UINT32_C(1);
    payload.mission_id = UINT32_C(2);
    payload.operating_mode_code = UINT16_C(1);
    payload.navigation_zone_code = UINT16_C(2);
    payload.load_state_code = UINT16_C(3);
    payload.sea_state_code = UINT16_C(1);
    return payload;
}

static Tr2Result metadata(void *context, const ValidatedConfiguration *validated,
                           ConfigurationActivationMetadata *out)
{
    (void)context;
    /* Explicit test provisioning: correlate persistent generation with staging. */
    out->persistent_generation = validated->generation;
    out->revision_counter = 1u;
    return TR2_OK;
}

static void command(ModbusSystemServer *server, TestSerial *serial,
                    uint16_t code, uint16_t id, uint16_t param, uint16_t control)
{
    uint8_t pdu[] = {16u, 0x13u, 0x88u, 0u, 8u, 16u,
                    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
                    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u};
    pdu[6] = (uint8_t)(code >> 8u); pdu[7] = (uint8_t)code;
    pdu[8] = (uint8_t)(id >> 8u); pdu[9] = (uint8_t)id;
    pdu[10] = (uint8_t)(param >> 8u); pdu[11] = (uint8_t)param;
    pdu[20] = (uint8_t)(control >> 8u); pdu[21] = (uint8_t)control;
    load(serial, 17u, pdu, sizeof(pdu), false);
    assert(run(server, serial) == TR2_OK);
}

static void b5(ModbusSystemServer *server, TestSerial *serial,
               uint16_t status, uint16_t result, uint16_t last_id)
{
    const uint8_t read[] = {3u, 0x13u, 0x92u, 0u, 10u};
    load(serial, 17u, read, sizeof(read), false);
    assert(run(server, serial) == TR2_OK);
    ModbusRtuAduView r = response(serial);
    assert(r.pdu[0] == 3u && r.pdu[1] == 20u);
    assert(((uint16_t)r.pdu[2] << 8u | r.pdu[3]) == status);
    assert(((uint16_t)r.pdu[4] << 8u | r.pdu[5]) == result);
    assert(((uint16_t)r.pdu[12] << 8u | r.pdu[13]) == last_id);
}

static Tr2Result (*real_commit)(void *context);
static unsigned commits_before_failure;
static Tr2Result fail_commit(void *context)
{
    if (commits_before_failure == 0u) return TR2_ERROR_STORAGE;
    --commits_before_failure;
    return real_commit(context);
}

int main(void)
{
    static HostPlatform platform;
    static ProductionApplication application;
    static ProductionApplication rebooted;
    static ProductionApplication fault_rebooted;
    static CampaignDataStorePersistentComposition historical;
    ConfigurationValidationEnvironment environment = {true, 4096u};
    SystemRuntimeDependencies deps = {0};
    ModbusSystemServerBinding binding = {0};
    ModbusSystemServer server;
    TestSerial serial = {0};
    SerialTransport transport = {&serial, start, transmit, poll};
    ConfigurationStagingService *staging = &application.staging;
    ConfigurationWorkflow *workflow = &application.configuration;
    CommandSnapshot snapshot;
    CommandJournalEntry entry;
    CommandAdmissionResult admission;
    CommandRequest active = {100u, {COMMAND_CODE_REFRESH_INDICATORS, 0u, 0u, 0u, 0u}};

    host_platform_init(&platform);
    MonotonicClock monotonic = host_platform_monotonic_clock(&platform);
    WallClock wall = host_platform_wall_clock(&platform);
    ResetCauseProvider reset = host_platform_reset_cause_provider(&platform);
    TimeContinuityEvidenceProvider continuity = host_platform_time_continuity_evidence_provider(&platform);
    PersistentMedia media = host_platform_persistent_media(&platform);
    VibrationSource vibration = host_platform_vibration_source(&platform);
    assert(campaign_data_store_persistent_composition_init(&historical, &media) == TR2_OK);
    deps = (SystemRuntimeDependencies){&monotonic, &wall, &reset, &continuity, &media,
        &environment, &vibration, NULL, NULL, campaign_data_store_persistent_interface(&historical.store)};
    assert(production_application_binding(&application, &transport, 17u, NULL, &binding)
           == TR2_ERROR_NOT_AVAILABLE);
    assert(production_application_boot(&application, &deps, NULL) == TR2_OK);
    assert(production_application_boot(&application, &deps, NULL) == TR2_ERROR_INVALID_STATE);
    assert(application.runtime.deps.persistent_media == &application.media);
    assert(application.runtime.deps.monotonic_clock == &application.monotonic);
    assert(application.runtime.deps.configuration_validation_environment == &application.environment);
    assert(production_application_binding(&application, &transport, 0u, NULL, &binding)
           == TR2_ERROR_INVALID_ARGUMENT);
    assert(production_application_binding(&application, &transport, 17u, NULL, &binding) == TR2_OK);
    assert(modbus_system_server_init(&server, &binding) == TR2_OK);
    assert(modbus_system_server_start(&server) == TR2_OK);

    command(&server, &serial, 7u, 1u, 0u, 1u);
    assert(response(&serial).pdu[0] == 16u);
    b5(&server, &serial, COMMAND_STATUS_SUCCESS, COMMAND_RESULT_SUCCESS, 1u);
    uint32_t generation = application.runtime.command_engine.snapshot_generation;
    command(&server, &serial, 7u, 1u, 0u, 1u);
    assert(application.runtime.command_engine.snapshot_generation == generation);
    command(&server, &serial, 7u, 1u, 9u, 1u);
    exception(&serial, 16u, 4u);
    assert(application.runtime.command_engine.snapshot_generation == generation);

    command(&server, &serial, 7u, 0u, 0u, 1u);
    assert(response(&serial).pdu[0] == 16u);
    b5(&server, &serial, COMMAND_STATUS_REFUSED, COMMAND_RESULT_INVALID_TRANSACTION_ID, 1u);
    assert(application.runtime.command_engine.snapshot_generation == generation);
    assert(application.runtime.command_engine.journal->find(
        application.runtime.command_engine.journal->context, 0u, &entry) != TR2_OK);
    command(&server, &serial, 0u, 0u, 0u, 1u);
    b5(&server, &serial, COMMAND_STATUS_REFUSED, COMMAND_RESULT_INVALID_TRANSACTION_ID, 1u);
    command(&server, &serial, 7u, 2u, 1u, 1u);
    b5(&server, &serial, COMMAND_STATUS_REFUSED, COMMAND_RESULT_INVALID_PARAMETER, 2u);
    assert(application.runtime.command_engine.journal->find(
        application.runtime.command_engine.journal->context, 2u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);

    command(&server, &serial, 65535u, 3u, 0u, 1u);
    b5(&server, &serial, COMMAND_STATUS_UNKNOWN, COMMAND_RESULT_UNKNOWN_COMMAND, 3u);
    command(&server, &serial, 2u, 4u, 0u, 1u);
    b5(&server, &serial, COMMAND_STATUS_REFUSED, COMMAND_RESULT_PREPARED_TIME_ABSENT, 4u);
    assert(time_service_prepare_time(&application.runtime.time_service, 1700000000u) == TR2_OK);
    command(&server, &serial, 2u, 5u, 0u, 1u);
    b5(&server, &serial, COMMAND_STATUS_SUCCESS, COMMAND_RESULT_SUCCESS, 5u);
    assert(platform.civil_time == 1700000000u);
    TimeSnapshot time;
    assert(time_service_get_snapshot(&application.runtime.time_service, &time) == TR2_OK);
    assert(time.sync_source == 1u);
    command(&server, &serial, 2u, 5u, 0u, 1u);
    assert(platform.civil_time == 1700000000u);
    command(&server, &serial, 5u, 6u, 0u, 1u);
    exception(&serial, 16u, 4u);
    command(&server, &serial, 11u, 6u, 0u, 1u);
    exception(&serial, 16u, 4u);
    assert(application.runtime.command_engine.journal->find(
        application.runtime.command_engine.journal->context, 6u, &entry) == TR2_ERROR_NOT_FOUND);

    command(&server, &serial, 3u, 13u, 0u, 1u);
    b5(&server, &serial, COMMAND_STATUS_REFUSED, COMMAND_RESULT_ACTIVE_CONFIGURATION_INVALID, 13u);
    assert(platform.vibration_start_calls == 0u);

    /* Actual ConfigurationStore activation through the existing adapter. */
    assert(production_application_enable_configuration(&application, NULL, metadata) == TR2_OK);
    assert(production_application_enable_configuration(&application, NULL, metadata) == TR2_ERROR_INVALID_STATE);
    server.binding.configuration_workflow = workflow;
    command(&server, &serial, 1u, 7u, 0u, 1u);
    b5(&server, &serial, COMMAND_STATUS_REFUSED, COMMAND_RESULT_PREPARED_CONFIGURATION_INCOMPLETE, 7u);
    ConfigurationPayload payload = valid_payload();
    configuration_staging_set_config_id(staging, 42u);
    configuration_staging_replace_payload(staging, &payload);
    configuration_staging_set_supplied_crc(staging, tr2_b4_prepared_payload_crc(&payload));
    configuration_workflow_note_prepared_payload_modified(workflow);
    configuration_staging_set_supplied_crc(staging, tr2_b4_prepared_payload_crc(&payload) ^ 1u);
    command(&server, &serial, 1u, 12u, 0u, 1u);
    b5(&server, &serial, COMMAND_STATUS_REFUSED, COMMAND_RESULT_INVALID_CONFIGURATION, 12u);
    assert(configuration_workflow_state(workflow) == CONFIGURATION_STATE_VALIDATION_ERROR);
    assert(!configuration_service_active_snapshot(&application.runtime.configuration_service, &(ActiveConfigurationSnapshot){0}));
    configuration_staging_set_supplied_crc(staging, tr2_b4_prepared_payload_crc(&payload));
    configuration_workflow_note_prepared_payload_modified(workflow);
    command(&server, &serial, 1u, 8u, 0u, 1u);
    b5(&server, &serial, COMMAND_STATUS_SUCCESS, COMMAND_RESULT_SUCCESS, 8u);
    ActiveConfigurationSnapshot configuration;
    assert(configuration_service_active_snapshot(&application.runtime.configuration_service, &configuration));
    assert(configuration.config_id == 42u);
    command(&server, &serial, 8u, 9u, 0u, 1u);
    assert(maintenance_service_active(&application.runtime.maintenance_service));
    command(&server, &serial, 9u, 10u, 0u, 1u);
    assert(!maintenance_service_active(&application.runtime.maintenance_service));
    command(&server, &serial, 4u, 11u, 0u, 1u);
    b5(&server, &serial, COMMAND_STATUS_REFUSED, COMMAND_RESULT_ACQUISITION_NOT_ACTIVE, 11u);

    /* Cancellation is consumed once, does not complete or destroy the active identity. */
    assert(command_engine_admit(&application.runtime.command_engine, &active, &admission) == TR2_OK);
    const uint8_t cancel[] = {16u, 0x13u, 0x8Fu, 0u, 1u, 2u, 0u, 2u};
    load(&serial, 17u, cancel, sizeof(cancel), false);
    assert(run(&server, &serial) == TR2_OK);
    assert(!command_request_mailbox_cancel_requested(&application.runtime.command_mailbox));
    b5(&server, &serial, COMMAND_STATUS_ACCEPTED, COMMAND_RESULT_NOT_CANCELLABLE, 11u);
    assert(command_engine_active_transaction_id(&application.runtime.command_engine) == 100u);
    command(&server, &serial, 7u, 101u, 0u, 1u);
    b5(&server, &serial, COMMAND_STATUS_REFUSED, COMMAND_RESULT_COMMAND_ALREADY_RUNNING, 11u);
    assert(command_engine_active_transaction_id(&application.runtime.command_engine) == 100u);
    assert(application.runtime.command_engine.journal->find(
        application.runtime.command_engine.journal->context, 101u, &entry) == TR2_ERROR_NOT_FOUND);
    CommandFinalResult final = {COMMAND_STATUS_REFUSED, COMMAND_RESULT_INCOMPATIBLE_STATE, 0u};
    CommandTerminalTimestamp unavailable = {false, 0u};
    assert(command_engine_complete(&application.runtime.command_engine, 100u,
        &final, &unavailable, &entry) == TR2_OK);

    /* Reboot reconstructs durable configuration and refusals; ID0 view is volatile. */
    assert(production_application_boot(&rebooted, &deps, NULL) == TR2_OK);
    assert(configuration_service_active_snapshot(&rebooted.runtime.configuration_service, &configuration));
    assert(configuration.config_id == 42u);
    assert(system_command_dispatcher_snapshot(&rebooted.commands, &snapshot) == TR2_OK);
    assert(snapshot.last.transaction_id == 100u && !snapshot.last.terminal_timestamp.available);
    CommandRequest retry = {2u, {7u, 1u, 0u, 0u, 0u}};
    assert(system_command_dispatcher_submit(&rebooted.commands, COMMAND_MAILBOX_SUBMISSION_CAPTURED,
        &retry) == TR2_OK);
    assert(system_command_dispatcher_snapshot(&rebooted.commands, &snapshot) == TR2_OK);
    assert(snapshot.result_code == COMMAND_RESULT_INVALID_PARAMETER && snapshot.active_transaction_id == 2u);
    assert(snapshot.last.transaction_id == 100u); /* historical last not rewritten by retry */
    /* Failure of RESERVED prevents dispatch. Failure of COMPLETED must not
       publish a success, even though REFRESH already performed its effect. */
    real_commit = rebooted.media.commit;
    rebooted.media.commit = fail_commit;
    commits_before_failure = 0u;
    retry = (CommandRequest){200u, {7u, 0u, 0u, 0u, 0u}};
    generation = rebooted.runtime.system_state_snapshot.generation;
    assert(system_command_dispatcher_submit(&rebooted.commands,
        COMMAND_MAILBOX_SUBMISSION_CAPTURED, &retry) == TR2_ERROR_STORAGE);
    assert(rebooted.runtime.system_state_snapshot.generation == generation);
    assert(!command_engine_has_active_transaction(&rebooted.runtime.command_engine));
    /* Restore the failed candidate from committed bytes, as a host power cycle.
       Production does not auto-retry after persistence failure. */
    memcpy(platform.persistent_candidate, platform.persistent_committed,
           HOST_PLATFORM_PERSISTENT_BYTES);
    assert(production_application_boot(&fault_rebooted, &deps, NULL) == TR2_OK);
    real_commit = fault_rebooted.media.commit;
    fault_rebooted.media.commit = fail_commit;
    commits_before_failure = 2u; /* RESERVED, STARTED pass; COMPLETED fails. */
    retry.transaction_id = 201u;
    assert(system_command_dispatcher_submit(&fault_rebooted.commands,
        COMMAND_MAILBOX_SUBMISSION_CAPTURED, &retry) == TR2_ERROR_STORAGE);
    assert(command_engine_active_transaction_id(&fault_rebooted.runtime.command_engine) == 201u);
    assert(command_journal_bounded_store_recovery_required(&fault_rebooted.runtime.command_journal_store));
    assert(!fault_rebooted.commands.has_view); /* no final success manufactured */
    assert(system_command_dispatcher_snapshot(&fault_rebooted.commands, &snapshot)
           == TR2_ERROR_INVALID_STATE); /* fail closed until explicit recovery */
    fault_rebooted.media.commit = real_commit;
    return 0;
}
