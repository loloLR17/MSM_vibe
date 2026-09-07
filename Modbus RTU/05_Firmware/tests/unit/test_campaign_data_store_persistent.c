#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/campaign_data_store_persistent.h"

typedef struct {
    uint8_t durable[TR2_CAMPAIGN_DATA_STORAGE_SIZE];
    uint8_t working[TR2_CAMPAIGN_DATA_STORAGE_SIZE];
    uint32_t commit_calls;
    uint32_t fail_commit_call;
    Tr2Result read_result;
    Tr2Result write_result;
} FakeBulkMedia;

static Tr2Result fake_read(void *context,
                           uint32_t offset,
                           void *buffer,
                           size_t size)
{
    FakeBulkMedia *fake = context;

    if (fake->read_result != TR2_OK) {
        return fake->read_result;
    }
    if ((size_t)offset + size > sizeof(fake->working)) {
        return TR2_ERROR_STORAGE;
    }

    memcpy(buffer, &fake->working[offset], size);
    return TR2_OK;
}

static Tr2Result fake_write(void *context,
                            uint32_t offset,
                            const void *buffer,
                            size_t size)
{
    FakeBulkMedia *fake = context;

    if (fake->write_result != TR2_OK) {
        return fake->write_result;
    }
    if ((size_t)offset + size > sizeof(fake->working)) {
        return TR2_ERROR_STORAGE;
    }

    memcpy(&fake->working[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result fake_commit(void *context)
{
    FakeBulkMedia *fake = context;

    fake->commit_calls += 1u;
    if (fake->fail_commit_call != 0u &&
        fake->commit_calls == fake->fail_commit_call) {
        return TR2_ERROR_STORAGE;
    }

    memcpy(fake->durable, fake->working, sizeof(fake->durable));
    return TR2_OK;
}

static void fake_init(FakeBulkMedia *fake)
{
    memset(fake, 0, sizeof(*fake));
    fake->read_result = TR2_OK;
    fake->write_result = TR2_OK;
}

static void fake_reboot(FakeBulkMedia *fake)
{
    memcpy(fake->working, fake->durable, sizeof(fake->working));
    fake->fail_commit_call = 0u;
}

static void init_store(FakeBulkMedia *fake,
                       PersistentStorageCore *core,
                       CampaignDataStorePersistent *store,
                       PersistentMedia *media)
{
    media->context = fake;
    media->read = fake_read;
    media->write = fake_write;
    media->commit = fake_commit;

    assert(persistent_storage_core_init(core, media) == TR2_OK);
    assert(campaign_data_store_persistent_init(store, core) == TR2_OK);
}

static void fill_pattern(uint8_t *data, size_t size)
{
    size_t index;

    for (index = 0u; index < size; ++index) {
        data[index] = (uint8_t)((index * 37u) & UINT8_C(0xFF));
    }
}

static void test_checkpoint_recovers_exact_durable_prefix(void)
{
    FakeBulkMedia fake;
    PersistentStorageCore core1 = { 0 };
    PersistentStorageCore core2 = { 0 };
    CampaignDataStorePersistent store1 = { 0 };
    CampaignDataStorePersistent store2 = { 0 };
    PersistentMedia media1;
    PersistentMedia media2;
    CampaignDataStore *data_store;
    CampaignDataRecoveryResult recovery;
    uint8_t payload[130];

    fake_init(&fake);
    fill_pattern(payload, sizeof(payload));
    init_store(&fake, &core1, &store1, &media1);
    data_store = campaign_data_store_persistent_interface(&store1);

    assert(data_store->begin_campaign(data_store->context, 11u) == TR2_OK);
    assert(data_store->append(data_store->context,
                              11u,
                              payload,
                              sizeof(payload)) == TR2_OK);
    assert(data_store->checkpoint(data_store->context, 11u) == TR2_OK);

    fake_reboot(&fake);
    init_store(&fake, &core2, &store2, &media2);
    data_store = campaign_data_store_persistent_interface(&store2);
    assert(data_store->recover_campaign(data_store->context,
                                        11u,
                                        &recovery) == TR2_OK);
    assert(recovery.status == CAMPAIGN_DATA_RECOVERY_VALID);
    assert(recovery.durable_prefix_bytes == sizeof(payload));
}

static void test_uncheckpointed_tail_is_not_recovered(void)
{
    FakeBulkMedia fake;
    PersistentStorageCore core1 = { 0 };
    PersistentStorageCore core2 = { 0 };
    CampaignDataStorePersistent store1 = { 0 };
    CampaignDataStorePersistent store2 = { 0 };
    PersistentMedia media1;
    PersistentMedia media2;
    CampaignDataStore *data_store;
    CampaignDataRecoveryResult recovery;
    uint8_t first[80];
    uint8_t tail[17];

    fake_init(&fake);
    fill_pattern(first, sizeof(first));
    fill_pattern(tail, sizeof(tail));
    init_store(&fake, &core1, &store1, &media1);
    data_store = campaign_data_store_persistent_interface(&store1);

    assert(data_store->begin_campaign(data_store->context, 22u) == TR2_OK);
    assert(data_store->append(data_store->context,
                              22u,
                              first,
                              sizeof(first)) == TR2_OK);
    assert(data_store->checkpoint(data_store->context, 22u) == TR2_OK);
    assert(data_store->append(data_store->context,
                              22u,
                              tail,
                              sizeof(tail)) == TR2_OK);

    fake_reboot(&fake);
    init_store(&fake, &core2, &store2, &media2);
    data_store = campaign_data_store_persistent_interface(&store2);
    assert(data_store->recover_campaign(data_store->context,
                                        22u,
                                        &recovery) == TR2_OK);
    assert(recovery.status == CAMPAIGN_DATA_RECOVERY_VALID);
    assert(recovery.durable_prefix_bytes == sizeof(first));
}

static void test_failed_descriptor_commit_keeps_previous_checkpoint(void)
{
    FakeBulkMedia fake;
    PersistentStorageCore core1 = { 0 };
    PersistentStorageCore core2 = { 0 };
    CampaignDataStorePersistent store1 = { 0 };
    CampaignDataStorePersistent store2 = { 0 };
    PersistentMedia media1;
    PersistentMedia media2;
    CampaignDataStore *data_store;
    CampaignDataRecoveryResult recovery;
    uint8_t payload[40];

    fake_init(&fake);
    fill_pattern(payload, sizeof(payload));
    init_store(&fake, &core1, &store1, &media1);
    data_store = campaign_data_store_persistent_interface(&store1);

    assert(data_store->begin_campaign(data_store->context, 33u) == TR2_OK);
    assert(fake.commit_calls == 1u);
    assert(data_store->append(data_store->context,
                              33u,
                              payload,
                              sizeof(payload)) == TR2_OK);

    fake.fail_commit_call = 3u;
    assert(data_store->checkpoint(data_store->context, 33u) == TR2_ERROR_STORAGE);
    assert(campaign_data_store_persistent_recovery_required(&store1));

    fake_reboot(&fake);
    init_store(&fake, &core2, &store2, &media2);
    data_store = campaign_data_store_persistent_interface(&store2);
    assert(data_store->recover_campaign(data_store->context,
                                        33u,
                                        &recovery) == TR2_OK);
    assert(recovery.status == CAMPAIGN_DATA_RECOVERY_VALID);
    assert(recovery.durable_prefix_bytes == 0u);
}

static void test_corruption_inside_durable_prefix_is_detected(void)
{
    FakeBulkMedia fake;
    PersistentStorageCore core1 = { 0 };
    PersistentStorageCore core2 = { 0 };
    CampaignDataStorePersistent store1 = { 0 };
    CampaignDataStorePersistent store2 = { 0 };
    PersistentMedia media1;
    PersistentMedia media2;
    CampaignDataStore *data_store;
    CampaignDataRecoveryResult recovery;
    uint8_t payload[20];
    size_t first_payload_offset;

    fake_init(&fake);
    fill_pattern(payload, sizeof(payload));
    init_store(&fake, &core1, &store1, &media1);
    data_store = campaign_data_store_persistent_interface(&store1);

    assert(data_store->begin_campaign(data_store->context, 44u) == TR2_OK);
    assert(data_store->append(data_store->context,
                              44u,
                              payload,
                              sizeof(payload)) == TR2_OK);
    assert(data_store->checkpoint(data_store->context, 44u) == TR2_OK);

    first_payload_offset =
        TR2_CAMPAIGN_DATA_DESCRIPTOR_COPY_COUNT *
            TR2_CAMPAIGN_DATA_DESCRIPTOR_SIZE +
        20u;
    fake.durable[first_payload_offset] ^= UINT8_C(0x01);
    fake_reboot(&fake);

    init_store(&fake, &core2, &store2, &media2);
    data_store = campaign_data_store_persistent_interface(&store2);
    assert(data_store->recover_campaign(data_store->context,
                                        44u,
                                        &recovery) == TR2_OK);
    assert(recovery.status == CAMPAIGN_DATA_RECOVERY_CORRUPTED);
    assert(recovery.durable_prefix_bytes == 0u);
}

static void test_finish_publishes_final_prefix(void)
{
    FakeBulkMedia fake;
    PersistentStorageCore core1 = { 0 };
    PersistentStorageCore core2 = { 0 };
    CampaignDataStorePersistent store1 = { 0 };
    CampaignDataStorePersistent store2 = { 0 };
    PersistentMedia media1;
    PersistentMedia media2;
    CampaignDataStore *data_store;
    CampaignDataRecoveryResult recovery;
    uint8_t payload[95];

    fake_init(&fake);
    fill_pattern(payload, sizeof(payload));
    init_store(&fake, &core1, &store1, &media1);
    data_store = campaign_data_store_persistent_interface(&store1);

    assert(data_store->begin_campaign(data_store->context, 55u) == TR2_OK);
    assert(data_store->append(data_store->context,
                              55u,
                              payload,
                              sizeof(payload)) == TR2_OK);
    assert(data_store->finish_campaign(data_store->context, 55u) == TR2_OK);
    assert(!store1.campaign_active);

    fake_reboot(&fake);
    init_store(&fake, &core2, &store2, &media2);
    data_store = campaign_data_store_persistent_interface(&store2);
    assert(data_store->recover_campaign(data_store->context,
                                        55u,
                                        &recovery) == TR2_OK);
    assert(recovery.status == CAMPAIGN_DATA_RECOVERY_VALID);
    assert(recovery.durable_prefix_bytes == sizeof(payload));
}

static void test_empty_and_invalid_arguments(void)
{
    FakeBulkMedia fake;
    PersistentStorageCore core = { 0 };
    CampaignDataStorePersistent store = { 0 };
    PersistentMedia media;
    CampaignDataStore *data_store;
    CampaignDataRecoveryResult recovery;

    fake_init(&fake);
    init_store(&fake, &core, &store, &media);
    data_store = campaign_data_store_persistent_interface(&store);

    assert(data_store->recover_campaign(data_store->context,
                                        99u,
                                        &recovery) == TR2_OK);
    assert(recovery.status == CAMPAIGN_DATA_RECOVERY_EMPTY);
    assert(data_store->begin_campaign(data_store->context, 0u) ==
           TR2_ERROR_INVALID_ARGUMENT);
    assert(data_store->append(data_store->context, 99u, NULL, 1u) ==
           TR2_ERROR_INVALID_STATE);
}

int main(void)
{
    test_checkpoint_recovers_exact_durable_prefix();
    test_uncheckpointed_tail_is_not_recovered();
    test_failed_descriptor_commit_keeps_previous_checkpoint();
    test_corruption_inside_durable_prefix_is_detected();
    test_finish_publishes_final_prefix();
    test_empty_and_invalid_arguments();
    return 0;
}
