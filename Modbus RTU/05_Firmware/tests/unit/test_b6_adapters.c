#include <assert.h>
#include <string.h>

#include "tr2/application/campaign_inventory_service.h"
#include "tr2/modbus/projection.h"
#include "tr2/modbus/read_adapter.h"
#include "tr2/modbus/write_adapter.h"

typedef struct {
    CampaignMetadata campaigns[2];
} FakeRepository;

static Tr2Result fake_summary(void *context, CampaignInventorySummary *summary)
{
    (void)context;
    summary->total_campaign_count = 2u;
    summary->valid_campaign_count = 2u;
    return TR2_OK;
}

static Tr2Result fake_by_index(void *context, size_t index, CampaignMetadata *metadata)
{
    FakeRepository *fake = context;
    if (index >= 2u) {
        return TR2_ERROR_NOT_FOUND;
    }
    *metadata = fake->campaigns[index];
    return TR2_OK;
}

int main(void)
{
    FakeRepository fake;
    CampaignRepository repository;
    CampaignInventoryService inventory;
    CampaignInventoryViewSnapshot snapshot;
    ModbusBlock6ProjectionSource projection_source;
    ModbusBlock6Image image;
    ModbusReadSources read_sources = {0};
    ModbusReadOutcome read_outcome;
    ModbusWriteOutcome write_outcome;
    uint16_t values[64] = {0u};
    uint16_t write_value;

    memset(&fake, 0, sizeof(fake));
    fake.campaigns[0].campaign_id = 1u;
    fake.campaigns[0].mission_id = 11u;
    fake.campaigns[0].lifecycle_state = CAMPAIGN_LIFECYCLE_OPEN;
    memcpy(fake.campaigns[0].campaign_label, "first", 5u);
    fake.campaigns[1].campaign_id = 2u;
    fake.campaigns[1].mission_id = 22u;
    fake.campaigns[1].lifecycle_state = CAMPAIGN_LIFECYCLE_CLOSED;
    memcpy(fake.campaigns[1].campaign_label, "second", 6u);

    memset(&repository, 0, sizeof(repository));
    repository.context = &fake;
    repository.get_inventory_summary = fake_summary;
    repository.get_campaign_by_index = fake_by_index;

    assert(campaign_inventory_service_init(&inventory, &repository) == TR2_OK);
    assert(campaign_inventory_service_snapshot(&inventory, &snapshot) == TR2_OK);

    memset(&projection_source, 0, sizeof(projection_source));
    projection_source.inventory_snapshot = &snapshot;
    projection_source.inventory_structure_version = 1u;
    assert(modbus_project_b6(&projection_source, &image) == TR2_OK);

    read_sources.b6_image = &image;
    read_outcome = modbus_read_adapter_read(&read_sources, 6000u, 64u, values);
    assert(read_outcome.access_result == MODBUS_ACCESS_OK);
    assert(read_outcome.operation_result == TR2_OK);
    assert(values[2] == 2u);
    assert(values[3] == 0u);
    assert(values[4] == 1u);
    assert(values[13] == 1u);

    write_value = 1u;
    write_outcome = modbus_write_adapter_write_b6(&inventory,
                                                  &image,
                                                  6003u,
                                                  &write_value,
                                                  1u);
    assert(write_outcome.access_result == MODBUS_ACCESS_OK);
    assert(write_outcome.operation_result == TR2_OK);
    assert(image.registers[3] == 1u);
    assert(image.registers[4] == 1u);
    assert(image.registers[13] == 2u);
    assert(image.registers[15] == 22u);

    write_value = 99u;
    write_outcome = modbus_write_adapter_write_b6(&inventory,
                                                  &image,
                                                  6003u,
                                                  &write_value,
                                                  1u);
    assert(write_outcome.access_result == MODBUS_ACCESS_OK);
    assert(write_outcome.operation_result == TR2_OK);
    assert(image.registers[3] == 99u);
    assert(image.registers[4] == 0u);
    assert(image.registers[12] == 0u);
    assert(image.registers[13] == 0u);

    write_value = 0u;
    write_outcome = modbus_write_adapter_write_b6(&inventory,
                                                  &image,
                                                  6002u,
                                                  &write_value,
                                                  1u);
    assert(write_outcome.access_result == MODBUS_ACCESS_READ_ONLY);

    write_outcome = modbus_write_adapter_write_b6(&inventory,
                                                  &image,
                                                  6003u,
                                                  &write_value,
                                                  2u);
    assert(write_outcome.access_result == MODBUS_ACCESS_READ_ONLY);

    write_outcome = modbus_write_adapter_write_b6(&inventory,
                                                  &image,
                                                  6010u,
                                                  &write_value,
                                                  1u);
    assert(write_outcome.access_result == MODBUS_ACCESS_RESERVED);

    write_outcome = modbus_write_adapter_write_b6(&inventory,
                                                  &image,
                                                  6064u,
                                                  &write_value,
                                                  1u);
    assert(write_outcome.access_result == MODBUS_ACCESS_ILLEGAL_ADDRESS);

    values[0] = 0xBEEFu;
    read_outcome = modbus_read_adapter_read(&read_sources, 6063u, 2u, values);
    assert(read_outcome.access_result == MODBUS_ACCESS_ILLEGAL_ADDRESS);
    assert(values[0] == 0xBEEFu);

    return 0;
}
