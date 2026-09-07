#include "tr2/persistence/time_history_store.h"

#include <string.h>

static bool record_is_uniform(const uint8_t *record, uint8_t value)
{
    size_t index;

    for (index = 0u; index < TR2_TIME_HISTORY_RECORD_SIZE; ++index) {
        if (record[index] != value) {
            return false;
        }
    }

    return true;
}

static bool record_is_empty(const uint8_t *record)
{
    return record_is_uniform(record, UINT8_C(0x00)) ||
           record_is_uniform(record, UINT8_C(0xFF));
}

Tr2Result time_history_store_init(TimeHistoryStore *store,
                                  PersistentStorageCore *storage,
                                  uint32_t storage_offset)
{
    if (store == NULL || storage == NULL ||
        !persistent_storage_core_is_initialized(storage)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    store->storage = storage;
    store->storage_offset = storage_offset;
    store->initialized = true;
    store->recovery_required = false;
    return TR2_OK;
}

bool time_history_store_is_initialized(const TimeHistoryStore *store)
{
    return store != NULL && store->initialized && store->storage != NULL &&
           persistent_storage_core_is_initialized(store->storage);
}

bool time_history_store_recovery_required(const TimeHistoryStore *store)
{
    return time_history_store_is_initialized(store) && store->recovery_required;
}

Tr2Result time_history_store_commit(TimeHistoryStore *store,
                                    const LastSyncHistory *history)
{
    uint8_t record[TR2_TIME_HISTORY_RECORD_SIZE];
    Tr2Result result;

    if (!time_history_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (history == NULL || history->state != LAST_SYNC_HISTORY_VALID) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = tr2_time_history_record_encode(history, record, sizeof(record));
    if (result != TR2_OK) {
        return result;
    }

    result = persistent_storage_core_write(store->storage,
                                           store->storage_offset,
                                           record,
                                           sizeof(record));
    if (result != TR2_OK) {
        store->recovery_required = true;
        return result;
    }

    result = persistent_storage_core_commit(store->storage);
    if (result != TR2_OK) {
        store->recovery_required = true;
        return result;
    }

    return TR2_OK;
}

Tr2Result time_history_store_recover(const TimeHistoryStore *store,
                                     TimeHistoryRecoveryResult *result)
{
    uint8_t record[TR2_TIME_HISTORY_RECORD_SIZE];
    Tr2Result status;

    if (!time_history_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (result == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(result, 0, sizeof(*result));
    result->history = time_last_sync_history_none();

    status = persistent_storage_core_read(store->storage,
                                          store->storage_offset,
                                          record,
                                          sizeof(record));
    if (status != TR2_OK) {
        result->status = TIME_HISTORY_RECOVERY_UNAVAILABLE;
        return TR2_OK;
    }

    if (record_is_empty(record)) {
        result->status = TIME_HISTORY_RECOVERY_EMPTY;
        return TR2_OK;
    }

    status = tr2_time_history_record_decode(record,
                                            sizeof(record),
                                            &result->history);
    if (status == TR2_OK) {
        result->status = TIME_HISTORY_RECOVERY_VALID;
        return TR2_OK;
    }

    result->history = time_last_sync_history_none();
    if (status == TR2_ERROR_UNSUPPORTED) {
        result->status = TIME_HISTORY_RECOVERY_UNSUPPORTED;
    } else {
        result->status = TIME_HISTORY_RECOVERY_CORRUPTED;
    }

    return TR2_OK;
}
