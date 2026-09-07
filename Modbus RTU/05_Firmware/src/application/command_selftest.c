#include <stddef.h>
#include <string.h>

#include "tr2/application/command_selftest.h"

static bool request_parameters_valid(const CommandRequestIdentity *identity)
{
    return identity->param1 == 0u &&
           identity->param2 == 0u &&
           identity->param3 == 0u;
}

static Tr2Result complete_selftest_command(
    CommandEngine *engine,
    uint16_t transaction_id,
    uint16_t status,
    uint16_t result_code,
    uint16_t result_detail,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry)
{
    CommandFinalResult final_result;

    memset(&final_result, 0, sizeof(final_result));
    final_result.status = status;
    final_result.result_code = result_code;
    final_result.result_detail = result_detail;
    return command_engine_complete(engine,
                                   transaction_id,
                                   &final_result,
                                   terminal_timestamp,
                                   entry);
}

Tr2Result command_selftest_execute(
    CommandEngine *engine,
    SelfTestService *selftest_service,
    const SelfTestExecutor *executor,
    uint16_t transaction_id,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry)
{
    CommandJournalEntry current;
    SelfTestExecutionResult execution;
    Tr2Result result;

    if (engine == NULL || !selftest_service_is_initialized(selftest_service) ||
        executor == NULL || executor->run_standard == NULL ||
        terminal_timestamp == NULL || entry == NULL ||
        !command_transaction_id_is_valid(transaction_id) ||
        !command_engine_has_active_transaction(engine) ||
        command_engine_active_transaction_id(engine) != transaction_id) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = engine->journal->find(engine->journal->context, transaction_id, &current);
    if (result != TR2_OK) {
        return result;
    }
    if (current.request_identity.command_code != COMMAND_CODE_SELFTEST ||
        current.lifecycle != COMMAND_LIFECYCLE_RESERVED ||
        current.has_recovery_context) {
        return TR2_ERROR_INVALID_STATE;
    }

    if (!request_parameters_valid(&current.request_identity)) {
        return complete_selftest_command(engine,
                                         transaction_id,
                                         COMMAND_STATUS_REFUSED,
                                         COMMAND_RESULT_INVALID_PARAMETER,
                                         0u,
                                         terminal_timestamp,
                                         entry);
    }
    if (selftest_service_running(selftest_service)) {
        return complete_selftest_command(engine,
                                         transaction_id,
                                         COMMAND_STATUS_REFUSED,
                                         COMMAND_RESULT_INCOMPATIBLE_STATE,
                                         0u,
                                         terminal_timestamp,
                                         entry);
    }

    result = command_engine_mark_started(engine, transaction_id, entry);
    if (result != TR2_OK) {
        return result;
    }

    result = selftest_service_begin_standard(selftest_service);
    if (result != TR2_OK) {
        return result;
    }

    memset(&execution, 0, sizeof(execution));
    result = executor->run_standard(executor->context, &execution);
    if (result != TR2_OK) {
        return result;
    }

    result = selftest_service_complete(selftest_service,
                                       execution.passed,
                                       execution.result_code,
                                       execution.detail);
    if (result != TR2_OK) {
        return result;
    }

    return complete_selftest_command(
        engine,
        transaction_id,
        execution.passed ? COMMAND_STATUS_SUCCESS : COMMAND_STATUS_FAILED,
        execution.passed ? COMMAND_RESULT_SUCCESS : COMMAND_RESULT_SELFTEST_FAILED,
        execution.passed ? 0u : execution.detail,
        terminal_timestamp,
        entry);
}

CommandReconciliationOutcome command_selftest_reconcile(
    const CommandJournalEntry *entry)
{
    if (entry == NULL ||
        entry->request_identity.command_code != COMMAND_CODE_SELFTEST ||
        entry->lifecycle != COMMAND_LIFECYCLE_STARTED) {
        return COMMAND_RECONCILIATION_INDETERMINATE;
    }

    return COMMAND_RECONCILIATION_INDETERMINATE;
}
