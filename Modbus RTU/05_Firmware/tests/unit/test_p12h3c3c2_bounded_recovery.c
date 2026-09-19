#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/command_journal_bounded_recovery.h"
#include "tr2/persistence/command_journal_bounded_slot.h"

typedef struct {
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE];
} TestMedia;

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

static void test_empty_store(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedRecoveryResult result;

    init_core(&media, &persistent_media, &core);
    assert(command_journal_bounded_recovery_scan(&core, &result) == TR2_OK);
    assert(result.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_EMPTY);
    assert(result.known_transaction_count == 0u);
    assert(result.next_admission_order == 1u);
    assert(result.next_completion_order == 1u);
}

static void test_reconstructs_counters_and_extreme_transaction_ids(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedRecoveryResult result;
    CommandJournalBoundedRecord first =
        make_record(1u, 3u, 10u, COMMAND_LIFECYCLE_COMPLETED, 4u);
    CommandJournalBoundedRecord second =
        make_record(UINT16_MAX, 8u, 25u, COMMAND_LIFECYCLE_COMPLETED, 9u);

    init_core(&media, &persistent_media, &core);
    write_record(&media, 17u, 0u, &first);
    write_record(&media, 201u, 1u, &second);

    assert(command_journal_bounded_recovery_scan(&core, &result) == TR2_OK);
    assert(result.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID);
    assert(result.known_transaction_count == 2u);
    assert(result.next_admission_order == 26u);
    assert(result.next_completion_order == 10u);
}

static void test_single_reserved_is_valid(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedRecoveryResult result;
    CommandJournalBoundedRecord record =
        make_record(7u, 1u, 1u, COMMAND_LIFECYCLE_RESERVED, 0u);

    init_core(&media, &persistent_media, &core);
    write_record(&media, 4u, 0u, &record);

    assert(command_journal_bounded_recovery_scan(&core, &result) == TR2_OK);
    assert(result.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID);
    assert(result.known_transaction_count == 1u);
    assert(result.next_admission_order == 2u);
    assert(result.next_completion_order == 1u);
}

static void test_single_started_is_valid(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedRecoveryResult result;
    CommandJournalBoundedRecord record =
        make_record(8u, 2u, 3u, COMMAND_LIFECYCLE_STARTED, 0u);

    init_core(&media, &persistent_media, &core);
    write_record(&media, 9u, 1u, &record);

    assert(command_journal_bounded_recovery_scan(&core, &result) == TR2_OK);
    assert(result.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID);
    assert(result.next_admission_order == 4u);
    assert(result.next_completion_order == 1u);
}

static void test_two_nonterminal_records_are_corrupted(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedRecoveryResult result;
    CommandJournalBoundedRecord first =
        make_record(11u, 1u, 1u, COMMAND_LIFECYCLE_RESERVED, 0u);
    CommandJournalBoundedRecord second =
        make_record(12u, 1u, 2u, COMMAND_LIFECYCLE_STARTED, 0u);

    init_core(&media, &persistent_media, &core);
    write_record(&media, 0u, 0u, &first);
    write_record(&media, 255u, 0u, &second);

    assert(command_journal_bounded_recovery_scan(&core, &result) == TR2_OK);
    assert(result.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_CORRUPTED);
}

static void test_duplicate_transaction_id_is_corrupted(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedRecoveryResult result;
    CommandJournalBoundedRecord first =
        make_record(33u, 1u, 5u, COMMAND_LIFECYCLE_COMPLETED, 1u);
    CommandJournalBoundedRecord second =
        make_record(33u, 1u, 6u, COMMAND_LIFECYCLE_COMPLETED, 2u);

    init_core(&media, &persistent_media, &core);
    write_record(&media, 2u, 0u, &first);
    write_record(&media, 200u, 0u, &second);

    assert(command_journal_bounded_recovery_scan(&core, &result) == TR2_OK);
    assert(result.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_CORRUPTED);
}

static void test_duplicate_admission_order_is_corrupted(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedRecoveryResult result;
    CommandJournalBoundedRecord first =
        make_record(41u, 1u, 17u, COMMAND_LIFECYCLE_COMPLETED, 1u);
    CommandJournalBoundedRecord second =
        make_record(42u, 1u, 17u, COMMAND_LIFECYCLE_COMPLETED, 2u);

    init_core(&media, &persistent_media, &core);
    write_record(&media, 3u, 0u, &first);
    write_record(&media, 4u, 0u, &second);

    assert(command_journal_bounded_recovery_scan(&core, &result) == TR2_OK);
    assert(result.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_CORRUPTED);
}

static void test_duplicate_completion_order_is_corrupted(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedRecoveryResult result;
    CommandJournalBoundedRecord first =
        make_record(51u, 1u, 18u, COMMAND_LIFECYCLE_COMPLETED, 7u);
    CommandJournalBoundedRecord second =
        make_record(52u, 1u, 19u, COMMAND_LIFECYCLE_COMPLETED, 7u);

    init_core(&media, &persistent_media, &core);
    write_record(&media, 5u, 0u, &first);
    write_record(&media, 6u, 0u, &second);

    assert(command_journal_bounded_recovery_scan(&core, &result) == TR2_OK);
    assert(result.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_CORRUPTED);
}

static void test_max_admission_order_is_unsupported(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedRecoveryResult result;
    CommandJournalBoundedRecord record =
        make_record(61u, 1u, UINT32_MAX, COMMAND_LIFECYCLE_COMPLETED, 1u);

    init_core(&media, &persistent_media, &core);
    write_record(&media, 7u, 0u, &record);

    assert(command_journal_bounded_recovery_scan(&core, &result) == TR2_OK);
    assert(result.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_UNSUPPORTED);
}

static void test_max_completion_order_is_unsupported(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedRecoveryResult result;
    CommandJournalBoundedRecord record =
        make_record(62u, 1u, 1u, COMMAND_LIFECYCLE_COMPLETED, UINT32_MAX);

    init_core(&media, &persistent_media, &core);
    write_record(&media, 8u, 0u, &record);

    assert(command_journal_bounded_recovery_scan(&core, &result) == TR2_OK);
    assert(result.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_UNSUPPORTED);
}

static void test_max_generation_remains_recoverable(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedRecoveryResult result;
    CommandJournalBoundedRecord record =
        make_record(63u, UINT32_MAX, 2u, COMMAND_LIFECYCLE_COMPLETED, 1u);

    init_core(&media, &persistent_media, &core);
    write_record(&media, 10u, 1u, &record);

    assert(command_journal_bounded_recovery_scan(&core, &result) == TR2_OK);
    assert(result.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID);
    assert(result.known_transaction_count == 1u);
    assert(result.next_admission_order == 3u);
    assert(result.next_completion_order == 2u);
}

int main(void)
{
    test_empty_store();
    test_reconstructs_counters_and_extreme_transaction_ids();
    test_single_reserved_is_valid();
    test_single_started_is_valid();
    test_two_nonterminal_records_are_corrupted();
    test_duplicate_transaction_id_is_corrupted();
    test_duplicate_admission_order_is_corrupted();
    test_duplicate_completion_order_is_corrupted();
    test_max_admission_order_is_unsupported();
    test_max_completion_order_is_unsupported();
    test_max_generation_remains_recoverable();
    return 0;
}
