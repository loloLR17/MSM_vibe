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
    result->kind = COMMAND_ADMISSION_NEW;
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
    return TR2_OK;
}
