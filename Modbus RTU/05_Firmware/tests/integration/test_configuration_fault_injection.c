#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/configuration_service.h"
#include "tr2/application/system_runtime.h"
#include "tr2/modbus/projection.h"
#include "tr2/platform_host/host_platform.h"

#define NO_PARTIAL_FAILURE ((size_t)-1)

typedef struct {
    HostPlatform *platform;
    size_t partial_write_count;
    bool fail_commit;
} FaultMediaContext;

static bool fault_media_range_valid(uint32_t offset, size_t size)
{
    return offset <= HOST_PLATFORM_PERSISTENT_BYTES &&
           size <= HOST_PLATFORM_PERSISTENT_BYTES - offset;
}

static Tr2Result fault_media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    FaultMediaContext *media = (FaultMediaContext *)context;

    if (media == NULL || media->platform == NULL ||
        media->platform->persistent_committed == NULL || buffer == NULL ||
        !fault_media_range_valid(offset, size)) {
        return TR2_ERROR_STORAGE;
    }

    memcpy(buffer, &media->platform->persistent_committed[offset], size);
    return TR2_OK;
}

static Tr2Result fault_media_write(void *context,
                                   uint32_t offset,
                                   const void *buffer,
                                   size_t size)
{
    FaultMediaContext *media = (FaultMediaContext *)context;
    size_t copy_size = size;

    if (media == NULL || media->platform == NULL ||
        media->platform->persistent_candidate == NULL || buffer == NULL ||
        !fault_media_range_valid(offset, size)) {
        return TR2_ERROR_STORAGE;
    }

    if (offset < (uint32_t)TR2_CONFIGURATION_STORE_STORAGE_SIZE &&
        media->partial_write_count != NO_PARTIAL_FAILURE &&
        media->partial_write_count < size) {
        copy_size = media->partial_write_count;
        memcpy(&media->platform->persistent_candidate[offset], buffer, copy_size);
        return TR2_ERROR_STORAGE;
    }

    memcpy(&media->platform->persistent_candidate[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result fault_media_commit(void *context)
{
    FaultMediaContext *media = (FaultMediaContext *)context;

    if (media == NULL || media->platform == NULL ||
        media->platform->persistent_committed == NULL ||
        media->platform->persistent_candidate == NULL) {
        return TR2_ERROR_STORAGE;
    }
    if (media->fail_commit) {
        return TR2_ERROR_STORAGE;
    }

    memcpy(media->platform->persistent_committed,
           media->platform->persistent_candidate,
           HOST_PLATFORM_PERSISTENT_BYTES);
    return TR2_OK;
}

static void fault_media_init(FaultMediaContext *media, HostPlatform *platform)
{
    memset(media, 0, sizeof(*media));
    media->platform = platform;
    media->partial_write_count = NO_PARTIAL_FAILURE;
    assert(platform != NULL);
    assert(platform->persistent_committed != NULL);
    assert(platform->persistent_candidate != NULL);
    memset(platform->persistent_committed, 0xFF, HOST_PLATFORM_PERSISTENT_BYTES);
    memcpy(platform->persistent_candidate,
           platform->persistent_committed,
           HOST_PLATFORM_PERSISTENT_BYTES);
}

static void simulate_power_loss(FaultMediaContext *media)
{
    assert(media != NULL);
    assert(media->platform != NULL);
    memcpy(media->platform->persistent_candidate,
           media->platform->persistent_committed,
           HOST_PLATFORM_PERSISTENT_BYTES);
    media->partial_write_count = NO_PARTIAL_FAILURE;
    media->fail_commit = false;
}

static PersistentMedia fault_media_port(FaultMediaContext *media)
{
    PersistentMedia port = {
        media,
        fault_media_read,
        fault_media_write,
        fault_media_commit
    };
    return port;
}

static ConfigurationPayload make_payload(uint16_t marker)
{
    ConfigurationPayload payload;

    memset(&payload, 0, sizeof(payload));
    payload.sampling_frequency_hz = UINT16_C(26667);
    payload.axes_enable_mask = UINT16_C(7);
    payload.full_scale_code = (uint16_t)(marker & UINT16_C(3));
    payload.acquisition_mode = UINT16_C(1);
    payload.window_size_samples = UINT16_C(4096);
    payload.indicator_period_ms = UINT16_C(2000);
    payload.campaign_duration_s = UINT32_C(3600) + marker;
    payload.storage_mode = UINT16_C(1);
    payload.storage_limit_mb = UINT32_C(512);
    payload.supervision_enable_mask = UINT16_C(1);
    payload.rms_warn_threshold_mg = (uint16_t)(UINT16_C(100) + marker);
    payload.rms_alarm_threshold_mg = (uint16_t)(UINT16_C(200) + marker);
    payload.peak_warn_threshold_mg = (uint16_t)(UINT16_C(300) + marker);
    payload.peak_alarm_threshold_mg = (uint16_t)(UINT16_C(400) + marker);
    payload.threshold_hysteresis_mg = (uint16_t)(UINT16_C(20) + marker);
    payload.alarm_hold_time_ms = UINT16_C(500);
    payload.campaign_context_id = UINT32_C(0x10000000) + marker;
    payload.mission_id = UINT32_C(0x20000000) + marker;
    payload.operating_mode_code = UINT16_C(1);
    payload.navigation_zone_code = (uint16_t)(UINT16_C(2) + marker);
    payload.load_state_code = (uint16_t)(UINT16_C(3) + marker);
    payload.sea_state_code = (uint16_t)(UINT16_C(1) + marker);
    return payload;
}

static ValidatedConfiguration make_validated(uint32_t generation,
                                             uint32_t config_id,
                                             uint16_t marker)
{
    ValidatedConfiguration validated;

    memset(&validated, 0, sizeof(validated));
    validated.generation = generation;
    validated.config_id = config_id;
    validated.payload = make_payload(marker);
    return validated;
}

static ActiveConfigurationSnapshot active_from_validated(
    const ValidatedConfiguration *validated,
    uint32_t revision_counter)
{
    ActiveConfigurationSnapshot snapshot;

    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.generation = validated->generation;
    snapshot.config_id = validated->config_id;
    snapshot.revision_counter = revision_counter;
    snapshot.payload = validated->payload;
    return snapshot;
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

    memset(&deps, 0, sizeof(deps));
    deps.monotonic_clock = monotonic;
    deps.wall_clock = wall;
    deps.reset_cause_provider = reset;
    deps.time_continuity_evidence_provider = time_continuity;
    deps.persistent_media = media;
    deps.configuration_validation_environment = environment;
    return deps;
}

static void assert_runtime_matches(
    const SystemRuntime *runtime,
    const ActiveConfigurationSnapshot *expected)
{
    ActiveConfigurationSnapshot recovered;
    ModbusBlock4ProjectionSource source;
    ModbusBlock4Image expected_image;
    ModbusBlock4Image actual_image;

    assert(system_runtime_is_ready_for_modbus(runtime));
    assert(configuration_service_active_snapshot(&runtime->configuration_service, &recovered));
    assert(recovered.generation == expected->generation);
    assert(recovered.config_id == expected->config_id);
    assert(recovered.revision_counter == expected->revision_counter);
    assert(memcmp(&recovered.payload, &expected->payload, sizeof(recovered.payload)) == 0);

    memset(&source, 0, sizeof(source));
    source.config_state = UINT16_C(4);
    source.active = expected;
    assert(modbus_project_b4(&source, &expected_image) == TR2_OK);
    assert(system_runtime_b4_image(runtime, &actual_image));
    assert(memcmp(&actual_image, &expected_image, sizeof(actual_image)) == 0);

    /* Prepared / validated are volatile and must not reappear after reboot. */
    assert(actual_image.registers[2] == UINT16_C(0));
    assert(actual_image.registers[3] == UINT16_C(0));
    assert(actual_image.registers[8] == UINT16_C(0));
    assert(actual_image.registers[9] == UINT16_C(0));
}

static void boot_runtime(SystemRuntime *runtime,
                         const SystemRuntimeDependencies *deps)
{
    assert(system_runtime_init(runtime, deps) == TR2_OK);
    assert(system_runtime_boot(runtime) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(runtime));
}

static void establish_active_a(SystemRuntime *runtime,
                               const SystemRuntimeDependencies *deps,
                               const ValidatedConfiguration *validated_a,
                               uint32_t revision_a,
                               ActiveConfigurationSnapshot *active_a)
{
    boot_runtime(runtime, deps);
    assert(configuration_service_commit_validated(&runtime->configuration_service,
                                                  validated_a,
                                                  revision_a,
                                                  active_a) == TR2_OK);
}

static void test_every_partial_write_recovers_a(void)
{
    size_t failed_bytes;

    for (failed_bytes = 0u; failed_bytes < TR2_CONFIGURATION_RECORD_SIZE; ++failed_bytes) {
        HostPlatform platform;
        FaultMediaContext fault_media;
        PersistentMedia media;
        MonotonicClock monotonic;
        WallClock wall;
        ResetCauseProvider reset;
        TimeContinuityEvidenceProvider time_continuity;
        ConfigurationValidationEnvironment environment = { true, UINT32_C(4096) };
        SystemRuntimeDependencies deps;
        SystemRuntime runtime_before;
        SystemRuntime runtime_after;
        ActiveConfigurationSnapshot active_a;
        const ValidatedConfiguration validated_a = make_validated(1u, 10u, 1u);
        const ValidatedConfiguration validated_b = make_validated(2u, 20u, 2u);

        host_platform_init(&platform);
        fault_media_init(&fault_media, &platform);
        media = fault_media_port(&fault_media);
        monotonic = host_platform_monotonic_clock(&platform);
        wall = host_platform_wall_clock(&platform);
        reset = host_platform_reset_cause_provider(&platform);
        time_continuity = host_platform_time_continuity_evidence_provider(&platform);
        deps = make_dependencies(&monotonic, &wall, &reset, &time_continuity, &media, &environment);

        establish_active_a(&runtime_before, &deps, &validated_a, UINT32_C(100), &active_a);
        fault_media.partial_write_count = failed_bytes;
        assert(configuration_service_commit_validated(&runtime_before.configuration_service,
                                                      &validated_b,
                                                      UINT32_C(200),
                                                      NULL) == TR2_ERROR_STORAGE);

        simulate_power_loss(&fault_media);
        boot_runtime(&runtime_after, &deps);
        assert_runtime_matches(&runtime_after, &active_a);
    }
}

static void test_commit_failure_recovers_a(void)
{
    HostPlatform platform;
    FaultMediaContext fault_media;
    PersistentMedia media;
    MonotonicClock monotonic;
    WallClock wall;
    ResetCauseProvider reset;
    TimeContinuityEvidenceProvider time_continuity;
    ConfigurationValidationEnvironment environment = { true, UINT32_C(4096) };
    SystemRuntimeDependencies deps;
    SystemRuntime runtime_before;
    SystemRuntime runtime_after;
    ActiveConfigurationSnapshot active_a;
    const ValidatedConfiguration validated_a = make_validated(1u, 10u, 1u);
    const ValidatedConfiguration validated_b = make_validated(2u, 20u, 2u);

    host_platform_init(&platform);
    fault_media_init(&fault_media, &platform);
    media = fault_media_port(&fault_media);
    monotonic = host_platform_monotonic_clock(&platform);
    wall = host_platform_wall_clock(&platform);
    reset = host_platform_reset_cause_provider(&platform);
    time_continuity = host_platform_time_continuity_evidence_provider(&platform);
    deps = make_dependencies(&monotonic, &wall, &reset, &time_continuity, &media, &environment);

    establish_active_a(&runtime_before, &deps, &validated_a, UINT32_C(100), &active_a);
    fault_media.fail_commit = true;
    assert(configuration_service_commit_validated(&runtime_before.configuration_service,
                                                  &validated_b,
                                                  UINT32_C(200),
                                                  NULL) == TR2_ERROR_STORAGE);

    simulate_power_loss(&fault_media);
    boot_runtime(&runtime_after, &deps);
    assert_runtime_matches(&runtime_after, &active_a);
}

static void test_power_loss_after_durable_commit_recovers_b(void)
{
    HostPlatform platform;
    FaultMediaContext fault_media;
    PersistentMedia media;
    MonotonicClock monotonic;
    WallClock wall;
    ResetCauseProvider reset;
    TimeContinuityEvidenceProvider time_continuity;
    ConfigurationValidationEnvironment environment = { true, UINT32_C(4096) };
    SystemRuntimeDependencies deps;
    SystemRuntime runtime_before;
    SystemRuntime runtime_after;
    ActiveConfigurationSnapshot active_a;
    const ValidatedConfiguration validated_a = make_validated(1u, 10u, 1u);
    const ValidatedConfiguration validated_b = make_validated(2u, 20u, 2u);
    const ActiveConfigurationSnapshot active_b =
        active_from_validated(&validated_b, UINT32_C(200));

    host_platform_init(&platform);
    fault_media_init(&fault_media, &platform);
    media = fault_media_port(&fault_media);
    monotonic = host_platform_monotonic_clock(&platform);
    wall = host_platform_wall_clock(&platform);
    reset = host_platform_reset_cause_provider(&platform);
    time_continuity = host_platform_time_continuity_evidence_provider(&platform);
    deps = make_dependencies(&monotonic, &wall, &reset, &time_continuity, &media, &environment);

    establish_active_a(&runtime_before, &deps, &validated_a, UINT32_C(100), &active_a);
    (void)active_a;

    /* Simulate power loss at P6: durable ConfigurationStore commit succeeded,
       but ConfigurationService runtime publication has not happened yet. */
    assert(configuration_store_commit(&runtime_before.configuration_store, &active_b) == TR2_OK);
    simulate_power_loss(&fault_media);

    boot_runtime(&runtime_after, &deps);
    assert_runtime_matches(&runtime_after, &active_b);
}

static void test_power_loss_after_runtime_publication_recovers_b(void)
{
    HostPlatform platform;
    FaultMediaContext fault_media;
    PersistentMedia media;
    MonotonicClock monotonic;
    WallClock wall;
    ResetCauseProvider reset;
    TimeContinuityEvidenceProvider time_continuity;
    ConfigurationValidationEnvironment environment = { true, UINT32_C(4096) };
    SystemRuntimeDependencies deps;
    SystemRuntime runtime_before;
    SystemRuntime runtime_after;
    ActiveConfigurationSnapshot active_a;
    ActiveConfigurationSnapshot active_b;
    const ValidatedConfiguration validated_a = make_validated(1u, 10u, 1u);
    const ValidatedConfiguration validated_b = make_validated(2u, 20u, 2u);

    host_platform_init(&platform);
    fault_media_init(&fault_media, &platform);
    media = fault_media_port(&fault_media);
    monotonic = host_platform_monotonic_clock(&platform);
    wall = host_platform_wall_clock(&platform);
    reset = host_platform_reset_cause_provider(&platform);
    time_continuity = host_platform_time_continuity_evidence_provider(&platform);
    deps = make_dependencies(&monotonic, &wall, &reset, &time_continuity, &media, &environment);

    establish_active_a(&runtime_before, &deps, &validated_a, UINT32_C(100), &active_a);
    (void)active_a;
    assert(configuration_service_commit_validated(&runtime_before.configuration_service,
                                                  &validated_b,
                                                  UINT32_C(200),
                                                  &active_b) == TR2_OK);

    simulate_power_loss(&fault_media);
    boot_runtime(&runtime_after, &deps);
    assert_runtime_matches(&runtime_after, &active_b);
}

int main(void)
{
    test_every_partial_write_recovers_a();
    test_commit_failure_recovers_a();
    test_power_loss_after_durable_commit_recovers_b();
    test_power_loss_after_runtime_publication_recovers_b();
    return 0;
}
