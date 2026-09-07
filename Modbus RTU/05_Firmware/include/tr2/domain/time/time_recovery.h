#ifndef TR2_DOMAIN_TIME_RECOVERY_H
#define TR2_DOMAIN_TIME_RECOVERY_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/platform/wall_clock.h"

typedef enum {
    TIME_CONTINUITY_PROVEN = 0,
    TIME_CONTINUITY_BROKEN,
    TIME_CONTINUITY_INDETERMINATE
} TimeContinuity;

typedef enum {
    LAST_SYNC_HISTORY_NONE = 0,
    LAST_SYNC_HISTORY_VALID
} LastSyncHistoryState;

typedef struct {
    LastSyncHistoryState state;
    Tr2CivilTimestamp timestamp;
    uint16_t source;
} LastSyncHistory;

typedef enum {
    TIME_SINCE_SYNC_UNAVAILABLE = 0,
    TIME_SINCE_SYNC_AVAILABLE
} TimeSinceSyncState;

typedef struct {
    TimeSinceSyncState state;
    uint32_t duration_s;
} TimeSinceSync;

typedef struct {
    bool civil_time_usable;
    TimeContinuity continuity;
    LastSyncHistory last_sync_history;
} TimeRecoveryContext;

LastSyncHistory time_last_sync_history_none(void);
LastSyncHistory time_last_sync_history_valid(Tr2CivilTimestamp timestamp, uint16_t source);
TimeSinceSync time_since_sync_unavailable(void);
TimeSinceSync time_since_sync_available(uint32_t duration_s);

#endif
