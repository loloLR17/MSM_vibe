#ifndef TR2_MODBUS_CODEC_H
#define TR2_MODBUS_CODEC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void modbus_codec_u32_to_msw_lsw(uint32_t value, uint16_t *msw, uint16_t *lsw);
uint32_t modbus_codec_u32_from_msw_lsw(uint16_t msw, uint16_t lsw);
uint16_t modbus_codec_i16_to_register(int16_t value);
int16_t modbus_codec_i16_from_register(uint16_t value);
bool modbus_codec_ascii_fixed_encode(const char *input,
                                     size_t character_count,
                                     uint16_t *registers,
                                     size_t register_count);

#endif
