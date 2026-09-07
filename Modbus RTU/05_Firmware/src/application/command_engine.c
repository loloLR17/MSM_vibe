#include <stddef.h>
#include <string.h>

#include "tr2/application/command_engine.h"

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
