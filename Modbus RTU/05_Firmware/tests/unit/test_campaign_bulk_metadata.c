#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/campaign_bulk_metadata.h"

typedef struct {
    uint8_t working[2048];
    uint8_t durable[2048];
    uint32_t write_calls;
    uint32_t sync_calls;
    uint32_t fail_write_call;
    uint32_t fail_sync_call;
    size_t tear_write_bytes;
    Tr2Result read_result;
} FakeMedia;

static Tr2Result fake_capacity(void *context, uint64_t *capacity)
{
    FakeMedia *fake = context;
    *capacity = sizeof(fake->working);
    return TR2_OK;
}

static Tr2Result fake_read(void *context,uint64_t offset,void *buffer,size_t size)
{
    FakeMedia *fake = context;
    if (fake->read_result != TR2_OK) return fake->read_result;
    if (offset > sizeof(fake->working) ||
        size > sizeof(fake->working) - (size_t)offset) return TR2_ERROR_STORAGE;
    memcpy(buffer, &fake->working[(size_t)offset], size);
    return TR2_OK;
}

static Tr2Result fake_write(void *context,uint64_t offset,const void *buffer,size_t size)
{
    FakeMedia *fake = context;
    size_t n = size;
    fake->write_calls += 1u;
    if (fake->fail_write_call != 0u &&
        fake->write_calls == fake->fail_write_call) return TR2_ERROR_STORAGE;
    if (offset > sizeof(fake->working) ||
        size > sizeof(fake->working) - (size_t)offset) return TR2_ERROR_STORAGE;
    if (fake->tear_write_bytes != 0u && fake->tear_write_bytes < n) {
        n = fake->tear_write_bytes;
        fake->tear_write_bytes = 0u;
    }
    memcpy(&fake->working[(size_t)offset], buffer, n);
    return TR2_OK;
}

static Tr2Result fake_sync(void *context)
{
    FakeMedia *fake = context;
    fake->sync_calls += 1u;
    if (fake->fail_sync_call != 0u &&
        fake->sync_calls == fake->fail_sync_call) return TR2_ERROR_STORAGE;
    memcpy(fake->durable, fake->working, sizeof(fake->durable));
    return TR2_OK;
}

static CampaignBulkMedia make_media(FakeMedia *fake)
{
    CampaignBulkMedia media = {
        fake, fake_capacity, fake_read, fake_write, fake_sync
    };
    return media;
}

static void reboot(FakeMedia *fake)
{
    memcpy(fake->working, fake->durable, sizeof(fake->working));
    fake->fail_write_call = 0u;
    fake->fail_sync_call = 0u;
    fake->tear_write_bytes = 0u;
    fake->read_result = TR2_OK;
}

static CampaignBulkDescriptor descriptor(uint64_t generation,
                                         uint64_t prefix,
                                         CampaignBulkMetadataState state)
{
    CampaignBulkDescriptor d;
    d.generation = generation;
    d.campaign_id = 42u;
    d.state = state;
    d.data_base = 1024u;
    d.durable_prefix_bytes = prefix;
    return d;
}

static void init_metadata(FakeMedia *fake,
                          CampaignBulkMedia *media,
                          CampaignBulkMetadata *metadata)
{
    *media = make_media(fake);
    assert(campaign_bulk_metadata_init(metadata, media, 0u, 512u) == TR2_OK);
}

static void test_publish_alternates_and_recovery_selects_latest(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media;
    CampaignBulkMetadata metadata;
    CampaignBulkMetadata recovered;
    CampaignBulkMetadataRecoveryResult result;
    CampaignBulkDescriptor d1 = descriptor(1u, 0u, CAMPAIGN_BULK_METADATA_STATE_OPEN);
    CampaignBulkDescriptor d2 = descriptor(2u, 64u, CAMPAIGN_BULK_METADATA_STATE_OPEN);

    init_metadata(&fake, &media, &metadata);
    assert(campaign_bulk_metadata_publish(&metadata, &d1) == TR2_OK);
    assert(metadata.active_copy == 0u);
    assert(campaign_bulk_metadata_publish(&metadata, &d2) == TR2_OK);
    assert(metadata.active_copy == 1u);
    assert(fake.sync_calls == 2u);

    reboot(&fake);
    init_metadata(&fake, &media, &recovered);
    assert(campaign_bulk_metadata_recover(&recovered, &result) == TR2_OK);
    assert(result.status == CAMPAIGN_BULK_METADATA_RECOVERY_VALID);
    assert(result.active_copy == 1u);
    assert(result.descriptor.generation == 2u);
    assert(result.descriptor.durable_prefix_bytes == 64u);
}

static void test_failed_sync_keeps_previous_authority_after_reboot(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media;
    CampaignBulkMetadata metadata;
    CampaignBulkMetadata recovered;
    CampaignBulkMetadataRecoveryResult result;
    CampaignBulkDescriptor d1 = descriptor(1u, 16u, CAMPAIGN_BULK_METADATA_STATE_OPEN);
    CampaignBulkDescriptor d2 = descriptor(2u, 32u, CAMPAIGN_BULK_METADATA_STATE_OPEN);

    init_metadata(&fake, &media, &metadata);
    assert(campaign_bulk_metadata_publish(&metadata, &d1) == TR2_OK);
    fake.fail_sync_call = 2u;
    assert(campaign_bulk_metadata_publish(&metadata, &d2) == TR2_ERROR_STORAGE);
    assert(metadata.generation == 1u);
    assert(metadata.active_copy == 0u);
    assert(campaign_bulk_metadata_recovery_required(&metadata));

    reboot(&fake);
    init_metadata(&fake, &media, &recovered);
    assert(campaign_bulk_metadata_recover(&recovered, &result) == TR2_OK);
    assert(result.status == CAMPAIGN_BULK_METADATA_RECOVERY_VALID);
    assert(result.descriptor.generation == 1u);
    assert(result.descriptor.durable_prefix_bytes == 16u);
}

