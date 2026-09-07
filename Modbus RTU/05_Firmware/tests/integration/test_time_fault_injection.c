#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/domain/time/time_service.h"
#include "tr2/persistence/persistent_storage_core.h"
#include "tr2/persistence/time_history_store.h"

#define TEST_MEDIA_SIZE 64u
#define TEST_TIME_OFFSET UINT32_C(8)

typedef enum {
    MEDIA_FAULT_NONE = 0,
    MEDIA_FAULT_PARTIAL_WRITE,
    MEDIA_FAULT_COMMIT
} MediaFault;

typedef struct {
    uint8_t durable[TEST_MEDIA_SIZE];
    uint8_t candidate[TEST_MEDIA_SIZE];
    MediaFault fault;
} FaultMedia;

typedef struct {
    Tr2CivilTimestamp current_time;
    WallClockReadResult read_result;
} TestWallClock;

typedef struct {
    MonotonicTimeMs now_ms;
} TestMonotonicClock;

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    FaultMedia *media = (FaultMedia *)context;

    if (buffer == NULL || (size_t)offset + size > TEST_MEDIA_SIZE) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(buffer, &media->durable[offset], size);
    return TR2_OK;
}

static Tr2Result media_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    FaultMedia *media = (FaultMedia *)context;
    size_t bytes_to_write = size;

    if (buffer == NULL || (size_t)offset + size > TEST_MEDIA_SIZE) {
        return TR2_ERROR_STORAGE;
    }
    if (media->fault == MEDIA_FAULT_PARTIAL_WRITE) {
        bytes_to_write = size / 2u;
    }
    memcpy(&media->candidate[offset], buffer, bytes_to_write);
    return media->fault == MEDIA_FAULT_PARTIAL_WRITE ? TR2_ERROR_STORAGE : TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    FaultMedia *media = (FaultMedia *)context;

    if (media->fault == MEDIA_FAULT_COMMIT) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(media->durable, media->candidate, sizeof(media->durable));
    return TR2_OK;
}

static WallClockReadResult wall_read(void *context, Tr2CivilTimestamp *timestamp)
{
    TestWallClock *clock = (TestWallClock *)context;

    *timestamp = clock->current_time;
    return clock->read_result;
}

static Tr2Result wall_set(void *context, Tr2CivilTimestamp timestamp)
{
    TestWallClock *clock = (TestWallClock *)context;

    clock->current_time = timestamp;
    clock->read_result = WALL_CLOCK_OK;
    return TR2_OK;
}

static MonotonicTimeMs monotonic_now(void *context)
{
    return ((TestMonotonicClock *)context)->now_ms;
}

static void init_store(FaultMedia *media,
                       PersistentStorageCore *storage,
                       TimeHistoryStore *store,
                       PersistentMedia *persistent_media)
{
    *persistent_media = (PersistentMedia){ media, media_read, media_write, media_commit };
    memset(storage, 0, sizeof(*storage));
    memset(store, 0, sizeof(*store));
    assert(persistent_storage_core_init(storage, persistent_media) == TR2_OK);
    assert(time_history_store_init(store, storage, TEST_TIME_OFFSET) == TR2_OK);
}

static void assert_recovered_history(TimeHistoryStore *store,
                                     Tr2CivilTimestamp timestamp,
                                     uint16_t source)
{
    TimeHistoryRecoveryResult recovery;

    assert(time_history_store_recover(store, &recovery) == TR2_OK);
    assert(recovery.status == TIME_HISTORY_RECOVERY_VALID);
    assert(recovery.history.state == LAST_SYNC_HISTORY_VALID);
    assert(recovery.history.timestamp == timestamp);
    assert(recovery.history.source == source);
}

