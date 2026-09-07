#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/acquisition_service.h"
#include "tr2/application/campaign_service.h"
#include "tr2/application/system_runtime.h"
#include "tr2/persistence/time_history_record.h"
#include "tr2/platform_host/host_platform.h"

#define TEST_CAMPAIGN_DATA_STORAGE_OFFSET \
    ((uint32_t)TR2_CONFIGURATION_STORE_STORAGE_SIZE + \
     (uint32_t)TR2_TIME_HISTORY_RECORD_SIZE + \
     (uint32_t)TR2_CAMPAIGN_REPOSITORY_STORAGE_SIZE)
#define TEST_FIRST_DATA_CHUNK_OFFSET \
    (TEST_CAMPAIGN_DATA_STORAGE_OFFSET + \
     (uint32_t)(TR2_CAMPAIGN_DATA_DESCRIPTOR_COPY_COUNT * \
                TR2_CAMPAIGN_DATA_DESCRIPTOR_SIZE))

typedef struct {
    uint32_t start_calls;
    uint32_t stop_calls;
} SourceContext;

typedef struct {
    CampaignDataStore *real_store;
    bool fail_recover_once;
    uint32_t finish_calls;
    uint32_t recover_calls;
} RecoveryWrapper;

static Tr2Result source_configure(void *context,
                                  const VibrationSourceConfiguration *configuration)
{
    (void)context;
    return configuration != NULL ? TR2_OK : TR2_ERROR_INVALID_ARGUMENT;
}

static Tr2Result source_start(void *context)
{
    SourceContext *source = context;
    source->start_calls++;
    return TR2_OK;
}

static Tr2Result source_read(void *context, VibrationSample *sample)
{
    (void)context;
    if (sample == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memset(sample, 0, sizeof(*sample));
    sample->valid = true;
    return TR2_OK;
}

static Tr2Result source_stop(void *context)
{
    SourceContext *source = context;
    source->stop_calls++;
    return TR2_OK;
}

static Tr2Result wrapped_begin(void *context, CampaignId campaign_id)
{
    RecoveryWrapper *wrapper = context;
    return wrapper->real_store->begin_campaign(wrapper->real_store->context,
                                               campaign_id);
}

static Tr2Result wrapped_append(void *context,
                                CampaignId campaign_id,
                                const uint8_t *data,
                                size_t size)
{
    RecoveryWrapper *wrapper = context;
    return wrapper->real_store->append(wrapper->real_store->context,
                                       campaign_id,
                                       data,
                                       size);
}

static Tr2Result wrapped_checkpoint(void *context, CampaignId campaign_id)
{
    RecoveryWrapper *wrapper = context;
    return wrapper->real_store->checkpoint(wrapper->real_store->context,
                                           campaign_id);
}

static Tr2Result wrapped_finish(void *context, CampaignId campaign_id)
{
    RecoveryWrapper *wrapper = context;
    wrapper->finish_calls++;
    return wrapper->real_store->finish_campaign(wrapper->real_store->context,
                                                campaign_id);
}

static Tr2Result wrapped_recover(void *context,
                                 CampaignId campaign_id,
                                 CampaignDataRecoveryResult *result)
{
    RecoveryWrapper *wrapper = context;
    wrapper->recover_calls++;
    if (wrapper->fail_recover_once) {
        wrapper->fail_recover_once = false;
        return TR2_ERROR_STORAGE;
    }
    return wrapper->real_store->recover_campaign(wrapper->real_store->context,
                                                 campaign_id,
                                                 result);
}

static SystemRuntimeDependencies make_dependencies(
    MonotonicClock *monotonic,
    WallClock *wall,
    ResetCauseProvider *reset,
    TimeContinuityEvidenceProvider *continuity,
    PersistentMedia *media,
    const ConfigurationValidationEnvironment *environment)
{
    SystemRuntimeDependencies deps;

    deps.monotonic_clock = monotonic;
    deps.wall_clock = wall;
    deps.reset_cause_provider = reset;
    deps.time_continuity_evidence_provider = continuity;
    deps.persistent_media = media;
    deps.configuration_validation_environment = environment;
    return deps;
}

static ConfigurationPayload valid_payload(uint32_t mission_id)
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
    payload.mission_id = mission_id;
    memcpy(payload.campaign_label, "P6I-campaign", 12u);
    memcpy(payload.mission_label, "P6I-mission", 11u);
    payload.operating_mode_code = UINT16_C(1);
    return payload;
}

