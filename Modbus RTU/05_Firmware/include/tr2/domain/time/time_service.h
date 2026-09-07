#ifndef TR2_DOMAIN_TIME_SERVICE_H
#define TR2_DOMAIN_TIME_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/time/time_recovery.h"
#include "tr2/persistence/time_history_store.h"
#include "tr2/platform/monotonic_clock.h"
#include "tr2/platform/wall_clock.h"

typedef struct {
    uint32_t generation;
    bool synchronization_facts_available;
    uint16_t time_status;
    uint16_t time_flags;
    bool current_time_available;
    Tr2CivilTimestamp current_time;
    Tr2CivilTimestamp last_sync_time;
    uint32_t time_since_sync_s;
    bool prepared_time_available;
    Tr2CivilTimestamp prepared_time;
    uint16_t prepared_time_status;
    uint16_t time_accuracy_ms;
    int16_t drift_ppm;
    uint16_t sync_source;

    bool civil_time_usable;
    TimeContinuity continuity;
    LastSyncHistory last_sync_history;
    TimeSinceSync time_since_sync;
} TimeSnapshot;

typedef struct {
    const WallClock *wall_clock;
    const MonotonicClock *monotonic_clock;
    TimeHistoryStore *time_history_store;
    uint32_t generation;
    bool prepared_time_available;
    Tr2CivilTimestamp prepared_time;
    uint16_t prepared_time_status;
    TimeRecoveryContext recovery_context;
    bool recovery_context_available;
    MonotonicTimeMs current_boot_sync_anchor_ms;
    bool current_boot_sync_anchor_available;
    bool initialized;
} TimeService;

Tr2Result time_service_init(TimeService *service, const WallClock *wall_clock);
Tr2Result time_service_bind_synchronization_dependencies(
    TimeService *service,
    const MonotonicClock *monotonic_clock,
    TimeHistoryStore *time_history_store);
Tr2Result time_service_apply_recovery_context(TimeService *service,
                                              const TimeRecoveryContext *context);
Tr2Result time_service_get_snapshot(const TimeService *service, TimeSnapshot *snapshot);
Tr2Result time_service_get_prepared_time(const TimeService *service,
                                         bool *available,
                                         Tr2CivilTimestamp *prepared_time);
Tr2Result time_service_prepare_time(TimeService *service, Tr2CivilTimestamp prepared_time);
Tr2Result time_service_synchronize_prepared(TimeService *service, uint16_t sync_source);

#endif
