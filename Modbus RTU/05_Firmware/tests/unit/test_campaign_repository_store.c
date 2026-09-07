#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/campaign_repository_store.h"

typedef struct {
    uint8_t bytes[TR2_CAMPAIGN_REPOSITORY_STORAGE_SIZE];
    Tr2Result read_result;
    Tr2Result write_result;
    Tr2Result commit_result;
    PersistentMedia media;
} FakeMedia;

static Tr2Result fake_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    FakeMedia *fake = (FakeMedia *)context;
    if (fake->read_result != TR2_OK) return fake->read_result;
    if ((size_t)offset + size > sizeof(fake->bytes)) return TR2_ERROR_STORAGE;
    memcpy(buffer, &fake->bytes[offset], size);
    return TR2_OK;
}

static Tr2Result fake_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    FakeMedia *fake = (FakeMedia *)context;
    if (fake->write_result != TR2_OK) return fake->write_result;
    if ((size_t)offset + size > sizeof(fake->bytes)) return TR2_ERROR_STORAGE;
    memcpy(&fake->bytes[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result fake_commit(void *context)
{
    FakeMedia *fake = (FakeMedia *)context;
    return fake->commit_result;
}

static void init_fake(FakeMedia *fake)
{
    memset(fake, 0, sizeof(*fake));
    fake->read_result = TR2_OK;
    fake->write_result = TR2_OK;
    fake->commit_result = TR2_OK;
    fake->media.context = fake;
    fake->media.read = fake_read;
    fake->media.write = fake_write;
    fake->media.commit = fake_commit;
}

static void init_store(FakeMedia *fake,
                       PersistentStorageCore *core,
                       CampaignRepositoryStore *store)
{
    assert(persistent_storage_core_init(core, &fake->media) == TR2_OK);
    assert(campaign_repository_store_init(store, core) == TR2_OK);
}

static CampaignMetadata make_open(CampaignId id)
{
    CampaignMetadata metadata;
    memset(&metadata, 0, sizeof(metadata));
    metadata.campaign_id = id;
    metadata.mission_id = 77u;
    metadata.lifecycle_state = CAMPAIGN_LIFECYCLE_OPEN;
    metadata.data_integrity = CAMPAIGN_DATA_INTEGRITY_UNKNOWN;
    metadata.historical_context.configuration_generation = 3u;
    metadata.historical_context.configuration_id = 4u;
    metadata.historical_context.configuration_revision_counter = 5u;
    metadata.historical_context.configuration_payload.sampling_frequency_hz = 1000u;
    metadata.historical_context.configuration_payload.axes_enable_mask = 7u;
    metadata.historical_context.configuration_payload.window_size_samples = 128u;
    memcpy(metadata.campaign_label, "campaign", 8u);
    memcpy(metadata.mission_label, "mission", 7u);
    return metadata;
}

static void test_reservation_is_persistent_and_not_reused_after_reboot(void)
{
    FakeMedia fake;
    PersistentStorageCore core1 = {0};
    PersistentStorageCore core2 = {0};
    CampaignRepositoryStore store1 = {0};
    CampaignRepositoryStore store2 = {0};
    CampaignIdReservation first;
    CampaignIdReservation second;
    CampaignRepository *repo;

    init_fake(&fake);

    init_store(&fake, &core1, &store1);
    repo = campaign_repository_store_interface(&store1);
    assert(repo->reserve_campaign_id(repo->context, &first) == TR2_OK);
    assert(first.valid && first.campaign_id == 1u);

    init_store(&fake, &core2, &store2);
    repo = campaign_repository_store_interface(&store2);
    assert(repo->reserve_campaign_id(repo->context, &second) == TR2_OK);
    assert(second.valid && second.campaign_id == 2u);
}

static void test_open_close_inventory_and_lookup_survive_reboot(void)
{
    FakeMedia fake;
    PersistentStorageCore core1 = {0};
    PersistentStorageCore core2 = {0};
    CampaignRepositoryStore store1 = {0};
    CampaignRepositoryStore store2 = {0};
    CampaignRepository *repo;
    CampaignIdReservation reservation;
    CampaignMetadata metadata;
    CampaignMetadata recovered;
    CampaignInventorySummary summary;
    CampaignRepositoryRecoveryResult recovery;

    init_fake(&fake);

    init_store(&fake, &core1, &store1);
    repo = campaign_repository_store_interface(&store1);
    assert(repo->reserve_campaign_id(repo->context, &reservation) == TR2_OK);
    metadata = make_open(reservation.campaign_id);
    assert(repo->open_campaign(repo->context, &metadata) == TR2_OK);
    assert(repo->open_campaign(repo->context, &metadata) == TR2_ERROR_INVALID_ARGUMENT);

    assert(repo->get_inventory_summary(repo->context, &summary) == TR2_OK);
    assert(summary.total_campaign_count == 1u);
    assert(summary.valid_campaign_count == 1u);
    assert(repo->get_campaign_by_index(repo->context, 0u, &recovered) == TR2_OK);
    assert(recovered.campaign_id == reservation.campaign_id);

    metadata.lifecycle_state = CAMPAIGN_LIFECYCLE_CLOSED;
    metadata.duration.available = true;
    metadata.duration.seconds = 12u;
    metadata.durable_data_size_bytes = 1234u;
    metadata.data_integrity = CAMPAIGN_DATA_INTEGRITY_COMPLETE;
    assert(repo->close_campaign(repo->context, &metadata) == TR2_OK);

    init_store(&fake, &core2, &store2);
    repo = campaign_repository_store_interface(&store2);
    assert(repo->recover(repo->context, &recovery) == TR2_OK);
    assert(recovery.status == CAMPAIGN_REPOSITORY_RECOVERY_VALID);
    assert(recovery.inventory.valid_campaign_count == 1u);
    assert(repo->get_campaign_by_id(repo->context, reservation.campaign_id, &recovered) == TR2_OK);
    assert(recovered.lifecycle_state == CAMPAIGN_LIFECYCLE_CLOSED);
    assert(recovered.duration.available && recovered.duration.seconds == 12u);
    assert(recovered.durable_data_size_bytes == 1234u);
}

static void test_failed_commit_requires_recovery(void)
{
    FakeMedia fake;
    PersistentStorageCore core = {0};
    CampaignRepositoryStore store = {0};
    CampaignRepository *repo;
    CampaignIdReservation reservation;
    CampaignRepositoryRecoveryResult recovery;

    init_fake(&fake);
    fake.commit_result = TR2_ERROR_STORAGE;
    init_store(&fake, &core, &store);
    repo = campaign_repository_store_interface(&store);

    assert(repo->reserve_campaign_id(repo->context, &reservation) == TR2_ERROR_STORAGE);
    assert(campaign_repository_store_recovery_required(&store));
    assert(repo->reserve_campaign_id(repo->context, &reservation) == TR2_ERROR_INVALID_STATE);

    fake.commit_result = TR2_OK;
    assert(repo->recover(repo->context, &recovery) == TR2_OK);
    assert(!campaign_repository_store_recovery_required(&store));
}

static void test_capacity_is_bounded(void)
{
    FakeMedia fake;
    PersistentStorageCore core = {0};
    CampaignRepositoryStore store = {0};
    CampaignRepository *repo;
    size_t index;

    init_fake(&fake);
    init_store(&fake, &core, &store);
    repo = campaign_repository_store_interface(&store);

    for (index = 0u; index < TR2_CAMPAIGN_REPOSITORY_CAPACITY; ++index) {
        CampaignIdReservation reservation;
        CampaignMetadata metadata;
        assert(repo->reserve_campaign_id(repo->context, &reservation) == TR2_OK);
        metadata = make_open(reservation.campaign_id);
        assert(repo->open_campaign(repo->context, &metadata) == TR2_OK);
    }
    {
        CampaignIdReservation reservation;
        CampaignMetadata metadata;
        assert(repo->reserve_campaign_id(repo->context, &reservation) == TR2_OK);
        metadata = make_open(reservation.campaign_id);
        assert(repo->open_campaign(repo->context, &metadata) == TR2_ERROR_NOT_AVAILABLE);
    }
}

int main(void)
{
    test_reservation_is_persistent_and_not_reused_after_reboot();
    test_open_close_inventory_and_lookup_survive_reboot();
    test_failed_commit_requires_recovery();
    test_capacity_is_bounded();
    return 0;
}
