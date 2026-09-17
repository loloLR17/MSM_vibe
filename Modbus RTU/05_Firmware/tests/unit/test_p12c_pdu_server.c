#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/application/command_request_mailbox.h"
#include "tr2/modbus/pdu_server.h"

static void assert_exception(const uint8_t *response,
                             size_t response_length,
                             uint8_t function_code,
                             uint8_t exception_code)
{
    assert(response_length == 2u);
    assert(response[0] == (uint8_t)(function_code | UINT8_C(0x80)));
    assert(response[1] == exception_code);
}

int main(void)
{
    ModbusBlock0Image b0 = { {0u}, 0u };
    CommandRequestMailbox mailbox;
    ModbusPduServerContext context = {0};
    ModbusPduServerOutcome outcome;
    uint8_t response[MODBUS_PDU_MAX_SIZE] = {0u};

    const uint8_t fc03_ok[] = {
        MODBUS_PDU_FC_READ_HOLDING_REGISTERS,
        0x00u, 0x00u,
        0x00u, 0x02u
    };
    const uint8_t fc03_bad_quantity[] = {
        MODBUS_PDU_FC_READ_HOLDING_REGISTERS,
        0x00u, 0x00u,
        0x00u, 0x00u
    };
    const uint8_t fc03_bad_address[] = {
        MODBUS_PDU_FC_READ_HOLDING_REGISTERS,
        0x00u, 0x14u,
        0x00u, 0x02u
    };
    const uint8_t unsupported_fc06[] = {
        0x06u, 0x00u, 0x00u, 0x00u, 0x01u
    };
    const uint8_t fc10_b5_prepare[] = {
        MODBUS_PDU_FC_WRITE_MULTIPLE_REGISTERS,
        0x13u, 0x88u,
        0x00u, 0x02u,
        0x04u,
        0x00u, 0x03u,
        0x12u, 0x34u
    };
    const uint8_t fc10_bad_byte_count[] = {
        MODBUS_PDU_FC_WRITE_MULTIPLE_REGISTERS,
        0x13u, 0x88u,
        0x00u, 0x02u,
        0x02u,
        0x00u, 0x03u
    };
    const uint8_t fc10_read_only[] = {
        MODBUS_PDU_FC_WRITE_MULTIPLE_REGISTERS,
        0x00u, 0x00u,
        0x00u, 0x01u,
        0x02u,
        0x00u, 0x01u
    };

    b0.registers[0] = UINT16_C(0x1234);
    b0.registers[1] = UINT16_C(0x5678);
    context.read_sources.b0_image = &b0;
    command_request_mailbox_init(&mailbox);
    context.command_mailbox = &mailbox;

    outcome = modbus_pdu_server_process(&context,
                                        fc03_ok, sizeof(fc03_ok),
                                        response, sizeof(response));
    assert(outcome.operation_result == TR2_OK);
    assert(outcome.response_length == 6u);
    assert(response[0] == MODBUS_PDU_FC_READ_HOLDING_REGISTERS);
    assert(response[1] == 4u);
    assert(response[2] == 0x12u);
    assert(response[3] == 0x34u);
    assert(response[4] == 0x56u);
    assert(response[5] == 0x78u);

    outcome = modbus_pdu_server_process(&context,
                                        fc03_bad_quantity, sizeof(fc03_bad_quantity),
                                        response, sizeof(response));
    assert(outcome.operation_result == TR2_OK);
    assert_exception(response, outcome.response_length,
                     MODBUS_PDU_FC_READ_HOLDING_REGISTERS,
                     MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_VALUE);

    outcome = modbus_pdu_server_process(&context,
                                        fc03_bad_address, sizeof(fc03_bad_address),
                                        response, sizeof(response));
    assert(outcome.operation_result == TR2_OK);
    assert_exception(response, outcome.response_length,
                     MODBUS_PDU_FC_READ_HOLDING_REGISTERS,
                     MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_ADDRESS);

    outcome = modbus_pdu_server_process(&context,
                                        unsupported_fc06, sizeof(unsupported_fc06),
                                        response, sizeof(response));
    assert(outcome.operation_result == TR2_OK);
    assert_exception(response, outcome.response_length,
                     0x06u,
                     MODBUS_PDU_EXCEPTION_ILLEGAL_FUNCTION);

    outcome = modbus_pdu_server_process(&context,
                                        fc10_b5_prepare, sizeof(fc10_b5_prepare),
                                        response, sizeof(response));
    assert(outcome.operation_result == TR2_OK);
    assert(outcome.response_length == 5u);
    assert(response[0] == MODBUS_PDU_FC_WRITE_MULTIPLE_REGISTERS);
    assert(response[1] == 0x13u);
    assert(response[2] == 0x88u);
    assert(response[3] == 0x00u);
    assert(response[4] == 0x02u);
    assert(mailbox.command_code == UINT16_C(3));
    assert(mailbox.transaction_id == UINT16_C(0x1234));

    outcome = modbus_pdu_server_process(&context,
                                        fc10_bad_byte_count, sizeof(fc10_bad_byte_count),
                                        response, sizeof(response));
    assert(outcome.operation_result == TR2_OK);
    assert_exception(response, outcome.response_length,
                     MODBUS_PDU_FC_WRITE_MULTIPLE_REGISTERS,
                     MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_VALUE);

    outcome = modbus_pdu_server_process(&context,
                                        fc10_read_only, sizeof(fc10_read_only),
                                        response, sizeof(response));
    assert(outcome.operation_result == TR2_OK);
    assert_exception(response, outcome.response_length,
                     MODBUS_PDU_FC_WRITE_MULTIPLE_REGISTERS,
                     MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_ADDRESS);

    outcome = modbus_pdu_server_process(NULL,
                                        fc03_ok, sizeof(fc03_ok),
                                        response, sizeof(response));
    assert(outcome.operation_result == TR2_ERROR_INVALID_ARGUMENT);
    assert(outcome.response_length == 0u);

    return 0;
}
