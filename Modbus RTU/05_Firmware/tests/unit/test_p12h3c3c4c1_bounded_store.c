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
    bool fail_write;
    bool fail_commit;
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
    if (media->fail_write) return TR2_ERROR_STORAGE;
    if ((size_t)offset + size > sizeof(media->bytes)) return TR2_ERROR_STORAGE;
    memcpy(&media->bytes[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    TestMedia *media = (TestMedia *)context;
    return media->fail_commit ? TR2_ERROR_STORAGE : TR2_OK;
}

static void init_core(TestMedia *media, PersistentMedia *pm, PersistentStorageCore *core)
{
    memset(media, 0xFF, sizeof(*media));
    media->fail_read = false;
    media->fail_write = false;
    media->fail_commit = false;
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


static CommandRequest make_request(uint16_t id, uint16_t command_code)
{
    CommandRequest request;
    memset(&request, 0, sizeof(request));
    request.transaction_id = id;
    request.identity.command_code = command_code;
    return request;
}

static void test_reserve_empty_persists_and_advances_order(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRequest request = make_request(42u, COMMAND_CODE_APPLY_CONFIGURATION);
    CommandJournalEntry entry; CommandJournalBoundedSlotSelection selection;

    init_core(&media, &pm, &core);
    assert(command_journal_bounded_store_init(&store, &core) == TR2_OK);
    assert(command_journal_bounded_store_recover(&store, &recovery) == TR2_OK);
    assert(store.journal.reserve(store.journal.context, &request, &entry) == TR2_OK);
    assert(entry.transaction_id == 42u);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);
    assert(store.next_admission_order == 2u);
    assert(command_journal_bounded_slot_select(&core, 0u, &selection) == TR2_OK);
    assert(selection.record.entry.transaction_id == 42u);
    assert(selection.record.admission_order == 1u);
}

static void test_reserve_existing_retry_and_collision_do_not_write(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRequest first = make_request(50u, COMMAND_CODE_APPLY_CONFIGURATION);
    CommandRequest collision = make_request(50u, COMMAND_CODE_STOP_ACQUISITION);
    CommandJournalEntry entry;

    init_core(&media, &pm, &core);
    assert(command_journal_bounded_store_init(&store, &core) == TR2_OK);
    assert(command_journal_bounded_store_recover(&store, &recovery) == TR2_OK);
    assert(store.journal.reserve(store.journal.context, &first, &entry) == TR2_OK);
    assert(store.journal.reserve(store.journal.context, &first, &entry) == TR2_ERROR_INVALID_STATE);
    assert(store.journal.reserve(store.journal.context, &collision, &entry) == TR2_ERROR_INVALID_STATE);
    assert(store.next_admission_order == 2u);
    assert(!store.recovery_required);
}

static void test_reserve_write_and_commit_failure_rearm_recovery(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRequest request = make_request(60u, COMMAND_CODE_APPLY_CONFIGURATION);
    CommandJournalEntry entry;

    init_core(&media, &pm, &core);
    assert(command_journal_bounded_store_init(&store, &core) == TR2_OK);
    assert(command_journal_bounded_store_recover(&store, &recovery) == TR2_OK);
    media.fail_write = true;
    assert(store.journal.reserve(store.journal.context, &request, &entry) == TR2_ERROR_STORAGE);
    assert(store.recovery_required);
    assert(store.next_admission_order == 1u);

    init_core(&media, &pm, &core);
    assert(command_journal_bounded_store_init(&store, &core) == TR2_OK);
    assert(command_journal_bounded_store_recover(&store, &recovery) == TR2_OK);
    media.fail_commit = true;
    assert(store.journal.reserve(store.journal.context, &request, &entry) == TR2_ERROR_STORAGE);
    assert(store.recovery_required);
    assert(store.next_admission_order == 1u);
}

static void test_reserve_evicts_oldest_completed_when_full(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandJournalBoundedRecord record; CommandRequest request;
    CommandJournalEntry entry; size_t slot;

    init_core(&media, &pm, &core);
    for (slot = 0u; slot < TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT; ++slot) {
        record = make_completed((uint16_t)(slot + 1u),
                                (uint32_t)(slot + 2u),
                                (uint32_t)(slot + 1u));
        write_record(&media, slot, &record);
    }
    record = make_completed(201u, 1u, 201u);
    write_record(&media, 200u, &record);

    assert(command_journal_bounded_store_init(&store, &core) == TR2_OK);
    assert(command_journal_bounded_store_recover(&store, &recovery) == TR2_OK);
    assert(store.next_admission_order == 258u);
    request = make_request(60000u, COMMAND_CODE_APPLY_CONFIGURATION);
    assert(store.journal.reserve(store.journal.context, &request, &entry) == TR2_OK);
    assert(entry.transaction_id == 60000u);
    assert(store.next_admission_order == 259u);
    assert(store.journal.find(store.journal.context, 201u, &entry) == TR2_ERROR_NOT_FOUND);
    assert(store.journal.find(store.journal.context, 60000u, &entry) == TR2_OK);
}


static void test_set_recovery_context_then_mark_started(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRequest request = make_request(70u, COMMAND_CODE_APPLY_CONFIGURATION);
    CommandRecoveryContext context;
    CommandJournalEntry entry;
    CommandJournalBoundedSlotSelection selection;

    init_core(&media, &pm, &core);
    assert(command_journal_bounded_store_init(&store, &core) == TR2_OK);
    assert(command_journal_bounded_store_recover(&store, &recovery) == TR2_OK);
    assert(store.journal.reserve(store.journal.context, &request, &entry) == TR2_OK);

    memset(&context, 0, sizeof(context));
    context.kind = COMMAND_RECOVERY_CONTEXT_CONFIGURATION;
    context.value1 = 123u;
    assert(store.journal.set_recovery_context(
               store.journal.context, 70u, &context, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);
    assert(entry.has_recovery_context);
    assert(entry.recovery_context.value1 == 123u);

    assert(command_journal_bounded_slot_select(&core, 0u, &selection) == TR2_OK);
    assert(selection.record.generation == 2u);
    assert(selection.record.admission_order == 1u);

    assert(store.journal.mark_started(store.journal.context, 70u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_STARTED);
    assert(entry.has_recovery_context);
    assert(entry.recovery_context.value1 == 123u);
    assert(command_journal_bounded_slot_select(&core, 0u, &selection) == TR2_OK);
    assert(selection.record.generation == 3u);
    assert(selection.record.admission_order == 1u);
}

static void test_context_and_start_transition_refusals(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRequest request = make_request(71u, COMMAND_CODE_APPLY_CONFIGURATION);
    CommandRecoveryContext context;
    CommandJournalEntry entry;

    init_core(&media, &pm, &core);
    assert(command_journal_bounded_store_init(&store, &core) == TR2_OK);
    assert(command_journal_bounded_store_recover(&store, &recovery) == TR2_OK);
    assert(store.journal.reserve(store.journal.context, &request, &entry) == TR2_OK);

    memset(&context, 0, sizeof(context));
    context.kind = COMMAND_RECOVERY_CONTEXT_CONFIGURATION;
    assert(store.journal.set_recovery_context(
               store.journal.context, 71u, &context, &entry) == TR2_OK);
    assert(store.journal.set_recovery_context(
               store.journal.context, 71u, &context, &entry) == TR2_ERROR_INVALID_STATE);

    assert(store.journal.mark_started(store.journal.context, 71u, &entry) == TR2_OK);
    assert(store.journal.mark_started(store.journal.context, 71u, &entry) ==
           TR2_ERROR_INVALID_STATE);
    assert(store.journal.set_recovery_context(
               store.journal.context, 71u, &context, &entry) == TR2_ERROR_INVALID_STATE);
    assert(store.journal.mark_started(store.journal.context, 999u, &entry) ==
           TR2_ERROR_NOT_FOUND);
    assert(!store.recovery_required);
}

static void test_context_and_start_storage_failure_rearms_recovery(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRequest request = make_request(72u, COMMAND_CODE_APPLY_CONFIGURATION);
    CommandRecoveryContext context;
    CommandJournalEntry entry;

    init_core(&media, &pm, &core);
    assert(command_journal_bounded_store_init(&store, &core) == TR2_OK);
    assert(command_journal_bounded_store_recover(&store, &recovery) == TR2_OK);
    assert(store.journal.reserve(store.journal.context, &request, &entry) == TR2_OK);
    memset(&context, 0, sizeof(context));
    context.kind = COMMAND_RECOVERY_CONTEXT_CONFIGURATION;

    media.fail_write = true;
    assert(store.journal.set_recovery_context(
               store.journal.context, 72u, &context, &entry) == TR2_ERROR_STORAGE);
    assert(store.recovery_required);

    init_core(&media, &pm, &core);
    assert(command_journal_bounded_store_init(&store, &core) == TR2_OK);
    assert(command_journal_bounded_store_recover(&store, &recovery) == TR2_OK);
    assert(store.journal.reserve(store.journal.context, &request, &entry) == TR2_OK);
    media.fail_commit = true;
    assert(store.journal.mark_started(store.journal.context, 72u, &entry) ==
           TR2_ERROR_STORAGE);
    assert(store.recovery_required);
}

int main(void)
{
    test_init_blocks_all_operations_until_recovery();
    test_empty_recovery_opens_read_store();
    test_valid_recovery_reconstructs_counters_and_reads();
    test_reader_degradation_rearms_recovery();
    test_visitor_business_error_does_not_rearm();
    test_failed_recovery_stays_closed_then_can_recover();
    test_reserve_empty_persists_and_advances_order();
    test_reserve_existing_retry_and_collision_do_not_write();
    test_reserve_write_and_commit_failure_rearm_recovery();
    test_reserve_evicts_oldest_completed_when_full();
    test_set_recovery_context_then_mark_started();
    test_context_and_start_transition_refusals();
    test_context_and_start_storage_failure_rearms_recovery();
    return 0;
}
