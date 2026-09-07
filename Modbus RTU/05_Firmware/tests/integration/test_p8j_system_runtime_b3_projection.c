#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/system_runtime.h"
#include "tr2/platform_host/host_platform.h"

static ConfigurationPayload valid_payload(void)
{
    ConfigurationPayload payload;

    memset(&payload, 0, sizeof(payload));
    payload.sampling_frequency_hz = UINT16_C(1000);
    payload.axes_enable_mask = UINT16_C(7);
    payload.full_scale_code = UINT16_C(2);
    payload.acquisition_mode = UINT16_C(1);
    payload.window_size_samples = UINT16_C(2);
    payload.indicator_period_ms = UINT16_C(2000);
    payload.campaign_duration_s = UINT32_C(60);
    payload.storage_mode = UINT16_C(1);
    payload.storage_limit_mb = UINT32_C(8);
    payload.campaign_context_id = UINT32_C(1);
    payload.mission_id = UINT32_C(2);
    payload.operating_mode_code = UINT16_C(1);
    memcpy(payload.campaign_label, "p8j", 3u);
    memcpy(payload.mission_label, "b3", 2u);
    return payload;
}

static SystemRuntimeDependencies make_dependencies(
    MonotonicClock *monotonic,
    WallClock *wall,
    ResetCauseProvider *reset,
    TimeContinuityEvidenceProvider *time_continuity,
    PersistentMedia *media,
    const ConfigurationValidationEnvironment *environment,
    VibrationSource *vibration_source)
{
    SystemRuntimeDependencies deps;

    deps.monotonic_clock = monotonic;
    deps.wall_clock = wall;
    deps.reset_cause_provider = reset;
    deps.time_continuity_evidence_provider = time_continuity;
    deps.persistent_media = media;
    deps.configuration_validation_environment = environment;
    deps.vibration_source = vibration_source;
    return deps;
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
    VibrationSource vibration;
    SystemRuntimeDependencies deps;
    SystemRuntime runtime_a;
    SystemRuntime runtime_b;
    ValidatedConfiguration validated;
    ActiveConfigurationSnapshot committed;
    CommandRequest request;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandTerminalTimestamp timestamp = { false, 0u };
    CampaignAcquisitionStep step;
    ModbusBlock3Image b3;

    host_platform_init(&platform);
    monotonic = host_platform_monotonic_clock(&platform);
    wall = host_platform_wall_clock(&platform);
    reset = host_platform_reset_cause_provider(&platform);
    time_continuity = host_platform_time_continuity_evidence_provider(&platform);
    media = host_platform_persistent_media(&platform);
    vibration = host_platform_vibration_source(&platform);
    deps = make_dependencies(&monotonic,
                             &wall,
                             &reset,
                             &time_continuity,
                             &media,
                             &environment,
                             &vibration);

    assert(system_runtime_init(&runtime_a, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime_a) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime_a));
    assert(!system_runtime_b3_image(&runtime_a, &b3));

    memset(&validated, 0, sizeof(validated));
    validated.generation = UINT32_C(1);
    validated.config_id = UINT32_C(1);
    validated.payload = valid_payload();
    assert(configuration_service_commit_validated(&runtime_a.configuration_service,
                                                  &validated,
                                                  UINT32_C(1),
                                                  &committed) == TR2_OK);

    memset(&request, 0, sizeof(request));
    request.transaction_id = UINT16_C(601);
    request.identity.command_code = COMMAND_CODE_START_ACQUISITION;
    assert(system_runtime_execute_acquisition_command(&runtime_a,
                                                      &request,
                                                      &timestamp,
                                                      &admission,
                                                      &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_NEW);
    assert(campaign_service_acquisition_running(&runtime_a.campaign_service));
    assert(!system_runtime_b3_image(&runtime_a, &b3));

    assert(system_runtime_drive_acquisition_step(&runtime_a, &step) == TR2_OK);
    assert(step.kind == CAMPAIGN_ACQUISITION_STEP_SAMPLE_READ);
    assert(step.storage_result == TR2_OK);
    assert(!system_runtime_b3_image(&runtime_a, &b3));

    assert(system_runtime_drive_acquisition_step(&runtime_a, &step) == TR2_OK);
    assert(step.kind == CAMPAIGN_ACQUISITION_STEP_SAMPLE_READ);
    assert(step.storage_result == TR2_OK);
    assert(!system_runtime_b3_image(&runtime_a, &b3));

    host_platform_advance_monotonic(&platform, UINT64_C(25));
    assert(system_runtime_drive_acquisition_step(&runtime_a, &step) == TR2_OK);
    assert(step.kind == CAMPAIGN_ACQUISITION_STEP_WINDOW_COMPLETED);
    assert(step.storage_result == TR2_OK);
    assert(step.window.complete);
    assert(step.window.valid_sample_count == UINT32_C(2));
    assert(!campaign_service_acquisition_running(&runtime_a.campaign_service));

    assert(system_runtime_b3_image(&runtime_a, &b3));
    assert(b3.source_calculation_sequence == UINT32_C(1));
    assert(b3.registers[8] == UINT16_C(0));
    assert(b3.registers[9] == UINT16_C(1));
    assert(b3.registers[12] == UINT16_C(0));
    assert(b3.registers[13] == UINT16_C(2));
    assert(b3.registers[14] == UINT16_C(0));
    assert(b3.registers[15] == UINT16_C(0));
    assert(b3.registers[16] == UINT16_C(0));
    assert(b3.registers[17] == UINT16_C(0));

    /* A reboot may recover the OPEN campaign and durable data prefix, but
       it must never restore a live SupervisionSnapshot/B3 authority. */
    host_platform_set_reset_cause(&platform, RESET_CAUSE_SOFTWARE);
    assert(system_runtime_init(&runtime_b, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime_b) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime_b));
    assert(!campaign_service_campaign_open(&runtime_b.campaign_service));
    assert(!campaign_service_acquisition_running(&runtime_b.campaign_service));
    assert(!system_runtime_b3_image(&runtime_b, &b3));

    return 0;
}
