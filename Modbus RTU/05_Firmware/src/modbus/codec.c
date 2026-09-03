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
