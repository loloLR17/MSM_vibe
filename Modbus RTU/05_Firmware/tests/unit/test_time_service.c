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
    TestWallClockContext context = { UINT32_C(0x10203040), WALL_CLOCK_OK, 0u };
    WallClock wall_clock = { &context, test_wall_read, test_wall_set };
    WallClock invalid_clock = { &context, NULL, test_wall_set };
    TimeService service = { 0 };
    TimeService invalid_service = { 0 };
    TimeSnapshot snapshot = { 0 };

    assert(time_service_init(NULL, &wall_clock) == TR2_ERROR_INVALID_ARGUMENT);
    assert(time_service_init(&invalid_service, &invalid_clock) == TR2_ERROR_INVALID_ARGUMENT);
    assert(time_service_prepare_time(&invalid_service, UINT32_C(1)) == TR2_ERROR_INVALID_STATE);
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_ERROR_INVALID_STATE);
    assert(time_service_init(&service, &wall_clock) == TR2_OK);
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.generation == 0u);
    assert(snapshot.current_time_available);
    assert(snapshot.current_time == UINT32_C(0x10203040));
    assert(!snapshot.prepared_time_available);
    assert(snapshot.prepared_time == 0u);
    assert(snapshot.prepared_time_status == 0u);

    assert(time_service_prepare_time(&service, UINT32_C(0x12345678)) == TR2_OK);
    assert(context.set_calls == 0u);
    assert(context.current_time == UINT32_C(0x10203040));
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.generation == 1u);
    assert(snapshot.current_time == UINT32_C(0x10203040));
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

    context.read_result = WALL_CLOCK_UNAVAILABLE;
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(!snapshot.current_time_available);
    assert(snapshot.current_time == 0u);
    assert(snapshot.prepared_time_available);
    assert(snapshot.prepared_time_status == 1u);

    context.read_result = WALL_CLOCK_INVALID;
    assert(time_service_get_snapshot(&service, &snapshot) == TR2_ERROR_INTERNAL);

    return 0;
}