static void commit_active(SystemRuntime *runtime, uint32_t mission_id)
{
    ValidatedConfiguration validated;

    memset(&validated, 0, sizeof(validated));
    validated.generation = UINT32_C(3);
    validated.config_id = UINT32_C(4);
    validated.payload = valid_payload(mission_id);
    assert(configuration_service_commit_validated(&runtime->configuration_service,
                                                  &validated,
                                                  UINT32_C(5),
                                                  NULL) == TR2_OK);
}

static VibrationSource make_source(SourceContext *context)
{
    VibrationSource source;

    source.context = context;
    source.configure = source_configure;
    source.start = source_start;
    source.read_sample = source_read;
    source.stop = source_stop;
    return source;
}

static void init_campaign_service(SystemRuntime *runtime,
                                  MonotonicClock *clock,
                                  SourceContext *source_context,
                                  AcquisitionService *acquisition,
                                  CampaignService *campaign,
                                  CampaignDataStore *override_data_store)
{
    VibrationSource source = make_source(source_context);
    CampaignRepository *repository = campaign_repository_store_interface(
        &runtime->campaign_repository_store);
    CampaignDataStore *data_store = override_data_store != NULL
                                        ? override_data_store
                                        : campaign_data_store_persistent_interface(
                                              &runtime->campaign_data_store);

    assert(repository != NULL);
    assert(data_store != NULL);
    assert(acquisition_service_init(acquisition,
                                    &runtime->configuration_service,
                                    clock,
                                    &source) == TR2_OK);
    assert(campaign_service_init(campaign,
                                 &runtime->configuration_service,
                                 acquisition,
                                 repository,
                                 data_store) == TR2_OK);
}

static void boot_runtime(SystemRuntime *runtime,
                         const SystemRuntimeDependencies *deps)
{
    assert(system_runtime_init(runtime, deps) == TR2_OK);
    assert(system_runtime_boot(runtime) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(runtime));
}

