#ifndef TR2_MODBUS_PROJECTION_H
#define TR2_MODBUS_PROJECTION_H

#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/identity/identity.h"

#define TR2_B0_REGISTER_COUNT 21u

typedef struct {
    uint16_t registers[TR2_B0_REGISTER_COUNT];
    uint32_t source_generation;
} ModbusBlock0Image;

Tr2Result modbus_project_b0(const IdentitySnapshot *snapshot, ModbusBlock0Image *output);

#endif
