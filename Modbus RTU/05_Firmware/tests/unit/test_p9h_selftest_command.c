#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/command_boot_recovery.h"
#include "tr2/application/command_selftest.h"

#define TEST_MEDIA_SIZE 128u
#define TEST_HISTORY_OFFSET UINT32_C(8)
#define TEST_JOURNAL_MAX_TRANSACTION_ID 4u
#define TEST_JOURNAL_STORAGE_SIZE \
    (TEST_JOURNAL_MAX_TRANSACTION_ID * TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS * \
     TR2_COMMAND_JOURNAL_RECORD_SIZE)

typedef struct {
    uint8_t durable[TEST_MEDIA_SIZE];
    uint8_t staged[TEST_MEDIA_SIZE];
    bool fail_commit;
} TestMedia;

typedef struct {
    CommandJournalEntry entry;
    bool present;
    uint32_t completion_order;
    unsigned int started_count;
} TestJournal;

typedef struct {
    unsigned int run_count;
    Tr2Result run_result;
    SelfTestExecutionResult execution;
} TestExecutor;

typedef struct {
    uint8_t durable[TEST_JOURNAL_STORAGE_SIZE];
    uint8_t staged[TEST_JOURNAL_STORAGE_SIZE];
} BootMedia;

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    if (media == NULL || buffer == NULL || (size_t)offset + size > TEST_MEDIA_SIZE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memcpy(buffer, &media->durable[offset], size);
    return TR2_OK;
}

