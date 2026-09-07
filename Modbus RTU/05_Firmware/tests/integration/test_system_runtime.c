#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/configuration_service.h"
#include "tr2/application/system_runtime.h"
#include "tr2/modbus/b4_configuration_codec.h"
#include "tr2/platform_host/host_platform.h"

static ConfigurationPayload valid_payload(void)
{
    ConfigurationPayload payload;

    memset(&payload, 0, sizeof(payload));
    payload.sampling_frequency_hz = UINT16_C(26667);
    payload.axes_enable_mask = UINT16_C(7);
    payload.full_scale_code = UINT16_C(2);
    payload.acquisition_mode = UINT16_C(1);
    payload.window_size_samples = UINT16_C(4096);
    payload.indicator_period_ms = UINT16_C(2000);
    payload.campaign_duration_s = UINT32_C(3600);
    payload.storage_mode = UINT16_C(1);
    payload.storage_limit_mb = UINT32_C(512);
    payload.campaign_context_id = UINT32_C(1);
    payload.mission_id = UINT32_C(2);
    payload.operating_mode_code = UINT16_C(1);
    return payload;
}

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

static void test_campaign_boot_recovery(void)
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

    /* Reset may occur after durable OPEN but before DataStore begin. */
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
    SystemRuntime runtime_c;
    ModbusBlock4Image image;
    ValidatedConfiguration validated;
    ActiveConfigurationSnapshot committed;
    ConfigurationRecoveryStatus recovery_status;
    const ConfigurationPayload payload = valid_payload();
    const uint32_t active_crc = tr2_b4_active_payload_crc(&payload);

    host_platform_init(&platform);
    host_platform_set_reset_cause(&platform, RESET_CAUSE_SOFTWARE);
    host_platform_advance_monotonic(&platform, UINT64_C(42));

    monotonic = host_platform_monotonic_clock(&platform);
    wall = host_platform_wall_clock(&platform);
    reset = host_platform_reset_cause_provider(&platform);
    time_continuity = host_platform_time_continuity_evidence_provider(&platform);
    media = host_platform_persistent_media(&platform);
    deps = make_dependencies(&monotonic, &wall, &reset, &time_continuity, &media, &environment);

    assert(system_runtime_init(&runtime_a, &deps) == TR2_OK);
    assert(!system_runtime_is_ready_for_modbus(&runtime_a));
    assert(!system_runtime_b4_image(&runtime_a, &image));
    assert(system_runtime_boot(&runtime_a) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime_a));
    assert(system_runtime_boot_context(&runtime_a) != NULL);
    assert(system_runtime_boot_context(&runtime_a)->reset_cause == RESET_CAUSE_SOFTWARE);
    assert(configuration_service_recovery_status(&runtime_a.configuration_service,
                                                 &recovery_status));
    assert(recovery_status == CONFIGURATION_RECOVERY_EMPTY);

    assert(system_runtime_b4_image(&runtime_a, &image));
    assert(image.registers[6] == UINT16_C(0));
    assert(image.registers[4] == UINT16_C(0));
    assert(image.registers[5] == UINT16_C(0));
    assert(image.registers[10] == UINT16_C(0x177C));
    assert(image.registers[11] == UINT16_C(0x92D9));
    assert(image.registers[12] == UINT16_C(0));
    assert(image.registers[13] == UINT16_C(0));

    memset(&validated, 0, sizeof(validated));
    validated.generation = UINT32_C(7);
    validated.config_id = UINT32_C(42);
    validated.payload = payload;
    assert(configuration_service_commit_validated(&runtime_a.configuration_service,
                                                  &validated,
                                                  UINT32_C(17),
                                                  &committed) == TR2_OK);
    assert(committed.generation == UINT32_C(7));
    assert(committed.config_id == UINT32_C(42));
    assert(committed.revision_counter == UINT32_C(17));

    /* Reboot: a new runtime instance must rebuild B4 only from durable authority. */
    host_platform_set_reset_cause(&platform, RESET_CAUSE_SOFTWARE);
    assert(system_runtime_init(&runtime_b, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime_b) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime_b));
    assert(configuration_service_recovery_status(&runtime_b.configuration_service,
                                                 &recovery_status));
    assert(recovery_status == CONFIGURATION_RECOVERY_VALID);
    assert(system_runtime_b4_image(&runtime_b, &image));

    assert(image.registers[6] == UINT16_C(4));
    assert(image.registers[2] == UINT16_C(0));
    assert(image.registers[3] == UINT16_C(0));
    assert(image.registers[4] == UINT16_C(0));
    assert(image.registers[5] == UINT16_C(42));
    assert(image.registers[10] == (uint16_t)(active_crc >> 16u));
    assert(image.registers[11] == (uint16_t)(active_crc & UINT32_C(0xFFFF)));
    assert(image.registers[12] == UINT16_C(0));
    assert(image.registers[13] == UINT16_C(17));
    assert(image.registers[100] == UINT16_C(26667));
    assert(image.registers[101] == UINT16_C(7));

    /* A non-recoverable active never becomes runtime authority: B4 returns neutral. */
    platform.persistent_committed[0] ^= UINT8_C(0x01);
    platform.persistent_candidate[0] = platform.persistent_committed[0];
    assert(system_runtime_init(&runtime_c, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime_c) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime_c));
    assert(configuration_service_recovery_status(&runtime_c.configuration_service,
                                                 &recovery_status));
    assert(recovery_status == CONFIGURATION_RECOVERY_CORRUPTED);
    assert(system_runtime_b4_image(&runtime_c, &image));
    assert(image.registers[6] == UINT16_C(0));
    assert(image.registers[4] == UINT16_C(0));
    assert(image.registers[5] == UINT16_C(0));
    assert(image.registers[10] == UINT16_C(0x177C));
    assert(image.registers[11] == UINT16_C(0x92D9));
    assert(image.registers[12] == UINT16_C(0));
    assert(image.registers[13] == UINT16_C(0));
    assert(image.registers[100] == UINT16_C(0));

    test_campaign_boot_recovery();
    return 0;
}
