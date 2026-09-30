#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/campaign_bulk_aggregator.h"

typedef struct {
    uint8_t bytes[128];
    uint32_t write_calls;
    uint64_t write_offsets[8];
    size_t write_sizes[8];
    uint32_t sync_calls;
    uint32_t fail_write_call;
} FakeMedia;

static Tr2Result fake_capacity(void *context, uint64_t *capacity)
{
    FakeMedia *fake = context;
    *capacity = sizeof(fake->bytes);
    return TR2_OK;
}

static Tr2Result fake_read(void *context,
                           uint64_t offset,
                           void *buffer,
                           size_t size)
{
    FakeMedia *fake = context;
    if (offset > sizeof(fake->bytes) ||
        size > sizeof(fake->bytes) - (size_t)offset) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(buffer, &fake->bytes[(size_t)offset], size);
    return TR2_OK;
}

static Tr2Result fake_write(void *context,
                            uint64_t offset,
                            const void *buffer,
                            size_t size)
{
    FakeMedia *fake = context;
    uint32_t call = fake->write_calls;

    fake->write_calls += 1u;
    if (call < 8u) {
        fake->write_offsets[call] = offset;
        fake->write_sizes[call] = size;
    }
    if (fake->fail_write_call != 0u &&
        fake->write_calls == fake->fail_write_call) {
        return TR2_ERROR_STORAGE;
    }
    if (offset > sizeof(fake->bytes) ||
        size > sizeof(fake->bytes) - (size_t)offset) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(&fake->bytes[(size_t)offset], buffer, size);
    return TR2_OK;
}

static Tr2Result fake_sync(void *context)
{
    FakeMedia *fake = context;
    fake->sync_calls += 1u;
    return TR2_OK;
}

static CampaignBulkMedia make_media(FakeMedia *fake)
{
    CampaignBulkMedia media;
    media.context = fake;
    media.capacity_bytes = fake_capacity;
    media.read = fake_read;
    media.write = fake_write;
    media.sync = fake_sync;
    return media;
}

static void fill_sequence(uint8_t *data, size_t size)
{
    size_t i;
    for (i = 0u; i < size; ++i) {
        data[i] = (uint8_t)(i + 1u);
    }
}

static void test_aggregates_small_appends_into_full_writes(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    CampaignBulkAggregator aggregator;
    uint8_t buffer[32];
    uint8_t input[80];

    fill_sequence(input, sizeof(input));
    assert(campaign_bulk_aggregator_init(&aggregator,
                                         &media,
                                         buffer,
                                         sizeof(buffer),
                                         8u) == TR2_OK);

    assert(campaign_bulk_aggregator_append(&aggregator, input, 16u) == TR2_OK);
    assert(fake.write_calls == 0u);
    assert(campaign_bulk_aggregator_append(&aggregator,
                                           &input[16],
                                           16u) == TR2_OK);
    assert(fake.write_calls == 1u);
    assert(fake.write_offsets[0] == 8u);
    assert(fake.write_sizes[0] == 32u);

    assert(campaign_bulk_aggregator_append(&aggregator,
                                           &input[32],
                                           48u) == TR2_OK);
    assert(fake.write_calls == 2u);
    assert(fake.write_offsets[1] == 40u);
    assert(fake.write_sizes[1] == 32u);
    assert(campaign_bulk_aggregator_buffered_bytes(&aggregator) == 16u);
    assert(campaign_bulk_aggregator_write_offset(&aggregator) == 72u);

    assert(memcmp(&fake.bytes[8], input, 64u) == 0);
    assert(fake.sync_calls == 0u);
}

static void test_flush_writes_only_the_tail_and_never_syncs(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    CampaignBulkAggregator aggregator;
    uint8_t buffer[32];
    uint8_t input[17];

    fill_sequence(input, sizeof(input));
    assert(campaign_bulk_aggregator_init(&aggregator,
                                         &media,
                                         buffer,
                                         sizeof(buffer),
                                         0u) == TR2_OK);
    assert(campaign_bulk_aggregator_append(&aggregator,
                                           input,
                                           sizeof(input)) == TR2_OK);
    assert(campaign_bulk_aggregator_flush(&aggregator) == TR2_OK);
    assert(fake.write_calls == 1u);
    assert(fake.write_sizes[0] == sizeof(input));
    assert(fake.sync_calls == 0u);
    assert(campaign_bulk_aggregator_buffered_bytes(&aggregator) == 0u);
    assert(campaign_bulk_aggregator_write_offset(&aggregator) == sizeof(input));
    assert(memcmp(fake.bytes, input, sizeof(input)) == 0);
}

static void test_failed_write_faults_aggregator_without_advancing_offset(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    CampaignBulkAggregator aggregator;
    uint8_t buffer[16];
    uint8_t input[16];

    fill_sequence(input, sizeof(input));
    fake.fail_write_call = 1u;
    assert(campaign_bulk_aggregator_init(&aggregator,
                                         &media,
                                         buffer,
                                         sizeof(buffer),
                                         4u) == TR2_OK);
    assert(campaign_bulk_aggregator_append(&aggregator,
                                           input,
                                           sizeof(input)) ==
           TR2_ERROR_STORAGE);
    assert(campaign_bulk_aggregator_is_faulted(&aggregator));
    assert(campaign_bulk_aggregator_write_offset(&aggregator) == 4u);
    assert(campaign_bulk_aggregator_buffered_bytes(&aggregator) ==
           sizeof(buffer));
    assert(campaign_bulk_aggregator_flush(&aggregator) ==
           TR2_ERROR_INVALID_STATE);
}

static void test_init_rejects_impossible_initial_extent(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    CampaignBulkAggregator aggregator;
    uint8_t buffer[32];

    assert(campaign_bulk_aggregator_init(&aggregator,
                                         &media,
                                         buffer,
                                         sizeof(buffer),
                                         100u) ==
           TR2_ERROR_NOT_AVAILABLE);
}

int main(void)
{
    test_aggregates_small_appends_into_full_writes();
    test_flush_writes_only_the_tail_and_never_syncs();
    test_failed_write_faults_aggregator_without_advancing_offset();
    test_init_rejects_impossible_initial_extent();
    return 0;
}
