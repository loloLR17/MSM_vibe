#include "tr2/persistence/campaign_bulk_block.h"

#include <limits.h>
#include <string.h>

#define CRC_SIZE TR2_CAMPAIGN_BULK_BLOCK_TRAILER_SIZE

static void put_u16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v >> 8u);
    p[1] = (uint8_t)v;
}

static void put_u32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24u);
    p[1] = (uint8_t)(v >> 16u);
    p[2] = (uint8_t)(v >> 8u);
    p[3] = (uint8_t)v;
}

static void put_u64(uint8_t *p, uint64_t v)
{
    put_u32(p, (uint32_t)(v >> 32u));
    put_u32(p + 4u, (uint32_t)v);
}

static uint16_t get_u16(const uint8_t *p)
{
    return (uint16_t)(((uint16_t)p[0] << 8u) | p[1]);
}

static uint32_t get_u32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24u) |
           ((uint32_t)p[1] << 16u) |
           ((uint32_t)p[2] << 8u) |
           p[3];
}

static uint64_t get_u64(const uint8_t *p)
{
    return ((uint64_t)get_u32(p) << 32u) | get_u32(p + 4u);
}

static uint32_t crc32_bytes(const uint8_t *p, size_t n)
{
    uint32_t crc = UINT32_C(0xFFFFFFFF);
    size_t i;

    for (i = 0u; i < n; ++i) {
        uint8_t bit;
        crc ^= p[i];
        for (bit = 0u; bit < 8u; ++bit) {
            crc = (crc & 1u) != 0u
                      ? (crc >> 1u) ^ UINT32_C(0xEDB88320)
                      : crc >> 1u;
        }
    }
    return crc ^ UINT32_C(0xFFFFFFFF);
}

Tr2Result campaign_bulk_block_encoded_size(size_t payload_size,
                                           size_t *encoded_size)
{
    if (encoded_size == NULL ||
        payload_size == 0u ||
        payload_size % TR2_CAMPAIGN_BULK_SAMPLE_RECORD_SIZE != 0u ||
        payload_size > UINT32_MAX ||
        payload_size > SIZE_MAX - TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE -
                           TR2_CAMPAIGN_BULK_BLOCK_TRAILER_SIZE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    *encoded_size = TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE +
                    payload_size +
                    TR2_CAMPAIGN_BULK_BLOCK_TRAILER_SIZE;
    return TR2_OK;
}

Tr2Result campaign_bulk_block_encode(
    CampaignId campaign_id,
    uint64_t block_index,
    const uint8_t *payload,
    size_t payload_size,
    uint8_t *output,
    size_t output_capacity,
    size_t *encoded_size)
{
    size_t required;
    uint32_t crc;
    Tr2Result result;

    if (campaign_id == TR2_CAMPAIGN_ID_INVALID ||
        block_index == UINT64_MAX ||
        payload == NULL || output == NULL || encoded_size == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = campaign_bulk_block_encoded_size(payload_size, &required);
    if (result != TR2_OK) {
        return result;
    }
    if (output_capacity < required) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    memset(output, 0, TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE);
    put_u32(&output[0], TR2_CAMPAIGN_BULK_BLOCK_MAGIC);
    put_u16(&output[4], TR2_CAMPAIGN_BULK_BLOCK_VERSION);
    put_u16(&output[6], (uint16_t)TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE);
    put_u32(&output[8], campaign_id);
    put_u64(&output[12], block_index);
    put_u32(&output[20], (uint32_t)payload_size);
    memcpy(&output[TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE],
           payload,
           payload_size);

    crc = crc32_bytes(output,
                      TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE + payload_size);
    put_u32(&output[TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE + payload_size], crc);
    *encoded_size = required;
    return TR2_OK;
}

Tr2Result campaign_bulk_block_decode(
    const uint8_t *block,
    size_t block_size,
    CampaignBulkBlockInfo *info,
    const uint8_t **payload)
{
    uint32_t payload_size;
    size_t required;
    size_t i;
    Tr2Result result;

    if (block == NULL || info == NULL || payload == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memset(info, 0, sizeof(*info));
    *payload = NULL;

    if (block_size < TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE +
                         TR2_CAMPAIGN_BULK_BLOCK_TRAILER_SIZE) {
        return TR2_ERROR_CORRUPTED;
    }
    if (get_u32(&block[0]) != TR2_CAMPAIGN_BULK_BLOCK_MAGIC ||
        get_u16(&block[6]) != TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE) {
        return TR2_ERROR_CORRUPTED;
    }
    if (get_u16(&block[4]) != TR2_CAMPAIGN_BULK_BLOCK_VERSION) {
        return TR2_ERROR_UNSUPPORTED;
    }
    for (i = 24u; i < TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE; ++i) {
        if (block[i] != 0u) {
            return TR2_ERROR_CORRUPTED;
        }
    }

    payload_size = get_u32(&block[20]);
    result = campaign_bulk_block_encoded_size((size_t)payload_size, &required);
    if (result != TR2_OK || required != block_size) {
        return TR2_ERROR_CORRUPTED;
    }
    if (get_u32(&block[TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE + payload_size]) !=
        crc32_bytes(block,
                    TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE + payload_size)) {
        return TR2_ERROR_CORRUPTED;
    }

    info->campaign_id = get_u32(&block[8]);
    info->block_index = get_u64(&block[12]);
    info->payload_size = payload_size;
    if (info->campaign_id == TR2_CAMPAIGN_ID_INVALID ||
        info->block_index == UINT64_MAX) {
        memset(info, 0, sizeof(*info));
        return TR2_ERROR_CORRUPTED;
    }

    *payload = &block[TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE];
    return TR2_OK;
}
