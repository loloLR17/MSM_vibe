#include <stddef.h>
#include <string.h>

#include "tr2/application/command_boot_recovery.h"

static bool software_reset_parameters_valid(const CommandRequestIdentity *identity)
{
    return identity != NULL &&
           identity->param1 == 0u &&
           identity->param2 == 0u &&
           identity->param3 == 0u &&
           identity->confirm_key == TR2_COMMAND_CONFIRM_KEY_VALID;
}

static Tr2Result complete_reset_refusal(
    CommandEngine *engine,
    uint16_t transaction_id,
    uint16_t result_code,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry)
{
    CommandFinalResult final_result;

    memset(&final_result, 0, sizeof(final_result));
    final_result.status = COMMAND_STATUS_REFUSED;
    final_result.result_code = result_code;
    return command_engine_complete(engine,
                                   transaction_id,
                                   &final_result,
                                   terminal_timestamp,
                                   entry);
}

Tr2Result command_software_reset_execute(
    CommandEngine *engine,
    BootIntentStore *boot_intent_store,
    const PlatformResetTrigger *reset_trigger,
    bool acquisition_active,
    bool critical_operation_active,
    uint16_t transaction_id,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry)
{
    CommandJournalEntry current;
    CommandRecoveryContext recovery_context;
    BootIntent boot_intent;
    Tr2Result result;

    if (engine == NULL || !boot_intent_store_is_initialized(boot_intent_store) ||
        !platform_reset_trigger_is_valid(reset_trigger) ||
        terminal_timestamp == NULL || entry == NULL ||
        !command_transaction_id_is_valid(transaction_id) ||
        !command_engine_has_active_transaction(engine) ||
        command_engine_active_transaction_id(engine) != transaction_id ||
        engine->journal == NULL || engine->journal->set_recovery_context == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = engine->journal->find(engine->journal->context, transaction_id, &current);
    if (result != TR2_OK) {
        return result;
    }
    if (current.request_identity.command_code != COMMAND_CODE_SOFTWARE_RESET ||
        current.lifecycle != COMMAND_LIFECYCLE_RESERVED ||
        current.has_recovery_context) {
        return TR2_ERROR_INVALID_STATE;
    }

    if (!software_reset_parameters_valid(&current.request_identity)) {
        return complete_reset_refusal(engine,
                                      transaction_id,
                                      current.request_identity.confirm_key ==
                                              TR2_COMMAND_CONFIRM_KEY_VALID
                                          ? COMMAND_RESULT_INVALID_PARAMETER
                                          : COMMAND_RESULT_CONFIRMATION_MISSING,
                                      terminal_timestamp,
                                      entry);
    }
    if (acquisition_active) {
        return complete_reset_refusal(engine,
                                      transaction_id,
                                      COMMAND_RESULT_ACQUISITION_RUNNING,
                                      terminal_timestamp,
                                      entry);
    }
    if (critical_operation_active) {
        return complete_reset_refusal(engine,
                                      transaction_id,
                                      COMMAND_RESULT_INCOMPATIBLE_STATE,
                                      terminal_timestamp,
                                      entry);
    }

    memset(&recovery_context, 0, sizeof(recovery_context));
    recovery_context.kind = COMMAND_RECOVERY_CONTEXT_BOOT_INTENT;
    recovery_context.value1 = transaction_id;
    result = engine->journal->set_recovery_context(engine->journal->context,
                                                   transaction_id,
                                                   &recovery_context,
                                                   entry);
    if (result != TR2_OK) {
        return result;
    }

    boot_intent = boot_intent_software_reset(transaction_id);
    result = boot_intent_store_commit(boot_intent_store, &boot_intent);
    if (result != TR2_OK) {
        return result;
    }

    result = command_engine_mark_started(engine, transaction_id, entry);
    if (result != TR2_OK) {
        return result;
    }

    return reset_trigger->software_reset(reset_trigger->context);
}

CommandReconciliationOutcome command_software_reset_reconcile(
    const CommandJournalEntry *entry,
    const BootIntentRecoveryResult *boot_intent,
    ResetCause reset_cause)
{
    if (entry == NULL ||
        entry->request_identity.command_code != COMMAND_CODE_SOFTWARE_RESET ||
        entry->lifecycle != COMMAND_LIFECYCLE_STARTED ||
        !entry->has_recovery_context ||
        entry->recovery_context.kind != COMMAND_RECOVERY_CONTEXT_BOOT_INTENT ||
        entry->recovery_context.value1 != entry->transaction_id ||
        entry->recovery_context.value2 != 0u ||
        entry->recovery_context.value3 != 0u ||
        boot_intent == NULL ||
        boot_intent->status != BOOT_INTENT_RECOVERY_VALID ||
        boot_intent->intent.kind != BOOT_INTENT_SOFTWARE_RESET ||
        boot_intent->intent.transaction_id != entry->transaction_id) {
        return COMMAND_RECONCILIATION_INDETERMINATE;
    }

    if (reset_cause == RESET_CAUSE_SOFTWARE) {
        return COMMAND_RECONCILIATION_TERMINAL_EFFECT_PROVEN;
    }

    return COMMAND_RECONCILIATION_INDETERMINATE;
}

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
    case COMMAND_CODE_SELFTEST:
        return command_selftest_reconcile(entry);
    case COMMAND_CODE_SOFTWARE_RESET:
        return command_software_reset_reconcile(entry,
                                                authorities->boot_intent,
                                                authorities->reset_cause);
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
