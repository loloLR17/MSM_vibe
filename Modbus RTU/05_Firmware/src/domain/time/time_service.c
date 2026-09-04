#include <stddef.h>
#include "tr2/domain/time/time_service.h"

#define TR2_PREPARED_TIME_STATUS_NONE UINT16_C(0)
#define TR2_PREPARED_TIME_STATUS_AVAILABLE UINT16_C(1)

Tr2Result time_service_init(TimeService *service, const WallClock *wall_clock)
{
    if (service == NULL || wall_clock == NULL || wall_clock->read == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    service->wall_clock = wall_clock;
    service->generation = 0u;
    service->prepared_time_available = false;
    service->prepared_time = 0u;
    service->prepared_time_status = TR2_PREPARED_TIME_STATUS_NONE;
    service->initialized = true;
    return TR2_OK;
}

Tr2Result time_service_get_snapshot(const TimeService *service, TimeSnapshot *snapshot)
{
    Tr2CivilTimestamp current_time = 0u;
    WallClockReadResult wall_result;

    if (service == NULL || snapshot == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!service->initialized || service->wall_clock == NULL || service->wall_clock->read == NULL) {
        return TR2_ERROR_INVALID_STATE;
    }

    wall_result = service->wall_clock->read(service->wall_clock->context, &current_time);
    if (wall_result == WALL_CLOCK_INVALID) {
        return TR2_ERROR_INTERNAL;
    }

    snapshot->generation = service->generation;
    snapshot->current_time_available = wall_result == WALL_CLOCK_OK;
    snapshot->current_time = snapshot->current_time_available ? current_time : 0u;
    snapshot->prepared_time_available = service->prepared_time_available;
    snapshot->prepared_time = service->prepared_time;
    snapshot->prepared_time_status = service->prepared_time_status;
    return TR2_OK;
}

Tr2Result time_service_prepare_time(TimeService *service, Tr2CivilTimestamp prepared_time)
{
    if (service == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!service->initialized) {
        return TR2_ERROR_INVALID_STATE;
    }

    service->prepared_time = prepared_time;
    service->prepared_time_available = true;
    service->prepared_time_status = TR2_PREPARED_TIME_STATUS_AVAILABLE;
    service->generation++;
    return TR2_OK;
}

#undef TR2_PREPARED_TIME_STATUS_NONE
#undef TR2_PREPARED_TIME_STATUS_AVAILABLE
