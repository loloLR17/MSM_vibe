#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/campaign_bulk_block.h"

static void fill(uint8_t *bytes, size_t size)
{
    size_t i;
    for (i = 0u; i < size; ++i) {
        bytes[i] = (uint8_t)((i * 29u + 7u) & UINT8_C(0xFF));
    }
}

static void test_round_trip_variable_payload(void)
{
    uint8_t payload[48];
    uint8_t encoded[128];
    size_t encoded_size = 0u;
    CampaignBulkBlockInfo info;
    const uint8_t *decoded = NULL;

    fill(payload, sizeof(payload));
    assert(campaign_bulk_block_encode(17u, 3u,
                                      payload, sizeof(payload),
                                      encoded, sizeof(encoded),
                                      &encoded_size) == TR2_OK);
    assert(encoded_size == TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE +
                               sizeof(payload) +
                               TR2_CAMPAIGN_BULK_BLOCK_TRAILER_SIZE);
    assert(campaign_bulk_block_decode(encoded, encoded_size,
                                      &info, &decoded) == TR2_OK);
    assert(info.campaign_id == 17u);
    assert(info.block_index == 3u);
    assert(info.payload_size == sizeof(payload));
    assert(memcmp(decoded, payload, sizeof(payload)) == 0);
}

static void test_payload_must_contain_complete_sample_records(void)
{
    uint8_t payload[17] = { 0 };
    uint8_t encoded[128];
    size_t encoded_size = 0u;

    assert(campaign_bulk_block_encode(1u, 0u,
                                      payload, sizeof(payload),
                                      encoded, sizeof(encoded),
                                      &encoded_size) ==
           TR2_ERROR_INVALID_ARGUMENT);
}

static void test_payload_corruption_is_detected(void)
{
    uint8_t payload[32];
    uint8_t encoded[128];
    size_t encoded_size = 0u;
    CampaignBulkBlockInfo info;
    const uint8_t *decoded = NULL;

    fill(payload, sizeof(payload));
    assert(campaign_bulk_block_encode(9u, 4u,
                                      payload, sizeof(payload),
                                      encoded, sizeof(encoded),
                                      &encoded_size) == TR2_OK);
    encoded[TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE + 5u] ^= UINT8_C(0x01);
    assert(campaign_bulk_block_decode(encoded, encoded_size,
                                      &info, &decoded) ==
           TR2_ERROR_CORRUPTED);
}

static void test_header_corruption_is_detected(void)
{
    uint8_t payload[16] = { 0 };
    uint8_t encoded[96];
    size_t encoded_size = 0u;
    CampaignBulkBlockInfo info;
    const uint8_t *decoded = NULL;

    assert(campaign_bulk_block_encode(5u, 0u,
                                      payload, sizeof(payload),
                                      encoded, sizeof(encoded),
                                      &encoded_size) == TR2_OK);
    encoded[12] ^= UINT8_C(0x01);
    assert(campaign_bulk_block_decode(encoded, encoded_size,
                                      &info, &decoded) ==
           TR2_ERROR_CORRUPTED);
}

static void test_unknown_version_is_unsupported(void)
{
    uint8_t payload[16] = { 0 };
    uint8_t encoded[96];
    size_t encoded_size = 0u;
    CampaignBulkBlockInfo info;
    const uint8_t *decoded = NULL;

    assert(campaign_bulk_block_encode(5u, 0u,
                                      payload, sizeof(payload),
                                      encoded, sizeof(encoded),
                                      &encoded_size) == TR2_OK);
    encoded[4] = 0u;
    encoded[5] = 2u;
    assert(campaign_bulk_block_decode(encoded, encoded_size,
                                      &info, &decoded) ==
           TR2_ERROR_UNSUPPORTED);
}

static void test_reserved_header_bytes_are_rejected(void)
{
    uint8_t payload[16] = { 0 };
    uint8_t encoded[96];
    size_t encoded_size = 0u;
    CampaignBulkBlockInfo info;
    const uint8_t *decoded = NULL;

    assert(campaign_bulk_block_encode(5u, 0u,
                                      payload, sizeof(payload),
                                      encoded, sizeof(encoded),
                                      &encoded_size) == TR2_OK);
    encoded[24] = 1u;
    assert(campaign_bulk_block_decode(encoded, encoded_size,
                                      &info, &decoded) ==
           TR2_ERROR_CORRUPTED);
}

static void test_truncated_and_extended_blocks_are_rejected(void)
{
    uint8_t payload[16] = { 0 };
    uint8_t encoded[97] = { 0 };
    size_t encoded_size = 0u;
    CampaignBulkBlockInfo info;
    const uint8_t *decoded = NULL;

    assert(campaign_bulk_block_encode(5u, 0u,
                                      payload, sizeof(payload),
                                      encoded, sizeof(encoded),
                                      &encoded_size) == TR2_OK);
    assert(campaign_bulk_block_decode(encoded, encoded_size - 1u,
                                      &info, &decoded) ==
           TR2_ERROR_CORRUPTED);
    assert(campaign_bulk_block_decode(encoded, encoded_size + 1u,
                                      &info, &decoded) ==
           TR2_ERROR_CORRUPTED);
}

static void test_output_capacity_is_checked(void)
{
    uint8_t payload[16] = { 0 };
    uint8_t encoded[51];
    size_t encoded_size = 0u;

    assert(campaign_bulk_block_encode(5u, 0u,
                                      payload, sizeof(payload),
                                      encoded, sizeof(encoded),
                                      &encoded_size) ==
           TR2_ERROR_NOT_AVAILABLE);
}

int main(void)
{
    test_round_trip_variable_payload();
    test_payload_must_contain_complete_sample_records();
    test_payload_corruption_is_detected();
    test_header_corruption_is_detected();
    test_unknown_version_is_unsupported();
    test_reserved_header_bytes_are_rejected();
    test_truncated_and_extended_blocks_are_rejected();
    test_output_capacity_is_checked();
    return 0;
}
