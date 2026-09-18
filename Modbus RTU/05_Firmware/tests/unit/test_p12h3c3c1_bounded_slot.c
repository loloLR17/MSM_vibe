#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/command_journal_bounded_slot.h"

typedef struct {
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE];
    bool fail_read;
} TestMedia;

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;

    if (media->fail_read) {
        return TR2_ERROR_STORAGE;
    }
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

static void init_core(TestMedia *media, PersistentMedia *persistent_media,
                      PersistentStorageCore *core)
{
    memset(media, 0xFF, sizeof(*media));
    media->fail_read = false;
    persistent_media->context = media;
    persistent_media->read = media_read;
    persistent_media->write = media_write;
    persistent_media->commit = media_commit;
    assert(persistent_storage_core_init(core, persistent_media) == TR2_OK);
}

static CommandJournalBoundedRecord make_record(uint16_t transaction_id,
                                               uint32_t generation,
                                               uint32_t admission_order)
{
    CommandJournalBoundedRecord record;

    memset(&record, 0, sizeof(record));
    record.generation = generation;
    record.admission_order = admission_order;
    record.entry.transaction_id = transaction_id;
    record.entry.request_identity.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    record.entry.lifecycle = COMMAND_LIFECYCLE_RESERVED;
    return record;
}

static void write_record(TestMedia *media, size_t logical_slot, size_t copy_index,
                         const CommandJournalBoundedRecord *record)
{
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE];
    uint32_t offset;

    assert(command_journal_bounded_slot_offset(logical_slot, copy_index, &offset) == TR2_OK);
    assert(tr2_command_journal_bounded_record_encode(record, bytes, sizeof(bytes)) == TR2_OK);
    memcpy(&media->bytes[offset], bytes, sizeof(bytes));
}

static void test_offsets(void)
{
    uint32_t offset = UINT32_MAX;

    assert(command_journal_bounded_slot_offset(0u, 0u, &offset) == TR2_OK);
    assert(offset == 0u);
    assert(command_journal_bounded_slot_offset(0u, 1u, &offset) == TR2_OK);
    assert(offset == TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE);
    assert(command_journal_bounded_slot_offset(255u, 1u, &offset) == TR2_OK);
    assert(offset + TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE ==
           TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE);
    assert(command_journal_bounded_slot_offset(256u, 0u, &offset) ==
           TR2_ERROR_INVALID_ARGUMENT);
    assert(command_journal_bounded_slot_offset(0u, 2u, &offset) ==
           TR2_ERROR_INVALID_ARGUMENT);
}

static void test_empty_and_single_valid_copy(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedSlotSelection selection;
    CommandJournalBoundedRecord record = make_record(1u, 4u, 9u);

    init_core(&media, &persistent_media, &core);
    assert(command_journal_bounded_slot_select(&core, 0u, &selection) == TR2_OK);
    assert(selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_EMPTY);

    write_record(&media, 0u, 1u, &record);
    assert(command_journal_bounded_slot_select(&core, 0u, &selection) == TR2_OK);
    assert(selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_VALID);
    assert(selection.has_record);
    assert(selection.current_copy == 1u);
    assert(selection.record.generation == 4u);
    assert(selection.record.entry.transaction_id == 1u);
}

static void test_newer_generation_wins(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedSlotSelection selection;
    CommandJournalBoundedRecord old_record = make_record(7u, 10u, 20u);
    CommandJournalBoundedRecord new_record = make_record(7u, 11u, 20u);

    init_core(&media, &persistent_media, &core);
    write_record(&media, 3u, 0u, &old_record);
    write_record(&media, 3u, 1u, &new_record);
    assert(command_journal_bounded_slot_select(&core, 3u, &selection) == TR2_OK);
    assert(selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_VALID);
    assert(selection.current_copy == 1u);
    assert(selection.record.generation == 11u);
}

static void test_corrupted_new_copy_falls_back_to_valid_old_copy(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedSlotSelection selection;
    CommandJournalBoundedRecord old_record = make_record(8u, 21u, 30u);
    CommandJournalBoundedRecord new_record = make_record(8u, 22u, 30u);
    uint32_t offset;

    init_core(&media, &persistent_media, &core);
    write_record(&media, 4u, 0u, &old_record);
    write_record(&media, 4u, 1u, &new_record);
    assert(command_journal_bounded_slot_offset(4u, 1u, &offset) == TR2_OK);
    media.bytes[offset + 22u] ^= UINT8_C(1);

    assert(command_journal_bounded_slot_select(&core, 4u, &selection) == TR2_OK);
    assert(selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_VALID);
    assert(selection.current_copy == 0u);
    assert(selection.record.generation == 21u);
}

