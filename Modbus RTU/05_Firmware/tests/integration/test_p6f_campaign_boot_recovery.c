#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/system_runtime.h"
#include "tr2/platform_host/host_platform.h"

static SystemRuntimeDependencies make_dependencies(
    MonotonicClock *monotonic,
    WallClock *wall,
    ResetCauseProvider *reset,
    TimeContinuityEvidenceProvider *time_continuity,
    PersistentMedia *media,
    const ConfigurationValidationEnvironment *environment)
{
    SystemRuntimeDependencies deps;

    deps.monotonic_clock = monotonic;
    deps.wall_clock = wall;
    deps.reset_cause_provider = reset;
    deps.time_continuity_evidence_provider = time_continuity;
    deps.persistent_media = media;
    deps.configuration_validation_environment = environment;
    return deps;
}

static CampaignMetadata make_open(CampaignId campaign_id, uint32_t mission_id)
{
    CampaignMetadata metadata;

    memset(&metadata, 0, sizeof(metadata));
    metadata.campaign_id = campaign_id;
    metadata.mission_id = mission_id;
    metadata.lifecycle_state = CAMPAIGN_LIFECYCLE_OPEN;
    metadata.data_integrity = CAMPAIGN_DATA_INTEGRITY_UNKNOWN;
    metadata.historical_context.configuration_generation = 3u;
    metadata.historical_context.configuration_id = 4u;
    metadata.historical_context.configuration_revision_counter = 5u;
    metadata.historical_context.configuration_payload.sampling_frequency_hz = 1000u;
    metadata.historical_context.configuration_payload.axes_enable_mask = 7u;
    metadata.historical_context.configuration_payload.window_size_samples = 128u;
    memcpy(metadata.campaign_label, "boot-open", 9u);
    memcpy(metadata.mission_label, "mission", 7u);
    return metadata;
}

int main(void)
{
    HostPlatform platform;
    ConfigurationValidationEnvironment environment = { true, UINT32_C(4096) };
    MonotonicClock monotonic;
    WallClock wall;
    ResetCauseProvider reset;
    TimeContinuityEvidenceProvider time_continuity;
    PersistentMedia media;
    SystemRuntimeDependencies deps;
    SystemRuntime runtime_a;
    SystemRuntime runtime_b;
    CampaignRepository *repository;
    CampaignDataStore *data_store;
    CampaignIdReservation first;
    CampaignIdReservation second;
    CampaignMetadata first_open;
    CampaignMetadata second_open;
    CampaignBootRecoverySnapshot recovery;
    const uint8_t durable[] = { 1u, 2u, 3u, 4u, 5u };
    const uint8_t uncheckpointed[] = { 6u, 7u, 8u };

    host_platform_init(&platform);
    monotonic = host_platform_monotonic_clock(&platform);
    wall = host_platform_wall_clock(&platform);
    reset = host_platform_reset_cause_provider(&platform);
    time_continuity = host_platform_time_continuity_evidence_provider(&platform);
    media = host_platform_persistent_media(&platform);
    deps = make_dependencies(&monotonic,
                             &wall,
                             &reset,
                             &time_continuity,
                             &media,
                             &environment);

    assert(system_runtime_init(&runtime_a, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime_a) == TR2_OK);

    repository = campaign_repository_store_interface(
        &runtime_a.campaign_repository_store);
    data_store = campaign_data_store_persistent_interface(
        &runtime_a.campaign_data_store);
    assert(repository != NULL);
    assert(data_store != NULL);

    assert(repository->reserve_campaign_id(repository->context, &first) == TR2_OK);
    first_open = make_open(first.campaign_id, 11u);
    assert(repository->open_campaign(repository->context, &first_open) == TR2_OK);
    assert(data_store->begin_campaign(data_store->context, first.campaign_id) == TR2_OK);
    assert(data_store->append(data_store->context,
                              first.campaign_id,
                              durable,
                              sizeof(durable)) == TR2_OK);
    assert(data_store->checkpoint(data_store->context, first.campaign_id) == TR2_OK);
    assert(data_store->append(data_store->context,
                              first.campaign_id,
                              uncheckpointed,
                              sizeof(uncheckpointed)) == TR2_OK);

    /* A second OPEN can exist durably even if reset happens before DataStore begin. */
    assert(repository->reserve_campaign_id(repository->context, &second) == TR2_OK);
    second_open = make_open(second.campaign_id, 22u);
    assert(repository->open_campaign(repository->context, &second_open) == TR2_OK);

    host_platform_set_reset_cause(&platform, RESET_CAUSE_SOFTWARE);
    assert(system_runtime_init(&runtime_b, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime_b) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime_b));
    assert(!runtime_b.campaign_data_store.campaign_active);

    assert(system_runtime_campaign_recovery_snapshot(&runtime_b, &recovery));
    assert(recovery.repository_status == CAMPAIGN_REPOSITORY_RECOVERY_VALID);
    assert(recovery.inventory.valid_campaign_count == 2u);
    assert(recovery.recovered_campaign_count == 2u);

    assert(recovery.campaigns[0].metadata.campaign_id == first.campaign_id);
    assert(recovery.campaigns[0].metadata.lifecycle_state == CAMPAIGN_LIFECYCLE_OPEN);
    assert(!recovery.campaigns[0].metadata.end_timestamp.available);
    assert(!recovery.campaigns[0].metadata.duration.available);
    assert(recovery.campaigns[0].data_recovery.status == CAMPAIGN_DATA_RECOVERY_VALID);
    assert(recovery.campaigns[0].data_recovery.durable_prefix_bytes == sizeof(durable));

    assert(recovery.campaigns[1].metadata.campaign_id == second.campaign_id);
    assert(recovery.campaigns[1].metadata.lifecycle_state == CAMPAIGN_LIFECYCLE_OPEN);
    assert(!recovery.campaigns[1].metadata.end_timestamp.available);
    assert(!recovery.campaigns[1].metadata.duration.available);
    assert(recovery.campaigns[1].data_recovery.status == CAMPAIGN_DATA_RECOVERY_EMPTY);
    assert(recovery.campaigns[1].data_recovery.durable_prefix_bytes == 0u);

    return 0;
}
