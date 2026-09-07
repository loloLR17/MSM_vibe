#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/campaign_inventory_service.h"
#include "tr2/modbus/projection.h"

typedef struct {
    CampaignMetadata campaigns[2];
    size_t count;
} FakeRepository;

static Tr2Result fake_inventory(void *context, CampaignInventorySummary *summary)
{
    FakeRepository *fake = context;

    summary->total_campaign_count = fake->count;
    summary->valid_campaign_count = fake->count;
    return TR2_OK;
}

static Tr2Result fake_by_index(void *context,
                               size_t index,
                               CampaignMetadata *metadata)
{
    FakeRepository *fake = context;

    if (index >= fake->count) {
        return TR2_ERROR_NOT_FOUND;
    }
    *metadata = fake->campaigns[index];
    return TR2_OK;
}

static CampaignMetadata make_campaign(CampaignId id,
                                      uint32_t mission_id,
                                      CampaignLifecycleState lifecycle,
                                      CampaignDataIntegrity integrity)
{
    CampaignMetadata metadata;

    memset(&metadata, 0, sizeof(metadata));
    metadata.campaign_id = id;
    metadata.mission_id = mission_id;
    metadata.lifecycle_state = lifecycle;
    metadata.data_integrity = integrity;
    return metadata;
}

static CampaignRepository make_repository(FakeRepository *fake)
{
    CampaignRepository repository;

    memset(&repository, 0, sizeof(repository));
    repository.context = fake;
    repository.get_inventory_summary = fake_inventory;
    repository.get_campaign_by_index = fake_by_index;
    return repository;
}

static void test_snapshot_selection_and_b6_projection(void)
{
    FakeRepository fake;
    CampaignRepository repository;
    CampaignInventoryService service;
    CampaignInventoryViewSnapshot snapshot;
    ModbusBlock6ProjectionSource source;
    ModbusBlock6Image image;

    memset(&fake, 0, sizeof(fake));
    fake.count = 2u;
    fake.campaigns[0] = make_campaign(7u,
                                      11u,
                                      CAMPAIGN_LIFECYCLE_OPEN,
                                      CAMPAIGN_DATA_INTEGRITY_UNKNOWN);
    fake.campaigns[1] = make_campaign(8u,
                                      22u,
                                      CAMPAIGN_LIFECYCLE_CLOSED,
                                      CAMPAIGN_DATA_INTEGRITY_COMPLETE);
    fake.campaigns[1].start_timestamp.available = true;
    fake.campaigns[1].start_timestamp.value = UINT32_C(1000);
    fake.campaigns[1].end_timestamp.available = true;
    fake.campaigns[1].end_timestamp.value = UINT32_C(1123);
    fake.campaigns[1].duration.available = true;
    fake.campaigns[1].duration.seconds = UINT32_C(123);
    fake.campaigns[1].durable_data_size_bytes = UINT64_C(2500000);
    memcpy(fake.campaigns[1].campaign_label, "campaign-B", 10u);
    memcpy(fake.campaigns[1].mission_label, "mission-B", 9u);

    repository = make_repository(&fake);
    assert(campaign_inventory_service_init(&service, &repository) == TR2_OK);
    assert(campaign_inventory_service_select(&service, 1u) == TR2_OK);
    assert(campaign_inventory_service_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.inventory.total_campaign_count == 2u);
    assert(snapshot.inventory.valid_campaign_count == 2u);
    assert(snapshot.selected_campaign_valid);
    assert(snapshot.selected_campaign.campaign_id == 8u);

    /* Projection consumes only the copied snapshot, not live repository state. */
    fake.campaigns[1].mission_id = 99u;

    memset(&source, 0, sizeof(source));
    source.inventory_snapshot = &snapshot;
    source.inventory_structure_version = 1u;
    source.storage_used_mb = UINT32_C(12);
    source.storage_free_mb = UINT32_C(34);
    source.storage_health_status = 0u;

    assert(modbus_project_b6(&source, &image) == TR2_OK);
    assert(image.registers[0] == 1u);
    assert(image.registers[1] == 2u);
    assert(image.registers[2] == 2u);
    assert(image.registers[3] == 1u);
    assert(image.registers[4] == 1u);
    assert(image.registers[5] == 0u && image.registers[6] == 12u);
    assert(image.registers[7] == 0u && image.registers[8] == 34u);
    assert(image.registers[9] == 0u);
    assert(image.registers[12] == 0u && image.registers[13] == 8u);
    assert(image.registers[14] == 0u && image.registers[15] == 22u);
    assert(image.registers[16] == 0u && image.registers[17] == 1000u);
    assert(image.registers[18] == 0u && image.registers[19] == 1123u);
    assert(image.registers[20] == 3u);
    assert(image.registers[21] == 0u && image.registers[22] == 123u);
    assert(image.registers[23] == 0u && image.registers[24] == 2u);
    assert(image.registers[57] == 1u);
    assert(image.registers[10] == 0u && image.registers[11] == 0u);
    assert(image.registers[58] == 0u && image.registers[63] == 0u);
    assert(image.source_generation == snapshot.generation);
}

