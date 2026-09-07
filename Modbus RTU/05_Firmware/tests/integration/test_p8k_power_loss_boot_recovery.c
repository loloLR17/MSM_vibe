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
    memcpy(payload.campaign_label, "p8k", 3u);
    memcpy(payload.mission_label, "recovery", 8u);
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

static void assert_recovered_open_campaign(
    SystemRuntime *runtime,
    CampaignId campaign_id,
    uint64_t expected_prefix_bytes)
{
    CampaignBootRecoverySnapshot recovery;

    assert(system_runtime_campaign_recovery_snapshot(runtime, &recovery));
    assert(recovery.repository_status == CAMPAIGN_REPOSITORY_RECOVERY_VALID);
    assert(recovery.inventory.valid_campaign_count == 1u);
    assert(recovery.recovered_campaign_count == 1u);
    assert(recovery.campaigns[0].metadata.campaign_id == campaign_id);
    assert(recovery.campaigns[0].metadata.lifecycle_state == CAMPAIGN_LIFECYCLE_OPEN);
    assert(!recovery.campaigns[0].metadata.end_timestamp.available);
    assert(!recovery.campaigns[0].metadata.duration.available);
    assert(recovery.campaigns[0].data_recovery.status == CAMPAIGN_DATA_RECOVERY_VALID);
    assert(recovery.campaigns[0].data_recovery.durable_prefix_bytes == expected_prefix_bytes);
}

