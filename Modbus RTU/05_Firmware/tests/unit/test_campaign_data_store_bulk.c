#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/campaign_data_store_bulk.h"

#define MEDIA_SIZE 16384u
#define PAYLOAD_BUFFER_SIZE 64u
#define BLOCK_SCRATCH_SIZE 128u

typedef struct {
    uint8_t working[MEDIA_SIZE];
    uint8_t durable[MEDIA_SIZE];
    Tr2Result read_result;
    Tr2Result write_result;
    Tr2Result sync_result;
    uint32_t sync_calls;
} FakeMedia;

static Tr2Result fake_capacity(void *context, uint64_t *capacity)
{
    (void)context;
    *capacity = MEDIA_SIZE;
    return TR2_OK;
}

static Tr2Result fake_read(void *context,uint64_t offset,void *buffer,size_t size)
{
    FakeMedia *fake = context;
    if (fake->read_result != TR2_OK) return fake->read_result;
    if (offset > MEDIA_SIZE || size > MEDIA_SIZE - (size_t)offset)
        return TR2_ERROR_STORAGE;
    memcpy(buffer, &fake->working[(size_t)offset], size);
    return TR2_OK;
}

static Tr2Result fake_write(void *context,uint64_t offset,const void *buffer,size_t size)
{
    FakeMedia *fake = context;
    if (fake->write_result != TR2_OK) return fake->write_result;
    if (offset > MEDIA_SIZE || size > MEDIA_SIZE - (size_t)offset)
        return TR2_ERROR_STORAGE;
    memcpy(&fake->working[(size_t)offset], buffer, size);
    return TR2_OK;
}

static Tr2Result fake_sync(void *context)
{
    FakeMedia *fake = context;
    fake->sync_calls += 1u;
    if (fake->sync_result != TR2_OK) return fake->sync_result;
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
    fake->read_result = TR2_OK;
    fake->write_result = TR2_OK;
    fake->sync_result = TR2_OK;
}

static void init_store(FakeMedia *fake,
                       CampaignBulkMedia *media,
                       CampaignDataStoreBulk *store,
                       uint8_t payload_buffer[PAYLOAD_BUFFER_SIZE],
                       uint8_t scratch[BLOCK_SCRATCH_SIZE])
{
    *media = make_media(fake);
    assert(campaign_data_store_bulk_init(store,
                                         media,
                                         payload_buffer,
                                         PAYLOAD_BUFFER_SIZE,
                                         scratch,
                                         BLOCK_SCRATCH_SIZE) == TR2_OK);
}

static void fill(uint8_t *data, size_t size, uint8_t seed)
{
    size_t i;
    for (i = 0u; i < size; ++i) data[i] = (uint8_t)(seed + i);
}

static void test_checkpoint_recovers_exact_prefix(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media1, media2;
    CampaignDataStoreBulk store1, store2;
    uint8_t buffer1[PAYLOAD_BUFFER_SIZE], buffer2[PAYLOAD_BUFFER_SIZE];
    uint8_t scratch1[BLOCK_SCRATCH_SIZE], scratch2[BLOCK_SCRATCH_SIZE];
    uint8_t data[80];
    CampaignDataRecoveryResult recovery;
    CampaignDataStore *iface;

    fill(data, sizeof(data), 3u);
    init_store(&fake, &media1, &store1, buffer1, scratch1);
    iface = campaign_data_store_bulk_interface(&store1);
    assert(iface->begin_campaign(iface->context, 11u) == TR2_OK);
    assert(iface->append(iface->context, 11u, data, sizeof(data)) == TR2_OK);
    assert(iface->checkpoint(iface->context, 11u) == TR2_OK);

    reboot(&fake);
    init_store(&fake, &media2, &store2, buffer2, scratch2);
    iface = campaign_data_store_bulk_interface(&store2);
    assert(iface->recover_campaign(iface->context, 11u, &recovery) == TR2_OK);
    assert(recovery.status == CAMPAIGN_DATA_RECOVERY_VALID);
    assert(recovery.durable_prefix_bytes == sizeof(data));
}

static void test_uncheckpointed_tail_is_not_authority(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media1, media2;
    CampaignDataStoreBulk store1, store2;
    uint8_t buffer1[PAYLOAD_BUFFER_SIZE], buffer2[PAYLOAD_BUFFER_SIZE];
    uint8_t scratch1[BLOCK_SCRATCH_SIZE], scratch2[BLOCK_SCRATCH_SIZE];
    uint8_t first[32], tail[16];
    CampaignDataRecoveryResult recovery;
    CampaignDataStore *iface;

    fill(first, sizeof(first), 1u);
    fill(tail, sizeof(tail), 9u);
    init_store(&fake, &media1, &store1, buffer1, scratch1);
    iface = campaign_data_store_bulk_interface(&store1);
    assert(iface->begin_campaign(iface->context, 22u) == TR2_OK);
    assert(iface->append(iface->context, 22u, first, sizeof(first)) == TR2_OK);
    assert(iface->checkpoint(iface->context, 22u) == TR2_OK);
    assert(iface->append(iface->context, 22u, tail, sizeof(tail)) == TR2_OK);

    reboot(&fake);
    init_store(&fake, &media2, &store2, buffer2, scratch2);
    iface = campaign_data_store_bulk_interface(&store2);
    assert(iface->recover_campaign(iface->context, 22u, &recovery) == TR2_OK);
    assert(recovery.status == CAMPAIGN_DATA_RECOVERY_VALID);
    assert(recovery.durable_prefix_bytes == sizeof(first));
}

