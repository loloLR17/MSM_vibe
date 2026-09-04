#include <stddef.h>

#include "tr2/modbus/codec.h"
#include "tr2/modbus/write_adapter.h"

#define TR2_B2_PREPARED_TIME_MSW_ADDRESS UINT16_C(2008)
#define TR2_B2_PREPARED_TIME_LSW_ADDRESS UINT16_C(2009)

ModbusWriteOutcome modbus_write_adapter_write_b2(TimeService *time_service,
                                                 uint16_t start_address,
                                                 const uint16_t *values,
                                                 uint16_t quantity)
{
    ModbusWriteOutcome outcome = { MODBUS_ACCESS_OK, TR2_OK };
    bool prepared_time_available = false;
    Tr2CivilTimestamp prepared_time = 0u;
    uint16_t msw = 0u;
    uint16_t lsw = 0u;
    uint32_t offset;

    if (time_service == NULL || values == NULL) {
        outcome.operation_result = TR2_ERROR_INVALID_ARGUMENT;
        return outcome;
    }

    outcome.access_result = modbus_register_model_validate_write(start_address, quantity);
    if (outcome.access_result != MODBUS_ACCESS_OK) {
        return outcome;
    }

    outcome.operation_result = time_service_get_prepared_time(time_service,
                                                              &prepared_time_available,
                                                              &prepared_time);
    if (outcome.operation_result != TR2_OK) {
        return outcome;
    }

    if (!prepared_time_available) {
        prepared_time = 0u;
    }
    modbus_codec_u32_to_msw_lsw(prepared_time, &msw, &lsw);

    for (offset = 0u; offset < (uint32_t)quantity; ++offset) {
        const uint16_t address = (uint16_t)((uint32_t)start_address + offset);
        if (address == TR2_B2_PREPARED_TIME_MSW_ADDRESS) {
            msw = values[offset];
        } else if (address == TR2_B2_PREPARED_TIME_LSW_ADDRESS) {
            lsw = values[offset];
        } else {
            outcome.operation_result = TR2_ERROR_INTERNAL;
            return outcome;
        }
    }

    outcome.operation_result = time_service_prepare_time(
        time_service,
        modbus_codec_u32_from_msw_lsw(msw, lsw));
    return outcome;
}

#undef TR2_B2_PREPARED_TIME_MSW_ADDRESS
#undef TR2_B2_PREPARED_TIME_LSW_ADDRESS
