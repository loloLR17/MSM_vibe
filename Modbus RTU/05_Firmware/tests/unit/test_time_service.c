#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/domain/time/time_service.h"

typedef struct {
    Tr2CivilTimestamp current_time;
    WallClockReadResult read_result;
    uint32_t set_calls;
} TestWallClockContext;

static WallClockReadResult test_wall_read(void *context, Tr2CivilTimestamp *timestamp)
{
    TestWallClockContext *clock = (TestWallClockContext *)context;
    if (clock == NULL || timestamp == NULL) {
        return WALL_CLOCK_INVALID;
    }
    *timestamp = clock->current_time;
    return clock->read_result;
}

static Tr2Result test_wall_set(void *context, Tr2CivilTimestamp timestamp)
{
    TestWallClockContext *clock = (TestWallClockContext *)context;
    if (clock == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    clock->current_time = timestamp;
    clock->set_calls++;
    return TR2_OK;
}

int main(void)
{
    TestWallClockContext context = { UINT32_C(1000), WALL_CLOCK_OK, 0u };
    WallClock wall_clock = { &context, test_wall_read, test_wall_set };
    WallClock invalid_clock = { &context, NULL, test_wall_set };
    TimeService service = { 0 };
    TimeService invalid_service = { 0 };
    TimeSnapshot snapshot = { 0 };
    TimeRecoveryContext recovery = { 0 };

    assert(time_service_init(NULL, &wall_clock) == TR2_ERROR_INVALID_ARGUMENT);
    assert(time_service_init(&invalid_service, &invalid_clock) == TR2_ERROR_INVALID_ARGUMENT);
    assert(time_service_apply_recovery_context(&invalid_service, &recovery) == TR2_ERROR_INVALID_STATE);
    assert(time_service_prepare_time(&invalid_service, UINT32_C(1)) == TR2_ERROR_INVALID_STATE);
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_ERROR_INVALID_STATE);

    assert(time_service_init(&service, &wall_clock) == TR2_OK);
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.generation == 0u);
    assert(!snapshot.civil_time_usable);
    assert(!snapshot.current_time_available);
    assert(snapshot.current_time == 0u);
    assert(snapshot.continuity == TIME_CONTINUITY_INDETERMINATE);
    assert(snapshot.last_sync_history.state == LAST_SYNC_HISTORY_NONE);
    assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_UNAVAILABLE);
    assert(!snapshot.synchronization_facts_available);

    recovery.civil_time_usable = true;
    recovery.continuity = TIME_CONTINUITY_PROVEN;
    recovery.last_sync_history = time_last_sync_history_valid(UINT32_C(900), UINT16_C(3));
    assert(time_service_apply_recovery_context(&service, &recovery) == TR2_OK);
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.civil_time_usable);
    assert(snapshot.current_time_available);
    assert(snapshot.current_time == UINT32_C(1000));
    assert(snapshot.continuity == TIME_CONTINUITY_PROVEN);
    assert(snapshot.synchronization_facts_available);
    assert(snapshot.last_sync_time == UINT32_C(900));
    assert(snapshot.sync_source == UINT16_C(3));
    assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_AVAILABLE);
    assert(snapshot.time_since_sync.duration_s == UINT32_C(100));
    assert(snapshot.time_since_sync_s == UINT32_C(100));

    recovery.continuity = TIME_CONTINUITY_INDETERMINATE;
    assert(time_service_apply_recovery_context(&service, &recovery) == TR2_OK);
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.civil_time_usable);
    assert(snapshot.synchronization_facts_available);
    assert(snapshot.last_sync_time == UINT32_C(900));
    assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_UNAVAILABLE);
    assert(snapshot.time_since_sync_s == 0u);

    recovery.continuity = TIME_CONTINUITY_BROKEN;
    assert(time_service_apply_recovery_context(&service, &recovery) == TR2_OK);
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.synchronization_facts_available);
    assert(snapshot.last_sync_time == UINT32_C(900));
    assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_UNAVAILABLE);

    recovery.continuity = TIME_CONTINUITY_PROVEN;
    context.current_time = UINT32_C(899);
    assert(time_service_apply_recovery_context(&service, &recovery) == TR2_OK);
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.civil_time_usable);
    assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_UNAVAILABLE);

    context.current_time = UINT32_C(1000);
    context.read_result = WALL_CLOCK_UNAVAILABLE;
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(!snapshot.civil_time_usable);
    assert(!snapshot.current_time_available);
    assert(snapshot.current_time == 0u);
    assert(snapshot.synchronization_facts_available);
    assert(snapshot.last_sync_time == UINT32_C(900));
    assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_UNAVAILABLE);

    context.read_result = WALL_CLOCK_INVALID;
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(!snapshot.civil_time_usable);
    assert(!snapshot.current_time_available);
    assert(snapshot.synchronization_facts_available);

    context.read_result = WALL_CLOCK_OK;
    recovery.civil_time_usable = false;
    recovery.continuity = TIME_CONTINUITY_PROVEN;
    assert(time_service_apply_recovery_context(&service, &recovery) == TR2_OK);
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(!snapshot.civil_time_usable);
    assert(!snapshot.current_time_available);
    assert(snapshot.synchronization_facts_available);
    assert(snapshot.time_since_sync.state == TIME_SINCE_SYNC_AVAILABLE);
    assert(snapshot.time_since_sync.duration_s == UINT32_C(100));

    assert(time_service_prepare_time(&service, UINT32_C(0x12345678)) == TR2_OK);
    assert(context.set_calls == 0u);
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.generation == 1u);
    assert(snapshot.prepared_time_available);
    assert(snapshot.prepared_time == UINT32_C(0x12345678));
    assert(snapshot.prepared_time_status == 1u);

    assert(time_service_prepare_time(&service, 0u) == TR2_OK);
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.generation == 2u);
    assert(snapshot.prepared_time_available);
    assert(snapshot.prepared_time == 0u);
    assert(snapshot.prepared_time_status == 1u);
    assert(context.set_calls == 0u);

    return 0;
}
