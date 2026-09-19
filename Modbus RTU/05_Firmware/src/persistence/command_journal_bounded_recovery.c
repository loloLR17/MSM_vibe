#include "tr2/persistence/command_journal_bounded_recovery.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "tr2/persistence/command_journal_bounded_slot.h"

static void set_classified_result(CommandJournalBoundedRecoveryResult *result,
                                  CommandJournalBoundedRecoveryStatus status)
{
    memset(result, 0, sizeof(*result));
    result->status = status;
}

Tr2Result command_journal_bounded_recovery_scan(
    const PersistentStorageCore *storage,
    CommandJournalBoundedRecoveryResult *result)
{
    bool transaction_ids[UINT16_MAX + 1u] = { false };
    uint32_t admission_orders[TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT];
    uint32_t completion_orders[TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT];
    size_t admission_count = 0u;
    size_t completion_count = 0u;
    uint32_t known_count = 0u;
    uint32_t nonterminal_count = 0u;
    uint32_t max_admission_order = 0u;
    uint32_t max_completion_order = 0u;
    bool saw_unavailable = false;
    bool saw_unsupported = false;
    bool saw_corrupted = false;
    size_t logical_slot;

    if (storage == NULL || result == NULL ||
        !persistent_storage_core_is_initialized(storage)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(result, 0, sizeof(*result));
    memset(admission_orders, 0, sizeof(admission_orders));
    memset(completion_orders, 0, sizeof(completion_orders));

    for (logical_slot = 0u;
         logical_slot < TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT;
         ++logical_slot) {
        CommandJournalBoundedSlotSelection selection;
        Tr2Result scan_result;
        size_t index;

        scan_result = command_journal_bounded_slot_select(storage,
                                                          logical_slot,
                                                          &selection);
        if (scan_result != TR2_OK) {
            saw_unavailable = true;
            continue;
        }

        if (selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_EMPTY) {
            continue;
        }
        if (selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_UNAVAILABLE) {
            saw_unavailable = true;
            continue;
        }
        if (selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_UNSUPPORTED) {
            saw_unsupported = true;
            continue;
        }
        if (selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_CORRUPTED ||
            selection.status != COMMAND_JOURNAL_BOUNDED_SLOT_VALID ||
            !selection.has_record) {
            saw_corrupted = true;
            continue;
        }

        known_count++;

        if (transaction_ids[selection.record.entry.transaction_id]) {
            saw_corrupted = true;
        } else {
            transaction_ids[selection.record.entry.transaction_id] = true;
        }

        for (index = 0u; index < admission_count; ++index) {
            if (admission_orders[index] == selection.record.admission_order) {
                saw_corrupted = true;
                break;
            }
        }
        admission_orders[admission_count++] = selection.record.admission_order;
        if (selection.record.admission_order > max_admission_order) {
            max_admission_order = selection.record.admission_order;
        }

        if (selection.record.entry.lifecycle != COMMAND_LIFECYCLE_COMPLETED) {
            nonterminal_count++;
            if (nonterminal_count > 1u) {
                saw_corrupted = true;
            }
        } else {
            for (index = 0u; index < completion_count; ++index) {
                if (completion_orders[index] == selection.record.entry.completion_order) {
                    saw_corrupted = true;
                    break;
                }
            }
            completion_orders[completion_count++] =
                selection.record.entry.completion_order;
            if (selection.record.entry.completion_order > max_completion_order) {
                max_completion_order = selection.record.entry.completion_order;
            }
        }
    }

    if (saw_unavailable) {
        set_classified_result(result, COMMAND_JOURNAL_BOUNDED_RECOVERY_UNAVAILABLE);
        return TR2_OK;
    }
    if (saw_unsupported ||
        (known_count != 0u && max_admission_order == UINT32_MAX) ||
        max_completion_order == UINT32_MAX) {
        set_classified_result(result, COMMAND_JOURNAL_BOUNDED_RECOVERY_UNSUPPORTED);
        return TR2_OK;
    }
    if (saw_corrupted) {
        set_classified_result(result, COMMAND_JOURNAL_BOUNDED_RECOVERY_CORRUPTED);
        return TR2_OK;
    }

    result->known_transaction_count = known_count;
    result->next_admission_order =
        known_count == 0u ? 1u : max_admission_order + 1u;
    result->next_completion_order =
        completion_count == 0u ? 1u : max_completion_order + 1u;
    result->status = known_count == 0u
                         ? COMMAND_JOURNAL_BOUNDED_RECOVERY_EMPTY
                         : COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID;
    return TR2_OK;
}
