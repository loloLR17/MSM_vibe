#include <stddef.h>
#include <string.h>

#include "tr2/application/command_apply_configuration.h"

static CommandFinalResult final_result(uint16_t status, uint16_t result_code)
{
    CommandFinalResult result;
    memset(&result, 0, sizeof(result));
    result.status = status;
    result.result_code = result_code;
    return result;
}

Tr2Result command_apply_configuration_execute(
    CommandEngine *engine,
    CommandJournalStore *journal_store,
    ConfigurationWorkflow *workflow,
    uint16_t transaction_id,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry)
{
    ValidatedConfiguration validated;
    CommandRecoveryContext context;
    CommandFinalResult result;
    CommandJournalEntry current;
    Tr2Result operation_result;

    if (engine == NULL || journal_store == NULL || workflow == NULL ||
        terminal_timestamp == NULL || entry == NULL ||
        !command_transaction_id_is_valid(transaction_id) ||
        !command_engine_has_active_transaction(engine) ||
        command_engine_active_transaction_id(engine) != transaction_id) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    operation_result = engine->journal->find(engine->journal->context,
                                             transaction_id,
                                             &current);
    if (operation_result != TR2_OK) {
        return operation_result;
    }
    if (current.request_identity.command_code != COMMAND_CODE_APPLY_CONFIGURATION ||
        current.lifecycle != COMMAND_LIFECYCLE_RESERVED) {
        return TR2_ERROR_INVALID_STATE;
    }

    memset(&validated, 0, sizeof(validated));
    if (!configuration_workflow_validated_snapshot(workflow, &validated)) {
        result = final_result(COMMAND_STATUS_REFUSED,
                              COMMAND_RESULT_PREPARED_CONFIGURATION_INCOMPLETE);
        return command_engine_complete(engine,
                                       transaction_id,
                                       &result,
                                       terminal_timestamp,
                                       entry);
    }

    memset(&context, 0, sizeof(context));
    context.kind = COMMAND_RECOVERY_CONTEXT_CONFIGURATION;
    context.value1 = validated.generation;
    context.value2 = validated.config_id;
    context.value3 = validated.supplied_crc;

    operation_result = command_journal_store_set_recovery_context(journal_store,
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

    operation_result = configuration_workflow_apply(workflow);
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

CommandReconciliationOutcome command_apply_configuration_reconcile(
    const CommandJournalEntry *entry,
    const ConfigurationService *configuration_service)
{
    ActiveConfigurationSnapshot active;
    ConfigurationRecoveryStatus recovery_status;

    if (entry == NULL || configuration_service == NULL ||
        entry->lifecycle != COMMAND_LIFECYCLE_STARTED ||
        !entry->has_recovery_context ||
        entry->recovery_context.kind != COMMAND_RECOVERY_CONTEXT_CONFIGURATION) {
        return COMMAND_RECONCILIATION_INDETERMINATE;
    }

    memset(&active, 0, sizeof(active));
    if (configuration_service_active_snapshot(configuration_service, &active)) {
        if (active.generation == entry->recovery_context.value1 &&
            active.config_id == entry->recovery_context.value2) {
            return COMMAND_RECONCILIATION_TERMINAL_EFFECT_PROVEN;
        }
        return COMMAND_RECONCILIATION_ABSENCE_PROVEN;
    }

    if (!configuration_service_recovery_status(configuration_service, &recovery_status)) {
        return COMMAND_RECONCILIATION_INDETERMINATE;
    }
    if (recovery_status == CONFIGURATION_RECOVERY_EMPTY) {
        return COMMAND_RECONCILIATION_ABSENCE_PROVEN;
    }

    return COMMAND_RECONCILIATION_INDETERMINATE;
}
