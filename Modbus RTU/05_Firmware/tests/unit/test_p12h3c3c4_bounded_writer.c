#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/command_journal_bounded_writer.h"

typedef struct {
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE];
    bool fail_write;
    bool fail_commit;
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

static Tr2Result media_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    media->write_count++;
    if (media->fail_write) return TR2_ERROR_STORAGE;
    if ((size_t)offset + size > sizeof(media->bytes)) return TR2_ERROR_STORAGE;
    memcpy(&media->bytes[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    TestMedia *media = (TestMedia *)context;
    media->commit_count++;
    return media->fail_commit ? TR2_ERROR_STORAGE : TR2_OK;
}

static void init_core(TestMedia *media, PersistentMedia *persistent_media,
                      PersistentStorageCore *core)
{
    memset(media, 0xFF, sizeof(*media));
    media->fail_write = false;
    media->fail_commit = false;
    media->write_count = 0u;
    media->commit_count = 0u;
    persistent_media->context = media;
    persistent_media->read = media_read;
    persistent_media->write = media_write;
    persistent_media->commit = media_commit;
    assert(persistent_storage_core_init(core, persistent_media) == TR2_OK);
}

static CommandJournalBoundedRecord make_record(uint16_t id, uint32_t admission)
{
    CommandJournalBoundedRecord record;
    memset(&record, 0, sizeof(record));
    record.generation = 99u;
    record.admission_order = admission;
    record.entry.transaction_id = id;
    record.entry.request_identity.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    record.entry.lifecycle = COMMAND_LIFECYCLE_RESERVED;
    return record;
}

static void test_admit_empty_uses_copy0_generation1(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedRecord record = make_record(1u, 7u);
    CommandJournalBoundedSlotSelection selection;

    init_core(&media, &pm, &core);
    assert(command_journal_bounded_writer_admit_empty(&core, 9u, &record) == TR2_OK);
    assert(media.write_count == 1u && media.commit_count == 1u);
    assert(command_journal_bounded_slot_select(&core, 9u, &selection) == TR2_OK);
    assert(selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_VALID);
    assert(selection.current_copy == 0u);
    assert(selection.record.generation == 1u);
    assert(selection.record.admission_order == 7u);
}

static void test_admit_empty_rejects_nonempty_slot(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedRecord first = make_record(2u, 8u);
    CommandJournalBoundedRecord second = make_record(3u, 9u);

    init_core(&media, &pm, &core);
    assert(command_journal_bounded_writer_admit_empty(&core, 2u, &first) == TR2_OK);
    assert(command_journal_bounded_writer_admit_empty(&core, 2u, &second) ==
           TR2_ERROR_INVALID_STATE);
}

static void test_mutate_alternates_and_increments_generation(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedRecord first = make_record(4u, 10u);
    CommandJournalBoundedRecord replacement = make_record(4u, 10u);
    CommandJournalBoundedSlotSelection current;
    CommandJournalBoundedSlotSelection after;

    init_core(&media, &pm, &core);
    assert(command_journal_bounded_writer_admit_empty(&core, 4u, &first) == TR2_OK);
    assert(command_journal_bounded_slot_select(&core, 4u, &current) == TR2_OK);
    replacement.entry.lifecycle = COMMAND_LIFECYCLE_STARTED;
    assert(command_journal_bounded_writer_mutate(&core, 4u, &current, &replacement) == TR2_OK);
    assert(command_journal_bounded_slot_select(&core, 4u, &after) == TR2_OK);
    assert(after.current_copy == 1u);
    assert(after.record.generation == 2u);
    assert(after.record.admission_order == 10u);
    assert(after.record.entry.lifecycle == COMMAND_LIFECYCLE_STARTED);

    current = after;
    replacement.entry.lifecycle = COMMAND_LIFECYCLE_COMPLETED;
    replacement.entry.has_final_result = true;
    replacement.entry.final_result.status = COMMAND_STATUS_SUCCESS;
    replacement.entry.completion_order = 1u;
    assert(command_journal_bounded_writer_mutate(&core, 4u, &current, &replacement) == TR2_OK);
    assert(command_journal_bounded_slot_select(&core, 4u, &after) == TR2_OK);
    assert(after.current_copy == 0u);
    assert(after.record.generation == 3u);
}

static void test_mutate_rejects_admission_change_and_generation_wrap(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedRecord replacement = make_record(5u, 12u);
    CommandJournalBoundedSlotSelection current;

    init_core(&media, &pm, &core);
    memset(&current, 0, sizeof(current));
    current.status = COMMAND_JOURNAL_BOUNDED_SLOT_VALID;
    current.has_record = true;
    current.current_copy = 0u;
    current.record = make_record(5u, 11u);
    current.record.generation = 4u;
    assert(command_journal_bounded_writer_mutate(&core, 5u, &current, &replacement) ==
           TR2_ERROR_INVALID_ARGUMENT);

    replacement.admission_order = 11u;
    current.record.generation = UINT32_MAX;
    assert(command_journal_bounded_writer_mutate(&core, 5u, &current, &replacement) ==
           TR2_ERROR_UNSUPPORTED);
}

static void test_write_failure_skips_commit(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedRecord record = make_record(6u, 13u);

    init_core(&media, &pm, &core);
    media.fail_write = true;
    assert(command_journal_bounded_writer_admit_empty(&core, 6u, &record) ==
           TR2_ERROR_STORAGE);
    assert(media.write_count == 1u);
    assert(media.commit_count == 0u);
}

static void test_commit_failure_is_propagated(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedRecord record = make_record(7u, 14u);

    init_core(&media, &pm, &core);
    media.fail_commit = true;
    assert(command_journal_bounded_writer_admit_empty(&core, 7u, &record) ==
           TR2_ERROR_STORAGE);
    assert(media.write_count == 1u);
    assert(media.commit_count == 1u);
}

int main(void)
{
    test_admit_empty_uses_copy0_generation1();
    test_admit_empty_rejects_nonempty_slot();
    test_mutate_alternates_and_increments_generation();
    test_mutate_rejects_admission_change_and_generation_wrap();
    test_write_failure_skips_commit();
    test_commit_failure_is_propagated();
    return 0;
}