static void test_clean_start_stop_survives_reboot(void)
{
    HostPlatform platform;
    ConfigurationValidationEnvironment environment = { true, UINT32_C(4096) };
    MonotonicClock monotonic;
    WallClock wall;
    ResetCauseProvider reset;
    TimeContinuityEvidenceProvider continuity;
    PersistentMedia media;
    SystemRuntimeDependencies deps;
    SystemRuntime runtime_a;
    SystemRuntime runtime_b;
    AcquisitionService acquisition;
    CampaignService campaign;
    SourceContext source_context = {0};
    CampaignDataStore *data_store;
    CampaignId campaign_id;
    CampaignMetadata closed;
    CampaignBootRecoverySnapshot recovery;
    const uint8_t payload[] = { 1u, 2u, 3u, 4u, 5u, 6u };

    host_platform_init(&platform);
    monotonic = host_platform_monotonic_clock(&platform);
    wall = host_platform_wall_clock(&platform);
    reset = host_platform_reset_cause_provider(&platform);
    continuity = host_platform_time_continuity_evidence_provider(&platform);
    media = host_platform_persistent_media(&platform);
    deps = make_dependencies(&monotonic,
                             &wall,
                             &reset,
                             &continuity,
                             &media,
                             &environment);

    boot_runtime(&runtime_a, &deps);
    commit_active(&runtime_a, UINT32_C(42));
    init_campaign_service(&runtime_a,
                          &monotonic,
                          &source_context,
                          &acquisition,
                          &campaign,
                          NULL);

    assert(campaign_service_start(&campaign, &campaign_id) == TR2_OK);
    assert(campaign_id != TR2_CAMPAIGN_ID_INVALID);
    assert(source_context.start_calls == 1u);
    data_store = campaign_data_store_persistent_interface(&runtime_a.campaign_data_store);
    assert(data_store->append(data_store->context,
                              campaign_id,
                              payload,
                              sizeof(payload)) == TR2_OK);
    assert(data_store->checkpoint(data_store->context, campaign_id) == TR2_OK);
    assert(campaign_service_stop(&campaign, &closed) == TR2_OK);
    assert(source_context.stop_calls == 1u);
    assert(closed.lifecycle_state == CAMPAIGN_LIFECYCLE_CLOSED);
    assert(closed.durable_data_size_bytes == sizeof(payload));
    assert(!closed.end_timestamp.available);
    assert(!closed.duration.available);

    boot_runtime(&runtime_b, &deps);
    assert(system_runtime_campaign_recovery_snapshot(&runtime_b, &recovery));
    assert(recovery.repository_status == CAMPAIGN_REPOSITORY_RECOVERY_VALID);
    assert(recovery.recovered_campaign_count == 1u);
    assert(recovery.campaigns[0].metadata.campaign_id == campaign_id);
    assert(recovery.campaigns[0].metadata.lifecycle_state == CAMPAIGN_LIFECYCLE_CLOSED);
    assert(recovery.campaigns[0].metadata.durable_data_size_bytes == sizeof(payload));
    assert(!recovery.campaigns[0].metadata.end_timestamp.available);
    assert(!recovery.campaigns[0].metadata.duration.available);
    assert(source_context.start_calls == 1u);
}

static void test_power_loss_recovers_open_durable_prefix_only(void)
{
    HostPlatform platform;
    ConfigurationValidationEnvironment environment = { true, UINT32_C(4096) };
    MonotonicClock monotonic;
    WallClock wall;
    ResetCauseProvider reset;
    TimeContinuityEvidenceProvider continuity;
    PersistentMedia media;
    SystemRuntimeDependencies deps;
    SystemRuntime runtime_a;
    SystemRuntime runtime_b;
    AcquisitionService acquisition;
    CampaignService campaign;
    SourceContext source_context = {0};
    CampaignDataStore *data_store;
    CampaignId campaign_id;
    CampaignBootRecoverySnapshot recovery;
    const uint8_t durable[] = { 10u, 11u, 12u, 13u };
    const uint8_t tail[] = { 20u, 21u, 22u };

    host_platform_init(&platform);
    monotonic = host_platform_monotonic_clock(&platform);
    wall = host_platform_wall_clock(&platform);
    reset = host_platform_reset_cause_provider(&platform);
    continuity = host_platform_time_continuity_evidence_provider(&platform);
    media = host_platform_persistent_media(&platform);
    deps = make_dependencies(&monotonic,
                             &wall,
                             &reset,
                             &continuity,
                             &media,
                             &environment);

    boot_runtime(&runtime_a, &deps);
    commit_active(&runtime_a, UINT32_C(77));
    init_campaign_service(&runtime_a,
                          &monotonic,
                          &source_context,
                          &acquisition,
                          &campaign,
                          NULL);
    assert(campaign_service_start(&campaign, &campaign_id) == TR2_OK);

    data_store = campaign_data_store_persistent_interface(&runtime_a.campaign_data_store);
    assert(data_store->append(data_store->context,
                              campaign_id,
                              durable,
                              sizeof(durable)) == TR2_OK);
    assert(data_store->checkpoint(data_store->context, campaign_id) == TR2_OK);
    assert(data_store->append(data_store->context,
                              campaign_id,
                              tail,
                              sizeof(tail)) == TR2_OK);

    host_platform_set_reset_cause(&platform, RESET_CAUSE_SOFTWARE);
    boot_runtime(&runtime_b, &deps);
    assert(system_runtime_campaign_recovery_snapshot(&runtime_b, &recovery));
    assert(recovery.repository_status == CAMPAIGN_REPOSITORY_RECOVERY_VALID);
    assert(recovery.recovered_campaign_count == 1u);
    assert(recovery.campaigns[0].metadata.lifecycle_state == CAMPAIGN_LIFECYCLE_OPEN);
    assert(!recovery.campaigns[0].metadata.end_timestamp.available);
    assert(!recovery.campaigns[0].metadata.duration.available);
    assert(recovery.campaigns[0].data_recovery.status == CAMPAIGN_DATA_RECOVERY_VALID);
    assert(recovery.campaigns[0].data_recovery.durable_prefix_bytes == sizeof(durable));
    assert(!runtime_b.campaign_data_store.campaign_active);
    assert(source_context.start_calls == 1u);
    assert(source_context.stop_calls == 0u);
}

