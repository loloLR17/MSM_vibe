#ifndef TR2_MODBUS_CODEC_H
#define TR2_MODBUS_CODEC_H

#include <stdint.h>

void modbus_codec_u32_to_msw_lsw(uint32_t value, uint16_t *msw, uint16_t *lsw);
uint32_t modbus_codec_u32_from_msw_lsw(uint16_t msw, uint16_t lsw);

#endif
