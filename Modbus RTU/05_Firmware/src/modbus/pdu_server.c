#include <stddef.h>
#include <stdint.h>

#include "tr2/modbus/pdu_server.h"
#include "tr2/modbus/register_model.h"
#include "tr2/modbus/write_adapter.h"

#define FC03_REQUEST_LENGTH ((size_t)5u)
#define FC03_MAX_REGISTERS UINT16_C(125)
#define FC10_MIN_REQUEST_LENGTH ((size_t)6u)
#define FC10_MAX_REGISTERS UINT16_C(123)

static uint16_t read_u16_be(const uint8_t *bytes)
{
    return (uint16_t)(((uint16_t)bytes[0] << 8u) | (uint16_t)bytes[1]);
}

static void write_u16_be(uint8_t *bytes, uint16_t value)
{
    bytes[0] = (uint8_t)(value >> 8u);
    bytes[1] = (uint8_t)(value & UINT16_C(0x00FF));
}

static ModbusPduServerOutcome failure(Tr2Result result)
{
    ModbusPduServerOutcome outcome = { result, 0u };
    return outcome;
}

static ModbusPduServerOutcome exception_response(uint8_t function_code,
                                                 uint8_t exception_code,
                                                 uint8_t *response,
                                                 size_t response_capacity)
{
    ModbusPduServerOutcome outcome = { TR2_OK, 0u };

    if (response_capacity < 2u) {
        return failure(TR2_ERROR_INVALID_ARGUMENT);
    }
    response[0] = (uint8_t)(function_code | UINT8_C(0x80));
    response[1] = exception_code;
    outcome.response_length = 2u;
    return outcome;
}

static uint8_t access_exception(ModbusAccessResult access_result)
{
    (void)access_result;
    return MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_ADDRESS;
}

static ModbusPduServerOutcome process_fc03(const ModbusPduServerContext *context,
                                           const uint8_t *request,
                                           size_t request_length,
                                           uint8_t *response,
                                           size_t response_capacity)
{
    uint16_t values[FC03_MAX_REGISTERS];
    uint16_t start_address;
    uint16_t quantity;
    uint16_t index;
    size_t required_response;
    ModbusReadOutcome read_outcome;
    ModbusPduServerOutcome outcome = { TR2_OK, 0u };

    if (request_length != FC03_REQUEST_LENGTH) {
        return exception_response(request[0], MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_VALUE,
                                  response, response_capacity);
    }
    start_address = read_u16_be(&request[1]);
    quantity = read_u16_be(&request[3]);
    if (quantity == 0u || quantity > FC03_MAX_REGISTERS) {
        return exception_response(request[0], MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_VALUE,
                                  response, response_capacity);
    }
    required_response = 2u + ((size_t)quantity * 2u);
    if (required_response > response_capacity || required_response > MODBUS_PDU_MAX_SIZE) {
        return failure(TR2_ERROR_INVALID_ARGUMENT);
    }

    read_outcome = modbus_read_adapter_read(&context->read_sources,
                                            start_address,
                                            quantity,
                                            values);
    if (read_outcome.access_result != MODBUS_ACCESS_OK) {
        return exception_response(request[0], access_exception(read_outcome.access_result),
                                  response, response_capacity);
    }
    if (read_outcome.operation_result != TR2_OK) {
        return exception_response(request[0], MODBUS_PDU_EXCEPTION_SLAVE_DEVICE_FAILURE,
                                  response, response_capacity);
    }

    response[0] = MODBUS_PDU_FC_READ_HOLDING_REGISTERS;
    response[1] = (uint8_t)(quantity * 2u);
    for (index = 0u; index < quantity; ++index) {
        write_u16_be(&response[2u + ((size_t)index * 2u)], values[index]);
    }
    outcome.response_length = required_response;
    return outcome;
}