static void test_finish_and_second_campaign_survive_reboot(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media1, media2;
    CampaignDataStoreBulk store1, store2;
    uint8_t buffer1[PAYLOAD_BUFFER_SIZE], buffer2[PAYLOAD_BUFFER_SIZE];
    uint8_t scratch1[BLOCK_SCRATCH_SIZE], scratch2[BLOCK_SCRATCH_SIZE];
    uint8_t first[16], second[32];
    CampaignDataRecoveryResult recovery;
    CampaignDataStore *iface;

    fill(first, sizeof(first), 1u);
    fill(second, sizeof(second), 2u);
    init_store(&fake, &media1, &store1, buffer1, scratch1);
    iface = campaign_data_store_bulk_interface(&store1);

    assert(iface->begin_campaign(iface->context, 31u) == TR2_OK);
    assert(iface->append(iface->context, 31u, first, sizeof(first)) == TR2_OK);
    assert(iface->finish_campaign(iface->context, 31u) == TR2_OK);

    assert(iface->begin_campaign(iface->context, 32u) == TR2_OK);
    assert(iface->append(iface->context, 32u, second, sizeof(second)) == TR2_OK);
    assert(iface->finish_campaign(iface->context, 32u) == TR2_OK);

    reboot(&fake);
    init_store(&fake, &media2, &store2, buffer2, scratch2);
    iface = campaign_data_store_bulk_interface(&store2);

    assert(iface->recover_campaign(iface->context, 31u, &recovery) == TR2_OK);
    assert(recovery.status == CAMPAIGN_DATA_RECOVERY_VALID);
    assert(recovery.durable_prefix_bytes == sizeof(first));
    assert(iface->recover_campaign(iface->context, 32u, &recovery) == TR2_OK);
    assert(recovery.status == CAMPAIGN_DATA_RECOVERY_VALID);
    assert(recovery.durable_prefix_bytes == sizeof(second));
}

static void test_durable_block_corruption_is_detected(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media1, media2;
    CampaignDataStoreBulk store1, store2;
    uint8_t buffer1[PAYLOAD_BUFFER_SIZE], buffer2[PAYLOAD_BUFFER_SIZE];
    uint8_t scratch1[BLOCK_SCRATCH_SIZE], scratch2[BLOCK_SCRATCH_SIZE];
    uint8_t data[16];
    CampaignDataRecoveryResult recovery;
    CampaignDataStore *iface;
    size_t payload_offset =
        (size_t)TR2_CAMPAIGN_BULK_METADATA_BYTES +
        TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE;

    fill(data, sizeof(data), 7u);
    init_store(&fake, &media1, &store1, buffer1, scratch1);
    iface = campaign_data_store_bulk_interface(&store1);
    assert(iface->begin_campaign(iface->context, 44u) == TR2_OK);
    assert(iface->append(iface->context, 44u, data, sizeof(data)) == TR2_OK);
    assert(iface->checkpoint(iface->context, 44u) == TR2_OK);

    fake.durable[payload_offset] ^= UINT8_C(0x01);
    reboot(&fake);
    init_store(&fake, &media2, &store2, buffer2, scratch2);
    iface = campaign_data_store_bulk_interface(&store2);
    assert(iface->recover_campaign(iface->context, 44u, &recovery) == TR2_OK);
    assert(recovery.status == CAMPAIGN_DATA_RECOVERY_CORRUPTED);
}

static void test_incomplete_record_tail_blocks_checkpoint(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media;
    CampaignDataStoreBulk store;
    uint8_t buffer[PAYLOAD_BUFFER_SIZE];
    uint8_t scratch[BLOCK_SCRATCH_SIZE];
    uint8_t data[17] = { 0 };
    CampaignDataStore *iface;

    init_store(&fake, &media, &store, buffer, scratch);
    iface = campaign_data_store_bulk_interface(&store);
    assert(iface->begin_campaign(iface->context, 55u) == TR2_OK);
    assert(iface->append(iface->context, 55u, data, sizeof(data)) == TR2_OK);
    assert(iface->checkpoint(iface->context, 55u) == TR2_ERROR_INVALID_STATE);
}

static void test_empty_campaign_recovery(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media;
    CampaignDataStoreBulk store;
    uint8_t buffer[PAYLOAD_BUFFER_SIZE];
    uint8_t scratch[BLOCK_SCRATCH_SIZE];
    CampaignDataRecoveryResult recovery;
    CampaignDataStore *iface;

    init_store(&fake, &media, &store, buffer, scratch);
    iface = campaign_data_store_bulk_interface(&store);
    assert(iface->recover_campaign(iface->context, 99u, &recovery) == TR2_OK);
    assert(recovery.status == CAMPAIGN_DATA_RECOVERY_EMPTY);
}

int main(void)
{
    test_checkpoint_recovers_exact_prefix();
    test_uncheckpointed_tail_is_not_authority();
    test_finish_and_second_campaign_survive_reboot();
    test_durable_block_corruption_is_detected();
    test_incomplete_record_tail_blocks_checkpoint();
    test_empty_campaign_recovery();
    return 0;
}
