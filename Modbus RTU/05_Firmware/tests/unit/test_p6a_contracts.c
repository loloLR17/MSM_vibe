#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/domain/campaign/campaign.h"
#include "tr2/persistence/campaign_data_store.h"
#include "tr2/persistence/campaign_repository.h"

static Tr2Result fake_reserve_campaign_id(void *context,
                                          CampaignIdReservation *reservation)
{
    CampaignId *next_id = (CampaignId *)context;
    if (next_id == NULL || reservation == NULL || *next_id == TR2_CAMPAIGN_ID_INVALID) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    reservation->campaign_id = *next_id;
    reservation->valid = true;
    (*next_id)++;
    return TR2_OK;
}

static void test_campaign_id_reservation_is_distinct_from_metadata(void)
{
    CampaignId next_id = 12u;
    CampaignRepository repository;
    CampaignIdReservation reservation;
    CampaignMetadata metadata;

    memset(&repository, 0, sizeof(repository));
    memset(&reservation, 0, sizeof(reservation));
    memset(&metadata, 0, sizeof(metadata));

    repository.context = &next_id;
    repository.reserve_campaign_id = fake_reserve_campaign_id;

    assert(TR2_CAMPAIGN_ID_INVALID == 0u);
    assert(repository.reserve_campaign_id(repository.context, &reservation) == TR2_OK);
    assert(reservation.valid);
    assert(reservation.campaign_id == 12u);
    assert(metadata.campaign_id == TR2_CAMPAIGN_ID_INVALID);
}

static void test_historical_configuration_context_is_a_value_snapshot(void)
{
    ActiveConfigurationSnapshot active;
    CampaignHistoricalContext historical;

    memset(&active, 0, sizeof(active));
    memset(&historical, 0, sizeof(historical));

    active.generation = 4u;
    active.config_id = 55u;
    active.revision_counter = 9u;
    active.payload.mission_id = 1001u;
    active.payload.sampling_frequency_hz = 26667u;
    active.payload.storage_limit_mb = 64u;

    historical.configuration_generation = active.generation;
    historical.configuration_id = active.config_id;
    historical.configuration_revision_counter = active.revision_counter;
    historical.configuration_payload = active.payload;

    active.generation = 5u;
    active.config_id = 56u;
    active.revision_counter = 10u;
    active.payload.mission_id = 1002u;
    active.payload.storage_limit_mb = 128u;

    assert(historical.configuration_generation == 4u);
    assert(historical.configuration_id == 55u);
    assert(historical.configuration_revision_counter == 9u);
    assert(historical.configuration_payload.mission_id == 1001u);
    assert(historical.configuration_payload.storage_limit_mb == 64u);
}

static void test_temporal_absence_is_explicit(void)
{
    CampaignMetadata metadata;

    memset(&metadata, 0, sizeof(metadata));

    assert(!metadata.start_timestamp.available);
    assert(!metadata.end_timestamp.available);
    assert(!metadata.duration.available);

    metadata.start_timestamp.available = true;
    metadata.start_timestamp.value = 123456u;
    assert(metadata.start_timestamp.available);
    assert(metadata.start_timestamp.value == 123456u);
}

static void test_inventory_snapshot_has_no_protocol_selection(void)
{
    CampaignInventorySnapshot snapshot;

    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.generation = 7u;
    snapshot.inventory.total_campaign_count = 3u;
    snapshot.inventory.valid_campaign_count = 2u;

    assert(snapshot.generation == 7u);
    assert(snapshot.inventory.total_campaign_count == 3u);
    assert(snapshot.inventory.valid_campaign_count == 2u);
}

static void test_data_store_contract_is_payload_opaque(void)
{
    CampaignDataStore store;
    CampaignDataRecoveryResult recovery;

    memset(&store, 0, sizeof(store));
    memset(&recovery, 0, sizeof(recovery));

    recovery.status = CAMPAIGN_DATA_RECOVERY_VALID;
    recovery.durable_prefix_bytes = 4096u;

    assert(store.context == NULL);
    assert(recovery.status == CAMPAIGN_DATA_RECOVERY_VALID);
    assert(recovery.durable_prefix_bytes == 4096u);
}

int main(void)
{
    test_campaign_id_reservation_is_distinct_from_metadata();
    test_historical_configuration_context_is_a_value_snapshot();
    test_temporal_absence_is_explicit();
    test_inventory_snapshot_has_no_protocol_selection();
    test_data_store_contract_is_payload_opaque();
    return 0;
}
