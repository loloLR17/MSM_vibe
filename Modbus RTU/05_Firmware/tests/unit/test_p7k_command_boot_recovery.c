#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/command_boot_recovery.h"

#define TEST_MAX_TRANSACTION_ID 4u
#define TEST_STORAGE_SIZE \
    (TEST_MAX_TRANSACTION_ID * TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS * \
     TR2_COMMAND_JOURNAL_RECORD_SIZE)

typedef struct {
    uint8_t durable[TEST_STORAGE_SIZE];
    uint8_t staged[TEST_STORAGE_SIZE];
} TestMedia;

typedef struct {
    bool present;
    CampaignMetadata metadata;
} TestRepositoryContext;

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    if ((size_t)offset + size > sizeof(media->durable)) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(buffer, &media->durable[offset], size);
    return TR2_OK;
}

static Tr2Result media_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    if ((size_t)offset + size > sizeof(media->staged)) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(&media->staged[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    TestMedia *media = (TestMedia *)context;
    memcpy(media->durable, media->staged, sizeof(media->durable));
    return TR2_OK;
}

static void init_store(TestMedia *media,
                       PersistentMedia *persistent_media,
                       PersistentStorageCore *storage,
                       CommandJournalStore *store)
{
    CommandJournalRecoveryResult recovery;

    memset(media, UINT8_C(0xFF), sizeof(*media));
    persistent_media->context = media;
    persistent_media->read = media_read;
    persistent_media->write = media_write;
    persistent_media->commit = media_commit;
    assert(persistent_storage_core_init(storage, persistent_media) == TR2_OK);
    assert(command_journal_store_init(store, storage, TEST_MAX_TRANSACTION_ID) == TR2_OK);
    assert(command_journal_store_recover(store, &recovery) == TR2_OK);
    assert(recovery.status == COMMAND_JOURNAL_RECOVERY_EMPTY);
}

static CommandRequest make_request(uint16_t transaction_id, uint16_t command_code)
{
    CommandRequest request;
    memset(&request, 0, sizeof(request));
    request.transaction_id = transaction_id;
    request.identity.command_code = command_code;
    return request;
}

static void reserve_entry(CommandJournalStore *store,
                          uint16_t transaction_id,
                          uint16_t command_code,
                          CommandJournalEntry *entry)
{
    CommandRequest request = make_request(transaction_id, command_code);
    CommandJournal *journal = command_journal_store_journal(store);
    assert(journal != NULL);
    assert(journal->reserve(journal->context, &request, entry) == TR2_OK);
}

static void start_with_context(CommandJournalStore *store,
                               uint16_t transaction_id,
                               uint16_t command_code,
                               CommandRecoveryContextKind context_kind,
                               uint32_t context_value,
                               CommandJournalEntry *entry)
{
    CommandRecoveryContext context;
    CommandJournal *journal = command_journal_store_journal(store);

    reserve_entry(store, transaction_id, command_code, entry);
    memset(&context, 0, sizeof(context));
    context.kind = context_kind;
    context.value1 = context_value;
    assert(journal->set_recovery_context != NULL);
    assert(journal->set_recovery_context(journal->context,
                                         transaction_id,
                                         &context,
                                         entry) == TR2_OK);
    assert(journal->mark_started(journal->context, transaction_id, entry) == TR2_OK);
}

static Tr2Result repository_get_by_id(void *context,
                                      CampaignId campaign_id,
                                      CampaignMetadata *metadata)
{
    TestRepositoryContext *repository = (TestRepositoryContext *)context;
    if (!repository->present || repository->metadata.campaign_id != campaign_id) {
        return TR2_ERROR_NOT_FOUND;
    }
    *metadata = repository->metadata;
    return TR2_OK;
}

static void test_completed_history_is_recovered_without_active_transaction(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore storage;
    CommandJournalStore store;
    CommandJournalEntry entry;
    CommandJournal *journal;
    CommandFinalResult final_result;
    CommandTerminalTimestamp timestamp;
    CommandBootRecoveryAuthorities authorities = {0};
    CommandBootRecoveryResult result;

    init_store(&media, &persistent_media, &storage, &store);
    reserve_entry(&store, 1u, COMMAND_CODE_APPLY_CONFIGURATION, &entry);
    journal = command_journal_store_journal(&store);
    memset(&final_result, 0, sizeof(final_result));
    final_result.status = COMMAND_STATUS_SUCCESS;
    final_result.result_code = COMMAND_RESULT_SUCCESS;
    memset(&timestamp, 0, sizeof(timestamp));
    assert(journal->complete(journal->context, 1u, &final_result, &timestamp, &entry) == TR2_OK);

    assert(command_boot_recovery_scan(&store, &authorities, &result) == TR2_OK);
    assert(result.status == COMMAND_BOOT_RECOVERY_CLEAN);
    assert(!result.has_incomplete_transaction);
    assert(result.has_latest_completed);
    assert(result.latest_completed.transaction_id == 1u);
    assert(result.latest_completed.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
}

static void test_reserved_transaction_is_consumed_without_replay(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore storage;
    CommandJournalStore store;
    CommandJournalEntry entry;
    CommandBootRecoveryAuthorities authorities = {0};
    CommandBootRecoveryResult result;

    init_store(&media, &persistent_media, &storage, &store);
    reserve_entry(&store, 2u, COMMAND_CODE_START_ACQUISITION, &entry);

    assert(command_boot_recovery_scan(&store, &authorities, &result) == TR2_OK);
    assert(result.status == COMMAND_BOOT_RECOVERY_RESERVED_NO_EFFECT);
    assert(result.has_incomplete_transaction);
    assert(result.incomplete_transaction.transaction_id == 2u);
    assert(result.incomplete_transaction.lifecycle == COMMAND_LIFECYCLE_RESERVED);
}

static void test_started_start_command_reconciles_presence_and_absence(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore storage;
    CommandJournalStore store;
    CommandJournalEntry entry;
    TestRepositoryContext repository_context;
    CampaignRepository repository;
    CommandBootRecoveryAuthorities authorities = {0};
    CommandBootRecoveryResult result;

    init_store(&media, &persistent_media, &storage, &store);
    start_with_context(&store, 3u, COMMAND_CODE_START_ACQUISITION,
                       COMMAND_RECOVERY_CONTEXT_START_CAMPAIGN, 77u, &entry);

    memset(&repository_context, 0, sizeof(repository_context));
    repository_context.present = true;
    repository_context.metadata.campaign_id = 77u;
    repository_context.metadata.lifecycle_state = CAMPAIGN_LIFECYCLE_OPEN;
    memset(&repository, 0, sizeof(repository));
    repository.context = &repository_context;
    repository.get_campaign_by_id = repository_get_by_id;
    authorities.campaign_repository = &repository;

    assert(command_boot_recovery_scan(&store, &authorities, &result) == TR2_OK);
    assert(result.status == COMMAND_BOOT_RECOVERY_STARTED_EFFECT_PROVEN);

    repository_context.present = false;
    assert(command_boot_recovery_scan(&store, &authorities, &result) == TR2_OK);
    assert(result.status == COMMAND_BOOT_RECOVERY_STARTED_ABSENCE_PROVEN);
}

static void test_started_stop_requires_durable_closed_proof(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore storage;
    CommandJournalStore store;
    CommandJournalEntry entry;
    TestRepositoryContext repository_context;
    CampaignRepository repository;
    CommandBootRecoveryAuthorities authorities = {0};
    CommandBootRecoveryResult result;

    init_store(&media, &persistent_media, &storage, &store);
    start_with_context(&store, 4u, COMMAND_CODE_STOP_ACQUISITION,
                       COMMAND_RECOVERY_CONTEXT_STOP_CAMPAIGN, 88u, &entry);

    memset(&repository_context, 0, sizeof(repository_context));
    repository_context.present = true;
    repository_context.metadata.campaign_id = 88u;
    repository_context.metadata.lifecycle_state = CAMPAIGN_LIFECYCLE_OPEN;
    memset(&repository, 0, sizeof(repository));
    repository.context = &repository_context;
    repository.get_campaign_by_id = repository_get_by_id;
    authorities.campaign_repository = &repository;

    assert(command_boot_recovery_scan(&store, &authorities, &result) == TR2_OK);
    assert(result.status == COMMAND_BOOT_RECOVERY_STARTED_INDETERMINATE);

    repository_context.metadata.lifecycle_state = CAMPAIGN_LIFECYCLE_CLOSED;
    assert(command_boot_recovery_scan(&store, &authorities, &result) == TR2_OK);
    assert(result.status == COMMAND_BOOT_RECOVERY_STARTED_EFFECT_PROVEN);
}

static void test_multiple_incomplete_transactions_are_corrupted_state(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore storage;
    CommandJournalStore store;
    CommandJournalEntry entry;
    CommandBootRecoveryAuthorities authorities = {0};
    CommandBootRecoveryResult result;

    init_store(&media, &persistent_media, &storage, &store);
    reserve_entry(&store, 1u, COMMAND_CODE_APPLY_CONFIGURATION, &entry);
    reserve_entry(&store, 2u, COMMAND_CODE_START_ACQUISITION, &entry);

    assert(command_boot_recovery_scan(&store, &authorities, &result) == TR2_ERROR_CORRUPTED);
}

int main(void)
{
    test_completed_history_is_recovered_without_active_transaction();
    test_reserved_transaction_is_consumed_without_replay();
    test_started_start_command_reconciles_presence_and_absence();
    test_started_stop_requires_durable_closed_proof();
    test_multiple_incomplete_transactions_are_corrupted_state();
    return 0;
}
