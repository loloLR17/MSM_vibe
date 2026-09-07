#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/command_engine.h"

#define FAKE_CAPACITY 4u

typedef struct {
    CommandJournalEntry entries[FAKE_CAPACITY];
    size_t count;
    uint32_t next_completion_order;
    Tr2Result mark_started_result;
    Tr2Result complete_result;
} FakeJournalContext;

static CommandJournalEntry *find_stored(FakeJournalContext *fake, uint16_t transaction_id)
{
    size_t index;
    for (index = 0u; index < fake->count; ++index) {
        if (fake->entries[index].transaction_id == transaction_id) {
            return &fake->entries[index];
        }
    }
    return NULL;
}

static Tr2Result fake_find(void *context, uint16_t transaction_id, CommandJournalEntry *entry)
{
    FakeJournalContext *fake = (FakeJournalContext *)context;
    CommandJournalEntry *stored;
    if (fake == NULL || entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    stored = find_stored(fake, transaction_id);
    if (stored == NULL) {
        return TR2_ERROR_NOT_FOUND;
    }
    *entry = *stored;
    return TR2_OK;
}

static Tr2Result fake_reserve(void *context, const CommandRequest *request, CommandJournalEntry *entry)
{
    FakeJournalContext *fake = (FakeJournalContext *)context;
    CommandJournalEntry *stored;
    if (fake == NULL || request == NULL || entry == NULL || fake->count >= FAKE_CAPACITY) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    stored = &fake->entries[fake->count++];
    memset(stored, 0, sizeof(*stored));
    stored->transaction_id = request->transaction_id;
    stored->request_identity = request->identity;
    stored->lifecycle = COMMAND_LIFECYCLE_RESERVED;
    *entry = *stored;
    return TR2_OK;
}

static Tr2Result fake_mark_started(void *context, uint16_t transaction_id, CommandJournalEntry *entry)
{
    FakeJournalContext *fake = (FakeJournalContext *)context;
    CommandJournalEntry *stored;
    if (fake == NULL || entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (fake->mark_started_result != TR2_OK) {
        return fake->mark_started_result;
    }
    stored = find_stored(fake, transaction_id);
    if (stored == NULL || stored->lifecycle != COMMAND_LIFECYCLE_RESERVED) {
        return TR2_ERROR_INVALID_STATE;
    }
    stored->lifecycle = COMMAND_LIFECYCLE_STARTED;
    *entry = *stored;
    return TR2_OK;
}

static Tr2Result fake_complete(void *context,
                               uint16_t transaction_id,
                               const CommandFinalResult *final_result,
                               const CommandTerminalTimestamp *terminal_timestamp,
                               CommandJournalEntry *entry)
{
    FakeJournalContext *fake = (FakeJournalContext *)context;
    CommandJournalEntry *stored;
    if (fake == NULL || final_result == NULL || terminal_timestamp == NULL || entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (fake->complete_result != TR2_OK) {
        return fake->complete_result;
    }
    stored = find_stored(fake, transaction_id);
    if (stored == NULL || stored->lifecycle == COMMAND_LIFECYCLE_COMPLETED) {
        return TR2_ERROR_INVALID_STATE;
    }
    stored->lifecycle = COMMAND_LIFECYCLE_COMPLETED;
    stored->has_final_result = true;
    stored->final_result = *final_result;
    stored->terminal_timestamp = *terminal_timestamp;
    stored->completion_order = fake->next_completion_order++;
    *entry = *stored;
    return TR2_OK;
}

static Tr2Result fake_latest_completed(void *context, CommandJournalEntry *entry)
{
    FakeJournalContext *fake = (FakeJournalContext *)context;
    size_t index;
    bool found = false;
    if (fake == NULL || entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    for (index = 0u; index < fake->count; ++index) {
        if (fake->entries[index].lifecycle == COMMAND_LIFECYCLE_COMPLETED &&
            (!found || fake->entries[index].completion_order > entry->completion_order)) {
            *entry = fake->entries[index];
            found = true;
        }
    }
    return found ? TR2_OK : TR2_ERROR_NOT_FOUND;
}

static CommandJournal make_journal(FakeJournalContext *fake)
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
    request.identity.param1 = 1u;
    return request;
}

static void init_fake(FakeJournalContext *fake)
{
    memset(fake, 0, sizeof(*fake));
    fake->next_completion_order = 1u;
    fake->mark_started_result = TR2_OK;
    fake->complete_result = TR2_OK;
}

static void test_reserved_started_completed_projection(void)
{
    FakeJournalContext fake;
    CommandJournal journal;
    CommandEngine engine;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandSnapshot snapshot;
    CommandRequest request;
    CommandFinalResult final_result;
    CommandTerminalTimestamp timestamp;

    init_fake(&fake);
    journal = make_journal(&fake);
    assert(command_engine_init(&engine, &journal) == TR2_OK);
    request = make_request(100u, COMMAND_CODE_APPLY_CONFIGURATION);

    assert(command_engine_admit(&engine, &request, &admission) == TR2_OK);
    assert(command_engine_snapshot(&engine, &snapshot) == TR2_OK);
    assert(snapshot.generation == 1u);
    assert(snapshot.active_command_code == COMMAND_CODE_APPLY_CONFIGURATION);
    assert(snapshot.active_transaction_id == 100u);
    assert(snapshot.status == COMMAND_STATUS_ACCEPTED);
    assert(!snapshot.last.present);

    assert(command_engine_mark_started(&engine, 100u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_STARTED);
    assert(command_engine_snapshot(&engine, &snapshot) == TR2_OK);
    assert(snapshot.generation == 2u);
    assert(snapshot.status == COMMAND_STATUS_RUNNING);

    memset(&final_result, 0, sizeof(final_result));
    final_result.status = COMMAND_STATUS_SUCCESS;
    final_result.result_code = COMMAND_RESULT_SUCCESS;
    timestamp.available = true;
    timestamp.value = UINT32_C(123456);

    assert(command_engine_complete(&engine, 100u, &final_result, &timestamp, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(!command_engine_has_active_transaction(&engine));
    assert(command_engine_snapshot(&engine, &snapshot) == TR2_OK);
    assert(snapshot.generation == 3u);
    assert(snapshot.active_transaction_id == TR2_COMMAND_TRANSACTION_ID_INVALID);
    assert(snapshot.status == COMMAND_STATUS_NONE);
    assert(snapshot.last.present);
    assert(snapshot.last.command_code == COMMAND_CODE_APPLY_CONFIGURATION);
    assert(snapshot.last.transaction_id == 100u);
    assert(snapshot.last.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(snapshot.last.final_result.result_code == COMMAND_RESULT_SUCCESS);
    assert(snapshot.last.terminal_timestamp.available);
    assert(snapshot.last.terminal_timestamp.value == UINT32_C(123456));
}

static void test_started_persistence_failure_keeps_reserved_active(void)
{
    FakeJournalContext fake;
    CommandJournal journal;
    CommandEngine engine;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandSnapshot snapshot;
    CommandRequest request;

    init_fake(&fake);
    fake.mark_started_result = TR2_ERROR_STORAGE;
    journal = make_journal(&fake);
    assert(command_engine_init(&engine, &journal) == TR2_OK);
    request = make_request(101u, COMMAND_CODE_SYNCHRONIZE_TIME);
    assert(command_engine_admit(&engine, &request, &admission) == TR2_OK);

    assert(command_engine_mark_started(&engine, 101u, &entry) == TR2_ERROR_STORAGE);
    assert(command_engine_has_active_transaction(&engine));
    assert(command_engine_snapshot(&engine, &snapshot) == TR2_OK);
    assert(snapshot.generation == 1u);
    assert(snapshot.status == COMMAND_STATUS_ACCEPTED);
}

static void test_completion_failure_never_publishes_false_terminal_result(void)
{
    FakeJournalContext fake;
    CommandJournal journal;
    CommandEngine engine;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandSnapshot snapshot;
    CommandRequest request;
    CommandFinalResult final_result;
    CommandTerminalTimestamp timestamp;

    init_fake(&fake);
    journal = make_journal(&fake);
    assert(command_engine_init(&engine, &journal) == TR2_OK);
    request = make_request(102u, COMMAND_CODE_START_ACQUISITION);
    assert(command_engine_admit(&engine, &request, &admission) == TR2_OK);
    assert(command_engine_mark_started(&engine, 102u, &entry) == TR2_OK);

    fake.complete_result = TR2_ERROR_STORAGE;
    memset(&final_result, 0, sizeof(final_result));
    final_result.status = COMMAND_STATUS_FAILED;
    final_result.result_code = COMMAND_RESULT_INTERNAL_TIMEOUT;
    memset(&timestamp, 0, sizeof(timestamp));

    assert(command_engine_complete(&engine, 102u, &final_result, &timestamp, &entry) == TR2_ERROR_STORAGE);
    assert(command_engine_has_active_transaction(&engine));
    assert(command_engine_snapshot(&engine, &snapshot) == TR2_OK);
    assert(snapshot.generation == 2u);
    assert(snapshot.status == COMMAND_STATUS_RUNNING);
    assert(!snapshot.last.present);
}

static void test_refusal_can_complete_directly_from_reserved(void)
{
    FakeJournalContext fake;
    CommandJournal journal;
    CommandEngine engine;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandSnapshot snapshot;
    CommandRequest request;
    CommandFinalResult final_result;
    CommandTerminalTimestamp timestamp;

    init_fake(&fake);
    journal = make_journal(&fake);
    assert(command_engine_init(&engine, &journal) == TR2_OK);
    request = make_request(103u, COMMAND_CODE_STOP_ACQUISITION);
    assert(command_engine_admit(&engine, &request, &admission) == TR2_OK);

    memset(&final_result, 0, sizeof(final_result));
    final_result.status = COMMAND_STATUS_REFUSED;
    final_result.result_code = COMMAND_RESULT_ACQUISITION_NOT_ACTIVE;
    memset(&timestamp, 0, sizeof(timestamp));

    assert(command_engine_complete(&engine, 103u, &final_result, &timestamp, &entry) == TR2_OK);
    assert(command_engine_snapshot(&engine, &snapshot) == TR2_OK);
    assert(snapshot.last.present);
    assert(snapshot.last.transaction_id == 103u);
    assert(snapshot.last.final_result.status == COMMAND_STATUS_REFUSED);
    assert(!snapshot.last.terminal_timestamp.available);
}

int main(void)
{
    test_reserved_started_completed_projection();
    test_started_persistence_failure_keeps_reserved_active();
    test_completion_failure_never_publishes_false_terminal_result();
    test_refusal_can_complete_directly_from_reserved();
    return 0;
}
