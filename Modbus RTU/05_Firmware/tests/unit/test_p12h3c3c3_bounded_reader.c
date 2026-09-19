#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/command_journal_bounded_reader.h"
#include "tr2/persistence/command_journal_bounded_slot.h"

typedef struct {
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE];
} TestMedia;

typedef struct {
    uint32_t count;
    bool seen_1;
    bool seen_7;
    bool seen_65535;
    Tr2Result result;
} VisitContext;

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;

    if ((size_t)offset + size > sizeof(media->bytes)) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(buffer, &media->bytes[offset], size);
    return TR2_OK;
}

static Tr2Result media_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;

    if ((size_t)offset + size > sizeof(media->bytes)) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(&media->bytes[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    (void)context;
    return TR2_OK;
}

static void init_core(TestMedia *media,
                      PersistentMedia *persistent_media,
                      PersistentStorageCore *core)
{
    memset(media, 0xFF, sizeof(*media));
    persistent_media->context = media;
    persistent_media->read = media_read;
    persistent_media->write = media_write;
    persistent_media->commit = media_commit;
    assert(persistent_storage_core_init(core, persistent_media) == TR2_OK);
}

static CommandJournalBoundedRecord make_record(uint16_t transaction_id,
                                               uint32_t generation,
                                               uint32_t admission_order,
                                               CommandLifecycleState lifecycle,
                                               uint32_t completion_order)
{
    CommandJournalBoundedRecord record;

    memset(&record, 0, sizeof(record));
    record.generation = generation;
    record.admission_order = admission_order;
    record.entry.transaction_id = transaction_id;
    record.entry.request_identity.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    record.entry.lifecycle = lifecycle;
    record.entry.completion_order = completion_order;
    if (lifecycle == COMMAND_LIFECYCLE_COMPLETED) {
        record.entry.has_final_result = true;
        record.entry.final_result.status = COMMAND_STATUS_SUCCESS;
    }
    return record;
}

static void write_record(TestMedia *media,
                         size_t logical_slot,
                         size_t copy_index,
                         const CommandJournalBoundedRecord *record)
{
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE];
    uint32_t offset;

    assert(command_journal_bounded_slot_offset(logical_slot, copy_index, &offset) == TR2_OK);
    assert(tr2_command_journal_bounded_record_encode(record, bytes, sizeof(bytes)) == TR2_OK);
    memcpy(&media->bytes[offset], bytes, sizeof(bytes));
}

static Tr2Result visit_entry(void *context,
                             size_t logical_slot,
                             const CommandJournalBoundedRecord *record)
{
    VisitContext *visit = (VisitContext *)context;

    (void)logical_slot;
    visit->count++;
    if (record->entry.transaction_id == 1u) {
        visit->seen_1 = true;
    } else if (record->entry.transaction_id == 7u) {
        visit->seen_7 = true;
    } else if (record->entry.transaction_id == UINT16_MAX) {
        visit->seen_65535 = true;
    }
    return visit->result;
}

static void test_find_empty_and_invalid_id(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    size_t slot = 0u;
    CommandJournalBoundedRecord record;

    init_core(&media, &persistent_media, &core);
    assert(command_journal_bounded_reader_find(&core, 1u, &slot, &record) ==
           TR2_ERROR_NOT_FOUND);
    assert(command_journal_bounded_reader_find(&core, 0u, &slot, &record) ==
           TR2_ERROR_INVALID_ARGUMENT);
}

static void test_find_extreme_ids_independent_of_slot(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedRecord first =
        make_record(1u, 2u, 1u, COMMAND_LIFECYCLE_RESERVED, 0u);
    CommandJournalBoundedRecord last =
        make_record(UINT16_MAX, 3u, 2u, COMMAND_LIFECYCLE_COMPLETED, 1u);
    CommandJournalBoundedRecord found;
    size_t slot;

    init_core(&media, &persistent_media, &core);
    write_record(&media, 200u, 0u, &first);
    write_record(&media, 4u, 1u, &last);

    assert(command_journal_bounded_reader_find(&core, 1u, &slot, &found) == TR2_OK);
    assert(slot == 200u);
    assert(found.entry.transaction_id == 1u);

    assert(command_journal_bounded_reader_find(&core, UINT16_MAX, &slot, &found) == TR2_OK);
    assert(slot == 4u);
    assert(found.entry.transaction_id == UINT16_MAX);

    assert(command_journal_bounded_reader_find(&core, 1234u, &slot, &found) ==
           TR2_ERROR_NOT_FOUND);
}

