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

    assert_descriptor(4000u, MODBUS_BLOCK_4, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT16);
    assert_descriptor(4002u, MODBUS_BLOCK_4, MODBUS_REGISTER_RW, MODBUS_REGISTER_UINT32_MSW);
    assert_descriptor(4003u, MODBUS_BLOCK_4, MODBUS_REGISTER_RW, MODBUS_REGISTER_UINT32_LSW);
    assert_descriptor(4008u, MODBUS_BLOCK_4, MODBUS_REGISTER_RW, MODBUS_REGISTER_UINT32_MSW);
    assert_descriptor(4014u, MODBUS_BLOCK_4, MODBUS_REGISTER_RESERVED, MODBUS_REGISTER_UINT16);
    assert_descriptor(4016u, MODBUS_BLOCK_4, MODBUS_REGISTER_RW, MODBUS_REGISTER_UINT16);
    assert_descriptor(4017u, MODBUS_BLOCK_4, MODBUS_REGISTER_RESERVED, MODBUS_REGISTER_UINT16);
    assert_descriptor(4018u, MODBUS_BLOCK_4, MODBUS_REGISTER_RW, MODBUS_REGISTER_BITFIELD16);
    assert_descriptor(4023u, MODBUS_BLOCK_4, MODBUS_REGISTER_RW, MODBUS_REGISTER_UINT32_MSW);
    assert_descriptor(4040u, MODBUS_BLOCK_4, MODBUS_REGISTER_RW, MODBUS_REGISTER_BITFIELD16);
    assert_descriptor(4056u, MODBUS_BLOCK_4, MODBUS_REGISTER_RW, MODBUS_REGISTER_UINT32_MSW);
    assert_descriptor(4060u, MODBUS_BLOCK_4, MODBUS_REGISTER_RW, MODBUS_REGISTER_ASCII);
    assert_descriptor(4096u, MODBUS_BLOCK_4, MODBUS_REGISTER_RESERVED, MODBUS_REGISTER_UINT16);
    assert_descriptor(4100u, MODBUS_BLOCK_4, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT16);
    assert_descriptor(4111u, MODBUS_BLOCK_4, MODBUS_REGISTER_RESERVED, MODBUS_REGISTER_UINT16);
    assert_descriptor(4132u, MODBUS_BLOCK_4, MODBUS_REGISTER_RO, MODBUS_REGISTER_ASCII);
    assert_descriptor(4175u, MODBUS_BLOCK_4, MODBUS_REGISTER_RESERVED, MODBUS_REGISTER_UINT16);

    assert(modbus_register_model_find(21u) == NULL);
    assert(modbus_register_model_find(999u) == NULL);
    assert(modbus_register_model_find(1020u) == NULL);
    assert(modbus_register_model_find(1999u) == NULL);
    assert(modbus_register_model_find(2016u) == NULL);
    assert(modbus_register_model_find(3999u) == NULL);
    assert(modbus_register_model_find(4176u) == NULL);

    assert(modbus_register_model_range_exists(0u, 21u));
    assert(modbus_register_model_range_exists(1000u, 20u));
    assert(modbus_register_model_range_exists(2000u, 16u));
    assert(modbus_register_model_range_exists(4000u, 176u));
    assert(!modbus_register_model_range_exists(0u, 22u));
    assert(!modbus_register_model_range_exists(20u, 2u));
    assert(!modbus_register_model_range_exists(4175u, 2u));
    assert(!modbus_register_model_range_exists(0u, 0u));
    assert(!modbus_register_model_range_exists(UINT16_MAX, 2u));

    assert(modbus_register_model_validate_read(0u, 21u) == MODBUS_ACCESS_OK);
    assert(modbus_register_model_validate_read(1000u, 20u) == MODBUS_ACCESS_OK);
    assert(modbus_register_model_validate_read(2000u, 16u) == MODBUS_ACCESS_OK);
    assert(modbus_register_model_validate_read(4000u, 176u) == MODBUS_ACCESS_OK);
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

    assert(modbus_register_model_validate_write(4002u, 2u) == MODBUS_ACCESS_OK);
    assert(modbus_register_model_validate_write(4008u, 2u) == MODBUS_ACCESS_OK);
    assert(modbus_register_model_validate_write(4016u, 1u) == MODBUS_ACCESS_OK);
    assert(modbus_register_model_validate_write(4018u, 10u) == MODBUS_ACCESS_OK);
    assert(modbus_register_model_validate_write(4040u, 7u) == MODBUS_ACCESS_OK);
    assert(modbus_register_model_validate_write(4056u, 40u) == MODBUS_ACCESS_OK);
    assert(modbus_register_model_validate_write(4000u, 1u) == MODBUS_ACCESS_READ_ONLY);
    assert(modbus_register_model_validate_write(4017u, 1u) == MODBUS_ACCESS_RESERVED);
    assert(modbus_register_model_validate_write(4016u, 2u) == MODBUS_ACCESS_RESERVED);
    assert(modbus_register_model_validate_write(4096u, 1u) == MODBUS_ACCESS_RESERVED);
    assert(modbus_register_model_validate_write(4100u, 1u) == MODBUS_ACCESS_READ_ONLY);
    assert(modbus_register_model_validate_write(4175u, 1u) == MODBUS_ACCESS_RESERVED);
    assert(modbus_register_model_validate_write(4176u, 1u) == MODBUS_ACCESS_ILLEGAL_ADDRESS);

    assert(modbus_register_is_writable(modbus_register_model_find(2008u)));
    assert(!modbus_register_is_writable(modbus_register_model_find(2007u)));
    assert(!modbus_register_is_writable(modbus_register_model_find(2014u)));
    assert(modbus_register_is_writable(modbus_register_model_find(4002u)));
    assert(modbus_register_is_writable(modbus_register_model_find(4060u)));
    assert(!modbus_register_is_writable(modbus_register_model_find(4100u)));
    assert(!modbus_register_is_writable(modbus_register_model_find(4168u)));

    return 0;
}