static void test_corrupted_durable_chunk_is_reported_without_fabrication(void)
{
    HostPlatform platform;
    ConfigurationValidationEnvironment environment = { true, UINT32_C(4096) };
    MonotonicClock monotonic;
    WallClock wall;
    ResetCauseProvider reset;
    TimeContinuityEvidenceProvider continuity;
    PersistentMedia media;
    SystemRuntimeDependencies deps;
    SystemRuntime runtime_a;
    SystemRuntime runtime_b;
    AcquisitionService acquisition;
    CampaignService campaign;
    SourceContext source_context = {0};
    CampaignDataStore *data_store;
    CampaignId campaign_id;
    CampaignBootRecoverySnapshot recovery;
    const uint8_t durable[] = { 31u, 32u, 33u, 34u };

    host_platform_init(&platform);
    monotonic = host_platform_monotonic_clock(&platform);
    wall = host_platform_wall_clock(&platform);
    reset = host_platform_reset_cause_provider(&platform);
    continuity = host_platform_time_continuity_evidence_provider(&platform);
    media = host_platform_persistent_media(&platform);
    deps = make_dependencies(&monotonic,
                             &wall,
                             &reset,
                             &continuity,
                             &media,
                             &environment);

    boot_runtime(&runtime_a, &deps);
    commit_active(&runtime_a, UINT32_C(88));
    init_campaign_service(&runtime_a,
                          &monotonic,
                          &source_context,
                          &acquisition,
                          &campaign,
                          NULL);
    assert(campaign_service_start(&campaign, &campaign_id) == TR2_OK);
    data_store = campaign_data_store_persistent_interface(&runtime_a.campaign_data_store);
    assert(data_store->append(data_store->context,
                              campaign_id,
                              durable,
                              sizeof(durable)) == TR2_OK);
    assert(data_store->checkpoint(data_store->context, campaign_id) == TR2_OK);

    platform.persistent_committed[TEST_FIRST_DATA_CHUNK_OFFSET] ^= UINT8_C(0x01);
    platform.persistent_candidate[TEST_FIRST_DATA_CHUNK_OFFSET] =
        platform.persistent_committed[TEST_FIRST_DATA_CHUNK_OFFSET];

    boot_runtime(&runtime_b, &deps);
    assert(system_runtime_campaign_recovery_snapshot(&runtime_b, &recovery));
    assert(recovery.repository_status == CAMPAIGN_REPOSITORY_RECOVERY_VALID);
    assert(recovery.recovered_campaign_count == 1u);
    assert(recovery.campaigns[0].metadata.lifecycle_state == CAMPAIGN_LIFECYCLE_OPEN);
    assert(!recovery.campaigns[0].metadata.end_timestamp.available);
    assert(!recovery.campaigns[0].metadata.duration.available);
    assert(recovery.campaigns[0].data_recovery.status == CAMPAIGN_DATA_RECOVERY_CORRUPTED);
    assert(source_context.start_calls == 1u);
}

