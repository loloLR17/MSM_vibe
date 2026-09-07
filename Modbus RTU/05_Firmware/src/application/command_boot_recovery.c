#include <stddef.h>
#include <string.h>

#include "tr2/application/command_boot_recovery.h"

static CommandReconciliationOutcome reconcile_started(
    const CommandJournalEntry *entry,
    const CommandBootRecoveryAuthorities *authorities)
{
    switch (entry->request_identity.command_code) {
    case COMMAND_CODE_APPLY_CONFIGURATION:
        return command_apply_configuration_reconcile(entry,
                                                      authorities->configuration_service);
    case COMMAND_CODE_SYNCHRONIZE_TIME:
        return command_synchronize_time_reconcile(entry,
                                                  authorities->time_service);
    case COMMAND_CODE_START_ACQUISITION:
        return command_start_acquisition_reconcile(entry,
                                                   authorities->campaign_repository);
    case COMMAND_CODE_STOP_ACQUISITION:
        return command_stop_acquisition_reconcile(entry,
                                                  authorities->campaign_repository);
    default:
        return COMMAND_RECONCILIATION_INDETERMINATE;
    }
}

Tr2Result command_boot_recovery_scan(
    CommandJournalStore *journal_store,
    const CommandBootRecoveryAuthorities *authorities,
    CommandBootRecoveryResult *result)
{
    CommandJournal *journal;
    CommandJournalEntry entry;
    CommandReconciliationOutcome reconciliation;
    uint32_t transaction_id;
    Tr2Result lookup_result;

    if (journal_store == NULL || authorities == NULL || result == NULL ||
        !command_journal_store_is_initialized(journal_store) ||
        command_journal_store_recovery_required(journal_store)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    journal = command_journal_store_journal(journal_store);
    if (journal == NULL) {
        return TR2_ERROR_INVALID_STATE;
    }

    memset(result, 0, sizeof(*result));
    result->status = COMMAND_BOOT_RECOVERY_CLEAN;

    lookup_result = journal->latest_completed(journal->context, &entry);
    if (lookup_result == TR2_OK) {
        result->has_latest_completed = true;
        result->latest_completed = entry;
    } else if (lookup_result != TR2_ERROR_NOT_FOUND) {
        return lookup_result;
    }

    for (transaction_id = 1u;
         transaction_id <= (uint32_t)journal_store->max_transaction_id;
         ++transaction_id) {
        lookup_result = journal->find(journal->context,
                                      (uint16_t)transaction_id,
                                      &entry);
        if (lookup_result == TR2_ERROR_NOT_FOUND) {
            continue;
        }
        if (lookup_result != TR2_OK) {
            return lookup_result;
        }
        if (entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED) {
            continue;
        }
        if (result->has_incomplete_transaction) {
            return TR2_ERROR_CORRUPTED;
        }
        result->has_incomplete_transaction = true;
        result->incomplete_transaction = entry;
    }

    if (!result->has_incomplete_transaction) {
        return TR2_OK;
    }

    entry = result->incomplete_transaction;
    if (entry.lifecycle == COMMAND_LIFECYCLE_RESERVED) {
        result->status = COMMAND_BOOT_RECOVERY_RESERVED_NO_EFFECT;
        return TR2_OK;
    }
    if (entry.lifecycle != COMMAND_LIFECYCLE_STARTED) {
        return TR2_ERROR_CORRUPTED;
    }

    reconciliation = reconcile_started(&entry, authorities);
    if (reconciliation == COMMAND_RECONCILIATION_TERMINAL_EFFECT_PROVEN) {
        result->status = COMMAND_BOOT_RECOVERY_STARTED_EFFECT_PROVEN;
    } else if (reconciliation == COMMAND_RECONCILIATION_ABSENCE_PROVEN) {
        result->status = COMMAND_BOOT_RECOVERY_STARTED_ABSENCE_PROVEN;
    } else {
        result->status = COMMAND_BOOT_RECOVERY_STARTED_INDETERMINATE;
    }
    return TR2_OK;
}
