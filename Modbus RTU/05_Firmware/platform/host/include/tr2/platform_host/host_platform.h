#ifndef TR2_PLATFORM_HOST_HOST_PLATFORM_H
#define TR2_PLATFORM_HOST_HOST_PLATFORM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "tr2/platform/monotonic_clock.h"
#include "tr2/platform/persistent_media.h"
#include "tr2/platform/reset_cause_provider.h"
#include "tr2/platform/wall_clock.h"

#define HOST_PLATFORM_PERSISTENT_BYTES 1024u

typedef struct {
    MonotonicTimeMs monotonic_ms;
    Tr2CivilTimestamp civil_time;
    bool civil_time_valid;
    ResetCause reset_cause;
    uint8_t persistent_committed[HOST_PLATFORM_PERSISTENT_BYTES];
    uint8_t persistent_candidate[HOST_PLATFORM_PERSISTENT_BYTES];
} HostPlatform;

void host_platform_init(HostPlatform *platform);
void host_platform_set_reset_cause(HostPlatform *platform, ResetCause cause);
void host_platform_advance_monotonic(HostPlatform *platform, MonotonicTimeMs delta_ms);

MonotonicClock host_platform_monotonic_clock(HostPlatform *platform);
WallClock host_platform_wall_clock(HostPlatform *platform);
ResetCauseProvider host_platform_reset_cause_provider(HostPlatform *platform);
PersistentMedia host_platform_persistent_media(HostPlatform *platform);

#endif