static void test_stop_retry_after_recovery_failure_keeps_durable_size(void)
{
    HostPlatform platform;
    ConfigurationValidationEnvironment environment = { true, UINT32_C(4096) };
    MonotonicClock monotonic;
    WallClock wall;
    ResetCauseProvider reset;
    TimeContinuityEvidenceProvider continuity;
    PersistentMedia media;
    SystemRuntimeDependencies deps;
    SystemRuntime runtime;
    AcquisitionService acquisition;
    CampaignService campaign;
    SourceContext source_context = {0};
    RecoveryWrapper wrapper;
    CampaignDataStore wrapped_store;
    CampaignDataStore *real_store;
    CampaignId campaign_id;
    CampaignMetadata closed;
    const uint8_t payload[] = { 41u, 42u, 43u, 44u, 45u };

    host_platform_init(&platform);
    monotonic = host_platform_monotonic_clock(&platform);
    wall = host_platform_wall_clock(&platform);
    reset = host_platform_reset_cause_provider(&platform);
    continuity = host_platform_time_continuity_evidence_provider(&platform);
    media = host_platform_persistent_media(&platform);
    deps = make_dependencies(&monotonic,
                             &wall,
                             &reset,
                             &continuity,
                             &media,
                             &environment);

    boot_runtime(&runtime, &deps);
    commit_active(&runtime, UINT32_C(99));
    real_store = campaign_data_store_persistent_interface(&runtime.campaign_data_store);
    assert(real_store != NULL);

    memset(&wrapper, 0, sizeof(wrapper));
    wrapper.real_store = real_store;
    wrapper.fail_recover_once = true;
    wrapped_store.context = &wrapper;
    wrapped_store.begin_campaign = wrapped_begin;
    wrapped_store.append = wrapped_append;
    wrapped_store.checkpoint = wrapped_checkpoint;
    wrapped_store.finish_campaign = wrapped_finish;
    wrapped_store.recover_campaign = wrapped_recover;

    init_campaign_service(&runtime,
                          &monotonic,
                          &source_context,
                          &acquisition,
                          &campaign,
                          &wrapped_store);
    assert(campaign_service_start(&campaign, &campaign_id) == TR2_OK);
    assert(wrapped_store.append(wrapped_store.context,
                                campaign_id,
                                payload,
                                sizeof(payload)) == TR2_OK);
    assert(wrapped_store.checkpoint(wrapped_store.context, campaign_id) == TR2_OK);

    assert(campaign_service_stop(&campaign, &closed) == TR2_ERROR_STORAGE);
    assert(campaign_service_campaign_open(&campaign));
    assert(!campaign_service_acquisition_running(&campaign));
    assert(campaign.data_store_recovery_pending);
    assert(wrapper.finish_calls == 1u);
    assert(wrapper.recover_calls == 1u);

    assert(campaign_service_stop(&campaign, &closed) == TR2_OK);
    assert(wrapper.finish_calls == 1u);
    assert(wrapper.recover_calls == 2u);
    assert(closed.lifecycle_state == CAMPAIGN_LIFECYCLE_CLOSED);
    assert(closed.durable_data_size_bytes == sizeof(payload));
    assert(!closed.end_timestamp.available);
    assert(!closed.duration.available);
    assert(!campaign_service_campaign_open(&campaign));
}

int main(void)
{
    test_clean_start_stop_survives_reboot();
    test_power_loss_recovers_open_durable_prefix_only();
    test_corrupted_durable_chunk_is_reported_without_fabrication();
    test_stop_retry_after_recovery_failure_keeps_durable_size();
    return 0;
}

#undef TEST_FIRST_DATA_CHUNK_OFFSET
#undef TEST_CAMPAIGN_DATA_STORAGE_OFFSET
