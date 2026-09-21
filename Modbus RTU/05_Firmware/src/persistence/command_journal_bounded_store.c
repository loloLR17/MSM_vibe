#include "tr2/persistence/command_journal_bounded_store.h"

#include <stddef.h>
#include <string.h>

#include "tr2/persistence/command_journal_bounded_reader.h"
#include "tr2/persistence/command_journal_bounded_admission.h"
#include "tr2/persistence/command_journal_bounded_writer.h"

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

static Tr2Result journal_reserve(void *context,
                                 const CommandRequest *request,
                                 CommandJournalEntry *entry)
{
    CommandJournalBoundedStore *store = (CommandJournalBoundedStore *)context;
    CommandJournalBoundedAdmissionPlan plan;
    CommandJournalBoundedRecord record;
    Tr2Result result;

    if (!command_journal_bounded_store_is_initialized(store) ||
        store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (request == NULL || entry == NULL ||
        !command_transaction_id_is_valid(request->transaction_id)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (store->next_admission_order == 0u ||
        store->next_admission_order == UINT32_MAX) {
        return TR2_ERROR_UNSUPPORTED;
    }

    result = command_journal_bounded_admission_plan(
        store->storage, request->transaction_id, &request->identity, &plan);
    if (result != TR2_OK) {
        return guard_read_result(store, result);
    }

    if (plan.kind == COMMAND_JOURNAL_BOUNDED_ADMISSION_RETRY_EXISTING ||
        plan.kind == COMMAND_JOURNAL_BOUNDED_ADMISSION_COLLISION_EXISTING) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (plan.kind == COMMAND_JOURNAL_BOUNDED_ADMISSION_NO_CAPACITY) {
        return TR2_ERROR_UNAVAILABLE;
    }
    if (!plan.has_logical_slot) {
        store->recovery_required = true;
        return TR2_ERROR_CORRUPTED;
    }

    memset(&record, 0, sizeof(record));
    record.admission_order = store->next_admission_order;
    record.entry.transaction_id = request->transaction_id;
    record.entry.request_identity = request->identity;
    record.entry.lifecycle = COMMAND_LIFECYCLE_RESERVED;

    if (plan.kind == COMMAND_JOURNAL_BOUNDED_ADMISSION_ADMIT_EMPTY) {
        result = command_journal_bounded_writer_admit_empty(
            store->storage, plan.logical_slot, &record);
    } else if (plan.kind == COMMAND_JOURNAL_BOUNDED_ADMISSION_EVICT_COMPLETED &&
               plan.has_current) {
        result = command_journal_bounded_writer_readmit_completed(
            store->storage, plan.logical_slot, &plan.current, &record);
    } else {
        store->recovery_required = true;
        return TR2_ERROR_CORRUPTED;
    }

    if (result != TR2_OK) {
        if (result_requires_recovery(result)) {
            store->recovery_required = true;
        }
        return result;
    }

    store->next_admission_order++;
    *entry = record.entry;
    return TR2_OK;
}

static Tr2Result journal_set_recovery_context(
    void *context,
    uint16_t transaction_id,
    const CommandRecoveryContext *recovery_context,
    CommandJournalEntry *entry)
{
    CommandJournalBoundedStore *store = (CommandJournalBoundedStore *)context;
    CommandJournalBoundedRecord current;
    CommandJournalBoundedRecord replacement;
    CommandJournalBoundedSlotSelection selection;
    size_t logical_slot;
    Tr2Result result;

    if (!command_journal_bounded_store_is_initialized(store) ||
        store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (!command_transaction_id_is_valid(transaction_id) ||
        recovery_context == NULL || entry == NULL ||
        !command_recovery_context_is_valid(recovery_context)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = command_journal_bounded_reader_find(
        store->storage, transaction_id, &logical_slot, &current);
    result = guard_read_result(store, result);
    if (result != TR2_OK) {
        return result;
    }
    if (current.entry.lifecycle != COMMAND_LIFECYCLE_RESERVED ||
        current.entry.has_recovery_context) {
        return TR2_ERROR_INVALID_STATE;
    }

    result = command_journal_bounded_slot_select(
        store->storage, logical_slot, &selection);
    if (result != TR2_OK) {
        return guard_read_result(store, result);
    }
    if (selection.status != COMMAND_JOURNAL_BOUNDED_SLOT_VALID ||
        !selection.has_record) {
        store->recovery_required = true;
        return TR2_ERROR_CORRUPTED;
    }

    replacement = current;
    replacement.entry.has_recovery_context = true;
    replacement.entry.recovery_context = *recovery_context;
    result = command_journal_bounded_writer_mutate(
        store->storage, logical_slot, &selection, &replacement);
    if (result != TR2_OK) {
        if (result_requires_recovery(result)) {
            store->recovery_required = true;
        }
        return result;
    }

    *entry = replacement.entry;
    return TR2_OK;
}

static Tr2Result journal_mark_started(void *context,
                                      uint16_t transaction_id,
                                      CommandJournalEntry *entry)
{
    CommandJournalBoundedStore *store = (CommandJournalBoundedStore *)context;
    CommandJournalBoundedRecord current;
    CommandJournalBoundedRecord replacement;
    CommandJournalBoundedSlotSelection selection;
    size_t logical_slot;
    Tr2Result result;

    if (!command_journal_bounded_store_is_initialized(store) ||
        store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (!command_transaction_id_is_valid(transaction_id) || entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = command_journal_bounded_reader_find(
        store->storage, transaction_id, &logical_slot, &current);
    result = guard_read_result(store, result);
    if (result != TR2_OK) {
        return result;
    }
    if (current.entry.lifecycle != COMMAND_LIFECYCLE_RESERVED) {
        return TR2_ERROR_INVALID_STATE;
    }

    result = command_journal_bounded_slot_select(
        store->storage, logical_slot, &selection);
    if (result != TR2_OK) {
        return guard_read_result(store, result);
    }
    if (selection.status != COMMAND_JOURNAL_BOUNDED_SLOT_VALID ||
        !selection.has_record) {
        store->recovery_required = true;
        return TR2_ERROR_CORRUPTED;
    }

    replacement = current;
    replacement.entry.lifecycle = COMMAND_LIFECYCLE_STARTED;
    result = command_journal_bounded_writer_mutate(
        store->storage, logical_slot, &selection, &replacement);
    if (result != TR2_OK) {
        if (result_requires_recovery(result)) {
            store->recovery_required = true;
        }
        return result;
    }

    *entry = replacement.entry;
    return TR2_OK;
}

static Tr2Result journal_complete(void *context,
                                  uint16_t transaction_id,
                                  const CommandFinalResult *final_result,
                                  const CommandTerminalTimestamp *terminal_timestamp,
                                  CommandJournalEntry *entry)
{
    CommandJournalBoundedStore *store = (CommandJournalBoundedStore *)context;
    CommandJournalBoundedRecord current;
    CommandJournalBoundedRecord replacement;
    CommandJournalBoundedSlotSelection selection;
    size_t logical_slot;
    Tr2Result result;

    if (!command_journal_bounded_store_is_initialized(store) ||
        store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (!command_transaction_id_is_valid(transaction_id) ||
        final_result == NULL || terminal_timestamp == NULL || entry == NULL ||
        !command_status_is_final(final_result->status)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (store->next_completion_order == 0u ||
        store->next_completion_order == UINT32_MAX) {
        return TR2_ERROR_UNSUPPORTED;
    }

    result = command_journal_bounded_reader_find(
        store->storage, transaction_id, &logical_slot, &current);
    result = guard_read_result(store, result);
    if (result != TR2_OK) {
        return result;
    }
    if (current.entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED) {
        return TR2_ERROR_INVALID_STATE;
    }

    result = command_journal_bounded_slot_select(
        store->storage, logical_slot, &selection);
    if (result != TR2_OK) {
        return guard_read_result(store, result);
    }
    if (selection.status != COMMAND_JOURNAL_BOUNDED_SLOT_VALID ||
        !selection.has_record) {
        store->recovery_required = true;
        return TR2_ERROR_CORRUPTED;
    }

    replacement = current;
    replacement.entry.lifecycle = COMMAND_LIFECYCLE_COMPLETED;
    replacement.entry.has_final_result = true;
    replacement.entry.final_result = *final_result;
    replacement.entry.terminal_timestamp = *terminal_timestamp;
    replacement.entry.completion_order = store->next_completion_order;

    result = command_journal_bounded_writer_mutate(
        store->storage, logical_slot, &selection, &replacement);
    if (result != TR2_OK) {
        if (result_requires_recovery(result)) {
            store->recovery_required = true;
        }
        return result;
    }

    store->next_completion_order++;
    *entry = replacement.entry;
    return TR2_OK;
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