static void test_torn_candidate_keeps_previous_valid_copy(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media;
    CampaignBulkMetadata metadata;
    CampaignBulkMetadata recovered;
    CampaignBulkMetadataRecoveryResult result;
    CampaignBulkDescriptor d1 = descriptor(1u, 16u, CAMPAIGN_BULK_METADATA_STATE_OPEN);
    CampaignBulkDescriptor d2 = descriptor(2u, 32u, CAMPAIGN_BULK_METADATA_STATE_OPEN);

    init_metadata(&fake, &media, &metadata);
    assert(campaign_bulk_metadata_publish(&metadata, &d1) == TR2_OK);
    fake.tear_write_bytes = 100u;
    assert(campaign_bulk_metadata_publish(&metadata, &d2) == TR2_ERROR_STORAGE);
    assert(campaign_bulk_metadata_recovery_required(&metadata));

    reboot(&fake);
    init_metadata(&fake, &media, &recovered);
    assert(campaign_bulk_metadata_recover(&recovered, &result) == TR2_OK);
    assert(result.status == CAMPAIGN_BULK_METADATA_RECOVERY_VALID);
    assert(result.descriptor.generation == 1u);
}

static void test_corrupted_crc_is_rejected(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media;
    CampaignBulkMetadata metadata;
    CampaignBulkMetadata recovered;
    CampaignBulkMetadataRecoveryResult result;
    CampaignBulkDescriptor d1 = descriptor(1u, 16u, CAMPAIGN_BULK_METADATA_STATE_OPEN);

    init_metadata(&fake, &media, &metadata);
    assert(campaign_bulk_metadata_publish(&metadata, &d1) == TR2_OK);
    fake.durable[10] ^= 0x01u;
    reboot(&fake);
    init_metadata(&fake, &media, &recovered);
    assert(campaign_bulk_metadata_recover(&recovered, &result) == TR2_OK);
    assert(result.status == CAMPAIGN_BULK_METADATA_RECOVERY_CORRUPTED);
}

static void test_unknown_version_is_unsupported(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media;
    CampaignBulkMetadata metadata;
    CampaignBulkMetadata recovered;
    CampaignBulkMetadataRecoveryResult result;
    CampaignBulkDescriptor d1 = descriptor(1u, 0u, CAMPAIGN_BULK_METADATA_STATE_OPEN);

    init_metadata(&fake, &media, &metadata);
    assert(campaign_bulk_metadata_publish(&metadata, &d1) == TR2_OK);
    fake.durable[4] = 0u;
    fake.durable[5] = 2u;
    reboot(&fake);
    init_metadata(&fake, &media, &recovered);
    assert(campaign_bulk_metadata_recover(&recovered, &result) == TR2_OK);
    assert(result.status == CAMPAIGN_BULK_METADATA_RECOVERY_UNSUPPORTED);
}

static void test_same_generation_incompatible_copies_are_corrupted(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media;
    CampaignBulkMetadata metadata;
    CampaignBulkMetadata recovered;
    CampaignBulkMetadataRecoveryResult result;
    CampaignBulkDescriptor a = descriptor(1u, 16u, CAMPAIGN_BULK_METADATA_STATE_OPEN);
    CampaignBulkDescriptor b = descriptor(1u, 32u, CAMPAIGN_BULK_METADATA_STATE_OPEN);
    uint8_t record[TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE];

    init_metadata(&fake, &media, &metadata);
    assert(campaign_bulk_descriptor_encode(&a, record) == TR2_OK);
    memcpy(&fake.durable[0], record, sizeof(record));
    assert(campaign_bulk_descriptor_encode(&b, record) == TR2_OK);
    memcpy(&fake.durable[512], record, sizeof(record));
    reboot(&fake);

    init_metadata(&fake, &media, &recovered);
    assert(campaign_bulk_metadata_recover(&recovered, &result) == TR2_OK);
    assert(result.status == CAMPAIGN_BULK_METADATA_RECOVERY_CORRUPTED);
}

static void test_finished_state_survives_recovery(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media;
    CampaignBulkMetadata metadata;
    CampaignBulkMetadata recovered;
    CampaignBulkMetadataRecoveryResult result;
    CampaignBulkDescriptor d1 = descriptor(1u, 80u, CAMPAIGN_BULK_METADATA_STATE_FINISHED);

    init_metadata(&fake, &media, &metadata);
    assert(campaign_bulk_metadata_publish(&metadata, &d1) == TR2_OK);
    reboot(&fake);
    init_metadata(&fake, &media, &recovered);
    assert(campaign_bulk_metadata_recover(&recovered, &result) == TR2_OK);
    assert(result.status == CAMPAIGN_BULK_METADATA_RECOVERY_VALID);
    assert(result.descriptor.state == CAMPAIGN_BULK_METADATA_STATE_FINISHED);
}

int main(void)
{
    test_publish_alternates_and_recovery_selects_latest();
    test_failed_sync_keeps_previous_authority_after_reboot();
    test_torn_candidate_keeps_previous_valid_copy();
    test_corrupted_crc_is_rejected();
    test_unknown_version_is_unsupported();
    test_same_generation_incompatible_copies_are_corrupted();
    test_finished_state_survives_recovery();
    return 0;
}
