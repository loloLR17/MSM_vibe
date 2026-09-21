#include "tr2/persistence/command_journal_bounded_writer.h"

#include <stdint.h>

static Tr2Result write_record(PersistentStorageCore *storage,
                              size_t logical_slot,
                              size_t copy_index,
                              const CommandJournalBoundedRecord *record)
{
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE];
    uint32_t offset;
    Tr2Result result;

    result = command_journal_bounded_slot_offset(logical_slot, copy_index, &offset);
    if (result != TR2_OK) {
        return result;
    }

    result = tr2_command_journal_bounded_record_encode(record, bytes, sizeof(bytes));
    if (result != TR2_OK) {
        return result;
    }

    result = persistent_storage_core_write(storage, offset, bytes, sizeof(bytes));
    if (result != TR2_OK) {
        return result;
    }

    return persistent_storage_core_commit(storage);
}

Tr2Result command_journal_bounded_writer_admit_empty(
    PersistentStorageCore *storage,
    size_t logical_slot,
    const CommandJournalBoundedRecord *record)
{
    CommandJournalBoundedSlotSelection selection;
    CommandJournalBoundedRecord persisted;
    Tr2Result result;

    if (storage == NULL || record == NULL ||
        !persistent_storage_core_is_initialized(storage) ||
        logical_slot >= TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = command_journal_bounded_slot_select(storage, logical_slot, &selection);
    if (result != TR2_OK) {
        return result;
    }
    if (selection.status != COMMAND_JOURNAL_BOUNDED_SLOT_EMPTY ||
        selection.has_record) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (record->admission_order == 0u) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    persisted = *record;
    persisted.generation = 1u;
    return write_record(storage, logical_slot, 0u, &persisted);
}

Tr2Result command_journal_bounded_writer_mutate(
    PersistentStorageCore *storage,
    size_t logical_slot,
    const CommandJournalBoundedSlotSelection *current,
    const CommandJournalBoundedRecord *replacement)
{
    CommandJournalBoundedRecord persisted;

    if (storage == NULL || current == NULL || replacement == NULL ||
        !persistent_storage_core_is_initialized(storage) ||
        logical_slot >= TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT ||
        current->status != COMMAND_JOURNAL_BOUNDED_SLOT_VALID ||
        !current->has_record ||
        current->current_copy >= TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COPY_COUNT) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (current->record.generation == UINT32_MAX) {
        return TR2_ERROR_UNSUPPORTED;
    }
    if (replacement->admission_order != current->record.admission_order) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    persisted = *replacement;
    persisted.generation = current->record.generation + 1u;

    return write_record(storage,
                        logical_slot,
                        1u - current->current_copy,
                        &persisted);
}

Tr2Result command_journal_bounded_writer_readmit_completed(
    PersistentStorageCore *storage,
    size_t logical_slot,
    const CommandJournalBoundedSlotSelection *current,
    const CommandJournalBoundedRecord *replacement)
{
    CommandJournalBoundedRecord persisted;

    if (storage == NULL || current == NULL || replacement == NULL ||
        !persistent_storage_core_is_initialized(storage) ||
        logical_slot >= TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT ||
        current->status != COMMAND_JOURNAL_BOUNDED_SLOT_VALID ||
        !current->has_record ||
        current->current_copy >= TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COPY_COUNT) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (current->record.entry.lifecycle != COMMAND_LIFECYCLE_COMPLETED) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (current->record.generation == UINT32_MAX) {
        return TR2_ERROR_UNSUPPORTED;
    }
    if (replacement->admission_order == 0u ||
        replacement->admission_order == current->record.admission_order ||
        replacement->entry.lifecycle != COMMAND_LIFECYCLE_RESERVED ||
        !command_transaction_id_is_valid(replacement->entry.transaction_id)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    persisted = *replacement;
    persisted.generation = current->record.generation + 1u;

    return write_record(storage,
                        logical_slot,
                        1u - current->current_copy,
                        &persisted);
}
