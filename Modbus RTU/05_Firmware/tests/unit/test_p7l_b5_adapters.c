#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/modbus/read_adapter.h"
#include "tr2/modbus/write_adapter.h"

static void test_projection_and_atomic_read(void)
{
    CommandRequestMailbox mailbox;
    CommandSnapshot snapshot;
    ModbusBlock5ProjectionSource source;
    ModbusBlock5Image image;
    ModbusReadSources read_sources;
    ModbusReadOutcome read_outcome;
    uint16_t values[20];

    command_request_mailbox_init(&mailbox);
    mailbox.command_code = 3u;
    mailbox.transaction_id = 41u;
    mailbox.param1 = 11u;
    mailbox.param2 = 12u;
    mailbox.param3 = UINT32_C(0x12345678);
    mailbox.confirm_key = UINT16_C(0xA55A);
    mailbox.control = COMMAND_REQUEST_CONTROL_CANCEL_REQUEST;

    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.generation = 9u;
    snapshot.active_command_code = COMMAND_CODE_START_ACQUISITION;
    snapshot.active_transaction_id = 41u;
    snapshot.status = COMMAND_STATUS_RUNNING;
    snapshot.engine_flags = UINT16_C(0x0055);
    snapshot.last.present = true;
    snapshot.last.command_code = COMMAND_CODE_SYNCHRONIZE_TIME;
    snapshot.last.transaction_id = 40u;
    snapshot.last.final_result.status = COMMAND_STATUS_SUCCESS;
    snapshot.last.final_result.result_code = COMMAND_RESULT_SUCCESS;
    snapshot.last.terminal_timestamp.available = true;
    snapshot.last.terminal_timestamp.value = UINT32_C(0x89ABCDEF);

    source.mailbox = &mailbox;
    source.snapshot = &snapshot;
    assert(modbus_project_b5(&source, &image) == TR2_OK);
    assert(image.source_generation == 9u);
    assert(image.registers[0] == 3u);
    assert(image.registers[1] == 41u);
    assert(image.registers[4] == UINT16_C(0x1234));
    assert(image.registers[5] == UINT16_C(0x5678));
    assert(image.registers[7] == COMMAND_REQUEST_CONTROL_CANCEL_REQUEST);
    assert(image.registers[8] == COMMAND_CODE_START_ACQUISITION);
    assert(image.registers[9] == 41u);
    assert(image.registers[10] == COMMAND_STATUS_RUNNING);
    assert(image.registers[13] == UINT16_C(0x0055));
    assert(image.registers[14] == COMMAND_CODE_SYNCHRONIZE_TIME);
    assert(image.registers[15] == 40u);
    assert(image.registers[18] == UINT16_C(0x89AB));
    assert(image.registers[19] == UINT16_C(0xCDEF));

    memset(&read_sources, 0, sizeof(read_sources));
    read_sources.b5_image = &image;
    memset(values, 0, sizeof(values));
    read_outcome = modbus_read_adapter_read(&read_sources, 5004u, 4u, values);
    assert(read_outcome.access_result == MODBUS_ACCESS_OK);
    assert(read_outcome.operation_result == TR2_OK);
    assert(values[0] == UINT16_C(0x1234));
    assert(values[1] == UINT16_C(0x5678));
    assert(values[2] == UINT16_C(0xA55A));
    assert(values[3] == COMMAND_REQUEST_CONTROL_CANCEL_REQUEST);

    read_outcome = modbus_read_adapter_read(&read_sources, 5019u, 2u, values);
    assert(read_outcome.access_result == MODBUS_ACCESS_ILLEGAL_ADDRESS);
}

