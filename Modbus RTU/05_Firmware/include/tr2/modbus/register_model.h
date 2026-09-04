#ifndef TR2_MODBUS_REGISTER_MODEL_H
#define TR2_MODBUS_REGISTER_MODEL_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    MODBUS_BLOCK_0 = 0,
    MODBUS_BLOCK_1,
    MODBUS_BLOCK_2,
    MODBUS_BLOCK_3,
    MODBUS_BLOCK_4,
    MODBUS_BLOCK_5,
    MODBUS_BLOCK_6,
    MODBUS_BLOCK_7
} ModbusBlock;

typedef enum {
    MODBUS_REGISTER_RO = 0,
    MODBUS_REGISTER_RW,
    MODBUS_REGISTER_RESERVED
} ModbusRegisterAccess;

typedef enum {
    MODBUS_REGISTER_UINT16 = 0,
    MODBUS_REGISTER_INT16,
    MODBUS_REGISTER_UINT32_MSW,
    MODBUS_REGISTER_UINT32_LSW,
    MODBUS_REGISTER_BITFIELD16,
    MODBUS_REGISTER_ENUM16,
    MODBUS_REGISTER_ASCII
} ModbusRegisterKind;

typedef struct {
    uint16_t address;
    ModbusBlock block;
    ModbusRegisterAccess access;
    ModbusRegisterKind kind;
} ModbusRegisterDescriptor;

typedef enum {
    MODBUS_ACCESS_OK = 0,
    MODBUS_ACCESS_ILLEGAL_ADDRESS,
    MODBUS_ACCESS_READ_ONLY,
    MODBUS_ACCESS_RESERVED
} ModbusAccessResult;

bool modbus_register_descriptor_is_valid(const ModbusRegisterDescriptor *descriptor);
bool modbus_register_is_writable(const ModbusRegisterDescriptor *descriptor);
const ModbusRegisterDescriptor *modbus_register_model_find(uint16_t address);
bool modbus_register_model_range_exists(uint16_t start_address, uint16_t quantity);
ModbusAccessResult modbus_register_model_validate_read(uint16_t start_address, uint16_t quantity);
ModbusAccessResult modbus_register_model_validate_write(uint16_t start_address, uint16_t quantity);

#endif
