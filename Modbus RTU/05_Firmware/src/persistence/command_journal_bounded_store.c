#include "tr2/persistence/command_journal_bounded_store.h"

#include <stddef.h>
#include <string.h>

#include "tr2/persistence/command_journal_bounded_reader.h"

static bool result_requires_recovery(Tr2Result result)
{
    return result == TR2_ERROR_STORAGE ||
           result == TR2_ERROR_UNAVAILABLE ||
           result == TR2_ERROR_CORRUPTED ||
           result == TR2_ERROR_UNSUPPORTED;
}

static Tr2Result guard_read_result(CommandJournalBoundedStore *store,
                                   Tr2Result result)
{
    if (result_requires_recovery(result)) {
        store->recovery_required = true;
    }
    return result;
}

static Tr2Result journal_find(void *context,
                              uint16_t transaction_id,
                              CommandJournalEntry *entry)
{
    CommandJournalBoundedStore *store = (CommandJournalBoundedStore *)context;
    CommandJournalBoundedRecord record;
    size_t logical_slot;
    Tr2Result result;

    if (!command_journal_bounded_store_is_initialized(store) ||
        store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (entry == NULL || !command_transaction_id_is_valid(transaction_id)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = command_journal_bounded_reader_find(store->storage,
                                                  transaction_id,
                                                  &logical_slot,
                                                  &record);
    (void)logical_slot;
    result = guard_read_result(store, result);
    if (result == TR2_OK) {
        *entry = record.entry;
    }
    return result;
}

typedef struct {
    CommandJournalVisitor visitor;
    void *visitor_context;
} VisitAdapter;

static Tr2Result visit_adapter(void *context,
                               size_t logical_slot,
                               const CommandJournalBoundedRecord *record)
{
    VisitAdapter *adapter = (VisitAdapter *)context;
    (void)logical_slot;
    return adapter->visitor(adapter->visitor_context, &record->entry);
}

static Tr2Result journal_visit(void *context,
                               CommandJournalVisitor visitor,
                               void *visitor_context)
{
    CommandJournalBoundedStore *store = (CommandJournalBoundedStore *)context;
    VisitAdapter adapter;
    Tr2Result result;

    if (!command_journal_bounded_store_is_initialized(store) ||
        store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (visitor == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    adapter.visitor = visitor;
    adapter.visitor_context = visitor_context;
    result = command_journal_bounded_reader_visit(store->storage,
                                                   visit_adapter,
                                                   &adapter);
    return guard_read_result(store, result);
}

static Tr2Result journal_latest_completed(void *context,
                                          CommandJournalEntry *entry)
{
    CommandJournalBoundedStore *store = (CommandJournalBoundedStore *)context;
    CommandJournalBoundedRecord record;
    size_t logical_slot;
    Tr2Result result;

    if (!command_journal_bounded_store_is_initialized(store) ||
        store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = command_journal_bounded_reader_latest_completed(store->storage,
                                                              &logical_slot,
                                                              &record);
    (void)logical_slot;
    result = guard_read_result(store, result);
    if (result == TR2_OK) {
        *entry = record.entry;
    }
    return result;
}

static Tr2Result operation_not_implemented(void)
{
    return TR2_ERROR_UNSUPPORTED;
}

static Tr2Result journal_reserve(void *context,
                                 const CommandRequest *request,
                                 CommandJournalEntry *entry)
{
    CommandJournalBoundedStore *store = (CommandJournalBoundedStore *)context;
    if (!command_journal_bounded_store_is_initialized(store) ||
        store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (request == NULL || entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    return operation_not_implemented();
}

static Tr2Result journal_set_recovery_context(
    void *context,
    uint16_t transaction_id,
    const CommandRecoveryContext *recovery_context,
    CommandJournalEntry *entry)
{
    CommandJournalBoundedStore *store = (CommandJournalBoundedStore *)context;
    if (!command_journal_bounded_store_is_initialized(store) ||
        store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (!command_transaction_id_is_valid(transaction_id) ||
        recovery_context == NULL || entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    return operation_not_implemented();
}

static Tr2Result journal_mark_started(void *context,
                                      uint16_t transaction_id,
                                      CommandJournalEntry *entry)
{
    CommandJournalBoundedStore *store = (CommandJournalBoundedStore *)context;
    if (!command_journal_bounded_store_is_initialized(store) ||
        store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (!command_transaction_id_is_valid(transaction_id) || entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    return operation_not_implemented();
}

static Tr2Result journal_complete(void *context,
                                  uint16_t transaction_id,
                                  const CommandFinalResult *final_result,
                                  const CommandTerminalTimestamp *terminal_timestamp,
                                  CommandJournalEntry *entry)
{
    CommandJournalBoundedStore *store = (CommandJournalBoundedStore *)context;
    if (!command_journal_bounded_store_is_initialized(store) ||
        store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (!command_transaction_id_is_valid(transaction_id) ||
        final_result == NULL || terminal_timestamp == NULL || entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    return operation_not_implemented();
}

Tr2Result command_journal_bounded_store_init(
    CommandJournalBoundedStore *store,
    PersistentStorageCore *storage)
{
    if (store == NULL || storage == NULL ||
        !persistent_storage_core_is_initialized(storage)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(store, 0, sizeof(*store));
    store->storage = storage;
    store->initialized = true;
    store->recovery_required = true;
    store->next_admission_order = 1u;
    store->next_completion_order = 1u;
    store->journal.context = store;
    store->journal.find = journal_find;
    store->journal.reserve = journal_reserve;
    store->journal.set_recovery_context = journal_set_recovery_context;
    store->journal.mark_started = journal_mark_started;
    store->journal.complete = journal_complete;
    store->journal.latest_completed = journal_latest_completed;
    store->journal.visit = journal_visit;
    return TR2_OK;
}

bool command_journal_bounded_store_is_initialized(
    const CommandJournalBoundedStore *store)
{
    return store != NULL && store->initialized && store->storage != NULL &&
           persistent_storage_core_is_initialized(store->storage);
}

bool command_journal_bounded_store_recovery_required(
    const CommandJournalBoundedStore *store)
{
    return command_journal_bounded_store_is_initialized(store) &&
           store->recovery_required;
}

CommandJournal *command_journal_bounded_store_journal(
    CommandJournalBoundedStore *store)
{
    if (!command_journal_bounded_store_is_initialized(store)) {
        return NULL;
    }
    return &store->journal;
}

Tr2Result command_journal_bounded_store_recover(
    CommandJournalBoundedStore *store,
    CommandJournalBoundedRecoveryResult *result)
{
    CommandJournalBoundedRecoveryResult scanned;
    Tr2Result scan_result;

    if (!command_journal_bounded_store_is_initialized(store) || result == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    scan_result = command_journal_bounded_recovery_scan(store->storage, &scanned);
    if (scan_result != TR2_OK) {
        store->recovery_required = true;
        return scan_result;
    }

    *result = scanned;
    if (scanned.status != COMMAND_JOURNAL_BOUNDED_RECOVERY_EMPTY &&
        scanned.status != COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID) {
        store->recovery_required = true;
        return TR2_OK;
    }

    store->next_admission_order = scanned.next_admission_order;
    store->next_completion_order = scanned.next_completion_order;
    store->recovery_required = false;
    return TR2_OK;
}
