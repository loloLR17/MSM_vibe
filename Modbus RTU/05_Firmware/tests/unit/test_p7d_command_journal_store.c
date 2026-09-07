#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/command_journal_store.h"

#define TEST_MAX_TRANSACTION_ID 4u
#define TEST_STORAGE_SIZE \
    (TEST_MAX_TRANSACTION_ID * TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS * \
     TR2_COMMAND_JOURNAL_RECORD_SIZE)
#define NO_PARTIAL_FAILURE ((size_t)-1)

typedef struct {
    uint8_t durable[TEST_STORAGE_SIZE];
    uint8_t staged[TEST_STORAGE_SIZE];
    size_t partial_write_count;
    bool fail_commit;
} TestMediaContext;

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
    size_t copy_size = size;

    if ((size_t)offset + size > sizeof(media->staged)) {
        return TR2_ERROR_STORAGE;
    }
    if (media->partial_write_count != NO_PARTIAL_FAILURE &&
        media->partial_write_count < size) {
        copy_size = media->partial_write_count;
        memcpy(&media->staged[offset], buffer, copy_size);
        return TR2_ERROR_STORAGE;
    }
    memcpy(&media->staged[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    TestMediaContext *media = (TestMediaContext *)context;
    if (media->fail_commit) {
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
    media->partial_write_count = NO_PARTIAL_FAILURE;
}

static void simulate_reboot(TestMediaContext *media)
{
    memcpy(media->staged, media->durable, sizeof(media->staged));
    media->partial_write_count = NO_PARTIAL_FAILURE;
    media->fail_commit = false;
}

static void init_store(TestMediaContext *media,
                       PersistentMedia *persistent_media,
                       PersistentStorageCore *core,
                       CommandJournalStore *store)
{
    persistent_media->context = media;
    persistent_media->read = media_read;
    persistent_media->write = media_write;
    persistent_media->commit = media_commit;
    assert(persistent_storage_core_init(core, persistent_media) == TR2_OK);
    assert(command_journal_store_init(store, core, TEST_MAX_TRANSACTION_ID) == TR2_OK);
}

static void recover_store(CommandJournalStore *store, CommandJournalRecoveryStatus expected)
{
    CommandJournalRecoveryResult recovery;
    assert(command_journal_store_recover(store, &recovery) == TR2_OK);
    assert(recovery.status == expected);
    if (expected == COMMAND_JOURNAL_RECOVERY_EMPTY ||
        expected == COMMAND_JOURNAL_RECOVERY_VALID) {
        assert(!command_journal_store_recovery_required(store));
    }
}

static CommandRequest make_request(uint16_t transaction_id, uint16_t command_code)
{
    CommandRequest request;
    memset(&request, 0, sizeof(request));
    request.transaction_id = transaction_id;
    request.identity.command_code = command_code;
    request.identity.param1 = (uint16_t)(10u + transaction_id);
    request.identity.param3 = UINT32_C(0x12000000) + transaction_id;
    return request;
}

static CommandFinalResult success_result(void)
{
    CommandFinalResult result;
    memset(&result, 0, sizeof(result));
    result.status = COMMAND_STATUS_SUCCESS;
    result.result_code = COMMAND_RESULT_SUCCESS;
    return result;
}

static void test_lifecycle_survives_reboots(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalStore store;
    CommandJournal *journal;
    CommandJournalEntry entry;
    CommandRequest request = make_request(1u, COMMAND_CODE_APPLY_CONFIGURATION);
    CommandFinalResult result = success_result();
    CommandTerminalTimestamp timestamp;

    media_init(&media);
    init_store(&media, &persistent_media, &core, &store);
    recover_store(&store, COMMAND_JOURNAL_RECOVERY_EMPTY);
    journal = command_journal_store_journal(&store);

    assert(journal->reserve(journal->context, &request, &entry) == TR2_OK);
    simulate_reboot(&media);
    init_store(&media, &persistent_media, &core, &store);
    recover_store(&store, COMMAND_JOURNAL_RECOVERY_VALID);
    journal = command_journal_store_journal(&store);
    assert(journal->find(journal->context, 1u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);

    assert(journal->mark_started(journal->context, 1u, &entry) == TR2_OK);
    simulate_reboot(&media);
    init_store(&media, &persistent_media, &core, &store);
    recover_store(&store, COMMAND_JOURNAL_RECOVERY_VALID);
    journal = command_journal_store_journal(&store);
    assert(journal->find(journal->context, 1u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_STARTED);

    memset(&timestamp, 0, sizeof(timestamp));
    timestamp.available = true;
    timestamp.value = 12345u;
    assert(journal->complete(journal->context, 1u, &result, &timestamp, &entry) == TR2_OK);
    simulate_reboot(&media);
    init_store(&media, &persistent_media, &core, &store);
    recover_store(&store, COMMAND_JOURNAL_RECOVERY_VALID);
    journal = command_journal_store_journal(&store);
    assert(journal->find(journal->context, 1u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(entry.terminal_timestamp.available);
    assert(entry.terminal_timestamp.value == 12345u);
}

static void test_partial_started_write_recovers_reserved_barrier(void)
{
    size_t failed_bytes;

    for (failed_bytes = 0u; failed_bytes < TR2_COMMAND_JOURNAL_RECORD_SIZE; ++failed_bytes) {
        TestMediaContext media;
        PersistentMedia persistent_media;
        PersistentStorageCore core;
        CommandJournalStore store;
        CommandJournal *journal;
        CommandJournalEntry entry;
        CommandRequest request = make_request(2u, COMMAND_CODE_SYNCHRONIZE_TIME);

        media_init(&media);
        init_store(&media, &persistent_media, &core, &store);
        recover_store(&store, COMMAND_JOURNAL_RECOVERY_EMPTY);
        journal = command_journal_store_journal(&store);
        assert(journal->reserve(journal->context, &request, &entry) == TR2_OK);

        media.partial_write_count = failed_bytes;
        assert(journal->mark_started(journal->context, 2u, &entry) == TR2_ERROR_STORAGE);
        assert(command_journal_store_recovery_required(&store));

        simulate_reboot(&media);
        init_store(&media, &persistent_media, &core, &store);
        recover_store(&store, COMMAND_JOURNAL_RECOVERY_VALID);
        journal = command_journal_store_journal(&store);
        assert(journal->find(journal->context, 2u, &entry) == TR2_OK);
        assert(entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);
    }
}

static void test_failed_commit_recovers_previous_barrier(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalStore store;
    CommandJournal *journal;
    CommandJournalEntry entry;
    CommandRequest request = make_request(3u, COMMAND_CODE_START_ACQUISITION);

    media_init(&media);
    init_store(&media, &persistent_media, &core, &store);
    recover_store(&store, COMMAND_JOURNAL_RECOVERY_EMPTY);
    journal = command_journal_store_journal(&store);
    assert(journal->reserve(journal->context, &request, &entry) == TR2_OK);

    media.fail_commit = true;
    assert(journal->mark_started(journal->context, 3u, &entry) == TR2_ERROR_STORAGE);
    assert(command_journal_store_recovery_required(&store));

    simulate_reboot(&media);
    init_store(&media, &persistent_media, &core, &store);
    recover_store(&store, COMMAND_JOURNAL_RECOVERY_VALID);
    journal = command_journal_store_journal(&store);
    assert(journal->find(journal->context, 3u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);
}

static void test_lifetime_strict_prevents_transaction_id_reuse(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalStore store;
    CommandJournal *journal;
    CommandJournalEntry entry;
    CommandRequest original = make_request(1u, COMMAND_CODE_STOP_ACQUISITION);
    CommandRequest different = make_request(1u, COMMAND_CODE_APPLY_CONFIGURATION);
    CommandFinalResult result = success_result();
    CommandTerminalTimestamp timestamp;

    media_init(&media);
    init_store(&media, &persistent_media, &core, &store);
    recover_store(&store, COMMAND_JOURNAL_RECOVERY_EMPTY);
    journal = command_journal_store_journal(&store);
    assert(journal->reserve(journal->context, &original, &entry) == TR2_OK);
    memset(&timestamp, 0, sizeof(timestamp));
    assert(journal->complete(journal->context, 1u, &result, &timestamp, &entry) == TR2_OK);

    simulate_reboot(&media);
    init_store(&media, &persistent_media, &core, &store);
    recover_store(&store, COMMAND_JOURNAL_RECOVERY_VALID);
    journal = command_journal_store_journal(&store);
    assert(journal->reserve(journal->context, &different, &entry) == TR2_ERROR_INVALID_STATE);
    assert(journal->find(journal->context, 1u, &entry) == TR2_OK);
    assert(command_request_identity_equal(&entry.request_identity, &original.identity));
}

static void test_completion_order_recovered_independently_of_transaction_id(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalStore store;
    CommandJournal *journal;
    CommandJournalEntry entry;
    CommandRequest first = make_request(4u, COMMAND_CODE_SYNCHRONIZE_TIME);
    CommandRequest second = make_request(1u, COMMAND_CODE_APPLY_CONFIGURATION);
    CommandFinalResult result = success_result();
    CommandTerminalTimestamp timestamp;

    media_init(&media);
    init_store(&media, &persistent_media, &core, &store);
    recover_store(&store, COMMAND_JOURNAL_RECOVERY_EMPTY);
    journal = command_journal_store_journal(&store);
    memset(&timestamp, 0, sizeof(timestamp));

    assert(journal->reserve(journal->context, &first, &entry) == TR2_OK);
    assert(journal->complete(journal->context, 4u, &result, &timestamp, &entry) == TR2_OK);
    assert(entry.completion_order == 1u);
    assert(journal->reserve(journal->context, &second, &entry) == TR2_OK);
    assert(journal->complete(journal->context, 1u, &result, &timestamp, &entry) == TR2_OK);
    assert(entry.completion_order == 2u);

    simulate_reboot(&media);
    init_store(&media, &persistent_media, &core, &store);
    recover_store(&store, COMMAND_JOURNAL_RECOVERY_VALID);
    journal = command_journal_store_journal(&store);
    assert(journal->latest_completed(journal->context, &entry) == TR2_OK);
    assert(entry.transaction_id == 1u);
    assert(entry.completion_order == 2u);
}

int main(void)
{
    test_lifecycle_survives_reboots();
    test_partial_started_write_recovers_reserved_barrier();
    test_failed_commit_recovers_previous_barrier();
    test_lifetime_strict_prevents_transaction_id_reuse();
    test_completion_order_recovered_independently_of_transaction_id();
    return 0;
}
