#include "tr2/persistence/diagnostic_history_record.h"

#include <string.h>

#define TR2_DIAGNOSTIC_HISTORY_RECORD_MAGIC UINT32_C(0x54524448)
#define TR2_DIAGNOSTIC_HISTORY_FLAG_TIMESTAMP_AVAILABLE UINT16_C(0x0001)
#define TR2_DIAGNOSTIC_HISTORY_RECORD_CRC_OFFSET 16u

_Static_assert(TR2_DIAGNOSTIC_HISTORY_RECORD_CRC_OFFSET + sizeof(uint32_t) ==
                   TR2_DIAGNOSTIC_HISTORY_RECORD_SIZE,
               "diagnostic history record size mismatch");

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

static uint16_t get_u16_be(const uint8_t *input)
{
    return (uint16_t)(((uint16_t)input[0] << 8u) | (uint16_t)input[1]);
}

static uint32_t get_u32_be(const uint8_t *input)
{
    return ((uint32_t)input[0] << 24u) |
           ((uint32_t)input[1] << 16u) |
           ((uint32_t)input[2] << 8u) |
           (uint32_t)input[3];
}

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

Tr2Result tr2_diagnostic_history_record_encode(const DiagnosticLastFault *last_fault,
                                               uint8_t *record,
                                               size_t record_size)
{
    uint16_t flags = 0u;
    uint32_t crc;

    if (last_fault == NULL || record == NULL ||
        record_size != TR2_DIAGNOSTIC_HISTORY_RECORD_SIZE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!last_fault->present) {
        return TR2_ERROR_INVALID_STATE;
    }

    if (last_fault->timestamp_available) {
        flags = TR2_DIAGNOSTIC_HISTORY_FLAG_TIMESTAMP_AVAILABLE;
    }

    memset(record, 0, record_size);
    put_u32_be(&record[0], TR2_DIAGNOSTIC_HISTORY_RECORD_MAGIC);
    put_u16_be(&record[4], TR2_DIAGNOSTIC_HISTORY_RECORD_FORMAT_VERSION);
    put_u16_be(&record[6], (uint16_t)TR2_DIAGNOSTIC_HISTORY_RECORD_SIZE);
    put_u16_be(&record[8], last_fault->code);
    put_u16_be(&record[10], flags);
    put_u32_be(&record[12], last_fault->timestamp_available ? last_fault->timestamp : 0u);

    crc = crc32_bytes(record, TR2_DIAGNOSTIC_HISTORY_RECORD_CRC_OFFSET);
    put_u32_be(&record[TR2_DIAGNOSTIC_HISTORY_RECORD_CRC_OFFSET], crc);
    return TR2_OK;
}

Tr2Result tr2_diagnostic_history_record_decode(const uint8_t *record,
                                               size_t record_size,
                                               DiagnosticLastFault *last_fault)
{
    uint16_t flags;
    uint32_t stored_crc;
    uint32_t computed_crc;

    if (record == NULL || last_fault == NULL ||
        record_size != TR2_DIAGNOSTIC_HISTORY_RECORD_SIZE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (get_u32_be(&record[0]) != TR2_DIAGNOSTIC_HISTORY_RECORD_MAGIC) {
        return TR2_ERROR_CORRUPTED;
    }
    if (get_u16_be(&record[6]) != (uint16_t)TR2_DIAGNOSTIC_HISTORY_RECORD_SIZE) {
        return TR2_ERROR_CORRUPTED;
    }

    stored_crc = get_u32_be(&record[TR2_DIAGNOSTIC_HISTORY_RECORD_CRC_OFFSET]);
    computed_crc = crc32_bytes(record, TR2_DIAGNOSTIC_HISTORY_RECORD_CRC_OFFSET);
    if (stored_crc != computed_crc) {
        return TR2_ERROR_CORRUPTED;
    }
    if (get_u16_be(&record[4]) != TR2_DIAGNOSTIC_HISTORY_RECORD_FORMAT_VERSION) {
        return TR2_ERROR_UNSUPPORTED;
    }

    flags = get_u16_be(&record[10]);
    if ((flags & (uint16_t)~TR2_DIAGNOSTIC_HISTORY_FLAG_TIMESTAMP_AVAILABLE) != 0u) {
        return TR2_ERROR_CORRUPTED;
    }

    memset(last_fault, 0, sizeof(*last_fault));
    last_fault->present = true;
    last_fault->code = get_u16_be(&record[8]);
    last_fault->timestamp_available =
        (flags & TR2_DIAGNOSTIC_HISTORY_FLAG_TIMESTAMP_AVAILABLE) != 0u;
    if (last_fault->timestamp_available) {
        last_fault->timestamp = get_u32_be(&record[12]);
    }
    return TR2_OK;
}

#undef TR2_DIAGNOSTIC_HISTORY_RECORD_MAGIC
#undef TR2_DIAGNOSTIC_HISTORY_FLAG_TIMESTAMP_AVAILABLE
#undef TR2_DIAGNOSTIC_HISTORY_RECORD_CRC_OFFSET
