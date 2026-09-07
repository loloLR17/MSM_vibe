#ifndef TR2_PERSISTENCE_TIME_HISTORY_STORE_H
#define TR2_PERSISTENCE_TIME_HISTORY_STORE_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/time/time_recovery.h"
#include "tr2/persistence/persistent_storage_core.h"
#include "tr2/persistence/time_history_record.h"

typedef enum {
    TIME_HISTORY_RECOVERY_VALID = 0,
    TIME_HISTORY_RECOVERY_EMPTY,
    TIME_HISTORY_RECOVERY_CORRUPTED,
    TIME_HISTORY_RECOVERY_UNAVAILABLE,
    TIME_HISTORY_RECOVERY_UNSUPPORTED
} TimeHistoryRecoveryStatus;

typedef struct {
    TimeHistoryRecoveryStatus status;
    LastSyncHistory history;
} TimeHistoryRecoveryResult;

typedef struct {
    PersistentStorageCore *storage;
    uint32_t storage_offset;
    bool initialized;
    bool recovery_required;
} TimeHistoryStore;

Tr2Result time_history_store_init(TimeHistoryStore *store,
                                  PersistentStorageCore *storage,
                                  uint32_t storage_offset);

bool time_history_store_is_initialized(const TimeHistoryStore *store);

bool time_history_store_recovery_required(const TimeHistoryStore *store);

Tr2Result time_history_store_commit(TimeHistoryStore *store,
                                    const LastSyncHistory *history);

Tr2Result time_history_store_recover(const TimeHistoryStore *store,
                                     TimeHistoryRecoveryResult *result);

#endif
