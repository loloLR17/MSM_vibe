#include "tr2/persistence/command_journal_bounded_admission.h"

#include <stdbool.h>
#include <string.h>

static Tr2Result slot_error(CommandJournalBoundedSlotStatus status)
{
    if (status == COMMAND_JOURNAL_BOUNDED_SLOT_UNAVAILABLE) {
        return TR2_ERROR_UNAVAILABLE;
    }
    if (status == COMMAND_JOURNAL_BOUNDED_SLOT_UNSUPPORTED) {
        return TR2_ERROR_UNSUPPORTED;
    }
    return TR2_ERROR_CORRUPTED;
}

Tr2Result command_journal_bounded_admission_plan(
    const PersistentStorageCore *storage,
    uint16_t transaction_id,
    const CommandRequestIdentity *request_identity,
    CommandJournalBoundedAdmissionPlan *plan)
{
    bool has_empty = false;
    size_t first_empty = 0u;
    bool has_victim = false;
    size_t victim_slot = 0u;
    CommandJournalBoundedSlotSelection victim;
    size_t logical_slot;

    if (storage == NULL || request_identity == NULL || plan == NULL ||
        !persistent_storage_core_is_initialized(storage) ||
        !command_transaction_id_is_valid(transaction_id)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(plan, 0, sizeof(*plan));

    for (logical_slot = 0u;
         logical_slot < TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT;
         ++logical_slot) {
        CommandJournalBoundedSlotSelection selection;
        Tr2Result result = command_journal_bounded_slot_select(
            storage, logical_slot, &selection);

        if (result != TR2_OK) {
            return result;
        }
        if (selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_EMPTY) {
            if (!has_empty) {
                has_empty = true;
                first_empty = logical_slot;
            }
            continue;
        }
        if (selection.status != COMMAND_JOURNAL_BOUNDED_SLOT_VALID ||
            !selection.has_record) {
            return slot_error(selection.status);
        }

        if (selection.record.entry.transaction_id == transaction_id) {
            plan->kind = command_request_identity_equal(
                             &selection.record.entry.request_identity,
                             request_identity)
                             ? COMMAND_JOURNAL_BOUNDED_ADMISSION_RETRY_EXISTING
                             : COMMAND_JOURNAL_BOUNDED_ADMISSION_COLLISION_EXISTING;
            plan->has_logical_slot = true;
            plan->logical_slot = logical_slot;
            plan->has_current = true;
            plan->current = selection;
            return TR2_OK;
        }

        if (selection.record.entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED &&
            selection.record.generation != UINT32_MAX &&
            (!has_victim ||
             selection.record.admission_order < victim.record.admission_order)) {
            has_victim = true;
            victim_slot = logical_slot;
            victim = selection;
        }
    }

    if (has_empty) {
        plan->kind = COMMAND_JOURNAL_BOUNDED_ADMISSION_ADMIT_EMPTY;
        plan->has_logical_slot = true;
        plan->logical_slot = first_empty;
        return TR2_OK;
    }

    if (has_victim) {
        plan->kind = COMMAND_JOURNAL_BOUNDED_ADMISSION_EVICT_COMPLETED;
        plan->has_logical_slot = true;
        plan->logical_slot = victim_slot;
        plan->has_current = true;
        plan->current = victim;
        return TR2_OK;
    }

    plan->kind = COMMAND_JOURNAL_BOUNDED_ADMISSION_NO_CAPACITY;
    return TR2_OK;
}
