#include <stdio.h>
#include "tr2/application/system_runtime.h"
#include "tr2/platform_host/host_platform.h"

int main(void)
{
    HostPlatform platform;
    host_platform_init(&platform);

    MonotonicClock monotonic = host_platform_monotonic_clock(&platform);
    WallClock wall = host_platform_wall_clock(&platform);
    ResetCauseProvider reset = host_platform_reset_cause_provider(&platform);
    PersistentMedia media = host_platform_persistent_media(&platform);

    SystemRuntimeDependencies deps = { &monotonic, &wall, &reset, &media };
    SystemRuntime runtime;

    if (system_runtime_init(&runtime, &deps) != TR2_OK ||
        system_runtime_boot(&runtime) != TR2_OK ||
        !system_runtime_is_ready_for_modbus(&runtime)) {
        return 1;
    }

    printf("TR2 P0 host runtime ready\n");
    return 0;
}
