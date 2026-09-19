#include "tr2/persistence/command_journal_bounded_reader.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "tr2/persistence/command_journal_bounded_slot.h"

static Tr2Result selection_error(CommandJournalBoundedSlotStatus status)
{
    if (status == COMMAND_JOURNAL_BOUNDED_SLOT_UNAVAILABLE) {
        return TR2_ERROR_UNAVAILABLE;
    }
    if (status == COMMAND_JOURNAL_BOUNDED_SLOT_UNSUPPORTED) {
        return TR2_ERROR_UNSUPPORTED;
    }
    return TR2_ERROR_CORRUPTED;
}

static Tr2Result select_valid_or_empty(
    const PersistentStorageCore *storage,
    size_t logical_slot,
    CommandJournalBoundedSlotSelection *selection)
{
    Tr2Result result =
        command_journal_bounded_slot_select(storage, logical_slot, selection);

    if (result != TR2_OK) {
        return result;
    }
    if (selection->status == COMMAND_JOURNAL_BOUNDED_SLOT_EMPTY ||
        (selection->status == COMMAND_JOURNAL_BOUNDED_SLOT_VALID &&
         selection->has_record)) {
        return TR2_OK;
    }
    return selection_error(selection->status);
}

Tr2Result command_journal_bounded_reader_find(
    const PersistentStorageCore *storage,
    uint16_t transaction_id,
    size_t *logical_slot,
    CommandJournalBoundedRecord *record)
{
    size_t slot;

    if (storage == NULL || logical_slot == NULL || record == NULL ||
        !persistent_storage_core_is_initialized(storage) ||
        transaction_id == 0u) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    for (slot = 0u; slot < TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT; ++slot) {
        CommandJournalBoundedSlotSelection selection;
        Tr2Result result = select_valid_or_empty(storage, slot, &selection);

        if (result != TR2_OK) {
            return result;
        }
        if (selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_VALID &&
            selection.record.entry.transaction_id == transaction_id) {
            *logical_slot = slot;
            *record = selection.record;
            return TR2_OK;
        }
    }

    return TR2_ERROR_NOT_FOUND;
}

Tr2Result command_journal_bounded_reader_visit(
    const PersistentStorageCore *storage,
    CommandJournalBoundedRecordVisitor visitor,
    void *visitor_context)
{
    size_t slot;

    if (storage == NULL || visitor == NULL ||
        !persistent_storage_core_is_initialized(storage)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    for (slot = 0u; slot < TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT; ++slot) {
        CommandJournalBoundedSlotSelection selection;
        Tr2Result result = select_valid_or_empty(storage, slot, &selection);

        if (result != TR2_OK) {
            return result;
        }
        if (selection.status != COMMAND_JOURNAL_BOUNDED_SLOT_VALID) {
            continue;
        }

        result = visitor(visitor_context, slot, &selection.record);
        if (result != TR2_OK) {
            return result;
        }
    }

    return TR2_OK;
}

Tr2Result command_journal_bounded_reader_latest_completed(
    const PersistentStorageCore *storage,
    size_t *logical_slot,
    CommandJournalBoundedRecord *record)
{
    bool found = false;
    size_t found_slot = 0u;
    CommandJournalBoundedRecord latest;
    size_t slot;

    if (storage == NULL || logical_slot == NULL || record == NULL ||
        !persistent_storage_core_is_initialized(storage)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(&latest, 0, sizeof(latest));

    for (slot = 0u; slot < TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT; ++slot) {
        CommandJournalBoundedSlotSelection selection;
        Tr2Result result = select_valid_or_empty(storage, slot, &selection);

        if (result != TR2_OK) {
            return result;
        }
        if (selection.status != COMMAND_JOURNAL_BOUNDED_SLOT_VALID ||
            selection.record.entry.lifecycle != COMMAND_LIFECYCLE_COMPLETED) {
            continue;
        }

        if (found &&
            selection.record.entry.completion_order == latest.entry.completion_order) {
            return TR2_ERROR_CORRUPTED;
        }
        if (!found ||
            selection.record.entry.completion_order > latest.entry.completion_order) {
            latest = selection.record;
            found_slot = slot;
            found = true;
        }
    }

    if (!found) {
        return TR2_ERROR_NOT_FOUND;
    }

    *logical_slot = found_slot;
    *record = latest;
    return TR2_OK;
}
