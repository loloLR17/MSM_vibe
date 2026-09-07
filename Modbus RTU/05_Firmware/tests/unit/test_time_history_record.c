#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/time_history_record.h"

static uint32_t crc32_bytes(const uint8_t *bytes, size_t byte_count)
{
    uint32_t crc = UINT32_C(0xFFFFFFFF);
    size_t byte_index;

    for (byte_index = 0u; byte_index < byte_count; ++byte_index) {
        uint8_t bit_index;
        crc ^= (uint32_t)bytes[byte_index];

        for (bit_index = 0u; bit_index < 8u; ++bit_index) {
            if ((crc & UINT32_C(1)) != 0u) {
                crc = (crc >> 1u) ^ UINT32_C(0xEDB88320);
            } else {
                crc >>= 1u;
            }
        }
    }

    return crc ^ UINT32_C(0xFFFFFFFF);
}

static void put_u16_be(uint8_t *output, uint16_t value)
{
    output[0] = (uint8_t)(value >> 8u);
    output[1] = (uint8_t)(value & UINT16_C(0x00FF));
}

static void put_u32_be(uint8_t *output, uint32_t value)
{
    output[0] = (uint8_t)(value >> 24u);
    output[1] = (uint8_t)(value >> 16u);
    output[2] = (uint8_t)(value >> 8u);
    output[3] = (uint8_t)(value & UINT32_C(0x000000FF));
}

int main(void)
{
    LastSyncHistory history = time_last_sync_history_valid(UINT32_C(0x12345678), UINT16_C(3));
    LastSyncHistory decoded = time_last_sync_history_none();
    LastSyncHistory none = time_last_sync_history_none();
    uint8_t record[TR2_TIME_HISTORY_RECORD_SIZE];
    uint8_t modified[TR2_TIME_HISTORY_RECORD_SIZE];
    uint32_t crc;

    assert(tr2_time_history_record_encode(NULL, record, sizeof(record)) == TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_time_history_record_encode(&history, NULL, sizeof(record)) == TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_time_history_record_encode(&history, record, sizeof(record) - 1u) == TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_time_history_record_encode(&none, record, sizeof(record)) == TR2_ERROR_INVALID_STATE);

    assert(tr2_time_history_record_encode(&history, record, sizeof(record)) == TR2_OK);
    assert(record[0] == UINT8_C(0x54));
    assert(record[1] == UINT8_C(0x52));
    assert(record[2] == UINT8_C(0x32));
    assert(record[3] == UINT8_C(0x54));
    assert(record[4] == UINT8_C(0x00));
    assert(record[5] == UINT8_C(0x01));
    assert(record[6] == UINT8_C(0x00));
    assert(record[7] == UINT8_C(18));
    assert(record[8] == UINT8_C(0x12));
    assert(record[9] == UINT8_C(0x34));
    assert(record[10] == UINT8_C(0x56));
    assert(record[11] == UINT8_C(0x78));
    assert(record[12] == UINT8_C(0x00));
    assert(record[13] == UINT8_C(0x03));

    assert(tr2_time_history_record_decode(NULL, sizeof(record), &decoded) == TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_time_history_record_decode(record, sizeof(record), NULL) == TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_time_history_record_decode(record, sizeof(record) - 1u, &decoded) == TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_time_history_record_decode(record, sizeof(record), &decoded) == TR2_OK);
    assert(decoded.state == LAST_SYNC_HISTORY_VALID);
    assert(decoded.timestamp == UINT32_C(0x12345678));
    assert(decoded.source == UINT16_C(3));

    memcpy(modified, record, sizeof(modified));
    modified[10] ^= UINT8_C(0x01);
    assert(tr2_time_history_record_decode(modified, sizeof(modified), &decoded) == TR2_ERROR_CORRUPTED);

    memcpy(modified, record, sizeof(modified));
    modified[0] ^= UINT8_C(0x01);
    assert(tr2_time_history_record_decode(modified, sizeof(modified), &decoded) == TR2_ERROR_CORRUPTED);

    memcpy(modified, record, sizeof(modified));
    put_u16_be(&modified[6], UINT16_C(17));
    crc = crc32_bytes(modified, 14u);
    put_u32_be(&modified[14], crc);
    assert(tr2_time_history_record_decode(modified, sizeof(modified), &decoded) == TR2_ERROR_CORRUPTED);

    memcpy(modified, record, sizeof(modified));
    put_u16_be(&modified[4], UINT16_C(2));
    crc = crc32_bytes(modified, 14u);
    put_u32_be(&modified[14], crc);
    assert(tr2_time_history_record_decode(modified, sizeof(modified), &decoded) == TR2_ERROR_UNSUPPORTED);

    return 0;
}