static void test_visit_empty_and_multiple_entries(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    VisitContext visit;
    CommandJournalBoundedRecord first =
        make_record(1u, 1u, 1u, COMMAND_LIFECYCLE_COMPLETED, 1u);
    CommandJournalBoundedRecord second =
        make_record(7u, 1u, 2u, COMMAND_LIFECYCLE_RESERVED, 0u);
    CommandJournalBoundedRecord third =
        make_record(UINT16_MAX, 1u, 3u, COMMAND_LIFECYCLE_COMPLETED, 2u);

    init_core(&media, &persistent_media, &core);
    memset(&visit, 0, sizeof(visit));
    visit.result = TR2_OK;
    assert(command_journal_bounded_reader_visit(&core, visit_entry, &visit) == TR2_OK);
    assert(visit.count == 0u);

    write_record(&media, 8u, 0u, &first);
    write_record(&media, 90u, 1u, &second);
    write_record(&media, 240u, 0u, &third);

    memset(&visit, 0, sizeof(visit));
    visit.result = TR2_OK;
    assert(command_journal_bounded_reader_visit(&core, visit_entry, &visit) == TR2_OK);
    assert(visit.count == 3u);
    assert(visit.seen_1);
    assert(visit.seen_7);
    assert(visit.seen_65535);
}

static void test_visit_propagates_visitor_error(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    VisitContext visit;
    CommandJournalBoundedRecord record =
        make_record(7u, 1u, 1u, COMMAND_LIFECYCLE_RESERVED, 0u);

    init_core(&media, &persistent_media, &core);
    write_record(&media, 3u, 0u, &record);
    memset(&visit, 0, sizeof(visit));
    visit.result = TR2_ERROR_INTERNAL;

    assert(command_journal_bounded_reader_visit(&core, visit_entry, &visit) ==
           TR2_ERROR_INTERNAL);
    assert(visit.count == 1u);
}

static void test_latest_completed_not_found(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedRecord reserved =
        make_record(9u, 1u, 1u, COMMAND_LIFECYCLE_RESERVED, 0u);
    CommandJournalBoundedRecord found;
    size_t slot;

    init_core(&media, &persistent_media, &core);
    assert(command_journal_bounded_reader_latest_completed(&core, &slot, &found) ==
           TR2_ERROR_NOT_FOUND);

    write_record(&media, 1u, 0u, &reserved);
    assert(command_journal_bounded_reader_latest_completed(&core, &slot, &found) ==
           TR2_ERROR_NOT_FOUND);
}

static void test_latest_completed_selects_highest_order(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedRecord older =
        make_record(20u, 1u, 1u, COMMAND_LIFECYCLE_COMPLETED, 3u);
    CommandJournalBoundedRecord nonterminal =
        make_record(21u, 1u, 2u, COMMAND_LIFECYCLE_STARTED, 0u);
    CommandJournalBoundedRecord latest =
        make_record(22u, 1u, 3u, COMMAND_LIFECYCLE_COMPLETED, 9u);
    CommandJournalBoundedRecord found;
    size_t slot;

    init_core(&media, &persistent_media, &core);
    write_record(&media, 150u, 0u, &older);
    write_record(&media, 151u, 0u, &nonterminal);
    write_record(&media, 2u, 1u, &latest);

    assert(command_journal_bounded_reader_latest_completed(&core, &slot, &found) == TR2_OK);
    assert(slot == 2u);
    assert(found.entry.transaction_id == 22u);
    assert(found.entry.completion_order == 9u);
}

static void test_latest_completed_rejects_equal_order(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedRecord first =
        make_record(30u, 1u, 1u, COMMAND_LIFECYCLE_COMPLETED, 5u);
    CommandJournalBoundedRecord second =
        make_record(31u, 1u, 2u, COMMAND_LIFECYCLE_COMPLETED, 5u);
    CommandJournalBoundedRecord found;
    size_t slot;

    init_core(&media, &persistent_media, &core);
    write_record(&media, 10u, 0u, &first);
    write_record(&media, 11u, 0u, &second);

    assert(command_journal_bounded_reader_latest_completed(&core, &slot, &found) ==
           TR2_ERROR_CORRUPTED);
}

int main(void)
{
    test_find_empty_and_invalid_id();
    test_find_extreme_ids_independent_of_slot();
    test_visit_empty_and_multiple_entries();
    test_visit_propagates_visitor_error();
    test_latest_completed_not_found();
    test_latest_completed_selects_highest_order();
    test_latest_completed_rejects_equal_order();
    return 0;
}
