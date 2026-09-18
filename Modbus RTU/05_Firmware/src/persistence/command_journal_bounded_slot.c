#include "tr2/persistence/command_journal_bounded_slot.h"

#include <string.h>

typedef struct {
    bool empty;
    Tr2Result decode_status;
    CommandJournalBoundedRecord record;
} BoundedCopyInfo;

static bool bytes_uniform(const uint8_t *bytes, size_t size, uint8_t value)
{
    size_t index;

    for (index = 0u; index < size; ++index) {
        if (bytes[index] != value) {
            return false;
        }
    }
    return true;
}

static bool record_bytes_empty(const uint8_t *bytes)
{
    return bytes_uniform(bytes, TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE, UINT8_C(0x00)) ||
           bytes_uniform(bytes, TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE, UINT8_C(0xFF));
}

Tr2Result command_journal_bounded_slot_offset(size_t logical_slot,
                                              size_t copy_index,
                                              uint32_t *offset)
{
    if (offset == NULL ||
        logical_slot >= TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT ||
        copy_index >= TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COPY_COUNT) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    *offset = ((uint32_t)logical_slot * TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COPY_COUNT +
               (uint32_t)copy_index) *
              TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE;
    return TR2_OK;
}

static Tr2Result read_copy(const PersistentStorageCore *storage,
                           size_t logical_slot,
                           size_t copy_index,
                           BoundedCopyInfo *info)
{
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE];
    uint32_t offset;
    Tr2Result result;

    memset(info, 0, sizeof(*info));

    result = command_journal_bounded_slot_offset(logical_slot, copy_index, &offset);
    if (result != TR2_OK) {
        return result;
    }

    result = persistent_storage_core_read(storage, offset, bytes, sizeof(bytes));
    if (result != TR2_OK) {
        return result;
    }

    if (record_bytes_empty(bytes)) {
        info->empty = true;
        return TR2_OK;
    }

    info->decode_status =
        tr2_command_journal_bounded_record_decode(bytes, sizeof(bytes), &info->record);
    return TR2_OK;
}

Tr2Result command_journal_bounded_slot_select(
    const PersistentStorageCore *storage,
    size_t logical_slot,
    CommandJournalBoundedSlotSelection *selection)
{
    BoundedCopyInfo copies[TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COPY_COUNT];
    size_t index;
    bool found = false;
    bool saw_corrupted = false;
    bool saw_unsupported = false;
    bool conflicting_valid = false;
    size_t current_copy = 0u;
    CommandJournalBoundedRecord current;

    if (storage == NULL || selection == NULL ||
        !persistent_storage_core_is_initialized(storage) ||
        logical_slot >= TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(selection, 0, sizeof(*selection));
    memset(&current, 0, sizeof(current));

    for (index = 0u; index < TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COPY_COUNT; ++index) {
        Tr2Result result = read_copy(storage, logical_slot, index, &copies[index]);

        if (result != TR2_OK) {
            selection->status = COMMAND_JOURNAL_BOUNDED_SLOT_UNAVAILABLE;
            return TR2_OK;
        }
        if (copies[index].empty) {
            continue;
        }
        if (copies[index].decode_status == TR2_OK) {
            if (!found || copies[index].record.generation > current.generation) {
                current = copies[index].record;
                current_copy = index;
                found = true;
            } else if (copies[index].record.generation == current.generation &&
                       memcmp(&copies[index].record,
                              &current,
                              sizeof(CommandJournalBoundedRecord)) != 0) {
                conflicting_valid = true;
            }
        } else if (copies[index].decode_status == TR2_ERROR_UNSUPPORTED) {
            saw_unsupported = true;
        } else {
            saw_corrupted = true;
        }
    }

    if (found) {
        if (conflicting_valid) {
            selection->status = COMMAND_JOURNAL_BOUNDED_SLOT_CORRUPTED;
            return TR2_OK;
        }
        selection->status = COMMAND_JOURNAL_BOUNDED_SLOT_VALID;
        selection->has_record = true;
        selection->current_copy = current_copy;
        selection->record = current;
        return TR2_OK;
    }

    if (saw_unsupported) {
        selection->status = COMMAND_JOURNAL_BOUNDED_SLOT_UNSUPPORTED;
    } else if (saw_corrupted) {
        selection->status = COMMAND_JOURNAL_BOUNDED_SLOT_CORRUPTED;
    } else {
        selection->status = COMMAND_JOURNAL_BOUNDED_SLOT_EMPTY;
    }
    return TR2_OK;
}
