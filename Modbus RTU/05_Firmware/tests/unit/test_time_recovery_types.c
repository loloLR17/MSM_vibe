#include <assert.h>
#include <stdint.h>

#include "tr2/domain/time/time_recovery.h"

int main(void)
{
    LastSyncHistory history_none = time_last_sync_history_none();
    LastSyncHistory history_valid = time_last_sync_history_valid(UINT32_C(0x12345678), UINT16_C(3));
    TimeSinceSync since_unavailable = time_since_sync_unavailable();
    TimeSinceSync since_available = time_since_sync_available(UINT32_C(42));
    TimeRecoveryContext context = {0};

    assert(history_none.state == LAST_SYNC_HISTORY_NONE);
    assert(history_none.timestamp == 0u);
    assert(history_none.source == 0u);

    assert(history_valid.state == LAST_SYNC_HISTORY_VALID);
    assert(history_valid.timestamp == UINT32_C(0x12345678));
    assert(history_valid.source == UINT16_C(3));

    assert(since_unavailable.state == TIME_SINCE_SYNC_UNAVAILABLE);
    assert(since_unavailable.duration_s == 0u);

    assert(since_available.state == TIME_SINCE_SYNC_AVAILABLE);
    assert(since_available.duration_s == UINT32_C(42));

    context.civil_time_usable = true;
    context.continuity = TIME_CONTINUITY_INDETERMINATE;
    context.last_sync_history = history_valid;

    assert(context.civil_time_usable);
    assert(context.continuity == TIME_CONTINUITY_INDETERMINATE);
    assert(context.last_sync_history.state == LAST_SYNC_HISTORY_VALID);

    context.continuity = TIME_CONTINUITY_BROKEN;
    assert(context.continuity == TIME_CONTINUITY_BROKEN);
    assert(context.last_sync_history.state == LAST_SYNC_HISTORY_VALID);

    context.continuity = TIME_CONTINUITY_PROVEN;
    assert(context.continuity == TIME_CONTINUITY_PROVEN);

    return 0;
}
