#ifndef TR2_MODBUS_REGISTER_MODEL_H
#define TR2_MODBUS_REGISTER_MODEL_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    MODBUS_REGISTER_RO = 0,
    MODBUS_REGISTER_RW,
    MODBUS_REGISTER_RESERVED
} ModbusRegisterAccess;

typedef struct {
    uint16_t address;
    ModbusRegisterAccess access;
    uint8_t block;
} ModbusRegisterDescriptor;

bool modbus_register_descriptor_is_valid(const ModbusRegisterDescriptor *descriptor);
bool modbus_register_is_writable(const ModbusRegisterDescriptor *descriptor);

#endif
