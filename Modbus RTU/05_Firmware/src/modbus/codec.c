#include <stddef.h>
#include "tr2/modbus/codec.h"

void modbus_codec_u32_to_msw_lsw(uint32_t value, uint16_t *msw, uint16_t *lsw)
{
    if (msw != NULL) {
        *msw = (uint16_t)(value >> 16);
    }
    if (lsw != NULL) {
        *lsw = (uint16_t)(value & 0xFFFFu);
    }
}

uint32_t modbus_codec_u32_from_msw_lsw(uint16_t msw, uint16_t lsw)
{
    return ((uint32_t)msw << 16) | (uint32_t)lsw;
}

uint16_t modbus_codec_i16_to_register(int16_t value)
{
    return (uint16_t)value;
}

int16_t modbus_codec_i16_from_register(uint16_t value)
{
    if (value <= (uint16_t)INT16_MAX) {
        return (int16_t)value;
    }

    return (int16_t)(-1 - (int32_t)(UINT16_MAX - value));
}

bool modbus_codec_ascii_fixed_encode(const char *input,
                                     size_t character_count,
                                     uint16_t *registers,
                                     size_t register_count)
{
    size_t index;
    size_t required_registers;

    if (input == NULL || registers == NULL) {
        return false;
    }

    required_registers = (character_count + 1u) / 2u;
    if (register_count < required_registers) {
        return false;
    }

    for (index = 0u; index < character_count; ++index) {
        if ((uint8_t)input[index] > UINT8_C(0x7F)) {
            return false;
        }
    }

    for (index = 0u; index < register_count; ++index) {
        registers[index] = 0u;
    }

    for (index = 0u; index < character_count; index += 2u) {
        const uint8_t high = (uint8_t)input[index];
        const uint8_t low = (index + 1u < character_count) ? (uint8_t)input[index + 1u] : 0u;
        registers[index / 2u] = (uint16_t)(((uint16_t)high << 8) | (uint16_t)low);
    }

    return true;
}
