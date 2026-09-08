#include <stddef.h>
#include <string.h>

#include "tr2/modbus/write_adapter.h"
#include "tr2/modbus/codec.h"

#define TR2_B5_CONTROL_ADDRESS UINT16_C(5007)

ModbusWriteOutcome modbus_write_adapter_write_b5(
    CommandRequestMailbox *mailbox,
    uint16_t start_address,
    const uint16_t *values,
    uint16_t quantity,
    CommandMailboxSubmitResult *submit_result,
    CommandRequest *captured_request)
{
    ModbusWriteOutcome outcome = { MODBUS_ACCESS_OK, TR2_OK };
    CommandRequestMailbox candidate;
    CommandMailboxSubmitResult local_submit = COMMAND_MAILBOX_NO_SUBMISSION;
    uint32_t last_address;
    uint16_t index;

    if (mailbox == NULL || values == NULL || submit_result == NULL ||
        captured_request == NULL) {
        outcome.operation_result = TR2_ERROR_INVALID_ARGUMENT;
        return outcome;
    }

    *submit_result = COMMAND_MAILBOX_NO_SUBMISSION;
    memset(captured_request, 0, sizeof(*captured_request));

    outcome.access_result = modbus_register_model_validate_write(start_address, quantity);
    if (outcome.access_result != MODBUS_ACCESS_OK) {
        return outcome;
    }
    last_address = (uint32_t)start_address + (uint32_t)quantity - 1u;

    for (index = 0u; index < quantity; ++index) {
        uint16_t address = (uint16_t)(start_address + index);
        if (address == TR2_B5_CONTROL_ADDRESS &&
            (values[index] & (uint16_t)~COMMAND_REQUEST_CONTROL_ALLOWED_MASK) != 0u) {
            outcome.access_result = MODBUS_ACCESS_RESERVED;
            return outcome;
        }
    }

    candidate = *mailbox;
    for (index = 0u; index < quantity; ++index) {
        uint16_t address = (uint16_t)(start_address + index);
        uint16_t value = values[index];

        switch (address) {
        case 5000u:
            candidate.command_code = value;
            break;
        case 5001u:
            candidate.transaction_id = value;
            break;
        case 5002u:
            candidate.param1 = value;
            break;
        case 5003u:
            candidate.param2 = value;
            break;
        case 5004u:
            candidate.param3 = modbus_codec_u32_from_msw_lsw(
                value, (uint16_t)(candidate.param3 & UINT32_C(0xFFFF)));
            break;
        case 5005u:
            candidate.param3 = modbus_codec_u32_from_msw_lsw(
                (uint16_t)(candidate.param3 >> 16), value);
            break;
        case 5006u:
            candidate.confirm_key = value;
            break;
        case 5007u:
            break;
        default:
            outcome.access_result = MODBUS_ACCESS_ILLEGAL_ADDRESS;
            return outcome;
        }
    }

    if (start_address <= TR2_B5_CONTROL_ADDRESS &&
        last_address >= TR2_B5_CONTROL_ADDRESS) {
        uint16_t control_index = (uint16_t)(TR2_B5_CONTROL_ADDRESS - start_address);
        local_submit = command_request_mailbox_write_control(&candidate,
                                                             values[control_index],
                                                             captured_request);
    }

    *mailbox = candidate;
    *submit_result = local_submit;
    return outcome;
}

#undef TR2_B5_CONTROL_ADDRESS
