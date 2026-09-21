#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/command_journal_bounded_record.h"
#include "tr2/persistence/command_journal_bounded_slot.h"
#include "tr2/persistence/command_journal_bounded_store.h"

typedef struct {
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE];
    uint32_t write_count;
    uint32_t commit_count;
} TestMedia;

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    if ((size_t)offset + size > sizeof(media->bytes)) return TR2_ERROR_STORAGE;
    memcpy(buffer, &media->bytes[offset], size);
    return TR2_OK;
}

static Tr2Result media_write(void *context, uint32_t offset,
                             const void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    if ((size_t)offset + size > sizeof(media->bytes)) return TR2_ERROR_STORAGE;
    media->write_count++;
    memcpy(&media->bytes[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    TestMedia *media = (TestMedia *)context;
    media->commit_count++;
    return TR2_OK;
}

static void init_media(TestMedia *media)
{
    memset(media->bytes, 0xFF, sizeof(media->bytes));
    media->write_count = 0u;
    media->commit_count = 0u;
}

static void init_core(TestMedia *media, PersistentMedia *pm, PersistentStorageCore *core)
{
    pm->context = media;
    pm->read = media_read;
    pm->write = media_write;
    pm->commit = media_commit;
    assert(persistent_storage_core_init(core, pm) == TR2_OK);
}

static void boot_store(TestMedia *media, PersistentMedia *pm,
                       PersistentStorageCore *core,
                       CommandJournalBoundedStore *store,
                       CommandJournalBoundedRecoveryResult *recovery)
{
    init_core(media, pm, core);
    assert(command_journal_bounded_store_init(store, core) == TR2_OK);
    assert(command_journal_bounded_store_recover(store, recovery) == TR2_OK);
}

static CommandRequest make_request(uint16_t id, uint16_t command_code)
{
    CommandRequest request;
    memset(&request, 0, sizeof(request));
    request.transaction_id = id;
    request.identity.command_code = command_code;
    request.identity.param1 = 0x1234u;
    return request;
}

static CommandJournalBoundedRecord make_record(uint16_t id,
                                                uint32_t admission,
                                                uint32_t generation,
                                                CommandLifecycleState lifecycle)
{
    CommandJournalBoundedRecord record;
    memset(&record, 0, sizeof(record));
    record.generation = generation;
    record.admission_order = admission;
    record.entry.transaction_id = id;
    record.entry.request_identity.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    record.entry.lifecycle = lifecycle;
    if (lifecycle == COMMAND_LIFECYCLE_COMPLETED) {
        record.entry.has_final_result = true;
        record.entry.final_result.status = COMMAND_STATUS_SUCCESS;
        record.entry.final_result.result_code = COMMAND_RESULT_SUCCESS;
        record.entry.completion_order = admission;
    }
    return record;
}

static void write_encoded(TestMedia *media, size_t slot, size_t copy,
                          const CommandJournalBoundedRecord *record)
{
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE];
    uint32_t offset;
    assert(command_journal_bounded_slot_offset(slot, copy, &offset) == TR2_OK);
    assert(tr2_command_journal_bounded_record_encode(record, bytes, sizeof(bytes)) == TR2_OK);
    memcpy(&media->bytes[offset], bytes, sizeof(bytes));
}

static void corrupt_copy(TestMedia *media, size_t slot, size_t copy)
{
    uint32_t offset;
    assert(command_journal_bounded_slot_offset(slot, copy, &offset) == TR2_OK);
    media->bytes[offset + 20u] ^= 0x5Au;
}

/* D6: one bad peer must be masked by the remaining valid A/B copy. */
static void test_d6_one_corrupt_peer_keeps_valid_copy_authoritative(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandJournalBoundedRecord a = make_record(10u, 1u, 1u, COMMAND_LIFECYCLE_COMPLETED);
    CommandJournalBoundedRecord b = make_record(10u, 1u, 2u, COMMAND_LIFECYCLE_COMPLETED);
    CommandJournalEntry entry;

    init_media(&media);
    write_encoded(&media, 0u, 0u, &a);
    write_encoded(&media, 0u, 1u, &b);
    corrupt_copy(&media, 0u, 1u);

    boot_store(&media, &pm, &core, &store, &recovery);
    assert(recovery.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID);
    assert(recovery.known_transaction_count == 1u);
    assert(store.journal.find(store.journal.context, 10u, &entry) == TR2_OK);
    assert(entry.transaction_id == 10u);
}

/* D6: with no valid peer left, recovery must stay closed and classify corruption. */
static void test_d6_both_corrupt_copies_block_recovery(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandJournalBoundedRecord a = make_record(11u, 1u, 1u, COMMAND_LIFECYCLE_COMPLETED);
    CommandJournalBoundedRecord b = make_record(11u, 1u, 2u, COMMAND_LIFECYCLE_COMPLETED);
    CommandJournalEntry entry;

    init_media(&media);
    write_encoded(&media, 0u, 0u, &a);
    write_encoded(&media, 0u, 1u, &b);
    corrupt_copy(&media, 0u, 0u);
    corrupt_copy(&media, 0u, 1u);

    boot_store(&media, &pm, &core, &store, &recovery);
    assert(recovery.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_CORRUPTED);
    assert(store.recovery_required);
    assert(store.journal.find(store.journal.context, 11u, &entry) ==
           TR2_ERROR_INVALID_STATE);
}

/* D7: after reboot, a retained identical request is still a retry and must not rewrite. */
static void test_d7_retry_after_reboot_does_not_reexecute_or_rewrite_journal(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRequest request = make_request(77u, COMMAND_CODE_APPLY_CONFIGURATION);
    CommandJournalEntry entry;
    uint32_t writes_before;
    uint32_t commits_before;

    init_media(&media);
    boot_store(&media, &pm, &core, &store, &recovery);
    assert(store.journal.reserve(store.journal.context, &request, &entry) == TR2_OK);

    boot_store(&media, &pm, &core, &store, &recovery);
    assert(recovery.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID);
    writes_before = media.write_count;
    commits_before = media.commit_count;
    assert(store.journal.reserve(store.journal.context, &request, &entry) ==
           TR2_ERROR_INVALID_STATE);
    assert(media.write_count == writes_before);
    assert(media.commit_count == commits_before);
    assert(!store.recovery_required);
    assert(store.journal.find(store.journal.context, 77u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);
}

/* D7: same retained ID with a different identity is a collision and also cannot rewrite. */
static void test_d7_collision_after_reboot_does_not_rewrite_journal(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRequest first = make_request(78u, COMMAND_CODE_APPLY_CONFIGURATION);
    CommandRequest collision = make_request(78u, COMMAND_CODE_STOP_ACQUISITION);
    CommandJournalEntry entry;
    uint32_t writes_before;
    uint32_t commits_before;

    init_media(&media);
    boot_store(&media, &pm, &core, &store, &recovery);
    assert(store.journal.reserve(store.journal.context, &first, &entry) == TR2_OK);

    boot_store(&media, &pm, &core, &store, &recovery);
    writes_before = media.write_count;
    commits_before = media.commit_count;
    assert(store.journal.reserve(store.journal.context, &collision, &entry) ==
           TR2_ERROR_INVALID_STATE);
    assert(media.write_count == writes_before);
    assert(media.commit_count == commits_before);
    assert(!store.recovery_required);
    assert(store.journal.find(store.journal.context, 78u, &entry) == TR2_OK);
    assert(entry.request_identity.command_code == COMMAND_CODE_APPLY_CONFIGURATION);
}

/* D8: recovery refuses an image containing more than one non-terminal transaction. */
static void test_d8_two_nonterminal_transactions_are_corruption(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandJournalBoundedRecord first =
        make_record(90u, 1u, 1u, COMMAND_LIFECYCLE_RESERVED);
    CommandJournalBoundedRecord second =
        make_record(91u, 2u, 1u, COMMAND_LIFECYCLE_STARTED);

    init_media(&media);
    write_encoded(&media, 0u, 0u, &first);
    write_encoded(&media, 1u, 0u, &second);

    boot_store(&media, &pm, &core, &store, &recovery);
    assert(recovery.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_CORRUPTED);
    assert(store.recovery_required);
}

/* D8 positive boundary: exactly one non-terminal is admissible. */
static void test_d8_one_nonterminal_with_completed_history_is_valid(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandJournalBoundedRecord active =
        make_record(92u, 2u, 1u, COMMAND_LIFECYCLE_STARTED);
    CommandJournalBoundedRecord completed =
        make_record(93u, 1u, 1u, COMMAND_LIFECYCLE_COMPLETED);
    CommandJournalEntry entry;

    init_media(&media);
    write_encoded(&media, 0u, 0u, &completed);
    write_encoded(&media, 1u, 0u, &active);

    boot_store(&media, &pm, &core, &store, &recovery);
    assert(recovery.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID);
    assert(recovery.known_transaction_count == 2u);
    assert(store.journal.find(store.journal.context, 92u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_STARTED);
}

int main(void)
{
    test_d6_one_corrupt_peer_keeps_valid_copy_authoritative();
    test_d6_both_corrupt_copies_block_recovery();
    test_d7_retry_after_reboot_does_not_reexecute_or_rewrite_journal();
    test_d7_collision_after_reboot_does_not_rewrite_journal();
    test_d8_two_nonterminal_transactions_are_corruption();
    test_d8_one_nonterminal_with_completed_history_is_valid();
    return 0;
}
