#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/acquisition_service.h"
#include "tr2/application/supervision_service.h"
#include "tr2/application/system_runtime.h"
#include "tr2/modbus/read_adapter.h"
#include "tr2/platform_host/host_platform.h"

#define TEST_SAMPLE_CAPACITY 4u

typedef struct {
    VibrationSourceConfiguration last_configuration;
    VibrationSample samples[TEST_SAMPLE_CAPACITY];
    size_t sample_count;
    size_t read_index;
    Tr2Result read_result;
    uint32_t configure_calls;
    uint32_t start_calls;
    uint32_t read_calls;
    uint32_t stop_calls;
} FakeVibrationContext;

static Tr2Result fake_vibration_configure(
    void *context,
    const VibrationSourceConfiguration *configuration)
{
    FakeVibrationContext *source = (FakeVibrationContext *)context;

    source->configure_calls++;
    if (configuration == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    source->last_configuration = *configuration;
    return TR2_OK;
}

static Tr2Result fake_vibration_start(void *context)
{
    ((FakeVibrationContext *)context)->start_calls++;
    return TR2_OK;
}

static Tr2Result fake_vibration_read(void *context, VibrationSample *sample)
{
    FakeVibrationContext *source = (FakeVibrationContext *)context;

    source->read_calls++;
    if (sample == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (source->read_result != TR2_OK) {
        return source->read_result;
    }
    if (source->read_index >= source->sample_count) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    *sample = source->samples[source->read_index++];
    return TR2_OK;
}

static Tr2Result fake_vibration_stop(void *context)
{
    ((FakeVibrationContext *)context)->stop_calls++;
    return TR2_OK;
}

static VibrationSource make_vibration_source(FakeVibrationContext *context)
{
    VibrationSource source;

    source.context = context;
    source.configure = fake_vibration_configure;
    source.start = fake_vibration_start;
    source.read_sample = fake_vibration_read;
    source.stop = fake_vibration_stop;
    return source;
}

static ConfigurationPayload make_payload(uint16_t window_size,
                                         uint16_t rms_warn,
                                         uint16_t rms_alarm)
{
    ConfigurationPayload payload;

    memset(&payload, 0, sizeof(payload));
    payload.sampling_frequency_hz = UINT16_C(26667);
    payload.axes_enable_mask = UINT16_C(7);
    payload.full_scale_code = UINT16_C(2);
    payload.acquisition_mode = UINT16_C(1);
    payload.window_size_samples = window_size;
    payload.indicator_period_ms = UINT16_C(2000);
    payload.campaign_duration_s = UINT32_C(3600);
    payload.storage_mode = UINT16_C(1);
    payload.storage_limit_mb = UINT32_C(512);
    payload.rms_warn_threshold_mg = rms_warn;
    payload.rms_alarm_threshold_mg = rms_alarm;
    payload.peak_warn_threshold_mg = UINT16_C(10);
    payload.peak_alarm_threshold_mg = UINT16_C(20);
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

static ActiveConfigurationSnapshot commit_active(SystemRuntime *runtime,
                                                  uint32_t generation,
                                                  uint32_t config_id,
                                                  uint32_t revision,
                                                  const ConfigurationPayload *payload)
{
    ValidatedConfiguration validated;
    ActiveConfigurationSnapshot committed;

    memset(&validated, 0, sizeof(validated));
    validated.generation = generation;
    validated.config_id = config_id;
    validated.payload = *payload;
    assert(configuration_service_commit_validated(&runtime->configuration_service,
                                                  &validated,
                                                  revision,
                                                  &committed) == TR2_OK);
    return committed;
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
    ConfigurationRecoveryStatus recovery_status;
    FakeVibrationContext source_context;
    VibrationSource source;
    AcquisitionService acquisition;
    SupervisionService supervision;
    SupervisionService reboot_supervision;
    SupervisionSnapshot snapshot;
    SupervisionSnapshot previous_snapshot;
    AcquisitionWindow window;
    VibrationSample sample;
    ModbusReadSources read_sources = {0};
    ModbusReadOutcome outcome;
    uint16_t registers[48] = {0u};
    const ConfigurationPayload payload_a = make_payload(2u, 8u, 20u);
    const ConfigurationPayload payload_b = make_payload(1u, 100u, 200u);
    ActiveConfigurationSnapshot active_a;
    ActiveConfigurationSnapshot active_b;

    host_platform_init(&platform);
    platform.monotonic_ms = UINT64_C(1000);
    platform.civil_time = UINT32_C(123456);
    platform.civil_time_valid = true;
    host_platform_set_time_continuity_evidence(&platform,
                                                TIME_CONTINUITY_EVIDENCE_PROVEN);

    monotonic = host_platform_monotonic_clock(&platform);
    wall = host_platform_wall_clock(&platform);
    reset = host_platform_reset_cause_provider(&platform);
    time_continuity = host_platform_time_continuity_evidence_provider(&platform);
    media = host_platform_persistent_media(&platform);
    deps = make_dependencies(&monotonic, &wall, &reset, &time_continuity, &media, &environment);

    assert(system_runtime_init(&runtime_a, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime_a) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime_a));

    memset(&source_context, 0, sizeof(source_context));
    source_context.read_result = TR2_OK;
    source = make_vibration_source(&source_context);

    assert(acquisition_service_init(&acquisition,
                                    &runtime_a.configuration_service,
                                    &monotonic,
                                    &source) == TR2_OK);
    assert(supervision_service_init(&supervision) == TR2_OK);
    assert(supervision_service_bind_temporal_dependencies(&supervision,
                                                          &monotonic,
                                                          &runtime_a.time_service) == TR2_OK);

    /* Boot alone must not start acquisition or manufacture a live B3 source. */
    assert(source_context.configure_calls == 0u);
    assert(source_context.start_calls == 0u);
    registers[0] = UINT16_C(0xCAFE);
    outcome = modbus_read_adapter_read(&read_sources,
                                       UINT16_C(3000),
                                       UINT16_C(1),
                                       registers);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_NOT_AVAILABLE);
    assert(registers[0] == UINT16_C(0xCAFE));

    active_a = commit_active(&runtime_a, 10u, 100u, 7u, &payload_a);
    source_context.samples[0] = (VibrationSample){ 3, 4, 0, true, false };
    source_context.samples[1] = (VibrationSample){ 0, 0, 12, true, false };
    source_context.sample_count = 2u;

    assert(acquisition_service_begin_window(&acquisition) == TR2_OK);
    assert(acquisition.current_window.configuration.generation == active_a.generation);
    assert(acquisition.current_window.configuration.config_id == active_a.config_id);
    assert(acquisition.current_window.configuration.revision_counter == active_a.revision_counter);
    assert(source_context.last_configuration.axes_enable_mask == UINT16_C(7));
    assert(source_context.last_configuration.full_scale_code == UINT16_C(2));

    /* Active configuration changes during the window must not retroact on it. */
    active_b = commit_active(&runtime_a, 11u, 101u, 8u, &payload_b);
    assert(active_b.config_id == UINT32_C(101));

    assert(acquisition_service_read_sample(&acquisition, &sample) == TR2_OK);
    assert(acquisition_service_read_sample(&acquisition, &sample) == TR2_OK);
    host_platform_advance_monotonic(&platform, UINT64_C(125));
    assert(acquisition_service_end_window(&acquisition, &window) == TR2_OK);
    assert(window.complete);
    assert(window.configuration.generation == active_a.generation);
    assert(window.configuration.config_id == active_a.config_id);
    assert(window.configuration.revision_counter == active_a.revision_counter);

    assert(supervision_service_publish_window(&supervision, &window) == TR2_OK);
    host_platform_advance_monotonic(&platform, UINT64_C(75));
    assert(supervision_service_snapshot(&supervision, &snapshot));
    assert(snapshot.configuration.generation == active_a.generation);
    assert(snapshot.configuration.config_id == active_a.config_id);
    assert(snapshot.configuration.revision_counter == active_a.revision_counter);
    assert(snapshot.rms_global_mg == UINT32_C(9));
    assert(snapshot.peak_global_mg == UINT32_C(12));
    assert(snapshot.rms_x_mg == UINT32_C(2));
    assert(snapshot.rms_y_mg == UINT32_C(2));
    assert(snapshot.rms_z_mg == UINT32_C(8));
    assert(snapshot.value_age_available);
    assert(snapshot.value_age_ms == UINT32_C(75));
    assert(snapshot.civil_timestamp_available);
    assert(snapshot.civil_timestamp == UINT32_C(123456));
    assert(snapshot.threshold_facts.global.rms_warning == SUPERVISION_THRESHOLD_ABOVE);
    assert(snapshot.threshold_facts.global.rms_alarm == SUPERVISION_THRESHOLD_BELOW);

    read_sources.supervision = &snapshot;
    outcome = modbus_read_adapter_read(&read_sources,
                                       UINT16_C(3000),
                                       UINT16_C(48),
                                       registers);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(registers[4] == UINT16_C(0));
    assert(registers[5] == UINT16_C(0xE240));
    assert(registers[6] == UINT16_C(0));
    assert(registers[7] == UINT16_C(75));
    assert(registers[14] == UINT16_C(0));
    assert(registers[15] == UINT16_C(9));
    assert(registers[16] == UINT16_C(0));
    assert(registers[17] == UINT16_C(12));
    assert(registers[30] == UINT16_C(0));
    assert(registers[39] == UINT16_C(0));

    previous_snapshot = snapshot;

    /* A source failure before any valid sample cannot replace the last good snapshot. */
    source_context.read_index = 0u;
    source_context.sample_count = 1u;
    source_context.read_result = TR2_ERROR_UNAVAILABLE;
    assert(acquisition_service_begin_window(&acquisition) == TR2_OK);
    assert(acquisition.current_window.configuration.generation == active_b.generation);
    assert(acquisition_service_read_sample(&acquisition, &sample) == TR2_ERROR_UNAVAILABLE);
    host_platform_advance_monotonic(&platform, UINT64_C(25));
    assert(acquisition_service_end_window(&acquisition, &window) == TR2_OK);
    assert(window.source_error);
    assert(!window.complete);
    assert(window.valid_sample_count == 0u);
    assert(supervision_service_publish_window(&supervision, &window) == TR2_ERROR_NOT_AVAILABLE);
    assert(supervision_service_snapshot(&supervision, &snapshot));
    assert(snapshot.calculation_sequence == previous_snapshot.calculation_sequence);
    assert(snapshot.configuration.config_id == previous_snapshot.configuration.config_id);
    assert(snapshot.rms_global_mg == previous_snapshot.rms_global_mg);

    /* Civil-time loss must not prevent a new factual vibration snapshot. */
    platform.civil_time_valid = false;
    source_context.read_index = 0u;
    source_context.read_result = TR2_OK;
    source_context.samples[0] = (VibrationSample){ 6, 8, 0, true, false };
    source_context.sample_count = 1u;
    assert(acquisition_service_begin_window(&acquisition) == TR2_OK);
    assert(acquisition_service_read_sample(&acquisition, &sample) == TR2_OK);
    host_platform_advance_monotonic(&platform, UINT64_C(50));
    assert(acquisition_service_end_window(&acquisition, &window) == TR2_OK);
    assert(supervision_service_publish_window(&supervision, &window) == TR2_OK);
    assert(supervision_service_snapshot(&supervision, &snapshot));
    assert(snapshot.configuration.config_id == active_b.config_id);
    assert(!snapshot.civil_timestamp_available);
    assert(snapshot.civil_timestamp == UINT32_C(0));
    assert(snapshot.value_age_available);

    read_sources.supervision = &snapshot;
    outcome = modbus_read_adapter_read(&read_sources,
                                       UINT16_C(3000),
                                       UINT16_C(8),
                                       registers);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(registers[4] == UINT16_C(0));
    assert(registers[5] == UINT16_C(0));

    /* Reboot recovers B4 authority only; no live B3 snapshot is restored or started. */
    assert(system_runtime_init(&runtime_b, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime_b) == TR2_OK);
    assert(configuration_service_recovery_status(&runtime_b.configuration_service,
                                                 &recovery_status));
    assert(recovery_status == CONFIGURATION_RECOVERY_VALID);
    assert(source_context.configure_calls == 3u);
    assert(source_context.start_calls == 3u);

    assert(supervision_service_init(&reboot_supervision) == TR2_OK);
    assert(!supervision_service_snapshot(&reboot_supervision, &snapshot));
    read_sources.supervision = NULL;
    registers[0] = UINT16_C(0xBEEF);
    outcome = modbus_read_adapter_read(&read_sources,
                                       UINT16_C(3000),
                                       UINT16_C(1),
                                       registers);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_NOT_AVAILABLE);
    assert(registers[0] == UINT16_C(0xBEEF));

    return 0;
}