static ModbusWriteOutcome dispatch_write(const ModbusPduServerContext *context,
                                         uint16_t start_address,
                                         const uint16_t *values,
                                         uint16_t quantity)
{
    const ModbusRegisterDescriptor *descriptor = modbus_register_model_find(start_address);
    ModbusWriteOutcome outcome = { MODBUS_ACCESS_ILLEGAL_ADDRESS, TR2_OK };

    if (descriptor == NULL) {
        return outcome;
    }

    switch (descriptor->block) {
    case MODBUS_BLOCK_2:
        if (context->time_service == NULL) {
            outcome.operation_result = TR2_ERROR_NOT_AVAILABLE;
            return outcome;
        }
        return modbus_write_adapter_write_b2(context->time_service,
                                             start_address, values, quantity);
    case MODBUS_BLOCK_4:
        if (context->configuration_staging == NULL) {
            outcome.operation_result = TR2_ERROR_NOT_AVAILABLE;
            return outcome;
        }
        return modbus_write_adapter_write_b4(context->configuration_staging,
                                             start_address, values, quantity);
    case MODBUS_BLOCK_5:
        if (context->command_mailbox == NULL) {
            outcome.operation_result = TR2_ERROR_NOT_AVAILABLE;
            return outcome;
        }
        {
            CommandMailboxSubmitResult submit_result = COMMAND_MAILBOX_SUBMIT_REJECTED_INVALID_STATE;
            CommandRequest captured_request = {0};
            return modbus_write_adapter_write_b5(context->command_mailbox,
                                                 start_address, values, quantity,
                                                 &submit_result, &captured_request);
        }
    case MODBUS_BLOCK_6:
        if (context->campaign_inventory == NULL || context->b6_image == NULL) {
            outcome.operation_result = TR2_ERROR_NOT_AVAILABLE;
            return outcome;
        }
        return modbus_write_adapter_write_b6(context->campaign_inventory,
                                             context->b6_image,
                                             start_address, values, quantity);
    default:
        return outcome;
    }
}

static ModbusPduServerOutcome process_fc10(const ModbusPduServerContext *context,
                                           const uint8_t *request,
                                           size_t request_length,
                                           uint8_t *response,
                                           size_t response_capacity)
{
    uint16_t values[FC10_MAX_REGISTERS];
    uint16_t start_address;
    uint16_t quantity;
    uint16_t index;
    uint8_t byte_count;
    size_t expected_length;
    ModbusWriteOutcome write_outcome;
    ModbusPduServerOutcome outcome = { TR2_OK, 0u };

    if (request_length < FC10_MIN_REQUEST_LENGTH) {
        return exception_response(request[0], MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_VALUE,
                                  response, response_capacity);
    }
    start_address = read_u16_be(&request[1]);
    quantity = read_u16_be(&request[3]);
    byte_count = request[5];
    if (quantity == 0u || quantity > FC10_MAX_REGISTERS ||
        (size_t)byte_count != ((size_t)quantity * 2u)) {
        return exception_response(request[0], MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_VALUE,
                                  response, response_capacity);
    }
    expected_length = 6u + (size_t)byte_count;
    if (request_length != expected_length || request_length > MODBUS_PDU_MAX_SIZE) {
        return exception_response(request[0], MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_VALUE,
                                  response, response_capacity);
    }
    if (response_capacity < 5u) {
        return failure(TR2_ERROR_INVALID_ARGUMENT);
    }

    for (index = 0u; index < quantity; ++index) {
        values[index] = read_u16_be(&request[6u + ((size_t)index * 2u)]);
    }
    write_outcome = dispatch_write(context, start_address, values, quantity);
    if (write_outcome.access_result != MODBUS_ACCESS_OK) {
        return exception_response(request[0], access_exception(write_outcome.access_result),
                                  response, response_capacity);
    }
    if (write_outcome.operation_result != TR2_OK) {
        return exception_response(request[0], MODBUS_PDU_EXCEPTION_SLAVE_DEVICE_FAILURE,
                                  response, response_capacity);
    }

    response[0] = MODBUS_PDU_FC_WRITE_MULTIPLE_REGISTERS;
    write_u16_be(&response[1], start_address);
    write_u16_be(&response[3], quantity);
    outcome.response_length = 5u;
    return outcome;
}

ModbusPduServerOutcome modbus_pdu_server_process(const ModbusPduServerContext *context,
                                                 const uint8_t *request_pdu,
                                                 size_t request_length,
                                                 uint8_t *response_pdu,
                                                 size_t response_capacity)
{
    if (context == NULL || request_pdu == NULL || response_pdu == NULL ||
        request_length == 0u || request_length > MODBUS_PDU_MAX_SIZE) {
        return failure(TR2_ERROR_INVALID_ARGUMENT);
    }

    switch (request_pdu[0]) {
    case MODBUS_PDU_FC_READ_HOLDING_REGISTERS:
        return process_fc03(context, request_pdu, request_length,
                            response_pdu, response_capacity);
    case MODBUS_PDU_FC_WRITE_MULTIPLE_REGISTERS:
        return process_fc10(context, request_pdu, request_length,
                            response_pdu, response_capacity);
    default:
        return exception_response(request_pdu[0], MODBUS_PDU_EXCEPTION_ILLEGAL_FUNCTION,
                                  response_pdu, response_capacity);
    }
}
