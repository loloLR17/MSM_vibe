#include <stdlib.h>
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

static TimeContinuityEvidence host_time_continuity_get(void *context)
{
    HostPlatform *platform = (HostPlatform *)context;
    return platform->time_continuity_evidence;
}

static bool host_media_range_valid(uint32_t offset, size_t size)
{
    return offset <= HOST_PLATFORM_PERSISTENT_BYTES &&
           size <= HOST_PLATFORM_PERSISTENT_BYTES - offset;
}

static Tr2Result host_media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    HostPlatform *platform = (HostPlatform *)context;
    if (buffer == NULL || platform == NULL || platform->persistent_committed == NULL ||
        !host_media_range_valid(offset, size)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memcpy(buffer, &platform->persistent_committed[offset], size);
    return TR2_OK;
}

static Tr2Result host_media_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    HostPlatform *platform = (HostPlatform *)context;
    if (buffer == NULL || platform == NULL || platform->persistent_candidate == NULL ||
        !host_media_range_valid(offset, size)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memcpy(&platform->persistent_candidate[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result host_media_commit(void *context)
{
    HostPlatform *platform = (HostPlatform *)context;
    if (platform == NULL || platform->persistent_committed == NULL ||
        platform->persistent_candidate == NULL) {
        return TR2_ERROR_INVALID_STATE;
    }
    memcpy(platform->persistent_committed,
           platform->persistent_candidate,
           HOST_PLATFORM_PERSISTENT_BYTES);
    return TR2_OK;
}

static Tr2Result host_vibration_configure(
    void *context,
    const VibrationSourceConfiguration *configuration)
{
    HostPlatform *platform = (HostPlatform *)context;

    if (platform == NULL || configuration == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    platform->vibration_configure_calls += 1u;
    return TR2_OK;
}

static Tr2Result host_vibration_start(void *context)
{
    HostPlatform *platform = (HostPlatform *)context;

    if (platform == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    platform->vibration_start_calls += 1u;
    platform->vibration_started = true;
    return TR2_OK;
}

static Tr2Result host_vibration_read(void *context, VibrationSample *sample)
{
    HostPlatform *platform = (HostPlatform *)context;

    if (platform == NULL || sample == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!platform->vibration_started) {
        return TR2_ERROR_INVALID_STATE;
    }

    platform->vibration_read_calls += 1u;
    memset(sample, 0, sizeof(*sample));
    sample->valid = true;
    return TR2_OK;
}

static Tr2Result host_vibration_stop(void *context)
{
    HostPlatform *platform = (HostPlatform *)context;

    if (platform == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    platform->vibration_stop_calls += 1u;
    platform->vibration_started = false;
    return TR2_OK;
}

void host_platform_init(HostPlatform *platform)
{
    if (platform == NULL) {
        return;
    }
    memset(platform, 0, sizeof(*platform));
    platform->persistent_committed =
        (uint8_t *)calloc(HOST_PLATFORM_PERSISTENT_BYTES, sizeof(uint8_t));
    platform->persistent_candidate =
        (uint8_t *)calloc(HOST_PLATFORM_PERSISTENT_BYTES, sizeof(uint8_t));
    if (platform->persistent_committed == NULL || platform->persistent_candidate == NULL) {
        free(platform->persistent_committed);
        free(platform->persistent_candidate);
        platform->persistent_committed = NULL;
        platform->persistent_candidate = NULL;
        return;
    }
    platform->reset_cause = RESET_CAUSE_POWER_ON;
    platform->time_continuity_evidence = TIME_CONTINUITY_EVIDENCE_INDETERMINATE;
}

void host_platform_set_reset_cause(HostPlatform *platform, ResetCause cause)
{
    if (platform != NULL) {
        platform->reset_cause = cause;
    }
}

void host_platform_set_time_continuity_evidence(HostPlatform *platform,
                                                TimeContinuityEvidence evidence)
{
    if (platform != NULL) {
        platform->time_continuity_evidence = evidence;
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

TimeContinuityEvidenceProvider host_platform_time_continuity_evidence_provider(HostPlatform *platform)
{
    TimeContinuityEvidenceProvider provider = { platform, host_time_continuity_get };
    return provider;
}

PersistentMedia host_platform_persistent_media(HostPlatform *platform)
{
    PersistentMedia media = { platform, host_media_read, host_media_write, host_media_commit };
    return media;
}

VibrationSource host_platform_vibration_source(HostPlatform *platform)
{
    VibrationSource source = {
        platform,
        host_vibration_configure,
        host_vibration_start,
        host_vibration_read,
        host_vibration_stop
    };
    return source;
}
