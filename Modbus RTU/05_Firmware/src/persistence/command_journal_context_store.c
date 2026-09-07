#include "tr2/persistence/command_journal_store.h"

#include <stddef.h>
#include <string.h>

typedef struct {
    bool empty;
    Tr2Result decode_status;
    CommandJournalRecord record;
} ContextSlotInfo;

static uint32_t context_slot_offset(uint16_t transaction_id, size_t slot_index)
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

static Tr2Result read_context_slot(const CommandJournalStore *store,
                                   uint16_t transaction_id,
                                   size_t slot_index,
                                   ContextSlotInfo *info)
{
    uint8_t bytes[TR2_COMMAND_JOURNAL_RECORD_SIZE];
    Tr2Result result;

    memset(info, 0, sizeof(*info));
    result = persistent_storage_core_read(store->storage,
                                          context_slot_offset(transaction_id, slot_index),
                                          bytes,
                                          sizeof(bytes));
    if (result != TR2_OK) {
        return result;
    }
    if (bytes_uniform(bytes, UINT8_C(0x00)) || bytes_uniform(bytes, UINT8_C(0xFF))) {
        info->empty = true;
        return TR2_OK;
    }

    info->decode_status = tr2_command_journal_record_decode(bytes,
                                                             sizeof(bytes),
                                                             &info->record);
    return TR2_OK;
}

Tr2Result command_journal_store_set_recovery_context(
    CommandJournalStore *store,
    uint16_t transaction_id,
    const CommandRecoveryContext *recovery_context,
    CommandJournalEntry *entry)
{
    ContextSlotInfo slots[TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS];
    CommandJournalRecord current;
    CommandJournalRecord next;
    bool found = false;
    bool saw_corrupted = false;
    bool saw_unsupported = false;
    size_t current_slot = 0u;
    size_t index;
    Tr2Result result;
    uint8_t bytes[TR2_COMMAND_JOURNAL_RECORD_SIZE];

    if (!command_journal_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (entry == NULL || !command_transaction_id_is_valid(transaction_id) ||
        transaction_id > store->max_transaction_id ||
        !command_recovery_context_is_valid(recovery_context)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(&current, 0, sizeof(current));
    for (index = 0u; index < TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS; ++index) {
        result = read_context_slot(store, transaction_id, index, &slots[index]);
        if (result != TR2_OK) {
            return result;
        }
        if (slots[index].empty) {
            continue;
        }
        if (slots[index].decode_status == TR2_ERROR_UNSUPPORTED) {
            saw_unsupported = true;
            continue;
        }
        if (slots[index].decode_status != TR2_OK ||
            slots[index].record.entry.transaction_id != transaction_id) {
            saw_corrupted = true;
            continue;
        }
        if (!found || slots[index].record.generation > current.generation) {
            current = slots[index].record;
            current_slot = index;
            found = true;
        }
    }

    if (!found) {
        if (saw_unsupported) {
            return TR2_ERROR_UNSUPPORTED;
        }
        if (saw_corrupted) {
            return TR2_ERROR_CORRUPTED;
        }
        return TR2_ERROR_NOT_FOUND;
    }
    if (current.entry.lifecycle != COMMAND_LIFECYCLE_RESERVED ||
        current.entry.has_recovery_context || current.generation == UINT32_MAX) {
        return TR2_ERROR_INVALID_STATE;
    }

    next = current;
    next.generation++;
    next.entry.has_recovery_context = true;
    next.entry.recovery_context = *recovery_context;

    result = tr2_command_journal_record_encode(&next, bytes, sizeof(bytes));
    if (result != TR2_OK) {
        return result;
    }
    result = persistent_storage_core_write(store->storage,
                                           context_slot_offset(transaction_id, 1u - current_slot),
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

    *entry = next.entry;
    return TR2_OK;
}
