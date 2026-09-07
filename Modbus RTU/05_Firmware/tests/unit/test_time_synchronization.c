#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/domain/time/time_service.h"
#include "tr2/persistence/persistent_storage_core.h"
#include "tr2/persistence/time_history_store.h"

#define TEST_MEDIA_SIZE 64u
#define TEST_TIME_OFFSET UINT32_C(8)

typedef struct {
    Tr2CivilTimestamp current_time;
    WallClockReadResult read_result;
    Tr2Result set_result;
    uint32_t set_calls;
} TestWallClock;

typedef struct {
    MonotonicTimeMs now_ms;
} TestMonotonicClock;

typedef struct {
    uint8_t durable[TEST_MEDIA_SIZE];
    uint8_t staged[TEST_MEDIA_SIZE];
    bool fail_write;
    bool fail_commit;
} TestMedia;

static WallClockReadResult wall_read(void *context, Tr2CivilTimestamp *timestamp)
{
    TestWallClock *clock = (TestWallClock *)context;
    *timestamp = clock->current_time;
    return clock->read_result;
}

static Tr2Result wall_set(void *context, Tr2CivilTimestamp timestamp)
{
    TestWallClock *clock = (TestWallClock *)context;
    clock->set_calls++;
    if (clock->set_result != TR2_OK) {
        return clock->set_result;
    }
    clock->current_time = timestamp;
    return TR2_OK;
}

static MonotonicTimeMs monotonic_now(void *context)
{
    return ((TestMonotonicClock *)context)->now_ms;
}

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    if ((size_t)offset + size > TEST_MEDIA_SIZE) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(buffer, &media->durable[offset], size);
    return TR2_OK;
}

static Tr2Result media_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    if (media->fail_write || (size_t)offset + size > TEST_MEDIA_SIZE) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(&media->staged[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    TestMedia *media = (TestMedia *)context;
    if (media->fail_commit) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(media->durable, media->staged, sizeof(media->durable));
    return TR2_OK;
}

int main(void)
{
    TestWallClock wall = { UINT32_C(100), WALL_CLOCK_OK, TR2_OK, 0u };
    TestMonotonicClock monotonic = { UINT64_C(5000) };
    TestMedia media;
    WallClock wall_clock = { &wall, wall_read, wall_set };
    MonotonicClock monotonic_clock = { &monotonic, monotonic_now };
    PersistentMedia persistent_media = { &media, media_read, media_write, media_commit };
    PersistentStorageCore storage = {0};
    TimeHistoryStore store = {0};
    TimeService service = {0};
    TimeSnapshot snapshot;
    TimeHistoryRecoveryResult recovery;
    bool prepared_available;
    Tr2CivilTimestamp prepared_time;

    memset(&media, UINT8_C(0xFF), sizeof(media));
    media.fail_write = false;
    media.fail_commit = false;

    assert(persistent_storage_core_init(&storage, &persistent_media) == TR2_OK);
    assert(time_history_store_init(&store, &storage, TEST_TIME_OFFSET) == TR2_OK);
    assert(time_service_init(&service, &wall_clock) == TR2_OK);

    assert(time_service_synchronize_prepared(&service, UINT16_C(7)) == TR2_ERROR_INVALID_STATE);
    assert(time_service_bind_synchronization_dependencies(&service, &monotonic_clock, &store) == TR2_OK);
    assert(time_service_synchronize_prepared(&service, UINT16_C(7)) == TR2_ERROR_INVALID_STATE);

    assert(time_service_prepare_time(&service, UINT32_C(1000)) == TR2_OK);
    assert(time_service_synchronize_prepared(&service, UINT16_C(7)) == TR2_OK);
    assert(wall.set_calls == 1u);
    assert(wall.current_time == UINT32_C(1000));

    assert(time_history_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == TIME_HISTORY_RECOVERY_VALID);
    assert(recovery.history.state == LAST_SYNC_HISTORY_VALID);
    assert(recovery.history.timestamp == UINT32_C(1000));
    assert(recovery.history.source == UINT16_C(7));

    assert(time_service_get_prepared_time(&service, &prepared_available, &prepared_time) == TR2_OK);
    assert(!prepared_available);
    assert(prepared_time == 0u);

    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.civil_time_usable);
    assert(snapshot.current_time_available);
    assert(snapshot.current_time == UINT32_C(1000));
    assert(snapshot.continuity == TIME_CONTINUITY_PROVEN);
    assert(snapshot.synchronization_facts_available);
    assert(snapshot.last_sync_time == UINT32_C(1000));
    assert(snapshot.sync_source == UINT16_C(7));
    assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_AVAILABLE);
    assert(snapshot.time_since_sync.duration_s == 0u);

    monotonic.now_ms = UINT64_C(12500);
    wall.current_time = UINT32_C(4000);
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_AVAILABLE);
    assert(snapshot.time_since_sync.duration_s == UINT32_C(7));

    wall.read_result = WALL_CLOCK_UNAVAILABLE;
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(!snapshot.current_time_available);
    assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_AVAILABLE);
    assert(snapshot.time_since_sync.duration_s == UINT32_C(7));

    wall.read_result = WALL_CLOCK_OK;
    assert(time_service_prepare_time(&service, UINT32_C(2000)) == TR2_OK);
    wall.set_result = TR2_ERROR_UNAVAILABLE;
    assert(time_service_synchronize_prepared(&service, UINT16_C(8)) == TR2_ERROR_UNAVAILABLE);
    assert(time_service_get_prepared_time(&service, &prepared_available, &prepared_time) == TR2_OK);
    assert(prepared_available);
    assert(prepared_time == UINT32_C(2000));
    assert(time_history_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.history.timestamp == UINT32_C(1000));
    assert(recovery.history.source == UINT16_C(7));

    wall.set_result = TR2_OK;
    media.fail_commit = true;
    assert(time_service_synchronize_prepared(&service, UINT16_C(8)) == TR2_ERROR_STORAGE);
    assert(wall.current_time == UINT32_C(2000));
    assert(time_history_store_recovery_required(&store));
    assert(time_service_get_prepared_time(&service, &prepared_available, &prepared_time) == TR2_OK);
    assert(prepared_available);
    assert(prepared_time == UINT32_C(2000));
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.last_sync_time == UINT32_C(1000));
    assert(snapshot.sync_source == UINT16_C(7));

    return 0;
}
