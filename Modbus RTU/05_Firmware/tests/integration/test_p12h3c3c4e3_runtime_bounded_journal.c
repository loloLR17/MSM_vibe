#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/system_runtime.h"
#include "tr2/persistence/command_journal_bounded_slot.h"
#include "tr2/platform_host/host_platform.h"

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

    memset(&deps, 0, sizeof(deps));
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
    CommandRequest request;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandJournalBoundedSlotSelection selection;

    host_platform_init(&platform);
    monotonic = host_platform_monotonic_clock(&platform);
    wall = host_platform_wall_clock(&platform);
    reset = host_platform_reset_cause_provider(&platform);
    time_continuity = host_platform_time_continuity_evidence_provider(&platform);
    media = host_platform_persistent_media(&platform);
    vibration = host_platform_vibration_source(&platform);
    deps = make_dependencies(&monotonic, &wall, &reset, &time_continuity,
                             &media, &environment, &vibration);

    assert(system_runtime_init(&runtime_a, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime_a) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime_a));
    assert(runtime_a.command_journal_recovery.status ==
           COMMAND_JOURNAL_BOUNDED_RECOVERY_EMPTY);
    assert(runtime_a.command_journal_media_region.size ==
           TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE);
    assert(runtime_a.command_engine.journal ==
           command_journal_bounded_store_journal(&runtime_a.command_journal_store));

    memset(&request, 0, sizeof(request));
    request.transaction_id = UINT16_C(65000);
    request.identity.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    assert(command_engine_admit(&runtime_a.command_engine, &request, &admission) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_NEW);
    assert(admission.entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);

    assert(command_journal_bounded_slot_select(
               &runtime_a.command_journal_storage_core, 0u, &selection) == TR2_OK);
    assert(selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_VALID);
    assert(selection.has_record);
    assert(selection.record.entry.transaction_id == request.transaction_id);
    assert(selection.record.entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);
    assert(selection.record.admission_order == 1u);
    assert(selection.record.generation == 1u);

    host_platform_set_reset_cause(&platform, RESET_CAUSE_POWER_ON);
    assert(system_runtime_init(&runtime_b, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime_b) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime_b));
    assert(runtime_b.command_journal_recovery.status ==
           COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID);
    assert(runtime_b.command_journal_recovery.known_transaction_count == 1u);
    assert(runtime_b.command_journal_recovery.next_admission_order == 2u);
    assert(runtime_b.command_boot_recovery.status ==
           COMMAND_BOOT_RECOVERY_RESERVED_NO_EFFECT);
    assert(runtime_b.command_boot_recovery.has_incomplete_transaction);
    assert(runtime_b.command_boot_recovery.incomplete_transaction.transaction_id ==
           request.transaction_id);

    assert(command_engine_admit(&runtime_b.command_engine, &request, &admission) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_RETRY);
    assert(admission.entry.transaction_id == request.transaction_id);

    request.identity.command_code = COMMAND_CODE_STOP_ACQUISITION;
    assert(command_engine_admit(&runtime_b.command_engine, &request, &admission) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_COLLISION);
    assert(admission.entry.transaction_id == request.transaction_id);

    assert(runtime_b.command_engine.journal->find(
               runtime_b.command_engine.journal->context,
               UINT16_C(65000),
               &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);

    return 0;
}
