#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/command_journal.h"

#define FAKE_JOURNAL_CAPACITY 4u

typedef struct {
    CommandJournalEntry entries[FAKE_JOURNAL_CAPACITY];
    size_t count;
    uint32_t next_completion_order;
} FakeCommandJournal;

static Tr2Result fake_find(void *context,
                           uint16_t transaction_id,
                           CommandJournalEntry *entry)
{
    FakeCommandJournal *fake = (FakeCommandJournal *)context;
    size_t index;

    if (fake == NULL || entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    for (index = 0u; index < fake->count; ++index) {
        if (fake->entries[index].transaction_id == transaction_id) {
            *entry = fake->entries[index];
            return TR2_OK;
        }
    }

    return TR2_ERROR_NOT_FOUND;
}

static Tr2Result fake_reserve(void *context,
                              const CommandRequest *request,
                              CommandJournalEntry *entry)
{
    FakeCommandJournal *fake = (FakeCommandJournal *)context;
    CommandJournalEntry *stored;

    if (fake == NULL || request == NULL || entry == NULL ||
        !command_transaction_id_is_valid(request->transaction_id)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (fake->count >= FAKE_JOURNAL_CAPACITY) {
        return TR2_ERROR_STORAGE;
    }
    if (fake_find(context, request->transaction_id, entry) == TR2_OK) {
        return TR2_ERROR_INVALID_STATE;
    }

    stored = &fake->entries[fake->count++];
    memset(stored, 0, sizeof(*stored));
    stored->transaction_id = request->transaction_id;
    stored->request_identity = request->identity;
    stored->lifecycle = COMMAND_LIFECYCLE_RESERVED;
    *entry = *stored;
    return TR2_OK;
}

static Tr2Result fake_mark_started(void *context,
                                   uint16_t transaction_id,
                                   CommandJournalEntry *entry)
{
    FakeCommandJournal *fake = (FakeCommandJournal *)context;
    size_t index;

    if (fake == NULL || entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    for (index = 0u; index < fake->count; ++index) {
        if (fake->entries[index].transaction_id == transaction_id) {
            if (fake->entries[index].lifecycle != COMMAND_LIFECYCLE_RESERVED) {
                return TR2_ERROR_INVALID_STATE;
            }
            fake->entries[index].lifecycle = COMMAND_LIFECYCLE_STARTED;
            *entry = fake->entries[index];
            return TR2_OK;
        }
    }

    return TR2_ERROR_NOT_FOUND;
}

static Tr2Result fake_complete(void *context,
                               uint16_t transaction_id,
                               const CommandFinalResult *final_result,
                               const CommandTerminalTimestamp *terminal_timestamp,
                               CommandJournalEntry *entry)
{
    FakeCommandJournal *fake = (FakeCommandJournal *)context;
    size_t index;

    if (fake == NULL || final_result == NULL || terminal_timestamp == NULL || entry == NULL ||
        !command_status_is_final(final_result->status)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    for (index = 0u; index < fake->count; ++index) {
        if (fake->entries[index].transaction_id == transaction_id) {
            if (fake->entries[index].lifecycle == COMMAND_LIFECYCLE_COMPLETED) {
                return TR2_ERROR_INVALID_STATE;
            }
            fake->entries[index].lifecycle = COMMAND_LIFECYCLE_COMPLETED;
            fake->entries[index].has_final_result = true;
            fake->entries[index].final_result = *final_result;
            fake->entries[index].terminal_timestamp = *terminal_timestamp;
            fake->entries[index].completion_order = fake->next_completion_order++;
            *entry = fake->entries[index];
            return TR2_OK;
        }
    }

    return TR2_ERROR_NOT_FOUND;
}

static Tr2Result fake_latest_completed(void *context, CommandJournalEntry *entry)
{
    FakeCommandJournal *fake = (FakeCommandJournal *)context;
    size_t index;
    const CommandJournalEntry *latest = NULL;

    if (fake == NULL || entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    for (index = 0u; index < fake->count; ++index) {
        if (fake->entries[index].lifecycle == COMMAND_LIFECYCLE_COMPLETED &&
            (latest == NULL ||
             fake->entries[index].completion_order > latest->completion_order)) {
            latest = &fake->entries[index];
        }
    }

    if (latest == NULL) {
        return TR2_ERROR_NOT_FOUND;
    }

    *entry = *latest;
    return TR2_OK;
}

static CommandJournal make_journal(FakeCommandJournal *fake)
{
    CommandJournal journal;

    memset(&journal, 0, sizeof(journal));
    journal.context = fake;
    journal.find = fake_find;
    journal.reserve = fake_reserve;
    journal.mark_started = fake_mark_started;
    journal.complete = fake_complete;
    journal.latest_completed = fake_latest_completed;
    return journal;
}

static CommandRequest make_request(uint16_t transaction_id, uint16_t command_code)
{
    CommandRequest request;

    memset(&request, 0, sizeof(request));
    request.transaction_id = transaction_id;
    request.identity.command_code = command_code;
    request.identity.param1 = 11u;
    request.identity.param2 = 22u;
    request.identity.param3 = 0x12345678u;
    return request;
}

static void test_reserved_started_completed_contract(void)
{
    FakeCommandJournal fake;
    CommandJournal journal;
    CommandRequest request;
    CommandJournalEntry entry;
    CommandFinalResult result;
    CommandTerminalTimestamp timestamp;

    memset(&fake, 0, sizeof(fake));
    fake.next_completion_order = 1u;
    journal = make_journal(&fake);
    request = make_request(41u, COMMAND_CODE_APPLY_CONFIGURATION);

    assert(journal.reserve(journal.context, &request, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);
    assert(command_journal_entry_is_consistent(&entry));
    assert(command_request_identity_equal(&entry.request_identity, &request.identity));

    assert(journal.mark_started(journal.context, 41u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_STARTED);
    assert(command_journal_entry_is_consistent(&entry));

    memset(&result, 0, sizeof(result));
    result.status = COMMAND_STATUS_SUCCESS;
    result.result_code = COMMAND_RESULT_SUCCESS;
    memset(&timestamp, 0, sizeof(timestamp));
    timestamp.available = true;
    timestamp.value = 123456u;

    assert(journal.complete(journal.context, 41u, &result, &timestamp, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(entry.has_final_result);
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(entry.terminal_timestamp.available);
    assert(entry.completion_order == 1u);
    assert(command_journal_entry_is_consistent(&entry));
}

static void test_refusal_can_complete_directly_from_reserved(void)
{
    FakeCommandJournal fake;
    CommandJournal journal;
    CommandRequest request;
    CommandJournalEntry entry;
    CommandFinalResult result;
    CommandTerminalTimestamp timestamp;

    memset(&fake, 0, sizeof(fake));
    fake.next_completion_order = 1u;
    journal = make_journal(&fake);
    request = make_request(42u, COMMAND_CODE_START_ACQUISITION);

    assert(journal.reserve(journal.context, &request, &entry) == TR2_OK);

    memset(&result, 0, sizeof(result));
    result.status = COMMAND_STATUS_REFUSED;
    result.result_code = COMMAND_RESULT_ACTIVE_CONFIGURATION_INVALID;
    memset(&timestamp, 0, sizeof(timestamp));

    assert(journal.complete(journal.context, 42u, &result, &timestamp, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(!entry.terminal_timestamp.available);
    assert(command_journal_entry_is_consistent(&entry));
}

static void test_latest_completed_uses_completion_order_not_transaction_id(void)
{
    FakeCommandJournal fake;
    CommandJournal journal;
    CommandRequest high_id;
    CommandRequest low_id;
    CommandJournalEntry entry;
    CommandFinalResult result;
    CommandTerminalTimestamp timestamp;

    memset(&fake, 0, sizeof(fake));
    fake.next_completion_order = 1u;
    journal = make_journal(&fake);
    high_id = make_request(60000u, COMMAND_CODE_SYNCHRONIZE_TIME);
    low_id = make_request(2u, COMMAND_CODE_STOP_ACQUISITION);
    memset(&result, 0, sizeof(result));
    result.status = COMMAND_STATUS_SUCCESS;
    result.result_code = COMMAND_RESULT_SUCCESS;
    memset(&timestamp, 0, sizeof(timestamp));

    assert(journal.reserve(journal.context, &high_id, &entry) == TR2_OK);
    assert(journal.complete(journal.context, high_id.transaction_id, &result, &timestamp, &entry) == TR2_OK);
    assert(journal.reserve(journal.context, &low_id, &entry) == TR2_OK);
    assert(journal.complete(journal.context, low_id.transaction_id, &result, &timestamp, &entry) == TR2_OK);

    assert(journal.latest_completed(journal.context, &entry) == TR2_OK);
    assert(entry.transaction_id == 2u);
    assert(entry.completion_order == 2u);
}

static void test_journal_preserves_identity_for_retry_comparison(void)
{
    FakeCommandJournal fake;
    CommandJournal journal;
    CommandRequest request;
    CommandRequest retry;
    CommandJournalEntry entry;

    memset(&fake, 0, sizeof(fake));
    fake.next_completion_order = 1u;
    journal = make_journal(&fake);
    request = make_request(77u, COMMAND_CODE_APPLY_CONFIGURATION);
    request.identity.confirm_key = TR2_COMMAND_CONFIRM_KEY_VALID;

    assert(journal.reserve(journal.context, &request, &entry) == TR2_OK);
    assert(journal.find(journal.context, 77u, &entry) == TR2_OK);

    retry = request;
    assert(command_request_identity_equal(&entry.request_identity, &retry.identity));
    retry.identity.param1++;
    assert(!command_request_identity_equal(&entry.request_identity, &retry.identity));
}

static void test_entry_consistency_rejects_false_terminal_state(void)
{
    CommandJournalEntry entry;

    memset(&entry, 0, sizeof(entry));
    entry.transaction_id = 5u;
    entry.lifecycle = COMMAND_LIFECYCLE_COMPLETED;
    entry.has_final_result = true;
    entry.final_result.status = COMMAND_STATUS_RUNNING;
    entry.completion_order = 1u;

    assert(!command_journal_entry_is_consistent(&entry));

    entry.final_result.status = COMMAND_STATUS_FAILED;
    assert(command_journal_entry_is_consistent(&entry));
}

int main(void)
{
    test_reserved_started_completed_contract();
    test_refusal_can_complete_directly_from_reserved();
    test_latest_completed_uses_completion_order_not_transaction_id();
    test_journal_preserves_identity_for_retry_comparison();
    test_entry_consistency_rejects_false_terminal_state();
    return 0;
}