static void test_out_of_range_selection_is_valid_write_but_invalid_view(void)
{
    FakeRepository fake;
    CampaignRepository repository;
    CampaignInventoryService service;
    CampaignInventoryViewSnapshot snapshot;
    ModbusBlock6ProjectionSource source;
    ModbusBlock6Image image;
    size_t index;

    memset(&fake, 0, sizeof(fake));
    fake.count = 1u;
    fake.campaigns[0] = make_campaign(1u,
                                      2u,
                                      CAMPAIGN_LIFECYCLE_OPEN,
                                      CAMPAIGN_DATA_INTEGRITY_UNKNOWN);
    repository = make_repository(&fake);

    assert(campaign_inventory_service_init(&service, &repository) == TR2_OK);
    assert(campaign_inventory_service_select(&service, UINT16_C(42)) == TR2_OK);
    assert(campaign_inventory_service_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.selected_campaign_index == UINT16_C(42));
    assert(!snapshot.selected_campaign_valid);

    memset(&source, 0, sizeof(source));
    source.inventory_snapshot = &snapshot;
    assert(modbus_project_b6(&source, &image) == TR2_OK);
    assert(image.registers[3] == UINT16_C(42));
    assert(image.registers[4] == 0u);
    for (index = 12u; index < TR2_B6_REGISTER_COUNT; ++index) {
        assert(image.registers[index] == 0u);
    }
}

static void test_partial_and_corrupted_state_mapping(void)
{
    FakeRepository fake;
    CampaignRepository repository;
    CampaignInventoryService service;
    CampaignInventoryViewSnapshot snapshot;
    ModbusBlock6ProjectionSource source;
    ModbusBlock6Image image;

    memset(&fake, 0, sizeof(fake));
    fake.count = 1u;
    repository = make_repository(&fake);
    assert(campaign_inventory_service_init(&service, &repository) == TR2_OK);

    fake.campaigns[0] = make_campaign(3u,
                                      4u,
                                      CAMPAIGN_LIFECYCLE_CLOSED,
                                      CAMPAIGN_DATA_INTEGRITY_PARTIAL);
    assert(campaign_inventory_service_snapshot(&service, &snapshot) == TR2_OK);
    memset(&source, 0, sizeof(source));
    source.inventory_snapshot = &snapshot;
    assert(modbus_project_b6(&source, &image) == TR2_OK);
    assert(image.registers[20] == 5u);
    assert(image.registers[57] == 3u);

    fake.campaigns[0].data_integrity = CAMPAIGN_DATA_INTEGRITY_CORRUPTED;
    assert(campaign_inventory_service_snapshot(&service, &snapshot) == TR2_OK);
    source.inventory_snapshot = &snapshot;
    assert(modbus_project_b6(&source, &image) == TR2_OK);
    assert(image.registers[20] == 4u);
    assert(image.registers[57] == 2u);
}

int main(void)
{
    test_snapshot_selection_and_b6_projection();
    test_out_of_range_selection_is_valid_write_but_invalid_view();
    test_partial_and_corrupted_state_mapping();
    return 0;
}
