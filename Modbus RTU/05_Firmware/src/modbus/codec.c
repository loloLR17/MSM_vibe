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

bool modbus_rtu_crc16(const uint8_t *data, size_t length, uint16_t *crc)
{
    uint16_t value = UINT16_C(0xFFFF);
    size_t index;

    if (data == NULL || crc == NULL) {
        return false;
    }

    for (index = 0u; index < length; ++index) {
        uint8_t bit;

        value ^= (uint16_t)data[index];
        for (bit = 0u; bit < 8u; ++bit) {
            if ((value & UINT16_C(0x0001)) != 0u) {
                value = (uint16_t)((value >> 1u) ^ UINT16_C(0xA001));
            } else {
                value >>= 1u;
            }
        }
    }

    *crc = value;
    return true;
}

ModbusRtuCodecResult modbus_rtu_adu_encode(uint8_t unit_id,
                                           const uint8_t *pdu,
                                           size_t pdu_length,
                                           uint8_t *adu,
                                           size_t adu_capacity,
                                           size_t *adu_length)
{
    size_t index;
    size_t required_length;
    uint16_t crc;

    if (pdu == NULL || adu == NULL || adu_length == NULL) {
        return MODBUS_RTU_CODEC_INVALID_ARGUMENT;
    }

    if (pdu_length == 0u || pdu_length > MODBUS_RTU_PDU_MAX_SIZE) {
        return MODBUS_RTU_CODEC_INVALID_LENGTH;
    }

    required_length = 1u + pdu_length + 2u;
    if (adu_capacity < required_length) {
        return MODBUS_RTU_CODEC_OUTPUT_TOO_SMALL;
    }

    adu[0] = unit_id;
    for (index = 0u; index < pdu_length; ++index) {
        adu[index + 1u] = pdu[index];
    }

    if (!modbus_rtu_crc16(adu, 1u + pdu_length, &crc)) {
        return MODBUS_RTU_CODEC_INVALID_ARGUMENT;
    }

    adu[1u + pdu_length] = (uint8_t)(crc & UINT16_C(0x00FF));
    adu[2u + pdu_length] = (uint8_t)(crc >> 8u);
    *adu_length = required_length;

    return MODBUS_RTU_CODEC_OK;
}

ModbusRtuCodecResult modbus_rtu_adu_decode(const uint8_t *adu,
                                           size_t adu_length,
                                           ModbusRtuAduView *view)
{
    uint16_t expected_crc;
    uint16_t received_crc;

    if (adu == NULL || view == NULL) {
        return MODBUS_RTU_CODEC_INVALID_ARGUMENT;
    }

    if (adu_length < MODBUS_RTU_ADU_MIN_SIZE || adu_length > MODBUS_RTU_ADU_MAX_SIZE) {
        return MODBUS_RTU_CODEC_INVALID_LENGTH;
    }

    if (!modbus_rtu_crc16(adu, adu_length - 2u, &expected_crc)) {
        return MODBUS_RTU_CODEC_INVALID_ARGUMENT;
    }

    received_crc = (uint16_t)adu[adu_length - 2u] |
                   (uint16_t)((uint16_t)adu[adu_length - 1u] << 8u);
    if (expected_crc != received_crc) {
        return MODBUS_RTU_CODEC_CRC_MISMATCH;
    }

    view->unit_id = adu[0];
    view->pdu = &adu[1];
    view->pdu_length = adu_length - 3u;

    return MODBUS_RTU_CODEC_OK;
}