static void assert_started_transaction_reconciled_without_replay(
    SystemRuntime *runtime,
    uint16_t transaction_id,
    CampaignId campaign_id)
{
    CommandBootRecoveryResult recovery;
    CommandSnapshot snapshot;

    assert(system_runtime_command_boot_recovery(runtime, &recovery));
    assert(recovery.status == COMMAND_BOOT_RECOVERY_STARTED_EFFECT_PROVEN);
    assert(recovery.has_incomplete_transaction);
    assert(recovery.incomplete_transaction.transaction_id == transaction_id);
    assert(recovery.incomplete_transaction.lifecycle == COMMAND_LIFECYCLE_STARTED);
    assert(recovery.incomplete_transaction.has_recovery_context);
    assert(recovery.incomplete_transaction.recovery_context.kind ==
           COMMAND_RECOVERY_CONTEXT_START_CAMPAIGN);
    assert(recovery.incomplete_transaction.recovery_context.value1 == campaign_id);
    assert(!recovery.incomplete_transaction.has_final_result);

    assert(system_runtime_command_snapshot(runtime, &snapshot));
    assert(snapshot.active_command_code == COMMAND_CODE_START_ACQUISITION);
    assert(snapshot.active_transaction_id == transaction_id);
    assert(snapshot.status == COMMAND_STATUS_RUNNING);
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
    SystemRuntime runtime_c;
    ValidatedConfiguration validated;
    ActiveConfigurationSnapshot committed;
    CommandRequest request;
    CommandAdmissionResult admission;
    CommandRecoveryContext recovery_context;
    CommandJournalEntry entry;
    CommandJournal *journal;
    CampaignId campaign_id;
    CampaignAcquisitionStep step;
    ModbusBlock3Image b3;
    uint32_t start_calls_before_reboot;
    uint32_t read_calls_before_reboot;
    uint32_t stop_calls_before_reboot;

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

    memset(&validated, 0, sizeof(validated));
    validated.generation = UINT32_C(1);
    validated.config_id = UINT32_C(1);
    validated.payload = valid_payload();
    assert(configuration_service_commit_validated(&runtime_a.configuration_service,
                                                  &validated,
                                                  UINT32_C(1),
                                                  &committed) == TR2_OK);

    /* Reproduce the frozen START ordering up to the power-loss cut point:
       RESERVED -> recovery context -> STARTED -> durable campaign effect. */
    memset(&request, 0, sizeof(request));
    request.transaction_id = UINT16_C(601);
    request.identity.command_code = COMMAND_CODE_START_ACQUISITION;
    assert(command_engine_admit(&runtime_a.command_engine, &request, &admission) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_NEW);

    assert(campaign_service_reserve_start_id(&runtime_a.campaign_service,
                                             &campaign_id) == TR2_OK);
    assert(campaign_id != TR2_CAMPAIGN_ID_INVALID);

    journal = runtime_a.command_engine.journal;
    assert(journal != NULL);
    assert(journal->set_recovery_context != NULL);
    memset(&recovery_context, 0, sizeof(recovery_context));
    recovery_context.kind = COMMAND_RECOVERY_CONTEXT_START_CAMPAIGN;
    recovery_context.value1 = campaign_id;
    assert(journal->set_recovery_context(journal->context,
                                         request.transaction_id,
                                         &recovery_context,
                                         &entry) == TR2_OK);
    assert(command_engine_mark_started(&runtime_a.command_engine,
                                       request.transaction_id,
                                       &entry) == TR2_OK);
    assert(campaign_service_start_reserved(&runtime_a.campaign_service,
                                           campaign_id) == TR2_OK);

    /* Produce one complete durable window. The START transaction deliberately
       remains STARTED: the simulated reset occurs before B5 COMPLETED. */
    assert(system_runtime_drive_acquisition_step(&runtime_a, &step) == TR2_OK);
    assert(step.kind == CAMPAIGN_ACQUISITION_STEP_SAMPLE_READ);
    assert(system_runtime_drive_acquisition_step(&runtime_a, &step) == TR2_OK);
    assert(step.kind == CAMPAIGN_ACQUISITION_STEP_SAMPLE_READ);
    assert(system_runtime_drive_acquisition_step(&runtime_a, &step) == TR2_OK);
    assert(step.kind == CAMPAIGN_ACQUISITION_STEP_WINDOW_COMPLETED);
    assert(step.storage_result == TR2_OK);
    assert(system_runtime_b3_image(&runtime_a, &b3));

    start_calls_before_reboot = platform.vibration_start_calls;
    read_calls_before_reboot = platform.vibration_read_calls;
    stop_calls_before_reboot = platform.vibration_stop_calls;
    assert(start_calls_before_reboot == 1u);
    assert(read_calls_before_reboot == 2u);
    assert(stop_calls_before_reboot == 1u);

    /* Power loss / reboot: historical campaign and transaction evidence are
       recovered, but no acquisition is automatically replayed or resumed. */
    host_platform_set_reset_cause(&platform, RESET_CAUSE_POWER_ON);
    assert(system_runtime_init(&runtime_b, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime_b) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime_b));

    assert(platform.vibration_start_calls == start_calls_before_reboot);
    assert(platform.vibration_read_calls == read_calls_before_reboot);
    assert(platform.vibration_stop_calls == stop_calls_before_reboot);
    assert(!campaign_service_campaign_open(&runtime_b.campaign_service));
    assert(!campaign_service_acquisition_running(&runtime_b.campaign_service));
    assert(!system_runtime_b3_image(&runtime_b, &b3));

    assert_recovered_open_campaign(&runtime_b,
                                   campaign_id,
                                   UINT64_C(2) * TR2_CAMPAIGN_SAMPLE_RECORD_SIZE);
    assert_started_transaction_reconciled_without_replay(&runtime_b,
                                                         request.transaction_id,
                                                         campaign_id);

    /* A second reboot with no new effect must converge to exactly the same
       evidence and must still not replay START or restore live B3 state. */
    assert(system_runtime_init(&runtime_c, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime_c) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime_c));

    assert(platform.vibration_start_calls == start_calls_before_reboot);
    assert(platform.vibration_read_calls == read_calls_before_reboot);
    assert(platform.vibration_stop_calls == stop_calls_before_reboot);
    assert(!campaign_service_campaign_open(&runtime_c.campaign_service));
    assert(!campaign_service_acquisition_running(&runtime_c.campaign_service));
    assert(!system_runtime_b3_image(&runtime_c, &b3));

    assert_recovered_open_campaign(&runtime_c,
                                   campaign_id,
                                   UINT64_C(2) * TR2_CAMPAIGN_SAMPLE_RECORD_SIZE);
    assert_started_transaction_reconciled_without_replay(&runtime_c,
                                                         request.transaction_id,
                                                         campaign_id);

    return 0;
}
