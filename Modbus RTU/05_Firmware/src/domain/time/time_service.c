#include <stddef.h>

#include "tr2/domain/time/time_service.h"

#define TR2_PREPARED_TIME_STATUS_NONE UINT16_C(0)
#define TR2_PREPARED_TIME_STATUS_AVAILABLE UINT16_C(1)

static TimeSinceSync reconstruct_time_since_sync(const TimeRecoveryContext *context,
                                                 WallClockReadResult wall_result,
                                                 Tr2CivilTimestamp current_time)
{
    if (context == NULL ||
        context->continuity != TIME_CONTINUITY_PROVEN ||
        context->last_sync_history.state != LAST_SYNC_HISTORY_VALID ||
        wall_result != WALL_CLOCK_OK ||
        current_time < context->last_sync_history.timestamp) {
        return time_since_sync_unavailable();
    }

    return time_since_sync_available(current_time - context->last_sync_history.timestamp);
}

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
    service->recovery_context.civil_time_usable = false;
    service->recovery_context.continuity = TIME_CONTINUITY_INDETERMINATE;
    service->recovery_context.last_sync_history = time_last_sync_history_none();
    service->recovery_context_available = false;
    service->initialized = true;
    return TR2_OK;
}

Tr2Result time_service_apply_recovery_context(TimeService *service,
                                              const TimeRecoveryContext *context)
{
    if (service == NULL || context == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!service->initialized) {
        return TR2_ERROR_INVALID_STATE;
    }

    service->recovery_context = *context;
    service->recovery_context_available = true;
    return TR2_OK;
}

Tr2Result time_service_get_snapshot(const TimeService *service, TimeSnapshot *snapshot)
{
    Tr2CivilTimestamp current_time = 0u;
    WallClockReadResult wall_result;
    TimeRecoveryContext context;

    if (service == NULL || snapshot == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!service->initialized || service->wall_clock == NULL || service->wall_clock->read == NULL) {
        return TR2_ERROR_INVALID_STATE;
    }

    wall_result = service->wall_clock->read(service->wall_clock->context, &current_time);

    context.civil_time_usable = false;
    context.continuity = TIME_CONTINUITY_INDETERMINATE;
    context.last_sync_history = time_last_sync_history_none();
    if (service->recovery_context_available) {
        context = service->recovery_context;
    }

    *snapshot = (TimeSnapshot){0};
    snapshot->generation = service->generation;
    snapshot->prepared_time_available = service->prepared_time_available;
    snapshot->prepared_time = service->prepared_time;
    snapshot->prepared_time_status = service->prepared_time_status;

    snapshot->civil_time_usable = context.civil_time_usable && wall_result == WALL_CLOCK_OK;
    snapshot->continuity = context.continuity;
    snapshot->last_sync_history = context.last_sync_history;
    snapshot->time_since_sync = reconstruct_time_since_sync(&context,
                                                            wall_result,
                                                            current_time);

    snapshot->current_time_available = snapshot->civil_time_usable;
    snapshot->current_time = snapshot->current_time_available ? current_time : 0u;

    snapshot->synchronization_facts_available =
        context.last_sync_history.state == LAST_SYNC_HISTORY_VALID;
    if (snapshot->synchronization_facts_available) {
        snapshot->last_sync_time = context.last_sync_history.timestamp;
        snapshot->sync_source = context.last_sync_history.source;
    }
    if (snapshot->time_since_sync.state == TIME_SINCE_SYNC_AVAILABLE) {
        snapshot->time_since_sync_s = snapshot->time_since_sync.duration_s;
    }

    return TR2_OK;
}

Tr2Result time_service_get_prepared_time(const TimeService *service,
                                         bool *available,
                                         Tr2CivilTimestamp *prepared_time)
{
    if (service == NULL || available == NULL || prepared_time == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!service->initialized) {
        return TR2_ERROR_INVALID_STATE;
    }

    *available = service->prepared_time_available;
    *prepared_time = service->prepared_time;
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
