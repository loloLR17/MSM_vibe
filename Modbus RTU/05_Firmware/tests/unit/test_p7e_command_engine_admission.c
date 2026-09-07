#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/command_engine.h"

#define FAKE_CAPACITY 4u

typedef struct {
    CommandJournalEntry entries[FAKE_CAPACITY];
    size_t count;
    unsigned reserve_calls;
    Tr2Result reserve_result;
} FakeJournalContext;

static Tr2Result fake_find(void *context,
                           uint16_t transaction_id,
                           CommandJournalEntry *entry)
{
    FakeJournalContext *fake = (FakeJournalContext *)context;
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
    FakeJournalContext *fake = (FakeJournalContext *)context;
    CommandJournalEntry *stored;

    if (fake == NULL || request == NULL || entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    ++fake->reserve_calls;
    if (fake->reserve_result != TR2_OK) {
        return fake->reserve_result;
    }
    if (fake->count >= FAKE_CAPACITY) {
        return TR2_ERROR_STORAGE;
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
    (void)context;
    (void)transaction_id;
    (void)entry;
    return TR2_ERROR_UNSUPPORTED;
}

static Tr2Result fake_complete(void *context,
                               uint16_t transaction_id,
                               const CommandFinalResult *final_result,
                               const CommandTerminalTimestamp *terminal_timestamp,
                               CommandJournalEntry *entry)
{
    (void)context;
    (void)transaction_id;
    (void)final_result;
    (void)terminal_timestamp;
    (void)entry;
    return TR2_ERROR_UNSUPPORTED;
}

static Tr2Result fake_latest_completed(void *context, CommandJournalEntry *entry)
{
    (void)context;
    (void)entry;
    return TR2_ERROR_NOT_FOUND;
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

static CommandRequest make_request(uint16_t transaction_id,
                                   uint16_t command_code,
                                   uint16_t param1)
{
    CommandRequest request;

    memset(&request, 0, sizeof(request));
    request.transaction_id = transaction_id;
    request.identity.command_code = command_code;
    request.identity.param1 = param1;
    request.identity.param2 = 22u;
    request.identity.param3 = UINT32_C(0x11223344);
    request.identity.confirm_key = TR2_COMMAND_CONFIRM_KEY_NONE;
    return request;
}

static void test_new_transaction_becomes_active_only_after_reserve(void)
{
    FakeJournalContext fake;
    CommandJournal journal;
    CommandEngine engine;
    CommandAdmissionResult result;
    CommandRequest request;

    memset(&fake, 0, sizeof(fake));
    fake.reserve_result = TR2_OK;
    journal = make_journal(&fake);
    assert(command_engine_init(&engine, &journal) == TR2_OK);

    request = make_request(10u, COMMAND_CODE_APPLY_CONFIGURATION, 1u);
    assert(command_engine_admit(&engine, &request, &result) == TR2_OK);
    assert(result.kind == COMMAND_ADMISSION_NEW);
    assert(result.entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);
    assert(fake.reserve_calls == 1u);
    assert(command_engine_has_active_transaction(&engine));
    assert(command_engine_active_transaction_id(&engine) == 10u);
}

static void test_reserve_failure_does_not_create_active_transaction(void)
{
    FakeJournalContext fake;
    CommandJournal journal;
    CommandEngine engine;
    CommandAdmissionResult result;
    CommandRequest request;

    memset(&fake, 0, sizeof(fake));
    fake.reserve_result = TR2_ERROR_STORAGE;
    journal = make_journal(&fake);
    assert(command_engine_init(&engine, &journal) == TR2_OK);

    request = make_request(11u, COMMAND_CODE_SYNCHRONIZE_TIME, 0u);
    assert(command_engine_admit(&engine, &request, &result) == TR2_ERROR_STORAGE);
    assert(fake.reserve_calls == 1u);
    assert(!command_engine_has_active_transaction(&engine));
}

static void test_exact_retry_is_never_reserved_again(void)
{
    FakeJournalContext fake;
    CommandJournal journal;
    CommandEngine engine;
    CommandAdmissionResult first;
    CommandAdmissionResult retry_result;
    CommandRequest request;

    memset(&fake, 0, sizeof(fake));
    fake.reserve_result = TR2_OK;
    journal = make_journal(&fake);
    assert(command_engine_init(&engine, &journal) == TR2_OK);

    request = make_request(12u, COMMAND_CODE_START_ACQUISITION, 4u);
    assert(command_engine_admit(&engine, &request, &first) == TR2_OK);
    assert(command_engine_admit(&engine, &request, &retry_result) == TR2_OK);
    assert(retry_result.kind == COMMAND_ADMISSION_RETRY);
    assert(retry_result.entry.transaction_id == 12u);
    assert(fake.reserve_calls == 1u);
    assert(command_engine_active_transaction_id(&engine) == 12u);
}

static void test_same_transaction_different_identity_is_collision(void)
{
    FakeJournalContext fake;
    CommandJournal journal;
    CommandEngine engine;
    CommandAdmissionResult result;
    CommandRequest original;
    CommandRequest collision;

    memset(&fake, 0, sizeof(fake));
    fake.reserve_result = TR2_OK;
    journal = make_journal(&fake);
    assert(command_engine_init(&engine, &journal) == TR2_OK);

    original = make_request(13u, COMMAND_CODE_STOP_ACQUISITION, 1u);
    collision = original;
    collision.identity.param1 = 2u;

    assert(command_engine_admit(&engine, &original, &result) == TR2_OK);
    assert(command_engine_admit(&engine, &collision, &result) == TR2_OK);
    assert(result.kind == COMMAND_ADMISSION_COLLISION);
    assert(fake.reserve_calls == 1u);
    assert(command_engine_active_transaction_id(&engine) == 13u);
}

static void test_new_transaction_is_busy_while_another_is_active(void)
{
    FakeJournalContext fake;
    CommandJournal journal;
    CommandEngine engine;
    CommandAdmissionResult result;
    CommandRequest first;
    CommandRequest second;

    memset(&fake, 0, sizeof(fake));
    fake.reserve_result = TR2_OK;
    journal = make_journal(&fake);
    assert(command_engine_init(&engine, &journal) == TR2_OK);

    first = make_request(20u, COMMAND_CODE_APPLY_CONFIGURATION, 1u);
    second = make_request(21u, COMMAND_CODE_SYNCHRONIZE_TIME, 2u);

    assert(command_engine_admit(&engine, &first, &result) == TR2_OK);
    assert(command_engine_admit(&engine, &second, &result) == TR2_OK);
    assert(result.kind == COMMAND_ADMISSION_BUSY);
    assert(fake.reserve_calls == 1u);
    assert(fake.count == 1u);
    assert(fake_find(&fake, 21u, &result.entry) == TR2_ERROR_NOT_FOUND);
}

static void test_release_allows_next_new_transaction(void)
{
    FakeJournalContext fake;
    CommandJournal journal;
    CommandEngine engine;
    CommandAdmissionResult result;
    CommandRequest first;
    CommandRequest second;

    memset(&fake, 0, sizeof(fake));
    fake.reserve_result = TR2_OK;
    journal = make_journal(&fake);
    assert(command_engine_init(&engine, &journal) == TR2_OK);

    first = make_request(30u, COMMAND_CODE_APPLY_CONFIGURATION, 0u);
    second = make_request(31u, COMMAND_CODE_SYNCHRONIZE_TIME, 0u);

    assert(command_engine_admit(&engine, &first, &result) == TR2_OK);
    assert(command_engine_release_active(&engine, 31u) == TR2_ERROR_INVALID_STATE);
    assert(command_engine_release_active(&engine, 30u) == TR2_OK);
    assert(!command_engine_has_active_transaction(&engine));

    assert(command_engine_admit(&engine, &second, &result) == TR2_OK);
    assert(result.kind == COMMAND_ADMISSION_NEW);
    assert(command_engine_active_transaction_id(&engine) == 31u);
    assert(fake.reserve_calls == 2u);
}

static void test_invalid_request_never_touches_journal(void)
{
    FakeJournalContext fake;
    CommandJournal journal;
    CommandEngine engine;
    CommandAdmissionResult result;
    CommandRequest request;

    memset(&fake, 0, sizeof(fake));
    fake.reserve_result = TR2_OK;
    journal = make_journal(&fake);
    assert(command_engine_init(&engine, &journal) == TR2_OK);

    request = make_request(0u, COMMAND_CODE_APPLY_CONFIGURATION, 0u);
    assert(command_engine_admit(&engine, &request, &result) == TR2_ERROR_INVALID_ARGUMENT);
    request.transaction_id = 40u;
    request.identity.command_code = COMMAND_CODE_NONE;
    assert(command_engine_admit(&engine, &request, &result) == TR2_ERROR_INVALID_ARGUMENT);
    assert(fake.reserve_calls == 0u);
    assert(fake.count == 0u);
}

int main(void)
{
    test_new_transaction_becomes_active_only_after_reserve();
    test_reserve_failure_does_not_create_active_transaction();
    test_exact_retry_is_never_reserved_again();
    test_same_transaction_different_identity_is_collision();
    test_new_transaction_is_busy_while_another_is_active();
    test_release_allows_next_new_transaction();
    test_invalid_request_never_touches_journal();
    return 0;
}
