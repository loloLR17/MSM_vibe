#ifndef TR2_MODBUS_READ_ADAPTER_H
#define TR2_MODBUS_READ_ADAPTER_H

#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/time/time_service.h"
#include "tr2/modbus/projection.h"
#include "tr2/modbus/register_model.h"

typedef struct {
    const ModbusBlock0Image *b0_image;
    const TimeSnapshot *time;
    const ModbusBlock1Image *b1_image;
    const ModbusBlock3Image *b3_image;
    const ModbusBlock4Image *b4_image;
    const ModbusBlock5Image *b5_image;
    const ModbusBlock6Image *b6_image;
    const ModbusBlock7Image *b7_image;
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
