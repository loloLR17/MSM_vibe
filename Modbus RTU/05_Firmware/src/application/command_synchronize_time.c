#include <stddef.h>
#include <string.h>

#include "tr2/application/command_synchronize_time.h"

static CommandFinalResult final_result(uint16_t status, uint16_t result_code)
{
    CommandFinalResult result;
    memset(&result, 0, sizeof(result));
    result.status = status;
    result.result_code = result_code;
    return result;
}

Tr2Result command_synchronize_time_execute(
    CommandEngine *engine,
    CommandJournalStore *journal_store,
    TimeService *time_service,
    uint16_t transaction_id,
    uint16_t sync_source,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry)
{
    bool prepared_available = false;
    Tr2CivilTimestamp prepared_time = 0u;
    CommandRecoveryContext context;
    CommandFinalResult result;
    CommandJournalEntry current;
    Tr2Result operation_result;

    if (engine == NULL || journal_store == NULL || time_service == NULL ||
        terminal_timestamp == NULL || entry == NULL ||
        !command_transaction_id_is_valid(transaction_id) ||
        !command_engine_has_active_transaction(engine) ||
        command_engine_active_transaction_id(engine) != transaction_id) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    (void)journal_store;

    operation_result = engine->journal->find(engine->journal->context,
                                             transaction_id,
                                             &current);
    if (operation_result != TR2_OK) {
        return operation_result;
    }
    if (current.request_identity.command_code != COMMAND_CODE_SYNCHRONIZE_TIME ||
        current.lifecycle != COMMAND_LIFECYCLE_RESERVED) {
        return TR2_ERROR_INVALID_STATE;
    }

    operation_result = time_service_get_prepared_time(time_service,
                                                      &prepared_available,
                                                      &prepared_time);
    if (operation_result != TR2_OK) {
        return operation_result;
    }
    if (!prepared_available) {
        result = final_result(COMMAND_STATUS_REFUSED,
                              COMMAND_RESULT_PREPARED_TIME_ABSENT);
        return command_engine_complete(engine,
                                       transaction_id,
                                       &result,
                                       terminal_timestamp,
                                       entry);
    }

    memset(&context, 0, sizeof(context));
    context.kind = COMMAND_RECOVERY_CONTEXT_TIME_SYNC;
    context.value1 = prepared_time;
    context.value2 = sync_source;

    if (engine->journal->set_recovery_context == NULL) {
        return TR2_ERROR_INVALID_STATE;
    }
    operation_result = engine->journal->set_recovery_context(engine->journal->context,
                                                              transaction_id,
                                                              &context,
                                                              entry);
    if (operation_result != TR2_OK) {
        return operation_result;
    }

    operation_result = command_engine_mark_started(engine, transaction_id, entry);
    if (operation_result != TR2_OK) {
        return operation_result;
    }

    operation_result = time_service_synchronize_prepared(time_service, sync_source);
    if (operation_result != TR2_OK) {
        return operation_result;
    }

    result = final_result(COMMAND_STATUS_SUCCESS, COMMAND_RESULT_SUCCESS);
    return command_engine_complete(engine,
                                   transaction_id,
                                   &result,
                                   terminal_timestamp,
                                   entry);
}

CommandReconciliationOutcome command_synchronize_time_reconcile(
    const CommandJournalEntry *entry,
    const TimeService *time_service)
{
    TimeSnapshot snapshot;
    Tr2Result result;

    if (entry == NULL || time_service == NULL ||
        entry->lifecycle != COMMAND_LIFECYCLE_STARTED ||
        !entry->has_recovery_context ||
        entry->recovery_context.kind != COMMAND_RECOVERY_CONTEXT_TIME_SYNC) {
        return COMMAND_RECONCILIATION_INDETERMINATE;
    }

    memset(&snapshot, 0, sizeof(snapshot));
    result = time_service_get_snapshot(time_service, &snapshot);
    if (result != TR2_OK) {
        return COMMAND_RECONCILIATION_INDETERMINATE;
    }

    if (snapshot.synchronization_facts_available &&
        snapshot.last_sync_history.state == LAST_SYNC_HISTORY_VALID) {
        if (snapshot.last_sync_history.timestamp == entry->recovery_context.value1 &&
            snapshot.last_sync_history.source == (uint16_t)entry->recovery_context.value2) {
            return COMMAND_RECONCILIATION_TERMINAL_EFFECT_PROVEN;
        }
        return COMMAND_RECONCILIATION_ABSENCE_PROVEN;
    }

    if (snapshot.last_sync_history.state == LAST_SYNC_HISTORY_NONE) {
        return COMMAND_RECONCILIATION_ABSENCE_PROVEN;
    }

    return COMMAND_RECONCILIATION_INDETERMINATE;
}
