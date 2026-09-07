#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/command_engine.h"
#include "tr2/persistence/command_journal_store.h"

#define TEST_MAX_TRANSACTION_ID 4u
#define TEST_STORAGE_SIZE \
    (TEST_MAX_TRANSACTION_ID * TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS * \
     TR2_COMMAND_JOURNAL_RECORD_SIZE)

typedef struct {
    uint8_t durable[TEST_STORAGE_SIZE];
    uint8_t staged[TEST_STORAGE_SIZE];
    unsigned commit_calls;
    unsigned fail_commit_call;
} TestMediaContext;

typedef struct {
    PersistentMedia media;
    PersistentStorageCore core;
    CommandJournalStore store;
    CommandEngine engine;
} TestRuntime;

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestMediaContext *media = (TestMediaContext *)context;
    if ((size_t)offset + size > sizeof(media->staged)) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(buffer, &media->staged[offset], size);
    return TR2_OK;
}

static Tr2Result media_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    TestMediaContext *media = (TestMediaContext *)context;
    if ((size_t)offset + size > sizeof(media->staged)) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(&media->staged[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    TestMediaContext *media = (TestMediaContext *)context;
    ++media->commit_calls;
    if (media->fail_commit_call != 0u && media->commit_calls == media->fail_commit_call) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(media->durable, media->staged, sizeof(media->durable));
    return TR2_OK;
}

static void media_init(TestMediaContext *media)
{
    memset(media, 0, sizeof(*media));
    memset(media->durable, 0xFF, sizeof(media->durable));
    memcpy(media->staged, media->durable, sizeof(media->staged));
}

static void runtime_init(TestMediaContext *media, TestRuntime *runtime)
{
    CommandJournalRecoveryResult recovery;

    memset(runtime, 0, sizeof(*runtime));
    runtime->media.context = media;
    runtime->media.read = media_read;
    runtime->media.write = media_write;
    runtime->media.commit = media_commit;
    assert(persistent_storage_core_init(&runtime->core, &runtime->media) == TR2_OK);
    assert(command_journal_store_init(&runtime->store, &runtime->core,
                                      TEST_MAX_TRANSACTION_ID) == TR2_OK);
    assert(command_journal_store_recover(&runtime->store, &recovery) == TR2_OK);
    assert(recovery.status == COMMAND_JOURNAL_RECOVERY_EMPTY ||
           recovery.status == COMMAND_JOURNAL_RECOVERY_VALID);
    assert(command_engine_init(&runtime->engine,
                               command_journal_store_journal(&runtime->store)) == TR2_OK);
}

static void runtime_reboot(TestMediaContext *media, TestRuntime *runtime)
{
    memcpy(media->staged, media->durable, sizeof(media->staged));
    media->fail_commit_call = 0u;
    runtime_init(media, runtime);
}

static CommandRequest make_request(uint16_t transaction_id)
{
    CommandRequest request;
    memset(&request, 0, sizeof(request));
    request.transaction_id = transaction_id;
    request.identity.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    return request;
}

static CommandRecoveryContext make_context(void)
{
    CommandRecoveryContext context;
    memset(&context, 0, sizeof(context));
    context.kind = COMMAND_RECOVERY_CONTEXT_CONFIGURATION;
    context.value1 = 7u;
    context.value2 = 42u;
    context.value3 = UINT32_C(0x11223344);
    return context;
}

static CommandFinalResult success_result(void)
{
    CommandFinalResult result;
    memset(&result, 0, sizeof(result));
    result.status = COMMAND_STATUS_SUCCESS;
    result.result_code = COMMAND_RESULT_SUCCESS;
    return result;
}

static void assert_not_known(TestRuntime *runtime, uint16_t transaction_id)
{
    CommandJournal *journal = command_journal_store_journal(&runtime->store);
    CommandJournalEntry entry;
    assert(journal->find(journal->context, transaction_id, &entry) == TR2_ERROR_NOT_FOUND);
}

static CommandJournalEntry find_entry(TestRuntime *runtime, uint16_t transaction_id)
{
    CommandJournal *journal = command_journal_store_journal(&runtime->store);
    CommandJournalEntry entry;
    assert(journal->find(journal->context, transaction_id, &entry) == TR2_OK);
    return entry;
}

static void test_failed_reserved_commit_does_not_admit_transaction(void)
{
    TestMediaContext media;
    TestRuntime runtime;
    CommandRequest request = make_request(1u);
    CommandAdmissionResult admission;

    media_init(&media);
    runtime_init(&media, &runtime);
    media.fail_commit_call = media.commit_calls + 1u;

    assert(command_engine_admit(&runtime.engine, &request, &admission) == TR2_ERROR_STORAGE);
    assert(!command_engine_has_active_transaction(&runtime.engine));
    assert(command_journal_store_recovery_required(&runtime.store));

    runtime_reboot(&media, &runtime);
    assert_not_known(&runtime, 1u);
}

static void test_failed_context_commit_recovers_reserved_without_effect_permission(void)
{
    TestMediaContext media;
    TestRuntime runtime;
    CommandRequest request = make_request(2u);
    CommandAdmissionResult admission;
    CommandRecoveryContext context = make_context();
    CommandJournalEntry entry;
    CommandJournal *journal;

    media_init(&media);
    runtime_init(&media, &runtime);
    assert(command_engine_admit(&runtime.engine, &request, &admission) == TR2_OK);
    journal = command_journal_store_journal(&runtime.store);
    media.fail_commit_call = media.commit_calls + 1u;

    assert(journal->set_recovery_context(journal->context, 2u, &context, &entry) ==
           TR2_ERROR_STORAGE);
    assert(command_journal_store_recovery_required(&runtime.store));

    runtime_reboot(&media, &runtime);
    entry = find_entry(&runtime, 2u);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);
    assert(!entry.has_recovery_context);
}

