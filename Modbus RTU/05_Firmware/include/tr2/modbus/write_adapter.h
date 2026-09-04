#ifndef TR2_MODBUS_WRITE_ADAPTER_H
#define TR2_MODBUS_WRITE_ADAPTER_H

#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/time/time_service.h"
#include "tr2/modbus/register_model.h"

typedef struct {
    ModbusAccessResult access_result;
    Tr2Result operation_result;
} ModbusWriteOutcome;

ModbusWriteOutcome modbus_write_adapter_write_b2(TimeService *time_service,
                                                 uint16_t start_address,
                                                 const uint16_t *values,
                                                 uint16_t quantity);

#endif