static Tr2Result media_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    if (media == NULL || buffer == NULL || (size_t)offset + size > TEST_MEDIA_SIZE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memcpy(&media->staged[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    TestMedia *media = (TestMedia *)context;
    if (media == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (media->fail_commit) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(media->durable, media->staged, sizeof(media->durable));
    return TR2_OK;
}

static Tr2Result boot_media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    BootMedia *media = (BootMedia *)context;
    if (media == NULL || buffer == NULL ||
        (size_t)offset + size > TEST_JOURNAL_STORAGE_SIZE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memcpy(buffer, &media->durable[offset], size);
    return TR2_OK;
}

static Tr2Result boot_media_write(void *context,
                                  uint32_t offset,
                                  const void *buffer,
                                  size_t size)
{
    BootMedia *media = (BootMedia *)context;
    if (media == NULL || buffer == NULL ||
        (size_t)offset + size > TEST_JOURNAL_STORAGE_SIZE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memcpy(&media->staged[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result boot_media_commit(void *context)
{
    BootMedia *media = (BootMedia *)context;
    if (media == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memcpy(media->durable, media->staged, sizeof(media->durable));
    return TR2_OK;
}

static Tr2Result journal_find(void *context,
                              uint16_t transaction_id,
                              CommandJournalEntry *entry)
{
    TestJournal *journal = (TestJournal *)context;
    if (journal == NULL || entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!journal->present || journal->entry.transaction_id != transaction_id) {
        return TR2_ERROR_NOT_FOUND;
    }
    *entry = journal->entry;
    return TR2_OK;
}

static Tr2Result journal_reserve(void *context,
                                 const CommandRequest *request,
                                 CommandJournalEntry *entry)
{
    TestJournal *journal = (TestJournal *)context;
    if (journal == NULL || request == NULL || entry == NULL || journal->present) {
        return TR2_ERROR_INVALID_STATE;
    }
    memset(&journal->entry, 0, sizeof(journal->entry));
    journal->entry.transaction_id = request->transaction_id;
    journal->entry.request_identity = request->identity;
    journal->entry.lifecycle = COMMAND_LIFECYCLE_RESERVED;
    journal->present = true;
    *entry = journal->entry;
    return TR2_OK;
}

static Tr2Result journal_mark_started(void *context,
                                      uint16_t transaction_id,
                                      CommandJournalEntry *entry)
{
    TestJournal *journal = (TestJournal *)context;
    if (journal == NULL || entry == NULL || !journal->present ||
        journal->entry.transaction_id != transaction_id ||
        journal->entry.lifecycle != COMMAND_LIFECYCLE_RESERVED) {
        return TR2_ERROR_INVALID_STATE;
    }
    journal->entry.lifecycle = COMMAND_LIFECYCLE_STARTED;
    ++journal->started_count;
    *entry = journal->entry;
    return TR2_OK;
}

static Tr2Result journal_complete(void *context,
                                  uint16_t transaction_id,
                                  const CommandFinalResult *final_result,
                                  const CommandTerminalTimestamp *terminal_timestamp,
                                  CommandJournalEntry *entry)
{
    TestJournal *journal = (TestJournal *)context;
    if (journal == NULL || final_result == NULL || terminal_timestamp == NULL ||
        entry == NULL || !journal->present ||
        journal->entry.transaction_id != transaction_id ||
        journal->entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED) {
        return TR2_ERROR_INVALID_STATE;
    }
    journal->entry.lifecycle = COMMAND_LIFECYCLE_COMPLETED;
    journal->entry.has_final_result = true;
    journal->entry.final_result = *final_result;
    journal->entry.terminal_timestamp = *terminal_timestamp;
    journal->entry.completion_order = ++journal->completion_order;
    *entry = journal->entry;
    return TR2_OK;
}

static Tr2Result journal_latest_completed(void *context, CommandJournalEntry *entry)
{
    TestJournal *journal = (TestJournal *)context;
    if (journal == NULL || entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!journal->present || journal->entry.lifecycle != COMMAND_LIFECYCLE_COMPLETED) {
        return TR2_ERROR_NOT_FOUND;
    }
    *entry = journal->entry;
    return TR2_OK;
}

static Tr2Result run_standard(void *context, SelfTestExecutionResult *result)
{
    TestExecutor *executor = (TestExecutor *)context;
    if (executor == NULL || result == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    ++executor->run_count;
    if (executor->run_result != TR2_OK) {
        return executor->run_result;
    }
    *result = executor->execution;
    return TR2_OK;
}

static PersistentMedia make_media(TestMedia *media)
{
    PersistentMedia backend;
    backend.context = media;
    backend.read = media_read;
    backend.write = media_write;
    backend.commit = media_commit;
    return backend;
}

static void setup_services(TestMedia *media,
                           PersistentStorageCore *storage,
                           DiagnosticHistoryStore *history,
                           DiagnosticService *diagnostic,
                           SelfTestService *selftest)
{
    static PersistentMedia backend;

    memset(media, 0xFF, sizeof(*media));
    media->fail_commit = false;
    backend = make_media(media);
    assert(persistent_storage_core_init(storage, &backend) == TR2_OK);
    assert(diagnostic_history_store_init(history, storage, TEST_HISTORY_OFFSET) == TR2_OK);
    assert(diagnostic_service_init(diagnostic) == TR2_OK);
    assert(selftest_service_init(selftest, diagnostic, history) == TR2_OK);
}

static void setup_engine(TestJournal *state, CommandJournal *journal, CommandEngine *engine)
{
    memset(state, 0, sizeof(*state));
    memset(journal, 0, sizeof(*journal));
    journal->context = state;
    journal->find = journal_find;
    journal->reserve = journal_reserve;
    journal->mark_started = journal_mark_started;
    journal->complete = journal_complete;
    journal->latest_completed = journal_latest_completed;
    assert(command_engine_init(engine, journal) == TR2_OK);
}

static CommandRequest request(uint16_t transaction_id)
{
    CommandRequest value;
    memset(&value, 0, sizeof(value));
    value.transaction_id = transaction_id;
    value.identity.command_code = COMMAND_CODE_SELFTEST;
    return value;
}

static void test_success_and_failure_are_terminal_and_durable(void)
{
    TestMedia media;
    PersistentStorageCore storage;
    DiagnosticHistoryStore history;
    DiagnosticService diagnostic;
    SelfTestService selftest;
    TestJournal journal_state;
    CommandJournal journal;
    CommandEngine engine;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandTerminalTimestamp timestamp = {false, 0u};
    TestExecutor executor_state;
    SelfTestExecutor executor;
    DiagnosticSnapshot snapshot;
    CommandRequest command;

    setup_services(&media, &storage, &history, &diagnostic, &selftest);
    setup_engine(&journal_state, &journal, &engine);
    memset(&executor_state, 0, sizeof(executor_state));
    executor_state.run_result = TR2_OK;
    executor_state.execution.passed = true;
    executor.context = &executor_state;
    executor.run_standard = run_standard;

    command = request(601u);
    assert(command_engine_admit(&engine, &command, &admission) == TR2_OK);
    assert(command_selftest_execute(&engine, &selftest, &executor, 601u,
                                    &timestamp, &entry) == TR2_OK);
    assert(journal_state.started_count == 1u);
    assert(executor_state.run_count == 1u);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(entry.final_result.result_code == COMMAND_RESULT_SUCCESS);
    assert(diagnostic_service_snapshot(&diagnostic, &snapshot));
    assert(snapshot.facts.selftest.state == DIAGNOSTIC_SELFTEST_PASSED);

    setup_engine(&journal_state, &journal, &engine);
    executor_state.execution.passed = false;
    executor_state.execution.result_code = UINT16_C(77);
    executor_state.execution.detail = UINT16_C(9);
    command = request(602u);
    assert(command_engine_admit(&engine, &command, &admission) == TR2_OK);
    assert(command_selftest_execute(&engine, &selftest, &executor, 602u,
                                    &timestamp, &entry) == TR2_OK);
    assert(entry.final_result.status == COMMAND_STATUS_FAILED);
    assert(entry.final_result.result_code == COMMAND_RESULT_SELFTEST_FAILED);
    assert(entry.final_result.result_detail == UINT16_C(9));
    assert(diagnostic_service_snapshot(&diagnostic, &snapshot));
    assert(snapshot.facts.selftest.state == DIAGNOSTIC_SELFTEST_FAILED);
    assert(snapshot.facts.selftest.result_code == UINT16_C(77));
    assert(snapshot.facts.selftest.detail == UINT16_C(9));
}

static void test_invalid_extension_is_refused_before_started(void)
{
    TestMedia media;
    PersistentStorageCore storage;
    DiagnosticHistoryStore history;
    DiagnosticService diagnostic;
    SelfTestService selftest;
    TestJournal journal_state;
    CommandJournal journal;
    CommandEngine engine;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandTerminalTimestamp timestamp = {false, 0u};
    TestExecutor executor_state = {0};
    SelfTestExecutor executor = {&executor_state, run_standard};
    CommandRequest command = request(603u);

    setup_services(&media, &storage, &history, &diagnostic, &selftest);
    setup_engine(&journal_state, &journal, &engine);
    command.identity.param1 = 1u;
    assert(command_engine_admit(&engine, &command, &admission) == TR2_OK);
    assert(command_selftest_execute(&engine, &selftest, &executor, 603u,
                                    &timestamp, &entry) == TR2_OK);
    assert(journal_state.started_count == 0u);
    assert(executor_state.run_count == 0u);
    assert(entry.final_result.status == COMMAND_STATUS_REFUSED);
    assert(entry.final_result.result_code == COMMAND_RESULT_INVALID_PARAMETER);
}

static void test_persistence_failure_leaves_started_and_running(void)
{
    TestMedia media;
    PersistentStorageCore storage;
    DiagnosticHistoryStore history;
    DiagnosticService diagnostic;
    SelfTestService selftest;
    TestJournal journal_state;
    CommandJournal journal;
    CommandEngine engine;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandTerminalTimestamp timestamp = {false, 0u};
    TestExecutor executor_state = {0};
    SelfTestExecutor executor = {&executor_state, run_standard};
    CommandRequest command = request(604u);

    setup_services(&media, &storage, &history, &diagnostic, &selftest);
    setup_engine(&journal_state, &journal, &engine);
    executor_state.run_result = TR2_OK;
    executor_state.execution.passed = true;
    media.fail_commit = true;

    assert(command_engine_admit(&engine, &command, &admission) == TR2_OK);
    assert(command_selftest_execute(&engine, &selftest, &executor, 604u,
                                    &timestamp, &entry) == TR2_ERROR_STORAGE);
    assert(journal_state.entry.lifecycle == COMMAND_LIFECYCLE_STARTED);
    assert(selftest_service_running(&selftest));
    assert(command_selftest_reconcile(&journal_state.entry) ==
           COMMAND_RECONCILIATION_INDETERMINATE);
}

static void test_started_selftest_boot_scan_is_indeterminate(void)
{
    BootMedia media;
    PersistentMedia backend;
    PersistentStorageCore storage;
    CommandJournalStore store;
    CommandJournalRecoveryResult recovery;
    CommandJournal *journal;
    CommandJournalEntry entry;
    CommandRequest command;
    CommandBootRecoveryAuthorities authorities = {0};
    CommandBootRecoveryResult boot_result;

    memset(&media, 0xFF, sizeof(media));
    backend.context = &media;
    backend.read = boot_media_read;
    backend.write = boot_media_write;
    backend.commit = boot_media_commit;
    assert(persistent_storage_core_init(&storage, &backend) == TR2_OK);
    assert(command_journal_store_init(&store, &storage,
                                      TEST_JOURNAL_MAX_TRANSACTION_ID) == TR2_OK);
    assert(command_journal_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == COMMAND_JOURNAL_RECOVERY_EMPTY);

    command = request(4u);
    journal = command_journal_store_journal(&store);
    assert(journal != NULL);
    assert(journal->reserve(journal->context, &command, &entry) == TR2_OK);
    assert(journal->mark_started(journal->context, 4u, &entry) == TR2_OK);

    assert(command_boot_recovery_scan(&store, &authorities, &boot_result) == TR2_OK);
    assert(boot_result.has_incomplete_transaction);
    assert(boot_result.incomplete_transaction.transaction_id == 4u);
    assert(boot_result.status == COMMAND_BOOT_RECOVERY_STARTED_INDETERMINATE);
}

int main(void)
{
    test_success_and_failure_are_terminal_and_durable();
    test_invalid_extension_is_refused_before_started();
    test_persistence_failure_leaves_started_and_running();
    test_started_selftest_boot_scan_is_indeterminate();
    return 0;
}
