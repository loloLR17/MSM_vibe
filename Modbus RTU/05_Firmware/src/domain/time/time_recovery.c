#include "tr2/domain/time/time_recovery.h"

LastSyncHistory time_last_sync_history_none(void)
{
    LastSyncHistory history = {0};

    history.state = LAST_SYNC_HISTORY_NONE;
    return history;
}

LastSyncHistory time_last_sync_history_valid(Tr2CivilTimestamp timestamp, uint16_t source)
{
    LastSyncHistory history = {0};

    history.state = LAST_SYNC_HISTORY_VALID;
    history.timestamp = timestamp;
    history.source = source;
    return history;
}

TimeSinceSync time_since_sync_unavailable(void)
{
    TimeSinceSync time_since_sync = {0};

    time_since_sync.state = TIME_SINCE_SYNC_UNAVAILABLE;
    return time_since_sync;
}

TimeSinceSync time_since_sync_available(uint32_t duration_s)
{
    TimeSinceSync time_since_sync = {0};

    time_since_sync.state = TIME_SINCE_SYNC_AVAILABLE;
    time_since_sync.duration_s = duration_s;
    return time_since_sync;
}