static void test_failed_started_commit_recovers_reserved_context_and_blocks_effect(void)
{
    TestMediaContext media;
    TestRuntime runtime;
    CommandRequest request = make_request(3u);
    CommandAdmissionResult admission;
    CommandRecoveryContext context = make_context();
    CommandJournalEntry entry;
    CommandJournal *journal;

    media_init(&media);
    runtime_init(&media, &runtime);
    assert(command_engine_admit(&runtime.engine, &request, &admission) == TR2_OK);
    journal = command_journal_store_journal(&runtime.store);
    assert(journal->set_recovery_context(journal->context, 3u, &context, &entry) == TR2_OK);
    media.fail_commit_call = media.commit_calls + 1u;

    assert(command_engine_mark_started(&runtime.engine, 3u, &entry) == TR2_ERROR_STORAGE);
    assert(command_engine_has_active_transaction(&runtime.engine));

    runtime_reboot(&media, &runtime);
    entry = find_entry(&runtime, 3u);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);
    assert(entry.has_recovery_context);
    assert(entry.recovery_context.kind == COMMAND_RECOVERY_CONTEXT_CONFIGURATION);
}

static void test_failed_completed_commit_recovers_started_and_never_publishes_terminal(void)
{
    TestMediaContext media;
    TestRuntime runtime;
    CommandRequest request = make_request(4u);
    CommandAdmissionResult admission;
    CommandRecoveryContext context = make_context();
    CommandJournalEntry entry;
    CommandJournal *journal;
    CommandFinalResult result = success_result();
    CommandTerminalTimestamp timestamp;
    CommandSnapshot snapshot;

    media_init(&media);
    runtime_init(&media, &runtime);
    assert(command_engine_admit(&runtime.engine, &request, &admission) == TR2_OK);
    journal = command_journal_store_journal(&runtime.store);
    assert(journal->set_recovery_context(journal->context, 4u, &context, &entry) == TR2_OK);
    assert(command_engine_mark_started(&runtime.engine, 4u, &entry) == TR2_OK);

    timestamp.available = true;
    timestamp.value = UINT32_C(123456);
    media.fail_commit_call = media.commit_calls + 1u;
    assert(command_engine_complete(&runtime.engine, 4u, &result, &timestamp, &entry) ==
           TR2_ERROR_STORAGE);
    assert(command_engine_snapshot(&runtime.engine, &snapshot) == TR2_OK);
    assert(snapshot.status == COMMAND_STATUS_RUNNING);
    assert(!snapshot.last.present);

    runtime_reboot(&media, &runtime);
    entry = find_entry(&runtime, 4u);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_STARTED);
    assert(!entry.has_final_result);
}

int main(void)
{
    test_failed_reserved_commit_does_not_admit_transaction();
    test_failed_context_commit_recovers_reserved_without_effect_permission();
    test_failed_started_commit_recovers_reserved_context_and_blocks_effect();
    test_failed_completed_commit_recovers_started_and_never_publishes_terminal();
    return 0;
}
