#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/command_journal_bounded_slot.h"
#include "tr2/persistence/command_journal_bounded_store.h"

typedef struct {
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE];
    bool fail_read;
} TestMedia;

typedef struct {
    uint32_t count;
    Tr2Result result;
} VisitContext;

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    if (media->fail_read) return TR2_ERROR_STORAGE;
    if ((size_t)offset + size > sizeof(media->bytes)) return TR2_ERROR_STORAGE;
    memcpy(buffer, &media->bytes[offset], size);
    return TR2_OK;
}

static Tr2Result media_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    if ((size_t)offset + size > sizeof(media->bytes)) return TR2_ERROR_STORAGE;
    memcpy(&media->bytes[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    (void)context;
    return TR2_OK;
}

static void init_core(TestMedia *media, PersistentMedia *pm, PersistentStorageCore *core)
{
    memset(media, 0xFF, sizeof(*media));
    media->fail_read = false;
    pm->context = media;
    pm->read = media_read;
    pm->write = media_write;
    pm->commit = media_commit;
    assert(persistent_storage_core_init(core, pm) == TR2_OK);
}

static CommandJournalBoundedRecord make_completed(uint16_t id,
                                                   uint32_t admission,
                                                   uint32_t completion)
{
    CommandJournalBoundedRecord record;
    memset(&record, 0, sizeof(record));
    record.generation = 1u;
    record.admission_order = admission;
    record.entry.transaction_id = id;
    record.entry.request_identity.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    record.entry.lifecycle = COMMAND_LIFECYCLE_COMPLETED;
    record.entry.has_final_result = true;
    record.entry.final_result.status = COMMAND_STATUS_SUCCESS;
    record.entry.completion_order = completion;
    return record;
}

static void write_record(TestMedia *media, size_t slot,
                         const CommandJournalBoundedRecord *record)
{
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE];
    uint32_t offset;
    assert(command_journal_bounded_slot_offset(slot, 0u, &offset) == TR2_OK);
    assert(tr2_command_journal_bounded_record_encode(record, bytes, sizeof(bytes)) == TR2_OK);
    memcpy(&media->bytes[offset], bytes, sizeof(bytes));
}

static Tr2Result visitor(void *context, const CommandJournalEntry *entry)
{
    VisitContext *visit = (VisitContext *)context;
    (void)entry;
    visit->count++;
    return visit->result;
}

static void test_init_blocks_all_operations_until_recovery(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store;
    CommandJournal *journal;
    CommandJournalEntry entry;
    CommandRequest request;
    CommandRecoveryContext recovery_context;
    CommandFinalResult final_result;
    CommandTerminalTimestamp timestamp;
    VisitContext visit = {0u, TR2_OK};

    init_core(&media, &pm, &core);
    assert(command_journal_bounded_store_init(&store, &core) == TR2_OK);
    assert(command_journal_bounded_store_recovery_required(&store));
    journal = command_journal_bounded_store_journal(&store);
    assert(journal != NULL);

    memset(&request, 0, sizeof(request)); request.transaction_id = 1u;
    memset(&recovery_context, 0, sizeof(recovery_context));
    memset(&final_result, 0, sizeof(final_result));
    memset(&timestamp, 0, sizeof(timestamp));

    assert(journal->find(journal->context, 1u, &entry) == TR2_ERROR_INVALID_STATE);
    assert(journal->visit(journal->context, visitor, &visit) == TR2_ERROR_INVALID_STATE);
    assert(journal->latest_completed(journal->context, &entry) == TR2_ERROR_INVALID_STATE);
    assert(journal->reserve(journal->context, &request, &entry) == TR2_ERROR_INVALID_STATE);
    assert(journal->set_recovery_context(journal->context, 1u, &recovery_context, &entry) ==
           TR2_ERROR_INVALID_STATE);
    assert(journal->mark_started(journal->context, 1u, &entry) == TR2_ERROR_INVALID_STATE);
    assert(journal->complete(journal->context, 1u, &final_result, &timestamp, &entry) ==
           TR2_ERROR_INVALID_STATE);
}

