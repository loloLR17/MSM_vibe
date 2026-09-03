#include <assert.h>
#include <stdint.h>
#include "tr2/application/system_runtime.h"
#include "tr2/platform_host/host_platform.h"

int main(void)
{
    HostPlatform platform;
    host_platform_init(&platform);
    host_platform_set_reset_cause(&platform, RESET_CAUSE_SOFTWARE);
    host_platform_advance_monotonic(&platform, UINT64_C(42));

    MonotonicClock monotonic = host_platform_monotonic_clock(&platform);
    WallClock wall = host_platform_wall_clock(&platform);
    ResetCauseProvider reset = host_platform_reset_cause_provider(&platform);
    PersistentMedia media = host_platform_persistent_media(&platform);
    SystemRuntimeDependencies deps = { &monotonic, &wall, &reset, &media };
    SystemRuntime runtime;

    assert(system_runtime_init(&runtime, &deps) == TR2_OK);
    assert(!system_runtime_is_ready_for_modbus(&runtime));
    assert(system_runtime_boot(&runtime) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime));
    assert(system_runtime_boot_context(&runtime) != NULL);
    assert(system_runtime_boot_context(&runtime)->reset_cause == RESET_CAUSE_SOFTWARE);

    return 0;
}
