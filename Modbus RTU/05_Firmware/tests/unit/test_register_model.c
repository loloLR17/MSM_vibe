#include <assert.h>
#include <stdint.h>
#include "tr2/modbus/codec.h"
#include "tr2/modbus/register_model.h"

int main(void)
{
    ModbusRegisterDescriptor ro = { 0u, MODBUS_REGISTER_RO, 0u };
    ModbusRegisterDescriptor rw = { 1u, MODBUS_REGISTER_RW, 0u };
    ModbusRegisterDescriptor reserved = { 2u, MODBUS_REGISTER_RESERVED, 0u };
    uint16_t msw = 0u;
    uint16_t lsw = 0u;

    assert(modbus_register_descriptor_is_valid(&ro));
    assert(!modbus_register_is_writable(&ro));
    assert(modbus_register_is_writable(&rw));
    assert(!modbus_register_is_writable(&reserved));

    modbus_codec_u32_to_msw_lsw(UINT32_C(0x12345678), &msw, &lsw);
    assert(msw == UINT16_C(0x1234));
    assert(lsw == UINT16_C(0x5678));
    assert(modbus_codec_u32_from_msw_lsw(msw, lsw) == UINT32_C(0x12345678));

    return 0;
}
