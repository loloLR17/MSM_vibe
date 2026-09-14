#ifndef TR2_MODBUS_CODEC_H
#define TR2_MODBUS_CODEC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MODBUS_RTU_ADU_MIN_SIZE ((size_t)4u)
#define MODBUS_RTU_ADU_MAX_SIZE ((size_t)256u)
#define MODBUS_RTU_PDU_MAX_SIZE ((size_t)253u)

typedef enum {
    MODBUS_RTU_CODEC_OK = 0,
    MODBUS_RTU_CODEC_INVALID_ARGUMENT,
    MODBUS_RTU_CODEC_INVALID_LENGTH,
    MODBUS_RTU_CODEC_OUTPUT_TOO_SMALL,
    MODBUS_RTU_CODEC_CRC_MISMATCH
} ModbusRtuCodecResult;

typedef struct {
    uint8_t unit_id;
    const uint8_t *pdu;
    size_t pdu_length;
} ModbusRtuAduView;

void modbus_codec_u32_to_msw_lsw(uint32_t value, uint16_t *msw, uint16_t *lsw);
uint32_t modbus_codec_u32_from_msw_lsw(uint16_t msw, uint16_t lsw);
uint16_t modbus_codec_i16_to_register(int16_t value);
int16_t modbus_codec_i16_from_register(uint16_t value);
bool modbus_codec_ascii_fixed_encode(const char *input,
                                     size_t character_count,
                                     uint16_t *registers,
                                     size_t register_count);

bool modbus_rtu_crc16(const uint8_t *data, size_t length, uint16_t *crc);
ModbusRtuCodecResult modbus_rtu_adu_encode(uint8_t unit_id,
                                           const uint8_t *pdu,
                                           size_t pdu_length,
                                           uint8_t *adu,
                                           size_t adu_capacity,
                                           size_t *adu_length);
ModbusRtuCodecResult modbus_rtu_adu_decode(const uint8_t *adu,
                                           size_t adu_length,
                                           ModbusRtuAduView *view);

#endif
