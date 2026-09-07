#include <stddef.h>
#include <string.h>

#include "tr2/application/command_acknowledge_fault.h"
#include "tr2/application/command_maintenance.h"
#include "tr2/application/command_refresh_indicators.h"

static bool journal_contract_valid(const CommandJournal *journal)
{
    return journal != NULL &&
           journal->find != NULL &&
           journal->reserve != NULL &&
           journal->mark_started != NULL &&
           journal->complete != NULL &&
           journal->latest_completed != NULL;
}

static bool active_entry_matches(const CommandEngine *engine,
                                 const CommandJournalEntry *entry,
                                 uint16_t transaction_id)
{
    return entry != NULL &&
           command_journal_entry_is_consistent(entry) &&
           entry->transaction_id == transaction_id &&
           engine->has_active_transaction &&
           engine->active_transaction_id == transaction_id;
}

Tr2Result command_engine_init(CommandEngine *engine, CommandJournal *journal)
{
    if (engine == NULL || !journal_contract_valid(journal)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(engine, 0, sizeof(*engine));
    engine->journal = journal;
    engine->initialized = true;
    return TR2_OK;
}

bool command_engine_is_initialized(const CommandEngine *engine)
{
    return engine != NULL && engine->initialized && journal_contract_valid(engine->journal);
}

bool command_engine_has_active_transaction(const CommandEngine *engine)
{
    return command_engine_is_initialized(engine) && engine->has_active_transaction;
}

uint16_t command_engine_active_transaction_id(const CommandEngine *engine)
{
    if (!command_engine_has_active_transaction(engine)) {
        return TR2_COMMAND_TRANSACTION_ID_INVALID;
    }

    return engine->active_transaction_id;
}

Tr2Result command_engine_restore_incomplete(CommandEngine *engine,
                                            const CommandJournalEntry *entry)
{
    CommandJournalEntry durable_entry;
    Tr2Result result;

    if (!command_engine_is_initialized(engine) || entry == NULL ||
        !command_journal_entry_is_consistent(entry) ||
        entry->lifecycle == COMMAND_LIFECYCLE_COMPLETED ||
        !command_transaction_id_is_valid(entry->transaction_id)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (engine->has_active_transaction) {
        return TR2_ERROR_INVALID_STATE;
    }

    result = engine->journal->find(engine->journal->context,
                                   entry->transaction_id,
                                   &durable_entry);
    if (result != TR2_OK) {
        return result;
    }
    if (!command_journal_entry_is_consistent(&durable_entry) ||
        durable_entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED ||
        durable_entry.transaction_id != entry->transaction_id ||
        !command_request_identity_equal(&durable_entry.request_identity,
                                        &entry->request_identity)) {
        return TR2_ERROR_CORRUPTED;
    }

    engine->has_active_transaction = true;
    engine->active_transaction_id = entry->transaction_id;
    ++engine->snapshot_generation;
    return TR2_OK;
}

Tr2Result command_engine_admit(CommandEngine *engine,
                               const CommandRequest *request,
                               CommandAdmissionResult *result)
{
    CommandJournalEntry existing;
    Tr2Result lookup_result;
    Tr2Result reserve_result;

    if (!command_engine_is_initialized(engine) || request == NULL || result == NULL ||
        !command_transaction_id_is_valid(request->transaction_id) ||
        request->identity.command_code == COMMAND_CODE_NONE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(result, 0, sizeof(*result));
    memset(&existing, 0, sizeof(existing));

    lookup_result = engine->journal->find(engine->journal->context,
                                          request->transaction_id,
                                          &existing);
    if (lookup_result == TR2_OK) {
        result->entry = existing;
        if (command_request_identity_equal(&existing.request_identity, &request->identity)) {
            result->kind = COMMAND_ADMISSION_RETRY;
        } else {
            result->kind = COMMAND_ADMISSION_COLLISION;
        }
        return TR2_OK;
    }
    if (lookup_result != TR2_ERROR_NOT_FOUND) {
        return lookup_result;
    }

    if (engine->has_active_transaction) {
        result->kind = COMMAND_ADMISSION_BUSY;
        return TR2_OK;
    }

    reserve_result = engine->journal->reserve(engine->journal->context,
                                              request,
                                              &result->entry);
    if (reserve_result != TR2_OK) {
        return reserve_result;
    }
    if (!command_journal_entry_is_consistent(&result->entry) ||
        result->entry.lifecycle != COMMAND_LIFECYCLE_RESERVED ||
        result->entry.transaction_id != request->transaction_id ||
        !command_request_identity_equal(&result->entry.request_identity,
                                        &request->identity)) {
        return TR2_ERROR_INTERNAL;
    }

    engine->has_active_transaction = true;
    engine->active_transaction_id = request->transaction_id;
    ++engine->snapshot_generation;
    result->kind = COMMAND_ADMISSION_NEW;
    return TR2_OK;
}

Tr2Result command_engine_mark_started(CommandEngine *engine,
                                      uint16_t transaction_id,
                                      CommandJournalEntry *entry)
{
    Tr2Result result;

    if (!command_engine_is_initialized(engine) || entry == NULL ||
        !command_transaction_id_is_valid(transaction_id)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!engine->has_active_transaction ||
        engine->active_transaction_id != transaction_id) {
        return TR2_ERROR_INVALID_STATE;
    }

    result = engine->journal->mark_started(engine->journal->context,
                                           transaction_id,
                                           entry);
    if (result != TR2_OK) {
        return result;
    }
    if (!active_entry_matches(engine, entry, transaction_id) ||
        entry->lifecycle != COMMAND_LIFECYCLE_STARTED) {
        return TR2_ERROR_INTERNAL;
    }

    ++engine->snapshot_generation;
    return TR2_OK;
}

Tr2Result command_engine_complete(CommandEngine *engine,
                                  uint16_t transaction_id,
                                  const CommandFinalResult *final_result,
                                  const CommandTerminalTimestamp *terminal_timestamp,
                                  CommandJournalEntry *entry)
{
    Tr2Result result;

    if (!command_engine_is_initialized(engine) || final_result == NULL ||
        terminal_timestamp == NULL || entry == NULL ||
        !command_transaction_id_is_valid(transaction_id) ||
        !command_status_is_final(final_result->status)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!engine->has_active_transaction ||
        engine->active_transaction_id != transaction_id) {
        return TR2_ERROR_INVALID_STATE;
    }

    result = engine->journal->complete(engine->journal->context,
                                       transaction_id,
                                       final_result,
                                       terminal_timestamp,
                                       entry);
    if (result != TR2_OK) {
        return result;
    }
    if (!active_entry_matches(engine, entry, transaction_id) ||
        entry->lifecycle != COMMAND_LIFECYCLE_COMPLETED ||
        !entry->has_final_result) {
        return TR2_ERROR_INTERNAL;
    }

    engine->has_active_transaction = false;
    engine->active_transaction_id = TR2_COMMAND_TRANSACTION_ID_INVALID;
    ++engine->snapshot_generation;
    return TR2_OK;
}

Tr2Result command_engine_snapshot(const CommandEngine *engine,
                                  CommandSnapshot *snapshot)
{
    CommandJournalEntry entry;
    Tr2Result result;

    if (!command_engine_is_initialized(engine) || snapshot == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(snapshot, 0, sizeof(*snapshot));
    snapshot->generation = engine->snapshot_generation;

    if (engine->has_active_transaction) {
        result = engine->journal->find(engine->journal->context,
                                       engine->active_transaction_id,
                                       &entry);
        if (result != TR2_OK) {
            return result;
        }
        if (!active_entry_matches(engine, &entry, engine->active_transaction_id) ||
            entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED) {
            return TR2_ERROR_INTERNAL;
        }

        snapshot->active_command_code = entry.request_identity.command_code;
        snapshot->active_transaction_id = entry.transaction_id;
        snapshot->status = (entry.lifecycle == COMMAND_LIFECYCLE_RESERVED)
                               ? COMMAND_STATUS_ACCEPTED
                               : COMMAND_STATUS_RUNNING;
    }

    result = engine->journal->latest_completed(engine->journal->context, &entry);
    if (result == TR2_OK) {
        if (!command_journal_entry_is_consistent(&entry) ||
            entry.lifecycle != COMMAND_LIFECYCLE_COMPLETED ||
            !entry.has_final_result) {
            return TR2_ERROR_INTERNAL;
        }

        snapshot->last.present = true;
        snapshot->last.command_code = entry.request_identity.command_code;
        snapshot->last.transaction_id = entry.transaction_id;
        snapshot->last.final_result = entry.final_result;
        snapshot->last.terminal_timestamp = entry.terminal_timestamp;
    } else if (result != TR2_ERROR_NOT_FOUND) {
        return result;
    }

    return TR2_OK;
}

Tr2Result command_engine_release_active(CommandEngine *engine,
                                        uint16_t transaction_id)
{
    if (!command_engine_is_initialized(engine) ||
        !command_transaction_id_is_valid(transaction_id)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!engine->has_active_transaction ||
        engine->active_transaction_id != transaction_id) {
        return TR2_ERROR_INVALID_STATE;
    }

    engine->has_active_transaction = false;
    engine->active_transaction_id = TR2_COMMAND_TRANSACTION_ID_INVALID;
    ++engine->snapshot_generation;
    return TR2_OK;
}

Tr2Result command_refresh_indicators_execute(
    CommandEngine *engine,
    DiagnosticService *diagnostic_service,
    SystemStateAggregator *aggregator,
    const SystemStateRefreshSource *refresh_source,
    uint16_t transaction_id,
    const CommandTerminalTimestamp *terminal_timestamp,
    DiagnosticSnapshot *diagnostic_snapshot,
    SystemStateSnapshot *system_snapshot,
    CommandJournalEntry *entry)
{
    CommandJournalEntry current;
    CommandFinalResult final_result;
    Tr2Result result;

    if (engine == NULL || diagnostic_service == NULL || aggregator == NULL ||
        refresh_source == NULL || terminal_timestamp == NULL ||
        diagnostic_snapshot == NULL || system_snapshot == NULL || entry == NULL ||
        !command_transaction_id_is_valid(transaction_id) ||
        !command_engine_has_active_transaction(engine) ||
        command_engine_active_transaction_id(engine) != transaction_id) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = engine->journal->find(engine->journal->context, transaction_id, &current);
    if (result != TR2_OK) {
        return result;
    }
    if (current.request_identity.command_code != COMMAND_CODE_REFRESH_INDICATORS ||
        current.lifecycle != COMMAND_LIFECYCLE_RESERVED ||
        current.has_recovery_context) {
        return TR2_ERROR_INVALID_STATE;
    }

    result = command_engine_mark_started(engine, transaction_id, entry);
    if (result != TR2_OK) {
        return result;
    }

    result = system_state_aggregator_refresh(aggregator,
                                             diagnostic_service,
                                             refresh_source,
                                             diagnostic_snapshot,
                                             system_snapshot);
    if (result != TR2_OK) {
        return result;
    }

    memset(&final_result, 0, sizeof(final_result));
    final_result.status = COMMAND_STATUS_SUCCESS;
    final_result.result_code = COMMAND_RESULT_SUCCESS;
    return command_engine_complete(engine,
                                   transaction_id,
                                   &final_result,
                                   terminal_timestamp,
                                   entry);
}

static bool acknowledge_fault_request_parameters_valid(const CommandRequestIdentity *identity)
{
    if (identity->param3 != 0u) {
        return false;
    }
    if (identity->param2 == 0u) {
        return identity->param1 != 0u;
    }
    if (identity->param2 == 1u) {
        return identity->param1 == 0u;
    }
    return false;
}

static Tr2Result complete_acknowledgement(
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

Tr2Result command_acknowledge_fault_execute(
    CommandEngine *engine,
    DiagnosticService *diagnostic_service,
    uint16_t transaction_id,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry)
{
    CommandJournalEntry current;
    DiagnosticFaultAcknowledgement fault_state;
    Tr2Result result;

    if (engine == NULL || diagnostic_service == NULL || terminal_timestamp == NULL ||
        entry == NULL || !command_transaction_id_is_valid(transaction_id) ||
        !command_engine_has_active_transaction(engine) ||
        command_engine_active_transaction_id(engine) != transaction_id) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = engine->journal->find(engine->journal->context, transaction_id, &current);
    if (result != TR2_OK) {
        return result;
    }
    if (current.request_identity.command_code != COMMAND_CODE_ACKNOWLEDGE_FAULT ||
        current.lifecycle != COMMAND_LIFECYCLE_RESERVED ||
        current.has_recovery_context) {
        return TR2_ERROR_INVALID_STATE;
    }

    if (!acknowledge_fault_request_parameters_valid(&current.request_identity)) {
        return complete_acknowledgement(engine,
                                        transaction_id,
                                        COMMAND_STATUS_REFUSED,
                                        COMMAND_RESULT_INVALID_PARAMETER,
                                        0u,
                                        terminal_timestamp,
                                        entry);
    }

    if (current.request_identity.param2 == 0u) {
        if (!diagnostic_service_fault_acknowledgement(diagnostic_service,
                                                      current.request_identity.param1,
                                                      &fault_state)) {
            return complete_acknowledgement(engine,
                                            transaction_id,
                                            COMMAND_STATUS_REFUSED,
                                            COMMAND_RESULT_INVALID_PARAMETER,
                                            current.request_identity.param1,
                                            terminal_timestamp,
                                            entry);
        }
        if (!fault_state.acknowledgeable) {
            return complete_acknowledgement(engine,
                                            transaction_id,
                                            COMMAND_STATUS_REFUSED,
                                            COMMAND_RESULT_FAULT_NOT_ACKNOWLEDGEABLE,
                                            current.request_identity.param1,
                                            terminal_timestamp,
                                            entry);
        }
    } else if (diagnostic_service_acknowledgeable_fault_count(diagnostic_service) == 0u) {
        return complete_acknowledgement(engine,
                                        transaction_id,
                                        COMMAND_STATUS_REFUSED,
                                        COMMAND_RESULT_FAULT_NOT_ACKNOWLEDGEABLE,
                                        0u,
                                        terminal_timestamp,
                                        entry);
    }

    result = command_engine_mark_started(engine, transaction_id, entry);
    if (result != TR2_OK) {
        return result;
    }

    if (current.request_identity.param2 == 0u) {
        result = diagnostic_service_acknowledge_fault(diagnostic_service,
                                                      current.request_identity.param1);
    } else {
        result = diagnostic_service_acknowledge_all(diagnostic_service);
    }
    if (result != TR2_OK) {
        return result;
    }

    return complete_acknowledgement(engine,
                                    transaction_id,
                                    COMMAND_STATUS_SUCCESS,
                                    COMMAND_RESULT_SUCCESS,
                                    current.request_identity.param2 == 0u
                                        ? current.request_identity.param1
                                        : 0u,
                                    terminal_timestamp,
                                    entry);
}

Tr2Result maintenance_service_init(MaintenanceService *service)
{
    if (service == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(service, 0, sizeof(*service));
    service->initialized = true;
    service->generation = 1u;
    service->mode = SYSTEM_MODE_NORMAL;
    return TR2_OK;
}

bool maintenance_service_is_initialized(const MaintenanceService *service)
{
    return service != NULL && service->initialized;
}

bool maintenance_service_active(const MaintenanceService *service)
{
    return maintenance_service_is_initialized(service) &&
           service->mode == SYSTEM_MODE_MAINTENANCE;
}

Tr2Result maintenance_service_enter(MaintenanceService *service)
{
    if (!maintenance_service_is_initialized(service)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (service->mode != SYSTEM_MODE_NORMAL) {
        return TR2_ERROR_INVALID_STATE;
    }

    service->mode = SYSTEM_MODE_MAINTENANCE;
    ++service->generation;
    return TR2_OK;
}

Tr2Result maintenance_service_exit(MaintenanceService *service)
{
    if (!maintenance_service_is_initialized(service)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (service->mode != SYSTEM_MODE_MAINTENANCE) {
        return TR2_ERROR_INVALID_STATE;
    }

    service->mode = SYSTEM_MODE_NORMAL;
    ++service->generation;
    return TR2_OK;
}

bool maintenance_service_snapshot(const MaintenanceService *service,
                                  SystemModeSnapshot *snapshot)
{
    if (!maintenance_service_is_initialized(service) || snapshot == NULL) {
        return false;
    }

    snapshot->generation = service->generation;
    snapshot->mode = service->mode;
    return true;
}

static Tr2Result complete_maintenance_command(
    CommandEngine *engine,
    uint16_t transaction_id,
    uint16_t status,
    uint16_t result_code,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry)
{
    CommandFinalResult final_result;

    memset(&final_result, 0, sizeof(final_result));
    final_result.status = status;
    final_result.result_code = result_code;
    return command_engine_complete(engine,
                                   transaction_id,
                                   &final_result,
                                   terminal_timestamp,
                                   entry);
}

Tr2Result command_enter_maintenance_execute(
    CommandEngine *engine,
    MaintenanceService *maintenance_service,
    bool acquisition_active,
    uint16_t transaction_id,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry)
{
    CommandJournalEntry current;
    Tr2Result result;

    if (engine == NULL || !maintenance_service_is_initialized(maintenance_service) ||
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
    if (current.request_identity.command_code != COMMAND_CODE_ENTER_MAINTENANCE ||
        current.lifecycle != COMMAND_LIFECYCLE_RESERVED ||
        current.has_recovery_context) {
        return TR2_ERROR_INVALID_STATE;
    }

    if (acquisition_active) {
        return complete_maintenance_command(engine,
                                            transaction_id,
                                            COMMAND_STATUS_REFUSED,
                                            COMMAND_RESULT_ACQUISITION_RUNNING,
                                            terminal_timestamp,
                                            entry);
    }
    if (maintenance_service_active(maintenance_service)) {
        return complete_maintenance_command(engine,
                                            transaction_id,
                                            COMMAND_STATUS_REFUSED,
                                            COMMAND_RESULT_MAINTENANCE_ACTIVE,
                                            terminal_timestamp,
                                            entry);
    }

    result = command_engine_mark_started(engine, transaction_id, entry);
    if (result != TR2_OK) {
        return result;
    }
    result = maintenance_service_enter(maintenance_service);
    if (result != TR2_OK) {
        return result;
    }

    return complete_maintenance_command(engine,
                                        transaction_id,
                                        COMMAND_STATUS_SUCCESS,
                                        COMMAND_RESULT_SUCCESS,
                                        terminal_timestamp,
                                        entry);
}

Tr2Result command_exit_maintenance_execute(
    CommandEngine *engine,
    MaintenanceService *maintenance_service,
    uint16_t transaction_id,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry)
{
    CommandJournalEntry current;
    Tr2Result result;

    if (engine == NULL || !maintenance_service_is_initialized(maintenance_service) ||
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
    if (current.request_identity.command_code != COMMAND_CODE_EXIT_MAINTENANCE ||
        current.lifecycle != COMMAND_LIFECYCLE_RESERVED ||
        current.has_recovery_context) {
        return TR2_ERROR_INVALID_STATE;
    }

    if (!maintenance_service_active(maintenance_service)) {
        return complete_maintenance_command(engine,
                                            transaction_id,
                                            COMMAND_STATUS_REFUSED,
                                            COMMAND_RESULT_INCOMPATIBLE_STATE,
                                            terminal_timestamp,
                                            entry);
    }

    result = command_engine_mark_started(engine, transaction_id, entry);
    if (result != TR2_OK) {
        return result;
    }
    result = maintenance_service_exit(maintenance_service);
    if (result != TR2_OK) {
        return result;
    }

    return complete_maintenance_command(engine,
                                        transaction_id,
                                        COMMAND_STATUS_SUCCESS,
                                        COMMAND_RESULT_SUCCESS,
                                        terminal_timestamp,
                                        entry);
}
