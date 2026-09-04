#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include "tr2/modbus/register_model.h"

static void assert_descriptor(uint16_t address,
                              ModbusBlock block,
                              ModbusRegisterAccess access,
                              ModbusRegisterKind kind)
{
    const ModbusRegisterDescriptor *descriptor = modbus_register_model_find(address);
    assert(descriptor != NULL);
    assert(descriptor->address == address);
    assert(descriptor->block == block);
    assert(descriptor->access == access);
    assert(descriptor->kind == kind);
    assert(modbus_register_descriptor_is_valid(descriptor));
}

int main(void)
{
    assert_descriptor(0u, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT32_MSW);
    assert_descriptor(20u, MODBUS_BLOCK_0, MODBUS_REGISTER_RESERVED, MODBUS_REGISTER_UINT16);
    assert_descriptor(1000u, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_ENUM16);
    assert_descriptor(1007u, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_INT16);
    assert_descriptor(1019u, MODBUS_BLOCK_1, MODBUS_REGISTER_RESERVED, MODBUS_REGISTER_UINT16);
    assert_descriptor(2008u, MODBUS_BLOCK_2, MODBUS_REGISTER_RW, MODBUS_REGISTER_UINT32_MSW);
    assert_descriptor(2009u, MODBUS_BLOCK_2, MODBUS_REGISTER_RW, MODBUS_REGISTER_UINT32_LSW);
    assert_descriptor(2015u, MODBUS_BLOCK_2, MODBUS_REGISTER_RESERVED, MODBUS_REGISTER_UINT16);

    assert(modbus_register_model_find(21u) == NULL);
    assert(modbus_register_model_find(999u) == NULL);
    assert(modbus_register_model_find(1020u) == NULL);
    assert(modbus_register_model_find(1999u) == NULL);
    assert(modbus_register_model_find(2016u) == NULL);

    assert(modbus_register_model_range_exists(0u, 21u));
    assert(modbus_register_model_range_exists(1000u, 20u));
    assert(modbus_register_model_range_exists(2000u, 16u));
    assert(!modbus_register_model_range_exists(0u, 22u));
    assert(!modbus_register_model_range_exists(20u, 2u));
    assert(!modbus_register_model_range_exists(0u, 0u));
    assert(!modbus_register_model_range_exists(UINT16_MAX, 2u));

    assert(modbus_register_model_validate_read(0u, 21u) == MODBUS_ACCESS_OK);
    assert(modbus_register_model_validate_read(1000u, 20u) == MODBUS_ACCESS_OK);
    assert(modbus_register_model_validate_read(2000u, 16u) == MODBUS_ACCESS_OK);
    assert(modbus_register_model_validate_read(20u, 2u) == MODBUS_ACCESS_ILLEGAL_ADDRESS);

    assert(modbus_register_model_validate_write(2008u, 1u) == MODBUS_ACCESS_OK);
    assert(modbus_register_model_validate_write(2009u, 1u) == MODBUS_ACCESS_OK);
    assert(modbus_register_model_validate_write(2008u, 2u) == MODBUS_ACCESS_OK);
    assert(modbus_register_model_validate_write(0u, 1u) == MODBUS_ACCESS_READ_ONLY);
    assert(modbus_register_model_validate_write(20u, 1u) == MODBUS_ACCESS_RESERVED);
    assert(modbus_register_model_validate_write(2007u, 2u) == MODBUS_ACCESS_READ_ONLY);
    assert(modbus_register_model_validate_write(2009u, 2u) == MODBUS_ACCESS_READ_ONLY);
    assert(modbus_register_model_validate_write(2014u, 1u) == MODBUS_ACCESS_RESERVED);
    assert(modbus_register_model_validate_write(21u, 1u) == MODBUS_ACCESS_ILLEGAL_ADDRESS);
    assert(modbus_register_model_validate_write(2008u, 0u) == MODBUS_ACCESS_ILLEGAL_ADDRESS);

    assert(modbus_register_is_writable(modbus_register_model_find(2008u)));
    assert(!modbus_register_is_writable(modbus_register_model_find(2007u)));
    assert(!modbus_register_is_writable(modbus_register_model_find(2014u)));

    return 0;
}
