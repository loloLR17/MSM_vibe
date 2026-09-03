#include <string.h>
#include "tr2/platform_host/host_platform.h"

static MonotonicTimeMs host_now_ms(void *context)
{
    HostPlatform *platform = (HostPlatform *)context;
    return platform->monotonic_ms;
}

static WallClockReadResult host_wall_read(void *context, Tr2CivilTimestamp *timestamp)
{
    HostPlatform *platform = (HostPlatform *)context;
    if (timestamp == NULL) {
        return WALL_CLOCK_INVALID;
    }
    if (!platform->civil_time_valid) {
        *timestamp = 0u;
        return WALL_CLOCK_UNAVAILABLE;
    }
    *timestamp = platform->civil_time;
    return WALL_CLOCK_OK;
}

static Tr2Result host_wall_set(void *context, Tr2CivilTimestamp timestamp)
{
    HostPlatform *platform = (HostPlatform *)context;
    platform->civil_time = timestamp;
    platform->civil_time_valid = true;
    return TR2_OK;
}

static ResetCause host_reset_cause_get(void *context)
{
    HostPlatform *platform = (HostPlatform *)context;
    return platform->reset_cause;
}

static bool host_media_range_valid(uint32_t offset, size_t size)
{
    return offset <= HOST_PLATFORM_PERSISTENT_BYTES &&
           size <= HOST_PLATFORM_PERSISTENT_BYTES - offset;
}

static Tr2Result host_media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    HostPlatform *platform = (HostPlatform *)context;
    if (buffer == NULL || !host_media_range_valid(offset, size)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memcpy(buffer, &platform->persistent_committed[offset], size);
    return TR2_OK;
}

static Tr2Result host_media_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    HostPlatform *platform = (HostPlatform *)context;
    if (buffer == NULL || !host_media_range_valid(offset, size)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memcpy(&platform->persistent_candidate[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result host_media_commit(void *context)
{
    HostPlatform *platform = (HostPlatform *)context;
    memcpy(platform->persistent_committed,
           platform->persistent_candidate,
           HOST_PLATFORM_PERSISTENT_BYTES);
    return TR2_OK;
}

void host_platform_init(HostPlatform *platform)
{
    if (platform == NULL) {
        return;
    }
    memset(platform, 0, sizeof(*platform));
    platform->reset_cause = RESET_CAUSE_POWER_ON;
}

void host_platform_set_reset_cause(HostPlatform *platform, ResetCause cause)
{
    if (platform != NULL) {
        platform->reset_cause = cause;
    }
}

void host_platform_advance_monotonic(HostPlatform *platform, MonotonicTimeMs delta_ms)
{
    if (platform != NULL) {
        platform->monotonic_ms += delta_ms;
    }
}

MonotonicClock host_platform_monotonic_clock(HostPlatform *platform)
{
    MonotonicClock clock = { platform, host_now_ms };
    return clock;
}

WallClock host_platform_wall_clock(HostPlatform *platform)
{
    WallClock clock = { platform, host_wall_read, host_wall_set };
    return clock;
}

ResetCauseProvider host_platform_reset_cause_provider(HostPlatform *platform)
{
    ResetCauseProvider provider = { platform, host_reset_cause_get };
    return provider;
}

PersistentMedia host_platform_persistent_media(HostPlatform *platform)
{
    PersistentMedia media = { platform, host_media_read, host_media_write, host_media_commit };
    return media;
}
