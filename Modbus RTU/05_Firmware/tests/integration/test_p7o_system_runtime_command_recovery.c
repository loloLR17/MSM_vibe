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

static CommandRequest make_request(uint16_t transaction_id)
{
    CommandRequest request;

    memset(&request, 0, sizeof(request));
    request.transaction_id = transaction_id;
    request.identity.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    request.identity.param1 = 1u;
    request.identity.param2 = 2u;
    request.identity.param3 = UINT32_C(3);
    request.identity.confirm_key = 4u;
    return request;
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
    CommandAdmissionResult admission;
    CommandBootRecoveryResult recovery;
    CommandSnapshot snapshot;
    ModbusBlock5Image b5;
    const CommandRequest request = make_request(9u);

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
    assert(system_runtime_is_ready_for_modbus(&runtime_a));
    assert(system_runtime_command_boot_recovery(&runtime_a, &recovery));
    assert(recovery.status == COMMAND_BOOT_RECOVERY_CLEAN);
    assert(!recovery.has_incomplete_transaction);
    assert(system_runtime_b5_image(&runtime_a, &b5));
    assert(b5.registers[8] == 0u);
    assert(b5.registers[9] == 0u);

    assert(command_engine_admit(&runtime_a.command_engine, &request, &admission) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_NEW);
    assert(admission.entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);

    host_platform_set_reset_cause(&platform, RESET_CAUSE_SOFTWARE);
    assert(system_runtime_init(&runtime_b, &deps) == TR2_OK);
    assert(!system_runtime_is_ready_for_modbus(&runtime_b));
    assert(system_runtime_boot(&runtime_b) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime_b));

    assert(system_runtime_command_boot_recovery(&runtime_b, &recovery));
    assert(recovery.status == COMMAND_BOOT_RECOVERY_RESERVED_NO_EFFECT);
    assert(recovery.has_incomplete_transaction);
    assert(recovery.incomplete_transaction.transaction_id == request.transaction_id);
    assert(recovery.incomplete_transaction.lifecycle == COMMAND_LIFECYCLE_RESERVED);

    assert(command_engine_has_active_transaction(&runtime_b.command_engine));
    assert(command_engine_active_transaction_id(&runtime_b.command_engine) == request.transaction_id);
    assert(system_runtime_command_snapshot(&runtime_b, &snapshot));
    assert(snapshot.active_transaction_id == request.transaction_id);
    assert(snapshot.active_command_code == COMMAND_CODE_APPLY_CONFIGURATION);
    assert(snapshot.status == COMMAND_STATUS_ACCEPTED);

    assert(system_runtime_b5_image(&runtime_b, &b5));
    assert(b5.registers[8] == COMMAND_CODE_APPLY_CONFIGURATION);
    assert(b5.registers[9] == request.transaction_id);
    assert(b5.registers[10] == COMMAND_STATUS_ACCEPTED);

    /* The recovered incomplete transaction is restored into the engine and
       therefore blocks admission of a different new transaction. */
    {
        const CommandRequest other = make_request(10u);
        assert(command_engine_admit(&runtime_b.command_engine, &other, &admission) == TR2_OK);
        assert(admission.kind == COMMAND_ADMISSION_BUSY);
    }

    return 0;
}
