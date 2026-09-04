#include <assert.h>
#include <stdint.h>
#include "tr2/modbus/codec.h"

int main(void)
{
    uint16_t msw = 0u;
    uint16_t lsw = 0u;
    uint16_t ascii[3] = { UINT16_C(0xFFFF), UINT16_C(0xFFFF), UINT16_C(0xFFFF) };

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

    return 0;
}
