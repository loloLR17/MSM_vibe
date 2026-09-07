#include "tr2/persistence/command_journal_store.h"

#include <stddef.h>
#include <string.h>

typedef struct {
    bool empty;
    Tr2Result decode_status;
    CommandJournalRecord record;
} JournalSlotInfo;

static uint32_t slot_offset(uint16_t transaction_id, size_t slot_index)
{
    return (((uint32_t)transaction_id - 1u) * TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS +
            (uint32_t)slot_index) * TR2_COMMAND_JOURNAL_RECORD_SIZE;
}

static bool bytes_uniform(const uint8_t *bytes, uint8_t value)
{
    size_t index;

    for (index = 0u; index < TR2_COMMAND_JOURNAL_RECORD_SIZE; ++index) {
        if (bytes[index] != value) {
            return false;
        }
    }
    return true;
}

static bool record_bytes_empty(const uint8_t *bytes)
{
    return bytes_uniform(bytes, UINT8_C(0x00)) || bytes_uniform(bytes, UINT8_C(0xFF));
}

static Tr2Result read_slot(const CommandJournalStore *store,
                           uint16_t transaction_id,
                           size_t slot_index,
                           JournalSlotInfo *info)
{
    uint8_t bytes[TR2_COMMAND_JOURNAL_RECORD_SIZE];
    Tr2Result result;

    memset(info, 0, sizeof(*info));
    result = persistent_storage_core_read(store->storage,
                                          slot_offset(transaction_id, slot_index),
                                          bytes,
                                          sizeof(bytes));
    if (result != TR2_OK) {
        return result;
    }

    if (record_bytes_empty(bytes)) {
        info->empty = true;
        return TR2_OK;
    }

    info->decode_status = tr2_command_journal_record_decode(bytes,
                                                             sizeof(bytes),
                                                             &info->record);
    return TR2_OK;
}

static Tr2Result select_current_record(const CommandJournalStore *store,
                                       uint16_t transaction_id,
                                       JournalSlotInfo slots[TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS],
                                       bool *found,
                                       size_t *current_slot,
                                       CommandJournalRecord *record)
{
    size_t index;
    bool saw_corrupted = false;
    bool saw_unsupported = false;

    *found = false;
    *current_slot = 0u;
    memset(record, 0, sizeof(*record));

    for (index = 0u; index < TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS; ++index) {
        Tr2Result result = read_slot(store, transaction_id, index, &slots[index]);
        if (result != TR2_OK) {
            return result;
        }
        if (slots[index].empty) {
            continue;
        }
        if (slots[index].decode_status == TR2_OK) {
            if (slots[index].record.entry.transaction_id != transaction_id) {
                saw_corrupted = true;
                continue;
            }
            if (!*found || slots[index].record.generation > record->generation) {
                *record = slots[index].record;
                *current_slot = index;
                *found = true;
            }
        } else if (slots[index].decode_status == TR2_ERROR_UNSUPPORTED) {
            saw_unsupported = true;
        } else {
            saw_corrupted = true;
        }
    }

    if (*found) {
        return TR2_OK;
    }
    if (saw_unsupported) {
        return TR2_ERROR_UNSUPPORTED;
    }
    if (saw_corrupted) {
        return TR2_ERROR_CORRUPTED;
    }
    return TR2_OK;
}

