#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/command_policy.h"
#include "tr2/application/system_runtime.h"
#include "tr2/platform_host/host_platform.h"

typedef struct {
    uint32_t calls;
    Tr2Result execution_result;
    SelfTestExecutionResult result;
} SelfTestTestDouble;

typedef struct {
    uint32_t calls;
    Tr2Result result;
} ResetTriggerTestDouble;

typedef struct {
    PersistentMedia base;
    uint32_t commit_calls;
    uint32_t fail_commit_call;
} FailingPersistentMediaContext;

static Tr2Result failing_media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    FailingPersistentMediaContext *media = (FailingPersistentMediaContext *)context;

    if (media == NULL || media->base.read == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    return media->base.read(media->base.context, offset, buffer, size);
}

static Tr2Result failing_media_write(void *context, uint32_t offset,
                                     const void *buffer, size_t size)
{
    FailingPersistentMediaContext *media = (FailingPersistentMediaContext *)context;

    if (media == NULL || media->base.write == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    return media->base.write(media->base.context, offset, buffer, size);
}

static Tr2Result failing_media_commit(void *context)
{
    FailingPersistentMediaContext *media = (FailingPersistentMediaContext *)context;

    if (media == NULL || media->base.commit == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    ++media->commit_calls;
    if (media->fail_commit_call != 0u &&
        media->commit_calls == media->fail_commit_call) {
        return TR2_ERROR_STORAGE;
    }
    return media->base.commit(media->base.context);
}

static Tr2Result selftest_run_standard(void *context, SelfTestExecutionResult *result)
{
    SelfTestTestDouble *test_double = (SelfTestTestDouble *)context;

    if (test_double == NULL || result == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    ++test_double->calls;
    if (test_double->execution_result != TR2_OK) {
        return test_double->execution_result;
    }
    *result = test_double->result;
    return TR2_OK;
}

static Tr2Result reset_trigger_software_reset(void *context)
{
    ResetTriggerTestDouble *test_double = (ResetTriggerTestDouble *)context;

    if (test_double == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    ++test_double->calls;
    return test_double->result;
}

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
    MonotonicClock *monotonic, WallClock *wall, ResetCauseProvider *reset,
    TimeContinuityEvidenceProvider *time_continuity, PersistentMedia *media,
    const ConfigurationValidationEnvironment *environment, VibrationSource *vibration_source,
    const SelfTestExecutor *selftest_executor, const PlatformResetTrigger *reset_trigger)
{
    SystemRuntimeDependencies deps;
    memset(&deps, 0, sizeof(deps));
    deps.monotonic_clock = monotonic;
    deps.wall_clock = wall;
    deps.reset_cause_provider = reset;
    deps.time_continuity_evidence_provider = time_continuity;
    deps.persistent_media = media;
    deps.configuration_validation_environment = environment;
    deps.vibration_source = vibration_source;
    deps.selftest_executor = selftest_executor;
    deps.reset_trigger = reset_trigger;
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
    PersistentMedia base_media;
    PersistentMedia media;
    FailingPersistentMediaContext failing_media;
    VibrationSource vibration;
    SelfTestTestDouble selftest_double = {
        0u,
        TR2_OK,
        { true, UINT16_C(0), UINT16_C(0) }
    };
    SelfTestExecutor selftest_executor = { &selftest_double, selftest_run_standard };
    ResetTriggerTestDouble reset_double = { 0u, TR2_OK };
    PlatformResetTrigger reset_trigger = { &reset_double, reset_trigger_software_reset };
    SystemRuntimeDependencies deps;
    SystemRuntime runtime;
    SystemRuntime runtime_clear_failure;
    SystemRuntime runtime_after_reset;
    SystemRuntime runtime_second_boot;
    ValidatedConfiguration validated;
    ActiveConfigurationSnapshot committed;
    CommandRequest request;
    CommandRequest collision;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandTerminalTimestamp timestamp = { false, 0u };
    CommandSnapshot command_snapshot;
    CommandBootRecoveryResult boot_recovery;
    CampaignInventoryViewSnapshot inventory;
    DiagnosticActiveFault active_fault;
    DiagnosticFaultAcknowledgement acknowledgement;
    DiagnosticSnapshot diagnostic_snapshot;
    BootIntentRecoveryResult boot_intent_recovery;
    ModbusBlock1Image b1;
    ModbusBlock5Image b5;
    uint32_t start_calls;
    uint32_t stop_calls;
    uint32_t b1_generation;

    host_platform_init(&platform);
    host_platform_set_reset_cause(&platform, RESET_CAUSE_BROWNOUT);
    monotonic = host_platform_monotonic_clock(&platform);
    wall = host_platform_wall_clock(&platform);
    reset = host_platform_reset_cause_provider(&platform);
    time_continuity = host_platform_time_continuity_evidence_provider(&platform);
    base_media = host_platform_persistent_media(&platform);
    memset(&failing_media, 0, sizeof(failing_media));
    failing_media.base = base_media;
    media.context = &failing_media;
    media.read = failing_media_read;
    media.write = failing_media_write;
    media.commit = failing_media_commit;
    vibration = host_platform_vibration_source(&platform);
    deps = make_dependencies(&monotonic, &wall, &reset, &time_continuity,
                             &media, &environment, &vibration,
                             &selftest_executor, &reset_trigger);

    assert(system_runtime_init(&runtime, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime));
    assert(!system_runtime_b1_image(&runtime, &b1));

    memset(&validated, 0, sizeof(validated));
    validated.generation = UINT32_C(1);
    validated.config_id = UINT32_C(1);
    validated.payload = valid_payload();
    assert(configuration_service_commit_validated(&runtime.configuration_service,
                                                  &validated, UINT32_C(1), &committed) == TR2_OK);

    memset(&request, 0, sizeof(request));
    request.transaction_id = UINT16_C(501);
    request.identity.command_code = COMMAND_CODE_START_ACQUISITION;
    assert(system_runtime_execute_acquisition_command(&runtime, &request, &timestamp,
                                                      &admission, &entry) == TR2_OK);
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
    assert(system_runtime_execute_acquisition_command(&runtime, &request, &timestamp,
                                                      &admission, &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_RETRY);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(platform.vibration_start_calls == start_calls);

    collision = request;
    collision.identity.command_code = COMMAND_CODE_STOP_ACQUISITION;
    stop_calls = platform.vibration_stop_calls;
    assert(system_runtime_execute_acquisition_command(&runtime, &collision, &timestamp,
                                                      &admission, &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_COLLISION);
    assert(platform.vibration_stop_calls == stop_calls);
    assert(campaign_service_campaign_open(&runtime.campaign_service));

    memset(&request, 0, sizeof(request));
    request.transaction_id = UINT16_C(502);
    request.identity.command_code = COMMAND_CODE_STOP_ACQUISITION;
    assert(system_runtime_execute_acquisition_command(&runtime, &request, &timestamp,
                                                      &admission, &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_NEW);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(entry.has_final_result);
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(!campaign_service_campaign_open(&runtime.campaign_service));
    assert(!campaign_service_acquisition_running(&runtime.campaign_service));
    assert(platform.vibration_stop_calls == stop_calls + 1u);
    assert(system_runtime_campaign_inventory_snapshot(&runtime, &inventory));
    assert(inventory.inventory.valid_campaign_count == 1u);
    assert(inventory.selected_campaign_valid);
    assert(inventory.selected_campaign.lifecycle_state == CAMPAIGN_LIFECYCLE_CLOSED);

    stop_calls = platform.vibration_stop_calls;
    assert(system_runtime_execute_acquisition_command(&runtime, &request, &timestamp,
                                                      &admission, &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_RETRY);
    assert(platform.vibration_stop_calls == stop_calls);

    memset(&active_fault, 0, sizeof(active_fault));
    active_fault.code = UINT16_C(42);
    active_fault.acknowledgeable = true;
    assert(diagnostic_service_publish_active_faults(&runtime.diagnostic_service,
                                                    &active_fault, 1u) == TR2_OK);
    memset(&request, 0, sizeof(request));
    request.transaction_id = UINT16_C(503);
    request.identity.command_code = COMMAND_CODE_ACKNOWLEDGE_FAULT;
    request.identity.param1 = UINT16_C(42);
    assert(system_runtime_execute_p9_command(&runtime, &request, &timestamp,
                                             &admission, &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_NEW);
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(diagnostic_service_fault_acknowledgement(&runtime.diagnostic_service,
                                                    UINT16_C(42), &acknowledgement));
    assert(acknowledgement.acknowledged);
    assert(system_runtime_execute_p9_command(&runtime, &request, &timestamp,
                                             &admission, &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_RETRY);

    memset(&request, 0, sizeof(request));
    request.transaction_id = UINT16_C(504);
    request.identity.command_code = COMMAND_CODE_ENTER_MAINTENANCE;
    assert(system_runtime_execute_p9_command(&runtime, &request, &timestamp,
                                             &admission, &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_NEW);
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(maintenance_service_active(&runtime.maintenance_service));
    assert(system_runtime_b5_image(&runtime, &b5));
    assert((b5.registers[13] & COMMAND_ENGINE_FLAG_MAINTENANCE_ACTIVE) != 0u);
    assert(system_runtime_execute_p9_command(&runtime, &request, &timestamp,
                                             &admission, &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_RETRY);

    memset(&request, 0, sizeof(request));
    request.transaction_id = UINT16_C(505);
    request.identity.command_code = COMMAND_CODE_EXIT_MAINTENANCE;
    assert(system_runtime_execute_p9_command(&runtime, &request, &timestamp,
                                             &admission, &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_NEW);
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(!maintenance_service_active(&runtime.maintenance_service));
    assert(system_runtime_b5_image(&runtime, &b5));
    assert((b5.registers[13] & COMMAND_ENGINE_FLAG_MAINTENANCE_ACTIVE) == 0u);

    host_platform_advance_monotonic(&platform, UINT64_C(12345));
    memset(&request, 0, sizeof(request));
    request.transaction_id = UINT16_C(506);
    request.identity.command_code = COMMAND_CODE_REFRESH_INDICATORS;
    assert(system_runtime_execute_p9_command(&runtime, &request, &timestamp,
                                             &admission, &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_NEW);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(system_runtime_b1_image(&runtime, &b1));
    assert(b1.registers[0] == UINT16_C(1));
    assert((b1.registers[1] & UINT16_C(0x0001)) != 0u);
    assert((b1.registers[1] & UINT16_C(0x0002)) == 0u);
    assert((b1.registers[1] & UINT16_C(0x0004)) != 0u);
    assert((b1.registers[1] & UINT16_C(0x0010)) != 0u);
    assert(b1.registers[4] == UINT16_C(0));
    assert(b1.registers[5] == UINT16_C(12));
    assert(b1.registers[6] == UINT16_C(4));
    assert(b1.registers[10] == UINT16_C(1));
    assert(b1.registers[12] == UINT16_C(0));
    assert(b1.registers[13] == UINT16_C(0));
    assert(b1.registers[14] == UINT16_C(0));
    b1_generation = b1.source_generation;

    host_platform_advance_monotonic(&platform, UINT64_C(5000));
    assert(system_runtime_execute_p9_command(&runtime, &request, &timestamp,
                                             &admission, &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_RETRY);
    assert(system_runtime_b1_image(&runtime, &b1));
    assert(b1.source_generation == b1_generation);
    assert(b1.registers[5] == UINT16_C(12));

    memset(&request, 0, sizeof(request));
    request.transaction_id = UINT16_C(507);
    request.identity.command_code = COMMAND_CODE_SELFTEST;
    runtime.deps.selftest_executor = NULL;
    assert(system_runtime_execute_p9_command(&runtime, &request, &timestamp,
                                             &admission, &entry) == TR2_ERROR_NOT_AVAILABLE);
    assert(!command_engine_has_active_transaction(&runtime.command_engine));
    assert(selftest_double.calls == 0u);

    runtime.deps.selftest_executor = &selftest_executor;
    assert(system_runtime_execute_p9_command(&runtime, &request, &timestamp,
                                             &admission, &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_NEW);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(entry.has_final_result);
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(entry.final_result.result_code == COMMAND_RESULT_SUCCESS);
    assert(selftest_double.calls == 1u);
    assert(diagnostic_service_snapshot(&runtime.diagnostic_service, &diagnostic_snapshot));
    assert(diagnostic_snapshot.facts.selftest.state == DIAGNOSTIC_SELFTEST_PASSED);
    assert(diagnostic_snapshot.facts.selftest.result_code == UINT16_C(0));
    assert(diagnostic_snapshot.facts.selftest.detail == UINT16_C(0));

    assert(system_runtime_execute_p9_command(&runtime, &request, &timestamp,
                                             &admission, &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_RETRY);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(selftest_double.calls == 1u);

    memset(&request, 0, sizeof(request));
    request.transaction_id = UINT16_C(508);
    request.identity.command_code = COMMAND_CODE_SOFTWARE_RESET;
    request.identity.confirm_key = TR2_COMMAND_CONFIRM_KEY_VALID;
    runtime.deps.reset_trigger = NULL;
    assert(system_runtime_execute_p9_command(&runtime, &request, &timestamp,
                                             &admission, &entry) == TR2_ERROR_NOT_AVAILABLE);
    assert(!command_engine_has_active_transaction(&runtime.command_engine));
    assert(reset_double.calls == 0u);

    runtime.deps.reset_trigger = &reset_trigger;
    assert(system_runtime_execute_p9_command(&runtime, &request, &timestamp,
                                             &admission, &entry) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_NEW);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_STARTED);
    assert(!entry.has_final_result);
    assert(entry.has_recovery_context);
    assert(entry.recovery_context.kind == COMMAND_RECOVERY_CONTEXT_BOOT_INTENT);
    assert(entry.recovery_context.value1 == UINT16_C(508));
    assert(reset_double.calls == 1u);

    assert(boot_intent_store_recover(&runtime.boot_intent_store,
                                     &boot_intent_recovery) == TR2_OK);
    assert(boot_intent_recovery.status == BOOT_INTENT_RECOVERY_VALID);
    assert(boot_intent_recovery.intent.kind == BOOT_INTENT_SOFTWARE_RESET);
    assert(boot_intent_recovery.intent.transaction_id == UINT16_C(508));

    host_platform_set_reset_cause(&platform, RESET_CAUSE_SOFTWARE);

    /* P9-N5c: fail exactly the next durable commit. On this boot the first
       commit is BootIntent consumption after reconciliation. Boot must fail
       closed, must not expose Modbus readiness, and the durable intent must
       remain available for a later boot. */
    failing_media.fail_commit_call = failing_media.commit_calls + 1u;
    assert(system_runtime_init(&runtime_clear_failure, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime_clear_failure) == TR2_ERROR_STORAGE);
    assert(!system_runtime_is_ready_for_modbus(&runtime_clear_failure));
    failing_media.fail_commit_call = 0u;
    assert(boot_intent_store_recover(&runtime.boot_intent_store,
                                     &boot_intent_recovery) == TR2_OK);
    assert(boot_intent_recovery.status == BOOT_INTENT_RECOVERY_VALID);
    assert(boot_intent_recovery.intent.transaction_id == UINT16_C(508));

    /* The next clean boot may use the still-durable evidence once, then must
       consume it successfully. */
    assert(system_runtime_init(&runtime_after_reset, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime_after_reset) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime_after_reset));
    assert(system_runtime_command_boot_recovery(&runtime_after_reset, &boot_recovery));
    assert(boot_recovery.status == COMMAND_BOOT_RECOVERY_STARTED_EFFECT_PROVEN);
    assert(boot_recovery.has_incomplete_transaction);
    assert(boot_recovery.incomplete_transaction.transaction_id == UINT16_C(508));
    assert(boot_recovery.incomplete_transaction.lifecycle == COMMAND_LIFECYCLE_STARTED);

    assert(boot_intent_store_recover(&runtime_after_reset.boot_intent_store,
                                     &boot_intent_recovery) == TR2_OK);
    assert(boot_intent_recovery.status == BOOT_INTENT_RECOVERY_EMPTY);
    assert(system_runtime_init(&runtime_second_boot, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime_second_boot) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime_second_boot));
    assert(system_runtime_command_boot_recovery(&runtime_second_boot, &boot_recovery));
    assert(boot_recovery.status == COMMAND_BOOT_RECOVERY_STARTED_INDETERMINATE);
    assert(boot_recovery.has_incomplete_transaction);
    assert(boot_recovery.incomplete_transaction.transaction_id == UINT16_C(508));

    return 0;
}