static void test_empty_recovery_opens_read_store(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store;
    CommandJournalBoundedRecoveryResult result;
    CommandJournalEntry entry;

    init_core(&media, &pm, &core);
    assert(command_journal_bounded_store_init(&store, &core) == TR2_OK);
    assert(command_journal_bounded_store_recover(&store, &result) == TR2_OK);
    assert(result.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_EMPTY);
    assert(!command_journal_bounded_store_recovery_required(&store));
    assert(store.next_admission_order == 1u);
    assert(store.next_completion_order == 1u);
    assert(store.journal.find(store.journal.context, 1u, &entry) == TR2_ERROR_NOT_FOUND);
    assert(!command_journal_bounded_store_recovery_required(&store));
}

static void test_valid_recovery_reconstructs_counters_and_reads(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store;
    CommandJournalBoundedRecoveryResult result;
    CommandJournalBoundedRecord first = make_completed(1u, 7u, 3u);
    CommandJournalBoundedRecord second = make_completed(UINT16_MAX, 12u, 9u);
    CommandJournalEntry entry;
    VisitContext visit = {0u, TR2_OK};

    init_core(&media, &pm, &core);
    write_record(&media, 200u, &first);
    write_record(&media, 4u, &second);
    assert(command_journal_bounded_store_init(&store, &core) == TR2_OK);
    assert(command_journal_bounded_store_recover(&store, &result) == TR2_OK);
    assert(result.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID);
    assert(result.known_transaction_count == 2u);
    assert(store.next_admission_order == 13u);
    assert(store.next_completion_order == 10u);

    assert(store.journal.find(store.journal.context, UINT16_MAX, &entry) == TR2_OK);
    assert(entry.transaction_id == UINT16_MAX);
    assert(store.journal.latest_completed(store.journal.context, &entry) == TR2_OK);
    assert(entry.transaction_id == UINT16_MAX);
    assert(store.journal.visit(store.journal.context, visitor, &visit) == TR2_OK);
    assert(visit.count == 2u);
}

static void test_reader_degradation_rearms_recovery(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store;
    CommandJournalBoundedRecoveryResult result;
    CommandJournalEntry entry;

    init_core(&media, &pm, &core);
    assert(command_journal_bounded_store_init(&store, &core) == TR2_OK);
    assert(command_journal_bounded_store_recover(&store, &result) == TR2_OK);
    media.fail_read = true;
    assert(store.journal.find(store.journal.context, 1u, &entry) == TR2_ERROR_UNAVAILABLE);
    assert(command_journal_bounded_store_recovery_required(&store));
    assert(store.journal.find(store.journal.context, 1u, &entry) == TR2_ERROR_INVALID_STATE);
}

static void test_visitor_business_error_does_not_rearm(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store;
    CommandJournalBoundedRecoveryResult result;
    CommandJournalBoundedRecord record = make_completed(7u, 1u, 1u);
    VisitContext visit = {0u, TR2_ERROR_INTERNAL};

    init_core(&media, &pm, &core);
    write_record(&media, 3u, &record);
    assert(command_journal_bounded_store_init(&store, &core) == TR2_OK);
    assert(command_journal_bounded_store_recover(&store, &result) == TR2_OK);
    assert(store.journal.visit(store.journal.context, visitor, &visit) == TR2_ERROR_INTERNAL);
    assert(visit.count == 1u);
    assert(!command_journal_bounded_store_recovery_required(&store));
}

static void test_failed_recovery_stays_closed_then_can_recover(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store;
    CommandJournalBoundedRecoveryResult result;
    CommandJournalEntry entry;

    init_core(&media, &pm, &core);
    assert(command_journal_bounded_store_init(&store, &core) == TR2_OK);
    media.fail_read = true;
    assert(command_journal_bounded_store_recover(&store, &result) == TR2_OK);
    assert(result.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_UNAVAILABLE);
    assert(command_journal_bounded_store_recovery_required(&store));

    media.fail_read = false;
    assert(command_journal_bounded_store_recover(&store, &result) == TR2_OK);
    assert(result.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_EMPTY);
    assert(!command_journal_bounded_store_recovery_required(&store));
    assert(store.journal.find(store.journal.context, 1u, &entry) == TR2_ERROR_NOT_FOUND);
}

int main(void)
{
    test_init_blocks_all_operations_until_recovery();
    test_empty_recovery_opens_read_store();
    test_valid_recovery_reconstructs_counters_and_reads();
    test_reader_degradation_rearms_recovery();
    test_visitor_business_error_does_not_rearm();
    test_failed_recovery_stays_closed_then_can_recover();
    return 0;
}
