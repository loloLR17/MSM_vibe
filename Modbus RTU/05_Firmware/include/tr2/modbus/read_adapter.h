#ifndef TR2_MODBUS_READ_ADAPTER_H
#define TR2_MODBUS_READ_ADAPTER_H

#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/identity/identity.h"
#include "tr2/domain/system_state/system_state.h"
#include "tr2/domain/time/time_service.h"
#include "tr2/modbus/register_model.h"

typedef struct {
    const IdentitySnapshot *identity;
    const SystemStateSnapshot *system_state;
    const TimeSnapshot *time;
} ModbusReadSources;

typedef struct {
    ModbusAccessResult access_result;
    Tr2Result operation_result;
} ModbusReadOutcome;

ModbusReadOutcome modbus_read_adapter_read(const ModbusReadSources *sources,
                                           uint16_t start_address,
                                           uint16_t quantity,
                                           uint16_t *values);

#endif
