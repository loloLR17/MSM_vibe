#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "tr2/modbus/codec.h"

int main(void)
{
    uint16_t msw = 0u;
    uint16_t lsw = 0u;
    uint16_t ascii[3] = { UINT16_C(0xFFFF), UINT16_C(0xFFFF), UINT16_C(0xFFFF) };
    const uint8_t known_crc_payload[] = {
        UINT8_C(0x01), UINT8_C(0x03), UINT8_C(0x00),
        UINT8_C(0x00), UINT8_C(0x00), UINT8_C(0x0A)
    };
    const uint8_t pdu[] = {
        UINT8_C(0x03), UINT8_C(0x00), UINT8_C(0x00), UINT8_C(0x00), UINT8_C(0x0A)
    };
    const uint8_t expected_adu[] = {
        UINT8_C(0x01), UINT8_C(0x03), UINT8_C(0x00), UINT8_C(0x00),
        UINT8_C(0x00), UINT8_C(0x0A), UINT8_C(0xC5), UINT8_C(0xCD)
    };
    uint8_t adu[MODBUS_RTU_ADU_MAX_SIZE] = { 0u };
    uint8_t max_pdu[MODBUS_RTU_PDU_MAX_SIZE] = { 0u };
    uint16_t crc = 0u;
    size_t adu_length = 0u;
    ModbusRtuAduView view = { 0u, NULL, 0u };

    modbus_codec_u32_to_msw_lsw(UINT32_C(0x12345678), &msw, &lsw);
    assert(msw == UINT16_C(0x1234));
    assert(lsw == UINT16_C(0x5678));
    assert(modbus_codec_u32_from_msw_lsw(msw, lsw) == UINT32_C(0x12345678));

    assert(modbus_codec_i16_to_register(INT16_C(-50)) == UINT16_C(0xFFCE));
    assert(modbus_codec_i16_from_register(UINT16_C(0xFFCE)) == INT16_C(-50));
    assert(modbus_codec_i16_to_register(INT16_C(253)) == UINT16_C(0x00FD));

    assert(modbus_codec_ascii_fixed_encode("ABC", 3u, ascii, 3u));
    assert(ascii[0] == UINT16_C(0x4142));
    assert(ascii[1] == UINT16_C(0x4300));
    assert(ascii[2] == 0u);

    assert(!modbus_codec_ascii_fixed_encode("ABC", 3u, ascii, 1u));
    {
        const char non_ascii[1] = { (char)0x80 };
        assert(!modbus_codec_ascii_fixed_encode(non_ascii, 1u, ascii, 3u));
    }
    assert(!modbus_codec_ascii_fixed_encode(NULL, 0u, ascii, 3u));
    assert(!modbus_codec_ascii_fixed_encode("", 0u, NULL, 0u));

    assert(modbus_rtu_crc16(known_crc_payload, sizeof(known_crc_payload), &crc));
    assert(crc == UINT16_C(0xCDC5));
    assert(!modbus_rtu_crc16(NULL, sizeof(known_crc_payload), &crc));
    assert(!modbus_rtu_crc16(known_crc_payload, sizeof(known_crc_payload), NULL));

    assert(modbus_rtu_adu_encode(UINT8_C(0x01),
                                 pdu,
                                 sizeof(pdu),
                                 adu,
                                 sizeof(adu),
                                 &adu_length) == MODBUS_RTU_CODEC_OK);
    assert(adu_length == sizeof(expected_adu));
    assert(memcmp(adu, expected_adu, sizeof(expected_adu)) == 0);

    assert(modbus_rtu_adu_decode(adu, adu_length, &view) == MODBUS_RTU_CODEC_OK);
    assert(view.unit_id == UINT8_C(0x01));
    assert(view.pdu_length == sizeof(pdu));
    assert(memcmp(view.pdu, pdu, sizeof(pdu)) == 0);

    adu[adu_length - 1u] ^= UINT8_C(0x01);
    assert(modbus_rtu_adu_decode(adu, adu_length, &view) == MODBUS_RTU_CODEC_CRC_MISMATCH);
    adu[adu_length - 1u] ^= UINT8_C(0x01);

    max_pdu[0] = UINT8_C(0x03);
    assert(modbus_rtu_adu_encode(UINT8_C(0xF7),
                                 max_pdu,
                                 sizeof(max_pdu),
                                 adu,
                                 sizeof(adu),
                                 &adu_length) == MODBUS_RTU_CODEC_OK);
    assert(adu_length == MODBUS_RTU_ADU_MAX_SIZE);
    assert(modbus_rtu_adu_decode(adu, adu_length, &view) == MODBUS_RTU_CODEC_OK);
    assert(view.unit_id == UINT8_C(0xF7));
    assert(view.pdu_length == MODBUS_RTU_PDU_MAX_SIZE);

    assert(modbus_rtu_adu_encode(UINT8_C(0x01),
                                 pdu,
                                 0u,
                                 adu,
                                 sizeof(adu),
                                 &adu_length) == MODBUS_RTU_CODEC_INVALID_LENGTH);
    assert(modbus_rtu_adu_encode(UINT8_C(0x01),
                                 max_pdu,
                                 sizeof(max_pdu),
                                 adu,
                                 MODBUS_RTU_ADU_MAX_SIZE - 1u,
                                 &adu_length) == MODBUS_RTU_CODEC_OUTPUT_TOO_SMALL);
    assert(modbus_rtu_adu_encode(UINT8_C(0x01),
                                 NULL,
                                 sizeof(pdu),
                                 adu,
                                 sizeof(adu),
                                 &adu_length) == MODBUS_RTU_CODEC_INVALID_ARGUMENT);
    assert(modbus_rtu_adu_encode(UINT8_C(0x01),
                                 pdu,
                                 sizeof(pdu),
                                 NULL,
                                 sizeof(adu),
                                 &adu_length) == MODBUS_RTU_CODEC_INVALID_ARGUMENT);
    assert(modbus_rtu_adu_encode(UINT8_C(0x01),
                                 pdu,
                                 sizeof(pdu),
                                 adu,
                                 sizeof(adu),
                                 NULL) == MODBUS_RTU_CODEC_INVALID_ARGUMENT);

    assert(modbus_rtu_adu_decode(adu, MODBUS_RTU_ADU_MIN_SIZE - 1u, &view) ==
           MODBUS_RTU_CODEC_INVALID_LENGTH);
    assert(modbus_rtu_adu_decode(adu, MODBUS_RTU_ADU_MAX_SIZE + 1u, &view) ==
           MODBUS_RTU_CODEC_INVALID_LENGTH);
    assert(modbus_rtu_adu_decode(NULL, MODBUS_RTU_ADU_MIN_SIZE, &view) ==
           MODBUS_RTU_CODEC_INVALID_ARGUMENT);
    assert(modbus_rtu_adu_decode(adu, MODBUS_RTU_ADU_MIN_SIZE, NULL) ==
           MODBUS_RTU_CODEC_INVALID_ARGUMENT);

    return 0;
}
