#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/system_runtime.h"
#include "tr2/modbus/read_adapter.h"
#include "tr2/platform_host/host_platform.h"

static ModbusReadOutcome read_one(const ModbusReadSources *sources,
                                  uint16_t address,
                                  uint16_t *value)
{
    *value = UINT16_C(0xBEEF);
    return modbus_read_adapter_read(sources, address, UINT16_C(1), value);
}

int main(void)
{
    HostPlatform platform;
    ConfigurationValidationEnvironment configuration_environment = { true, UINT32_C(4096) };
    MonotonicClock monotonic;
    WallClock wall;
    ResetCauseProvider reset;
    TimeContinuityEvidenceProvider time_continuity;
    PersistentMedia media;
    VibrationSource vibration;
    SystemRuntimeDependencies deps = {0};
    SystemRuntime runtime;
    IdentitySnapshot identity = {0};
    TimeSnapshot time;
    ModbusBlock0Image b0;
    ModbusBlock1Image b1;
    ModbusBlock3Image b3;
    ModbusBlock4Image b4;
    ModbusBlock5Image b5;
    ModbusBlock6Image b6;
    ModbusBlock7Image b7;
    ModbusReadSources sources = {0};
    ModbusReadOutcome outcome;
    uint16_t value;

    host_platform_init(&platform);
    monotonic = host_platform_monotonic_clock(&platform);
    wall = host_platform_wall_clock(&platform);
    reset = host_platform_reset_cause_provider(&platform);
    time_continuity = host_platform_time_continuity_evidence_provider(&platform);
    media = host_platform_persistent_media(&platform);
    vibration = host_platform_vibration_source(&platform);

    deps.monotonic_clock = &monotonic;
    deps.wall_clock = &wall;
    deps.reset_cause_provider = &reset;
    deps.time_continuity_evidence_provider = &time_continuity;
    deps.persistent_media = &media;
    deps.configuration_validation_environment = &configuration_environment;
    deps.vibration_source = &vibration;

    assert(system_runtime_init(&runtime, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime));

    /* B0 provisioning/source selection is deliberately outside A5.  The
       integration test injects an explicit logical identity fixture and
       verifies only the projection/read boundary. */
    identity.generation = UINT32_C(1);
    identity.device_id = UINT32_C(0x12345678);
    identity.hardware_version = UINT16_C(1);
    identity.firmware_version_major = UINT16_C(1);
    identity.protocol_version = UINT16_C(1);
    memcpy(identity.serial_number, "HOST-A5", 7u);
    memcpy(identity.manufacturer, "TR2", 3u);
    assert(modbus_project_b0(&identity, &b0) == TR2_OK);

    assert(system_runtime_time_snapshot(&runtime, &time));
    assert(system_runtime_b1_image(&runtime, &b1));
    assert(!system_runtime_b3_image(&runtime, &b3));
    assert(system_runtime_b4_image(&runtime, &b4));
    assert(system_runtime_b5_image(&runtime, &b5));
    assert(system_runtime_b6_image(&runtime, &b6));
    assert(system_runtime_b7_image(&runtime, &b7));

    sources.b0_image = &b0;
    sources.time = &time;
    sources.b1_image = &b1;
    sources.b3_image = NULL;
    sources.b4_image = &b4;
    sources.b5_image = &b5;
    sources.b6_image = &b6;
    sources.b7_image = &b7;

    outcome = read_one(&sources, UINT16_C(0), &value);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(value == UINT16_C(0x1234));

    outcome = read_one(&sources, UINT16_C(1000), &value);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);

    /* Host boot exposes the runtime TimeSnapshot, but B2 remains unavailable
       until authoritative current-time and synchronization facts exist. */
    outcome = read_one(&sources, UINT16_C(2000), &value);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_NOT_AVAILABLE);
    assert(value == UINT16_C(0xBEEF));

    outcome = read_one(&sources, UINT16_C(3000), &value);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_NOT_AVAILABLE);
    assert(value == UINT16_C(0xBEEF));

    outcome = read_one(&sources, UINT16_C(4000), &value);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);

    outcome = read_one(&sources, UINT16_C(5000), &value);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);

    outcome = read_one(&sources, UINT16_C(6000), &value);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);

    outcome = read_one(&sources, UINT16_C(7000), &value);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);

    return 0;
}