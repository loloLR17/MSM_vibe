#include "tr2/persistence/diagnostic_history_store.h"

#include <string.h>

static bool record_is_uniform(const uint8_t *record, uint8_t value)
{
    size_t index;

    for (index = 0u; index < TR2_DIAGNOSTIC_HISTORY_RECORD_SIZE; ++index) {
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

Tr2Result diagnostic_history_store_init(DiagnosticHistoryStore *store,
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

bool diagnostic_history_store_is_initialized(const DiagnosticHistoryStore *store)
{
    return store != NULL && store->initialized && store->storage != NULL &&
           persistent_storage_core_is_initialized(store->storage);
}

bool diagnostic_history_store_recovery_required(const DiagnosticHistoryStore *store)
{
    return diagnostic_history_store_is_initialized(store) && store->recovery_required;
}

Tr2Result diagnostic_history_store_commit_last_fault(DiagnosticHistoryStore *store,
                                                     const DiagnosticLastFault *last_fault)
{
    uint8_t record[TR2_DIAGNOSTIC_HISTORY_RECORD_SIZE];
    Tr2Result result;

    if (!diagnostic_history_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (last_fault == NULL || !last_fault->present) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = tr2_diagnostic_history_record_encode(last_fault, record, sizeof(record));
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

Tr2Result diagnostic_history_store_recover(const DiagnosticHistoryStore *store,
                                           DiagnosticHistoryRecoveryResult *result)
{
    uint8_t record[TR2_DIAGNOSTIC_HISTORY_RECORD_SIZE];
    Tr2Result status;

    if (!diagnostic_history_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (result == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(result, 0, sizeof(*result));
    status = persistent_storage_core_read(store->storage,
                                          store->storage_offset,
                                          record,
                                          sizeof(record));
    if (status != TR2_OK) {
        result->status = DIAGNOSTIC_HISTORY_RECOVERY_UNAVAILABLE;
        return TR2_OK;
    }
    if (record_is_empty(record)) {
        result->status = DIAGNOSTIC_HISTORY_RECOVERY_EMPTY;
        return TR2_OK;
    }

    status = tr2_diagnostic_history_record_decode(record,
                                                  sizeof(record),
                                                  &result->last_fault);
    if (status == TR2_OK) {
        result->status = DIAGNOSTIC_HISTORY_RECOVERY_VALID;
        return TR2_OK;
    }

    memset(&result->last_fault, 0, sizeof(result->last_fault));
    result->status = status == TR2_ERROR_UNSUPPORTED
                         ? DIAGNOSTIC_HISTORY_RECOVERY_UNSUPPORTED
                         : DIAGNOSTIC_HISTORY_RECOVERY_CORRUPTED;
    return TR2_OK;
}
