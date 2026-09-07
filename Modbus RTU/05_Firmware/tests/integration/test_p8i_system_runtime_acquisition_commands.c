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
    payload.window_size_samples = UINT16_C(8);
    payload.indicator_period_ms = UINT16_C(2000);
    payload.campaign_duration_s = UINT32_C(60);
    payload.storage_mode = UINT16_C(1);
    payload.storage_limit_mb = UINT32_C(8);
    payload.campaign_context_id = UINT32_C(1);
    payload.mission_id = UINT32_C(2);
    payload.operating_mode_code = UINT16_C(1);
    memcpy(payload.campaign_label, "p8i", 3u);
    memcpy(payload.mission_label, "runtime", 7u);
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
    SystemRuntime runtime;
    ValidatedConfiguration validated;
    ActiveConfigurationSnapshot committed;
    CommandRequest request;
    CommandRequest collision;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandTerminalTimestamp timestamp = { false, 0u };
    CommandSnapshot command_snapshot;
    CampaignInventoryViewSnapshot inventory;
    uint32_t start_calls;
    uint32_t stop_calls;

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

    assert(system_runtime_init(&runtime, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime));

    memset(&validated, 0, sizeof(validated));
    validated.generation = UINT32_C(1);
    validated.config_id = UINT32_C(1);
    validated.payload = valid_payload();
    assert(configuration_service_commit_validated(&runtime.configuration_service,
                                                  &validated,
                                                  UINT32_C(1),
                                                  &committed) == TR2_OK);

    memset(&request, 0, sizeof(request));
    request.transaction_id = UINT16_C(501);
    request.identity.command_code = COMMAND_CODE_START_ACQUISITION;

    assert(system_runtime_execute_acquisition_command(&runtime,
                                                      &request,
                                                      &timestamp,
                                                      &admission,
                                                      &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_NEW);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(entry.has_final_result);
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(campaign_service_campaign_open(&runtime.campaign_service));
    assert(campaign_service_acquisition_running(&runtime.campaign_service));
    assert(platform.vibration_start_calls == 1u);

    assert(system_runtime_command_snapshot(&runtime, &command_snapshot));
    assert(command_snapshot.last.present);
    assert(command_snapshot.last.command_code == COMMAND_CODE_START_ACQUISITION);
    assert(command_snapshot.last.transaction_id == UINT16_C(501));
    assert(command_snapshot.last.final_result.status == COMMAND_STATUS_SUCCESS);

    assert(system_runtime_campaign_inventory_snapshot(&runtime, &inventory));
    assert(inventory.inventory.valid_campaign_count == 1u);
    assert(inventory.selected_campaign_valid);
    assert(inventory.selected_campaign.lifecycle_state == CAMPAIGN_LIFECYCLE_OPEN);

    start_calls = platform.vibration_start_calls;
    assert(system_runtime_execute_acquisition_command(&runtime,
                                                      &request,
                                                      &timestamp,
                                                      &admission,
                                                      &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_RETRY);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(platform.vibration_start_calls == start_calls);

    collision = request;
    collision.identity.command_code = COMMAND_CODE_STOP_ACQUISITION;
    stop_calls = platform.vibration_stop_calls;
    assert(system_runtime_execute_acquisition_command(&runtime,
                                                      &collision,
                                                      &timestamp,
                                                      &admission,
                                                      &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_COLLISION);
    assert(platform.vibration_stop_calls == stop_calls);
    assert(campaign_service_campaign_open(&runtime.campaign_service));

    memset(&request, 0, sizeof(request));
    request.transaction_id = UINT16_C(502);
    request.identity.command_code = COMMAND_CODE_STOP_ACQUISITION;
    assert(system_runtime_execute_acquisition_command(&runtime,
                                                      &request,
                                                      &timestamp,
                                                      &admission,
                                                      &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_NEW);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(entry.has_final_result);
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(!campaign_service_campaign_open(&runtime.campaign_service));
    assert(!campaign_service_acquisition_running(&runtime.campaign_service));
    assert(platform.vibration_stop_calls == stop_calls + 1u);

    assert(system_runtime_command_snapshot(&runtime, &command_snapshot));
    assert(command_snapshot.last.present);
    assert(command_snapshot.last.command_code == COMMAND_CODE_STOP_ACQUISITION);
    assert(command_snapshot.last.transaction_id == UINT16_C(502));
    assert(command_snapshot.last.final_result.status == COMMAND_STATUS_SUCCESS);

    assert(system_runtime_campaign_inventory_snapshot(&runtime, &inventory));
    assert(inventory.inventory.valid_campaign_count == 1u);
    assert(inventory.selected_campaign_valid);
    assert(inventory.selected_campaign.lifecycle_state == CAMPAIGN_LIFECYCLE_CLOSED);

    stop_calls = platform.vibration_stop_calls;
    assert(system_runtime_execute_acquisition_command(&runtime,
                                                      &request,
                                                      &timestamp,
                                                      &admission,
                                                      &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_RETRY);
    assert(platform.vibration_stop_calls == stop_calls);

    return 0;
}