static void test_equal_generation_different_content_is_corrupted(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedSlotSelection selection;
    CommandJournalBoundedRecord first = make_record(9u, 31u, 40u);
    CommandJournalBoundedRecord second = make_record(10u, 31u, 40u);

    init_core(&media, &persistent_media, &core);
    write_record(&media, 5u, 0u, &first);
    write_record(&media, 5u, 1u, &second);
    assert(command_journal_bounded_slot_select(&core, 5u, &selection) == TR2_OK);
    assert(selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_CORRUPTED);
}


static void test_equal_generation_identical_content_is_valid(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedSlotSelection selection;
    CommandJournalBoundedRecord record = make_record(11u, 41u, 50u);

    init_core(&media, &persistent_media, &core);
    write_record(&media, 6u, 0u, &record);
    write_record(&media, 6u, 1u, &record);
    assert(command_journal_bounded_slot_select(&core, 6u, &selection) == TR2_OK);
    assert(selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_VALID);
    assert(selection.has_record);
    assert(selection.current_copy == 0u);
}

static void test_valid_copy_masks_unsupported_peer(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedSlotSelection selection;
    CommandJournalBoundedRecord record = make_record(12u, 51u, 60u);
    uint32_t offset;

    init_core(&media, &persistent_media, &core);
    write_record(&media, 7u, 0u, &record);
    write_record(&media, 7u, 1u, &record);
    assert(command_journal_bounded_slot_offset(7u, 1u, &offset) == TR2_OK);
    media.bytes[offset + 4u] = 0u;
    media.bytes[offset + 5u] = 2u;
    {
        uint32_t crc = UINT32_C(0xFFFFFFFF);
        size_t i;
        size_t bit;
        for (i = 0u; i < 66u; ++i) {
            crc ^= media.bytes[offset + i];
            for (bit = 0u; bit < 8u; ++bit) {
                crc = (crc >> 1u) ^ ((crc & 1u) ? UINT32_C(0xEDB88320) : 0u);
            }
        }
        crc = ~crc;
        media.bytes[offset + 66u] = (uint8_t)(crc >> 24u);
        media.bytes[offset + 67u] = (uint8_t)(crc >> 16u);
        media.bytes[offset + 68u] = (uint8_t)(crc >> 8u);
        media.bytes[offset + 69u] = (uint8_t)crc;
    }

    assert(command_journal_bounded_slot_select(&core, 7u, &selection) == TR2_OK);
    assert(selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_VALID);
    assert(selection.current_copy == 0u);
}

static void test_unsupported_without_valid_copy_is_unsupported(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedSlotSelection selection;
    CommandJournalBoundedRecord record = make_record(13u, 61u, 70u);
    uint32_t offset;

    init_core(&media, &persistent_media, &core);
    write_record(&media, 8u, 0u, &record);
    assert(command_journal_bounded_slot_offset(8u, 0u, &offset) == TR2_OK);
    media.bytes[offset + 4u] = 0u;
    media.bytes[offset + 5u] = 2u;
    {
        uint32_t crc = UINT32_C(0xFFFFFFFF);
        size_t i;
        size_t bit;
        for (i = 0u; i < 66u; ++i) {
            crc ^= media.bytes[offset + i];
            for (bit = 0u; bit < 8u; ++bit) {
                crc = (crc >> 1u) ^ ((crc & 1u) ? UINT32_C(0xEDB88320) : 0u);
            }
        }
        crc = ~crc;
        media.bytes[offset + 66u] = (uint8_t)(crc >> 24u);
        media.bytes[offset + 67u] = (uint8_t)(crc >> 16u);
        media.bytes[offset + 68u] = (uint8_t)(crc >> 8u);
        media.bytes[offset + 69u] = (uint8_t)crc;
    }

    assert(command_journal_bounded_slot_select(&core, 8u, &selection) == TR2_OK);
    assert(selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_UNSUPPORTED);
}

static void test_read_failure_is_unavailable(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalBoundedSlotSelection selection;

    init_core(&media, &persistent_media, &core);
    media.fail_read = true;
    assert(command_journal_bounded_slot_select(&core, 0u, &selection) == TR2_OK);
    assert(selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_UNAVAILABLE);
}

int main(void)
{
    test_offsets();
    test_empty_and_single_valid_copy();
    test_newer_generation_wins();
    test_corrupted_new_copy_falls_back_to_valid_old_copy();
    test_equal_generation_different_content_is_corrupted();
    test_equal_generation_identical_content_is_valid();
    test_valid_copy_masks_unsupported_peer();
    test_unsupported_without_valid_copy_is_unsupported();
    test_read_failure_is_unavailable();
    return 0;
}
