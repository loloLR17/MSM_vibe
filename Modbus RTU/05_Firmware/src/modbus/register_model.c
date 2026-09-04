#include <stddef.h>
#include "tr2/modbus/register_model.h"

#define D(address_, block_, access_, kind_) \
    { (uint16_t)(address_), (block_), (access_), (kind_) }

static const ModbusRegisterDescriptor descriptors[] = {
    D(0, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT32_MSW),
    D(1, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT32_LSW),
    D(2, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT16),
    D(3, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT16),
    D(4, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT16),
    D(5, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT16),
    D(6, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT16),
    D(7, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_BITFIELD16),
    D(8, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_ASCII),
    D(9, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_ASCII),
    D(10, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_ASCII),
    D(11, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_ASCII),
    D(12, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_ASCII),
    D(13, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_ASCII),
    D(14, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_ASCII),
    D(15, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_ASCII),
    D(16, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_ASCII),
    D(17, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_ASCII),
    D(18, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_ASCII),
    D(19, MODBUS_BLOCK_0, MODBUS_REGISTER_RO, MODBUS_REGISTER_ASCII),
    D(20, MODBUS_BLOCK_0, MODBUS_REGISTER_RESERVED, MODBUS_REGISTER_UINT16),

    D(1000, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_ENUM16),
    D(1001, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_BITFIELD16),
    D(1002, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_BITFIELD16),
    D(1003, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_BITFIELD16),
    D(1004, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT32_MSW),
    D(1005, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT32_LSW),
    D(1006, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_ENUM16),
    D(1007, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_INT16),
    D(1008, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT16),
    D(1009, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT16),
    D(1010, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_ENUM16),
    D(1011, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT16),
    D(1012, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_ENUM16),
    D(1013, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT32_MSW),
    D(1014, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT32_LSW),
    D(1015, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT16),
    D(1016, MODBUS_BLOCK_1, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT16),
    D(1017, MODBUS_BLOCK_1, MODBUS_REGISTER_RESERVED, MODBUS_REGISTER_UINT16),
    D(1018, MODBUS_BLOCK_1, MODBUS_REGISTER_RESERVED, MODBUS_REGISTER_UINT16),
    D(1019, MODBUS_BLOCK_1, MODBUS_REGISTER_RESERVED, MODBUS_REGISTER_UINT16),

    D(2000, MODBUS_BLOCK_2, MODBUS_REGISTER_RO, MODBUS_REGISTER_ENUM16),
    D(2001, MODBUS_BLOCK_2, MODBUS_REGISTER_RO, MODBUS_REGISTER_BITFIELD16),
    D(2002, MODBUS_BLOCK_2, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT32_MSW),
    D(2003, MODBUS_BLOCK_2, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT32_LSW),
    D(2004, MODBUS_BLOCK_2, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT32_MSW),
    D(2005, MODBUS_BLOCK_2, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT32_LSW),
    D(2006, MODBUS_BLOCK_2, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT32_MSW),
    D(2007, MODBUS_BLOCK_2, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT32_LSW),
    D(2008, MODBUS_BLOCK_2, MODBUS_REGISTER_RW, MODBUS_REGISTER_UINT32_MSW),
    D(2009, MODBUS_BLOCK_2, MODBUS_REGISTER_RW, MODBUS_REGISTER_UINT32_LSW),
    D(2010, MODBUS_BLOCK_2, MODBUS_REGISTER_RO, MODBUS_REGISTER_ENUM16),
    D(2011, MODBUS_BLOCK_2, MODBUS_REGISTER_RO, MODBUS_REGISTER_UINT16),
    D(2012, MODBUS_BLOCK_2, MODBUS_REGISTER_RO, MODBUS_REGISTER_INT16),
    D(2013, MODBUS_BLOCK_2, MODBUS_REGISTER_RO, MODBUS_REGISTER_ENUM16),
    D(2014, MODBUS_BLOCK_2, MODBUS_REGISTER_RESERVED, MODBUS_REGISTER_UINT16),
    D(2015, MODBUS_BLOCK_2, MODBUS_REGISTER_RESERVED, MODBUS_REGISTER_UINT16)
};

#undef D

static size_t descriptor_count(void)
{
    return sizeof(descriptors) / sizeof(descriptors[0]);
}

bool modbus_register_descriptor_is_valid(const ModbusRegisterDescriptor *descriptor)
{
    if (descriptor == NULL) {
        return false;
    }

    if (descriptor->block < MODBUS_BLOCK_0 || descriptor->block > MODBUS_BLOCK_7) {
        return false;
    }

    if (descriptor->access < MODBUS_REGISTER_RO || descriptor->access > MODBUS_REGISTER_RESERVED) {
        return false;
    }

    return descriptor->kind >= MODBUS_REGISTER_UINT16 &&
           descriptor->kind <= MODBUS_REGISTER_ASCII;
}

bool modbus_register_is_writable(const ModbusRegisterDescriptor *descriptor)
{
    return modbus_register_descriptor_is_valid(descriptor) &&
           descriptor->access == MODBUS_REGISTER_RW;
}

const ModbusRegisterDescriptor *modbus_register_model_find(uint16_t address)
{
    size_t index;

    for (index = 0u; index < descriptor_count(); ++index) {
        if (descriptors[index].address == address) {
            return &descriptors[index];
        }
    }

    return NULL;
}

bool modbus_register_model_range_exists(uint16_t start_address, uint16_t quantity)
{
    uint32_t offset;

    if (quantity == 0u) {
        return false;
    }

    for (offset = 0u; offset < (uint32_t)quantity; ++offset) {
        const uint32_t address = (uint32_t)start_address + offset;
        if (address > UINT16_MAX || modbus_register_model_find((uint16_t)address) == NULL) {
            return false;
        }
    }

    return true;
}

ModbusAccessResult modbus_register_model_validate_read(uint16_t start_address, uint16_t quantity)
{
    return modbus_register_model_range_exists(start_address, quantity)
               ? MODBUS_ACCESS_OK
               : MODBUS_ACCESS_ILLEGAL_ADDRESS;
}

ModbusAccessResult modbus_register_model_validate_write(uint16_t start_address, uint16_t quantity)
{
    uint32_t offset;

    if (quantity == 0u) {
        return MODBUS_ACCESS_ILLEGAL_ADDRESS;
    }

    for (offset = 0u; offset < (uint32_t)quantity; ++offset) {
        const uint32_t address = (uint32_t)start_address + offset;
        const ModbusRegisterDescriptor *descriptor;

        if (address > UINT16_MAX) {
            return MODBUS_ACCESS_ILLEGAL_ADDRESS;
        }

        descriptor = modbus_register_model_find((uint16_t)address);
        if (descriptor == NULL) {
            return MODBUS_ACCESS_ILLEGAL_ADDRESS;
        }
        if (descriptor->access == MODBUS_REGISTER_RESERVED) {
            return MODBUS_ACCESS_RESERVED;
        }
        if (descriptor->access != MODBUS_REGISTER_RW) {
            return MODBUS_ACCESS_READ_ONLY;
        }
    }

    return MODBUS_ACCESS_OK;
}
