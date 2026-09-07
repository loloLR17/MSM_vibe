#include <string.h>

#include "tr2/modbus/projection.h"
#include "tr2/modbus/codec.h"

Tr2Result modbus_project_b5(const ModbusBlock5ProjectionSource *source,
                            ModbusBlock5Image *output)
{
    ModbusBlock5Image candidate;

    if (source == NULL || source->mailbox == NULL || source->snapshot == NULL ||
        output == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(&candidate, 0, sizeof(candidate));
    candidate.registers[0] = source->mailbox->command_code;
    candidate.registers[1] = source->mailbox->transaction_id;
    candidate.registers[2] = source->mailbox->param1;
    candidate.registers[3] = source->mailbox->param2;
    modbus_codec_u32_to_msw_lsw(source->mailbox->param3,
                                &candidate.registers[4],
                                &candidate.registers[5]);
    candidate.registers[6] = source->mailbox->confirm_key;
    candidate.registers[7] = source->mailbox->control & COMMAND_REQUEST_CONTROL_ALLOWED_MASK;

    if (source->snapshot->active_transaction_id != TR2_COMMAND_TRANSACTION_ID_INVALID) {
        candidate.registers[8] = source->snapshot->active_command_code;
        candidate.registers[9] = source->snapshot->active_transaction_id;
        candidate.registers[10] = source->snapshot->status;
        candidate.registers[11] = source->snapshot->result_code;
        candidate.registers[12] = source->snapshot->result_detail;
    } else if (source->snapshot->last.present) {
        candidate.registers[8] = source->snapshot->last.command_code;
        candidate.registers[9] = source->snapshot->last.transaction_id;
        candidate.registers[10] = source->snapshot->last.final_result.status;
        candidate.registers[11] = source->snapshot->last.final_result.result_code;
        candidate.registers[12] = source->snapshot->last.final_result.result_detail;
    }
    candidate.registers[13] = source->snapshot->engine_flags;

    if (source->snapshot->last.present) {
        candidate.registers[14] = source->snapshot->last.command_code;
        candidate.registers[15] = source->snapshot->last.transaction_id;
        candidate.registers[16] = source->snapshot->last.final_result.status;
        candidate.registers[17] = source->snapshot->last.final_result.result_code;
        if (source->snapshot->last.terminal_timestamp.available) {
            modbus_codec_u32_to_msw_lsw(source->snapshot->last.terminal_timestamp.value,
                                        &candidate.registers[18],
                                        &candidate.registers[19]);
        }
    }

    candidate.source_generation = source->snapshot->generation;
    *output = candidate;
    return TR2_OK;
}