static void test_completed_falls_back_to_last_ack(void)
{
    CommandRequestMailbox mailbox;
    CommandSnapshot snapshot;
    ModbusBlock5ProjectionSource source;
    ModbusBlock5Image image;

    command_request_mailbox_init(&mailbox);
    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.generation = 12u;
    snapshot.last.present = true;
    snapshot.last.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    snapshot.last.transaction_id = 88u;
    snapshot.last.final_result.status = COMMAND_STATUS_REFUSED;
    snapshot.last.final_result.result_code = COMMAND_RESULT_INVALID_CONFIGURATION;
    snapshot.last.final_result.result_detail = 7u;

    source.mailbox = &mailbox;
    source.snapshot = &snapshot;
    assert(modbus_project_b5(&source, &image) == TR2_OK);
    assert(image.registers[8] == COMMAND_CODE_APPLY_CONFIGURATION);
    assert(image.registers[9] == 88u);
    assert(image.registers[10] == COMMAND_STATUS_REFUSED);
    assert(image.registers[11] == COMMAND_RESULT_INVALID_CONFIGURATION);
    assert(image.registers[12] == 7u);
    assert(image.registers[14] == COMMAND_CODE_APPLY_CONFIGURATION);
    assert(image.registers[16] == COMMAND_STATUS_REFUSED);
    assert(image.registers[18] == 0u);
    assert(image.registers[19] == 0u);
}

static void test_write_capture_and_rejections_are_atomic(void)
{
    CommandRequestMailbox mailbox;
    CommandRequestMailbox before;
    CommandMailboxSubmitResult submit_result;
    CommandRequest captured;
    ModbusWriteOutcome outcome;
    uint16_t request_values[8] = {
        COMMAND_CODE_START_ACQUISITION,
        101u,
        21u,
        22u,
        UINT16_C(0xCAFE),
        UINT16_C(0xBABE),
        UINT16_C(0xA55A),
        COMMAND_REQUEST_CONTROL_SUBMIT
    };
    uint16_t reserved_control = UINT16_C(0x0008);
    uint16_t ro_value = 1u;
    uint16_t invalid_submit[2] = { 0u, COMMAND_REQUEST_CONTROL_SUBMIT };

    command_request_mailbox_init(&mailbox);
    outcome = modbus_write_adapter_write_b5(&mailbox, 5000u, request_values, 8u,
                                            &submit_result, &captured);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(submit_result == COMMAND_MAILBOX_SUBMISSION_CAPTURED);
    assert(captured.transaction_id == 101u);
    assert(captured.identity.command_code == COMMAND_CODE_START_ACQUISITION);
    assert(captured.identity.param1 == 21u);
    assert(captured.identity.param2 == 22u);
    assert(captured.identity.param3 == UINT32_C(0xCAFEBABE));
    assert(captured.identity.confirm_key == UINT16_C(0xA55A));
    assert((mailbox.control & COMMAND_REQUEST_CONTROL_SUBMIT) == 0u);

    before = mailbox;
    outcome = modbus_write_adapter_write_b5(&mailbox, 5007u, &reserved_control, 1u,
                                            &submit_result, &captured);
    assert(outcome.access_result == MODBUS_ACCESS_RESERVED);
    assert(memcmp(&mailbox, &before, sizeof(mailbox)) == 0);

    outcome = modbus_write_adapter_write_b5(&mailbox, 5008u, &ro_value, 1u,
                                            &submit_result, &captured);
    assert(outcome.access_result == MODBUS_ACCESS_READ_ONLY);
    assert(memcmp(&mailbox, &before, sizeof(mailbox)) == 0);

    outcome = modbus_write_adapter_write_b5(&mailbox, 5001u, invalid_submit, 2u,
                                            &submit_result, &captured);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(submit_result == COMMAND_MAILBOX_NO_SUBMISSION);
    assert(mailbox.transaction_id == 0u);

    outcome = modbus_write_adapter_write_b5(&mailbox, 5007u,
                                            &request_values[7], 1u,
                                            &submit_result, &captured);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(submit_result == COMMAND_MAILBOX_SUBMISSION_INVALID_TRANSACTION_ID);
    assert((mailbox.control & COMMAND_REQUEST_CONTROL_SUBMIT) == 0u);
}

int main(void)
{
    test_projection_and_atomic_read();
    test_completed_falls_back_to_last_ack();
    test_write_capture_and_rejections_are_atomic();
    return 0;
}
