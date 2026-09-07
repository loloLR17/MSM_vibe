#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/system_runtime.h"
#include "tr2/persistence/configuration_store.h"
#include "tr2/platform_host/host_platform.h"

#define TIME_HISTORY_OFFSET ((uint32_t)TR2_CONFIGURATION_STORE_STORAGE_SIZE)

typedef struct {
    HostPlatform *platform;
    bool fail_time_read;
} TestMediaContext;

typedef struct {
    WallClockReadResult result;
    Tr2CivilTimestamp timestamp;
} TestWallContext;

static bool range_valid(uint32_t offset, size_t size)
{
    return offset <= HOST_PLATFORM_PERSISTENT_BYTES &&
           size <= HOST_PLATFORM_PERSISTENT_BYTES - offset;
}

static Tr2Result test_media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestMediaContext *media = (TestMediaContext *)context;

    if (buffer == NULL || !range_valid(offset, size)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (media->fail_time_read && offset == TIME_HISTORY_OFFSET) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(buffer, &media->platform->persistent_committed[offset], size);
    return TR2_OK;
}

static Tr2Result test_media_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    TestMediaContext *media = (TestMediaContext *)context;

    if (buffer == NULL || !range_valid(offset, size)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memcpy(&media->platform->persistent_candidate[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result test_media_commit(void *context)
{
    TestMediaContext *media = (TestMediaContext *)context;

    memcpy(media->platform->persistent_committed,
           media->platform->persistent_candidate,
           HOST_PLATFORM_PERSISTENT_BYTES);
    return TR2_OK;
}

static WallClockReadResult test_wall_read(void *context, Tr2CivilTimestamp *timestamp)
{
    TestWallContext *wall = (TestWallContext *)context;

    if (timestamp == NULL) {
        return WALL_CLOCK_INVALID;
    }
    *timestamp = wall->timestamp;
    return wall->result;
}

static Tr2Result test_wall_set(void *context, Tr2CivilTimestamp timestamp)
{
    TestWallContext *wall = (TestWallContext *)context;

    wall->timestamp = timestamp;
    wall->result = WALL_CLOCK_OK;
    return TR2_OK;
}

static SystemRuntimeDependencies make_dependencies(
    MonotonicClock *monotonic,
    WallClock *wall,
    ResetCauseProvider *reset,
    TimeContinuityEvidenceProvider *continuity,
    PersistentMedia *media,
    const ConfigurationValidationEnvironment *environment)
{
    SystemRuntimeDependencies deps;

    deps.monotonic_clock = monotonic;
    deps.wall_clock = wall;
    deps.reset_cause_provider = reset;
    deps.time_continuity_evidence_provider = continuity;
    deps.persistent_media = media;
    deps.configuration_validation_environment = environment;
    return deps;
}

static void persist_history(PersistentMedia *media,
                            Tr2CivilTimestamp timestamp,
                            uint16_t source)
{
    PersistentStorageCore storage = { 0 };
    TimeHistoryStore store = { 0 };
    LastSyncHistory history = time_last_sync_history_valid(timestamp, source);

    assert(persistent_storage_core_init(&storage, media) == TR2_OK);
    assert(time_history_store_init(&store, &storage, TIME_HISTORY_OFFSET) == TR2_OK);
    assert(time_history_store_commit(&store, &history) == TR2_OK);
}

int main(void)
{
    const ConfigurationValidationEnvironment environment = { true, UINT32_C(4096) };
    HostPlatform platform;
    MonotonicClock monotonic;
    WallClock wall;
    ResetCauseProvider reset;
    TimeContinuityEvidenceProvider continuity;
    PersistentMedia media;
    SystemRuntimeDependencies deps;
    SystemRuntime runtime;
    TimeSnapshot snapshot;
    TimeHistoryRecoveryStatus history_status;

    /* EMPTY history + unavailable RTC: boot remains globally available. */
    host_platform_init(&platform);
    monotonic = host_platform_monotonic_clock(&platform);
    wall = host_platform_wall_clock(&platform);
    reset = host_platform_reset_cause_provider(&platform);
    continuity = host_platform_time_continuity_evidence_provider(&platform);
    media = host_platform_persistent_media(&platform);
    deps = make_dependencies(&monotonic, &wall, &reset, &continuity, &media, &environment);
    assert(system_runtime_init(&runtime, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime));
    assert(system_runtime_time_history_recovery_status(&runtime, &history_status));
    assert(history_status == TIME_HISTORY_RECOVERY_EMPTY);
    assert(system_runtime_time_snapshot(&runtime, &snapshot));
    assert(!snapshot.civil_time_usable);
    assert(snapshot.continuity == TIME_CONTINUITY_INDETERMINATE);
    assert(snapshot.last_sync_history.state == LAST_SYNC_HISTORY_NONE);
    assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_UNAVAILABLE);

    /* Durable history + positive platform proof permits post-boot reconstruction. */
    host_platform_init(&platform);
    monotonic = host_platform_monotonic_clock(&platform);
    wall = host_platform_wall_clock(&platform);
    reset = host_platform_reset_cause_provider(&platform);
    continuity = host_platform_time_continuity_evidence_provider(&platform);
    media = host_platform_persistent_media(&platform);
    persist_history(&media, UINT32_C(900), UINT16_C(2));
    assert(wall.set(wall.context, UINT32_C(1000)) == TR2_OK);
    host_platform_set_time_continuity_evidence(&platform, TIME_CONTINUITY_EVIDENCE_PROVEN);
    deps = make_dependencies(&monotonic, &wall, &reset, &continuity, &media, &environment);
    assert(system_runtime_init(&runtime, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime) == TR2_OK);
    assert(system_runtime_time_history_recovery_status(&runtime, &history_status));
    assert(history_status == TIME_HISTORY_RECOVERY_VALID);
    assert(system_runtime_time_snapshot(&runtime, &snapshot));
    assert(snapshot.civil_time_usable);
    assert(snapshot.continuity == TIME_CONTINUITY_PROVEN);
    assert(snapshot.last_sync_history.state == LAST_SYNC_HISTORY_VALID);
    assert(snapshot.last_sync_history.timestamp == UINT32_C(900));
    assert(snapshot.last_sync_history.source == UINT16_C(2));
    assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_AVAILABLE);
    assert(snapshot.time_since_sync.duration_s == UINT32_C(100));

    /* The same durable history survives BROKEN and INDETERMINATE continuity. */
    host_platform_set_time_continuity_evidence(&platform, TIME_CONTINUITY_EVIDENCE_BROKEN);
    assert(system_runtime_init(&runtime, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime) == TR2_OK);
    assert(system_runtime_time_snapshot(&runtime, &snapshot));
    assert(snapshot.continuity == TIME_CONTINUITY_BROKEN);
    assert(snapshot.last_sync_history.state == LAST_SYNC_HISTORY_VALID);
    assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_UNAVAILABLE);

    host_platform_set_time_continuity_evidence(&platform, TIME_CONTINUITY_EVIDENCE_INDETERMINATE);
    assert(system_runtime_init(&runtime, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime) == TR2_OK);
    assert(system_runtime_time_snapshot(&runtime, &snapshot));
    assert(snapshot.continuity == TIME_CONTINUITY_INDETERMINATE);
    assert(snapshot.last_sync_history.state == LAST_SYNC_HISTORY_VALID);
    assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_UNAVAILABLE);

    /* Corrupted TimeHistory is contained and does not become historical authority. */
    platform.persistent_committed[TIME_HISTORY_OFFSET] ^= UINT8_C(0x01);
    platform.persistent_candidate[TIME_HISTORY_OFFSET] =
        platform.persistent_committed[TIME_HISTORY_OFFSET];
    assert(system_runtime_init(&runtime, &deps) == TR2_OK);
    assert(system_runtime_boot(&runtime) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(&runtime));
    assert(system_runtime_time_history_recovery_status(&runtime, &history_status));
    assert(history_status == TIME_HISTORY_RECOVERY_CORRUPTED);
    assert(system_runtime_time_snapshot(&runtime, &snapshot));
    assert(snapshot.last_sync_history.state == LAST_SYNC_HISTORY_NONE);

    /* A TimeHistory read failure is distinct and still does not block global boot. */
    {
        TestMediaContext media_context = { &platform, true };
        PersistentMedia failing_media = {
            &media_context,
            test_media_read,
            test_media_write,
            test_media_commit
        };

        deps = make_dependencies(&monotonic,
                                 &wall,
                                 &reset,
                                 &continuity,
                                 &failing_media,
                                 &environment);
        assert(system_runtime_init(&runtime, &deps) == TR2_OK);
        assert(system_runtime_boot(&runtime) == TR2_OK);
        assert(system_runtime_is_ready_for_modbus(&runtime));
        assert(system_runtime_time_history_recovery_status(&runtime, &history_status));
        assert(history_status == TIME_HISTORY_RECOVERY_UNAVAILABLE);
        assert(system_runtime_time_snapshot(&runtime, &snapshot));
        assert(snapshot.last_sync_history.state == LAST_SYNC_HISTORY_NONE);
    }

    /* Explicitly invalid WallClock evidence dominates a contradictory PROVEN hint. */
    {
        TestWallContext wall_context = { WALL_CLOCK_INVALID, UINT32_C(1000) };
        WallClock invalid_wall = { &wall_context, test_wall_read, test_wall_set };

        host_platform_init(&platform);
        monotonic = host_platform_monotonic_clock(&platform);
        reset = host_platform_reset_cause_provider(&platform);
        continuity = host_platform_time_continuity_evidence_provider(&platform);
        media = host_platform_persistent_media(&platform);
        persist_history(&media, UINT32_C(900), UINT16_C(3));
        host_platform_set_time_continuity_evidence(&platform, TIME_CONTINUITY_EVIDENCE_PROVEN);
        deps = make_dependencies(&monotonic,
                                 &invalid_wall,
                                 &reset,
                                 &continuity,
                                 &media,
                                 &environment);
        assert(system_runtime_init(&runtime, &deps) == TR2_OK);
        assert(system_runtime_boot(&runtime) == TR2_OK);
        assert(system_runtime_is_ready_for_modbus(&runtime));
        assert(system_runtime_time_snapshot(&runtime, &snapshot));
        assert(!snapshot.civil_time_usable);
        assert(snapshot.continuity == TIME_CONTINUITY_BROKEN);
        assert(snapshot.last_sync_history.state == LAST_SYNC_HISTORY_VALID);
        assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_UNAVAILABLE);
    }

    return 0;
}

#undef TIME_HISTORY_OFFSET