int main(void)
{
    FaultMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore storage;
    TimeHistoryStore store;
    LastSyncHistory history_a = time_last_sync_history_valid(UINT32_C(1000), UINT16_C(7));
    LastSyncHistory history_b = time_last_sync_history_valid(UINT32_C(2000), UINT16_C(8));
    LastSyncHistory history_c = time_last_sync_history_valid(UINT32_C(3000), UINT16_C(9));
    TestWallClock wall = { UINT32_C(1000), WALL_CLOCK_OK };
    TestMonotonicClock monotonic = { UINT64_C(5000) };
    WallClock wall_clock = { &wall, wall_read, wall_set };
    MonotonicClock monotonic_clock = { &monotonic, monotonic_now };
    TimeService service;
    TimeSnapshot snapshot;
    TimeRecoveryContext recovery_context;

    memset(&media, UINT8_C(0xFF), sizeof(media));
    media.fault = MEDIA_FAULT_NONE;
    init_store(&media, &storage, &store, &persistent_media);

    /* Establish A as the last durable valid historical synchronization fact. */
    assert(time_history_store_commit(&store, &history_a) == TR2_OK);
    assert_recovered_history(&store, UINT32_C(1000), UINT16_C(7));

    /* J-TIME / generic P1-P5: an interrupted partial write never replaces A. */
    media.fault = MEDIA_FAULT_PARTIAL_WRITE;
    assert(time_history_store_commit(&store, &history_b) == TR2_ERROR_STORAGE);
    assert(time_history_store_recovery_required(&store));

    media.fault = MEDIA_FAULT_NONE;
    init_store(&media, &storage, &store, &persistent_media);
    assert_recovered_history(&store, UINT32_C(1000), UINT16_C(7));

    /* A second normal reboot converges to the same durable authority. */
    init_store(&media, &storage, &store, &persistent_media);
    assert_recovered_history(&store, UINT32_C(1000), UINT16_C(7));

    /* An interrupted durable commit also leaves A authoritative after reboot. */
    media.fault = MEDIA_FAULT_COMMIT;
    assert(time_history_store_commit(&store, &history_b) == TR2_ERROR_STORAGE);
    assert(time_history_store_recovery_required(&store));

    media.fault = MEDIA_FAULT_NONE;
    init_store(&media, &storage, &store, &persistent_media);
    assert_recovered_history(&store, UINT32_C(1000), UINT16_C(7));

    /* Once durability is confirmed, the complete new fact becomes authority. */
    assert(time_history_store_commit(&store, &history_b) == TR2_OK);
    init_store(&media, &storage, &store, &persistent_media);
    assert_recovered_history(&store, UINT32_C(2000), UINT16_C(8));

    /* Synchronization boundary: WallClock.set may succeed before persistence fails.
       The failed candidate C must not become runtime/durable synchronization history. */
    assert(time_service_init(&service, &wall_clock) == TR2_OK);
    assert(time_service_bind_synchronization_dependencies(&service,
                                                          &monotonic_clock,
                                                          &store) == TR2_OK);
    recovery_context.civil_time_usable = true;
    recovery_context.continuity = TIME_CONTINUITY_PROVEN;
    recovery_context.last_sync_history = history_b;
    assert(time_service_apply_recovery_context(&service, &recovery_context) == TR2_OK);
    assert(time_service_prepare_time(&service, history_c.timestamp) == TR2_OK);

    media.fault = MEDIA_FAULT_COMMIT;
    assert(time_service_synchronize_prepared(&service, history_c.source) == TR2_ERROR_STORAGE);
    assert(wall.current_time == UINT32_C(3000));
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.last_sync_history.state == LAST_SYNC_HISTORY_VALID);
    assert(snapshot.last_sync_history.timestamp == UINT32_C(2000));
    assert(snapshot.last_sync_history.source == UINT16_C(8));
    assert(!service.current_boot_sync_anchor_available);

    /* Reboot after the failed synchronization: only B is recoverable. */
    media.fault = MEDIA_FAULT_NONE;
    init_store(&media, &storage, &store, &persistent_media);
    assert_recovered_history(&store, UINT32_C(2000), UINT16_C(8));

    /* J-TIME-07: lack of continuity proof stays lack of proof across reboots. */
    assert(time_service_init(&service, &wall_clock) == TR2_OK);
    recovery_context.civil_time_usable = true;
    recovery_context.continuity = TIME_CONTINUITY_INDETERMINATE;
    recovery_context.last_sync_history = history_b;
    assert(time_service_apply_recovery_context(&service, &recovery_context) == TR2_OK);
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.continuity == TIME_CONTINUITY_INDETERMINATE);
    assert(snapshot.last_sync_history.timestamp == UINT32_C(2000));
    assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_UNAVAILABLE);

    assert(time_service_init(&service, &wall_clock) == TR2_OK);
    assert(time_service_apply_recovery_context(&service, &recovery_context) == TR2_OK);
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.continuity == TIME_CONTINUITY_INDETERMINATE);
    assert(snapshot.last_sync_history.timestamp == UINT32_C(2000));
    assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_UNAVAILABLE);

    return 0;
}