static Tr2Result persist_record(CommandJournalStore *store,
                                size_t target_slot,
                                const CommandJournalRecord *record)
{
    uint8_t bytes[TR2_COMMAND_JOURNAL_RECORD_SIZE];
    Tr2Result result;

    result = tr2_command_journal_record_encode(record, bytes, sizeof(bytes));
    if (result != TR2_OK) {
        return result;
    }

    result = persistent_storage_core_write(store->storage,
                                           slot_offset(record->entry.transaction_id, target_slot),
                                           bytes,
                                           sizeof(bytes));
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

static Tr2Result journal_find(void *context,
                              uint16_t transaction_id,
                              CommandJournalEntry *entry)
{
    CommandJournalStore *store = (CommandJournalStore *)context;
    JournalSlotInfo slots[TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS];
    CommandJournalRecord record;
    bool found;
    size_t current_slot;
    Tr2Result result;

    if (!command_journal_store_is_initialized(store) || entry == NULL ||
        transaction_id == 0u || transaction_id > store->max_transaction_id) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = select_current_record(store, transaction_id, slots, &found, &current_slot, &record);
    (void)current_slot;
    if (result != TR2_OK) {
        return result;
    }
    if (!found) {
        return TR2_ERROR_NOT_FOUND;
    }

    *entry = record.entry;
    return TR2_OK;
}

static Tr2Result journal_reserve(void *context,
                                 const CommandRequest *request,
                                 CommandJournalEntry *entry)
{
    CommandJournalStore *store = (CommandJournalStore *)context;
    JournalSlotInfo slots[TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS];
    CommandJournalRecord current;
    CommandJournalRecord next;
    bool found;
    size_t current_slot;
    Tr2Result result;

    if (!command_journal_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (request == NULL || entry == NULL || request->transaction_id == 0u ||
        request->transaction_id > store->max_transaction_id) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = select_current_record(store, request->transaction_id, slots,
                                   &found, &current_slot, &current);
    if (result != TR2_OK) {
        return result;
    }
    if (found) {
        return TR2_ERROR_INVALID_STATE;
    }

    memset(&next, 0, sizeof(next));
    next.generation = 1u;
    next.entry.transaction_id = request->transaction_id;
    next.entry.request_identity = request->identity;
    next.entry.lifecycle = COMMAND_LIFECYCLE_RESERVED;

    result = persist_record(store, 0u, &next);
    if (result == TR2_OK) {
        *entry = next.entry;
    }
    return result;
}

static Tr2Result journal_mark_started(void *context,
                                      uint16_t transaction_id,
                                      CommandJournalEntry *entry)
{
    CommandJournalStore *store = (CommandJournalStore *)context;
    JournalSlotInfo slots[TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS];
    CommandJournalRecord current;
    CommandJournalRecord next;
    bool found;
    size_t current_slot;
    Tr2Result result;

    if (!command_journal_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (entry == NULL || transaction_id == 0u || transaction_id > store->max_transaction_id) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = select_current_record(store, transaction_id, slots,
                                   &found, &current_slot, &current);
    if (result != TR2_OK) {
        return result;
    }
    if (!found) {
        return TR2_ERROR_NOT_FOUND;
    }
    if (current.entry.lifecycle != COMMAND_LIFECYCLE_RESERVED ||
        current.generation == UINT32_MAX) {
        return TR2_ERROR_INVALID_STATE;
    }

    next = current;
    next.generation++;
    next.entry.lifecycle = COMMAND_LIFECYCLE_STARTED;
    result = persist_record(store, 1u - current_slot, &next);
    if (result == TR2_OK) {
        *entry = next.entry;
    }
    return result;
}

static Tr2Result journal_complete(void *context,
                                  uint16_t transaction_id,
                                  const CommandFinalResult *final_result,
                                  const CommandTerminalTimestamp *terminal_timestamp,
                                  CommandJournalEntry *entry)
{
    CommandJournalStore *store = (CommandJournalStore *)context;
    JournalSlotInfo slots[TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS];
    CommandJournalRecord current;
    CommandJournalRecord next;
    bool found;
    size_t current_slot;
    Tr2Result result;

    if (!command_journal_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (final_result == NULL || terminal_timestamp == NULL || entry == NULL ||
        transaction_id == 0u || transaction_id > store->max_transaction_id ||
        !command_status_is_final(final_result->status)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = select_current_record(store, transaction_id, slots,
                                   &found, &current_slot, &current);
    if (result != TR2_OK) {
        return result;
    }
    if (!found) {
        return TR2_ERROR_NOT_FOUND;
    }
    if (current.entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED ||
        current.generation == UINT32_MAX || store->next_completion_order == 0u) {
        return TR2_ERROR_INVALID_STATE;
    }

    next = current;
    next.generation++;
    next.entry.lifecycle = COMMAND_LIFECYCLE_COMPLETED;
    next.entry.has_final_result = true;
    next.entry.final_result = *final_result;
    next.entry.terminal_timestamp = *terminal_timestamp;
    next.entry.completion_order = store->next_completion_order;

    result = persist_record(store, 1u - current_slot, &next);
    if (result == TR2_OK) {
        store->next_completion_order++;
        *entry = next.entry;
    }
    return result;
}

static Tr2Result journal_latest_completed(void *context, CommandJournalEntry *entry)
{
    CommandJournalStore *store = (CommandJournalStore *)context;
    uint32_t transaction_id;
    bool found_latest = false;
    CommandJournalEntry latest;

    if (!command_journal_store_is_initialized(store) || entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(&latest, 0, sizeof(latest));
    for (transaction_id = 1u; transaction_id <= store->max_transaction_id; ++transaction_id) {
        CommandJournalEntry candidate;
        Tr2Result result = journal_find(store, (uint16_t)transaction_id, &candidate);
        if (result == TR2_ERROR_NOT_FOUND) {
            continue;
        }
        if (result != TR2_OK) {
            return result;
        }
        if (candidate.lifecycle == COMMAND_LIFECYCLE_COMPLETED &&
            (!found_latest || candidate.completion_order > latest.completion_order)) {
            latest = candidate;
            found_latest = true;
        }
    }

    if (!found_latest) {
        return TR2_ERROR_NOT_FOUND;
    }
    *entry = latest;
    return TR2_OK;
}

Tr2Result command_journal_store_init(CommandJournalStore *store,
                                     PersistentStorageCore *storage,
                                     uint16_t max_transaction_id)
{
    if (store == NULL || storage == NULL ||
        !persistent_storage_core_is_initialized(storage) || max_transaction_id == 0u) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(store, 0, sizeof(*store));
    store->storage = storage;
    store->max_transaction_id = max_transaction_id;
    store->initialized = true;
    store->recovery_required = true;
    store->next_completion_order = 1u;
    store->journal.context = store;
    store->journal.find = journal_find;
    store->journal.reserve = journal_reserve;
    store->journal.mark_started = journal_mark_started;
    store->journal.complete = journal_complete;
    store->journal.latest_completed = journal_latest_completed;
    return TR2_OK;
}

bool command_journal_store_is_initialized(const CommandJournalStore *store)
{
    return store != NULL && store->initialized && store->storage != NULL &&
           persistent_storage_core_is_initialized(store->storage);
}

bool command_journal_store_recovery_required(const CommandJournalStore *store)
{
    return command_journal_store_is_initialized(store) && store->recovery_required;
}

CommandJournal *command_journal_store_journal(CommandJournalStore *store)
{
    if (!command_journal_store_is_initialized(store)) {
        return NULL;
    }
    return &store->journal;
}

Tr2Result command_journal_store_recover(CommandJournalStore *store,
                                        CommandJournalRecoveryResult *result)
{
    uint32_t transaction_id;
    uint32_t known_count = 0u;
    uint32_t max_completion_order = 0u;
    bool saw_any = false;

    if (!command_journal_store_is_initialized(store) || result == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(result, 0, sizeof(*result));
    for (transaction_id = 1u; transaction_id <= store->max_transaction_id; ++transaction_id) {
        JournalSlotInfo slots[TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS];
        CommandJournalRecord record;
        bool found;
        size_t current_slot;
        Tr2Result read_result = select_current_record(store,
                                                      (uint16_t)transaction_id,
                                                      slots,
                                                      &found,
                                                      &current_slot,
                                                      &record);
        (void)current_slot;
        if (read_result == TR2_ERROR_UNSUPPORTED) {
            result->status = COMMAND_JOURNAL_RECOVERY_UNSUPPORTED;
            return TR2_OK;
        }
        if (read_result == TR2_ERROR_CORRUPTED) {
            result->status = COMMAND_JOURNAL_RECOVERY_CORRUPTED;
            return TR2_OK;
        }
        if (read_result != TR2_OK) {
            result->status = COMMAND_JOURNAL_RECOVERY_UNAVAILABLE;
            return TR2_OK;
        }
        if (!found) {
            continue;
        }

        saw_any = true;
        known_count++;
        if (record.entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED &&
            record.entry.completion_order > max_completion_order) {
            max_completion_order = record.entry.completion_order;
        }
    }

    if (max_completion_order == UINT32_MAX) {
        result->status = COMMAND_JOURNAL_RECOVERY_UNSUPPORTED;
        return TR2_OK;
    }

    store->next_completion_order = max_completion_order + 1u;
    store->recovery_required = false;
    result->known_transaction_count = known_count;
    result->next_completion_order = store->next_completion_order;
    result->status = saw_any ? COMMAND_JOURNAL_RECOVERY_VALID : COMMAND_JOURNAL_RECOVERY_EMPTY;
    return TR2_OK;
}
