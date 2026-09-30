#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/campaign_bulk_checkpoint.h"

typedef struct {
    uint8_t bytes[256];
    uint32_t write_calls;
    uint32_t sync_calls;
    uint32_t fail_write_call;
    uint32_t fail_sync_call;
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
    fake->write_calls += 1u;
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
    if (fake->fail_sync_call != 0u &&
        fake->sync_calls == fake->fail_sync_call) {
        return TR2_ERROR_STORAGE;
    }
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

static void fill(uint8_t *data, size_t size, uint8_t seed)
{
    size_t i;
    for (i = 0u; i < size; ++i) {
        data[i] = (uint8_t)(seed + (uint8_t)i);
    }
}

static void test_successful_checkpoint_flushes_syncs_and_publishes(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    CampaignBulkAggregator aggregator;
    CampaignBulkCheckpoint checkpoint;
    uint8_t buffer[32];
    uint8_t data[17];

    fill(data, sizeof(data), 3u);
    assert(campaign_bulk_aggregator_init(&aggregator,
                                         &media,
                                         buffer,
                                         sizeof(buffer),
                                         16u) == TR2_OK);
    assert(campaign_bulk_checkpoint_init(&checkpoint, &aggregator) == TR2_OK);
    assert(campaign_bulk_aggregator_append(&aggregator,
                                           data,
                                           sizeof(data)) == TR2_OK);

    assert(campaign_bulk_checkpoint_written_prefix_bytes(&checkpoint) == 0u);
    assert(campaign_bulk_checkpoint_durable_prefix_bytes(&checkpoint) == 0u);

    assert(campaign_bulk_checkpoint_publish(&checkpoint) == TR2_OK);
    assert(fake.write_calls == 1u);
    assert(fake.sync_calls == 1u);
    assert(campaign_bulk_checkpoint_written_prefix_bytes(&checkpoint) ==
           sizeof(data));
    assert(campaign_bulk_checkpoint_durable_prefix_bytes(&checkpoint) ==
           sizeof(data));
}

static void test_sync_failure_never_advances_durable_prefix(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    CampaignBulkAggregator aggregator;
    CampaignBulkCheckpoint checkpoint;
    uint8_t buffer[32];
    uint8_t first[16];
    uint8_t second[7];

    fill(first, sizeof(first), 10u);
    fill(second, sizeof(second), 40u);

    assert(campaign_bulk_aggregator_init(&aggregator,
                                         &media,
                                         buffer,
                                         sizeof(buffer),
                                         0u) == TR2_OK);
    assert(campaign_bulk_checkpoint_init(&checkpoint, &aggregator) == TR2_OK);
    assert(campaign_bulk_aggregator_append(&aggregator,
                                           first,
                                           sizeof(first)) == TR2_OK);
    assert(campaign_bulk_checkpoint_publish(&checkpoint) == TR2_OK);
    assert(campaign_bulk_checkpoint_durable_prefix_bytes(&checkpoint) ==
           sizeof(first));

    assert(campaign_bulk_aggregator_append(&aggregator,
                                           second,
                                           sizeof(second)) == TR2_OK);
    fake.fail_sync_call = 2u;
    assert(campaign_bulk_checkpoint_publish(&checkpoint) == TR2_ERROR_STORAGE);

    assert(campaign_bulk_checkpoint_written_prefix_bytes(&checkpoint) ==
           sizeof(first) + sizeof(second));
    assert(campaign_bulk_checkpoint_durable_prefix_bytes(&checkpoint) ==
           sizeof(first));
    assert(campaign_bulk_checkpoint_is_faulted(&checkpoint));
}

static void test_flush_failure_never_syncs_or_advances_durable_prefix(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    CampaignBulkAggregator aggregator;
    CampaignBulkCheckpoint checkpoint;
    uint8_t buffer[32];
    uint8_t data[9];

    fill(data, sizeof(data), 7u);
    assert(campaign_bulk_aggregator_init(&aggregator,
                                         &media,
                                         buffer,
                                         sizeof(buffer),
                                         0u) == TR2_OK);
    assert(campaign_bulk_checkpoint_init(&checkpoint, &aggregator) == TR2_OK);
    assert(campaign_bulk_aggregator_append(&aggregator,
                                           data,
                                           sizeof(data)) == TR2_OK);

    fake.fail_write_call = 1u;
    assert(campaign_bulk_checkpoint_publish(&checkpoint) == TR2_ERROR_STORAGE);
    assert(fake.sync_calls == 0u);
    assert(campaign_bulk_checkpoint_written_prefix_bytes(&checkpoint) == 0u);
    assert(campaign_bulk_checkpoint_durable_prefix_bytes(&checkpoint) == 0u);
    assert(campaign_bulk_checkpoint_is_faulted(&checkpoint));
}

static void test_full_buffer_write_is_not_durable_before_checkpoint(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    CampaignBulkAggregator aggregator;
    CampaignBulkCheckpoint checkpoint;
    uint8_t buffer[16];
    uint8_t data[16];

    fill(data, sizeof(data), 1u);
    assert(campaign_bulk_aggregator_init(&aggregator,
                                         &media,
                                         buffer,
                                         sizeof(buffer),
                                         32u) == TR2_OK);
    assert(campaign_bulk_checkpoint_init(&checkpoint, &aggregator) == TR2_OK);
    assert(campaign_bulk_aggregator_append(&aggregator,
                                           data,
                                           sizeof(data)) == TR2_OK);

    assert(fake.write_calls == 1u);
    assert(fake.sync_calls == 0u);
    assert(campaign_bulk_checkpoint_written_prefix_bytes(&checkpoint) ==
           sizeof(data));
    assert(campaign_bulk_checkpoint_durable_prefix_bytes(&checkpoint) == 0u);

    assert(campaign_bulk_checkpoint_publish(&checkpoint) == TR2_OK);
    assert(fake.write_calls == 1u);
    assert(fake.sync_calls == 1u);
    assert(campaign_bulk_checkpoint_durable_prefix_bytes(&checkpoint) ==
           sizeof(data));
}

int main(void)
{
    test_successful_checkpoint_flushes_syncs_and_publishes();
    test_sync_failure_never_advances_durable_prefix();
    test_flush_failure_never_syncs_or_advances_durable_prefix();
    test_full_buffer_write_is_not_durable_before_checkpoint();
    return 0;
}
