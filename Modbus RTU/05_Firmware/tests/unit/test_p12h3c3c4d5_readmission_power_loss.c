#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/command_journal_bounded_record.h"
#include "tr2/persistence/command_journal_bounded_slot.h"
#include "tr2/persistence/command_journal_bounded_store.h"

typedef struct {
    uint8_t durable[TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE];
    uint8_t working[TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE];
    size_t tear_after;
    bool tear_enabled;
    bool fail_commit;
} FaultMedia;

static void media_init(FaultMedia *media)
{
    memset(media->durable, 0xFF, sizeof(media->durable));
    memcpy(media->working, media->durable, sizeof(media->working));
    media->tear_after = 0u;
    media->tear_enabled = false;
    media->fail_commit = false;
}

static void power_cycle(FaultMedia *media)
{
    memcpy(media->working, media->durable, sizeof(media->working));
    media->tear_enabled = false;
    media->fail_commit = false;
}

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    FaultMedia *media = (FaultMedia *)context;
    if ((size_t)offset + size > sizeof(media->working)) return TR2_ERROR_STORAGE;
    memcpy(buffer, &media->working[offset], size);
    return TR2_OK;
}

static Tr2Result media_write(void *context, uint32_t offset,
                             const void *buffer, size_t size)
{
    FaultMedia *media = (FaultMedia *)context;
    size_t count = size;
    if ((size_t)offset + size > sizeof(media->working)) return TR2_ERROR_STORAGE;
    if (media->tear_enabled && media->tear_after < size) count = media->tear_after;
    if (count != 0u) memcpy(&media->working[offset], buffer, count);
    if (media->tear_enabled && media->tear_after < size) return TR2_ERROR_STORAGE;
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    FaultMedia *media = (FaultMedia *)context;
    if (media->fail_commit) return TR2_ERROR_STORAGE;
    memcpy(media->durable, media->working, sizeof(media->durable));
    return TR2_OK;
}

static void init_core(FaultMedia *media, PersistentMedia *pm, PersistentStorageCore *core)
{
    pm->context = media;
    pm->read = media_read;
    pm->write = media_write;
    pm->commit = media_commit;
    assert(persistent_storage_core_init(core, pm) == TR2_OK);
}

static void boot_store(FaultMedia *media, PersistentMedia *pm,
                       PersistentStorageCore *core,
                       CommandJournalBoundedStore *store,
                       CommandJournalBoundedRecoveryResult *recovery)
{
    init_core(media, pm, core);
    assert(command_journal_bounded_store_init(store, core) == TR2_OK);
    assert(command_journal_bounded_store_recover(store, recovery) == TR2_OK);
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
    record.entry.final_result.result_code = COMMAND_RESULT_SUCCESS;
    record.entry.completion_order = completion;
    return record;
}

static void write_durable_copy0(FaultMedia *media, size_t slot,
                                const CommandJournalBoundedRecord *record)
{
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE];
    uint32_t offset;
    assert(command_journal_bounded_slot_offset(slot, 0u, &offset) == TR2_OK);
    assert(tr2_command_journal_bounded_record_encode(record, bytes, sizeof(bytes)) == TR2_OK);
    memcpy(&media->durable[offset], bytes, sizeof(bytes));
    memcpy(&media->working[offset], bytes, sizeof(bytes));
}

static void prepare_full_store(FaultMedia *media)
{
    size_t slot;
    media_init(media);
    for (slot = 0u; slot < TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT; ++slot) {
        CommandJournalBoundedRecord record =
            make_completed((uint16_t)(slot + 1u),
                           (uint32_t)(slot + 2u),
                           (uint32_t)(slot + 1u));
        write_durable_copy0(media, slot, &record);
    }

    /* Slot 200 becomes the unique oldest terminal admission and eviction victim. */
    {
        CommandJournalBoundedRecord victim = make_completed(201u, 1u, 201u);
        write_durable_copy0(media, 200u, &victim);
    }
}

static CommandRequest make_new_request(void)
{
    CommandRequest request;
    memset(&request, 0, sizeof(request));
    request.transaction_id = 60000u;
    request.identity.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    return request;
}

static void assert_old_victim_after_reboot(FaultMedia *media)
{
    PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandJournalEntry entry;
    CommandJournalBoundedSlotSelection selection;

    power_cycle(media);
    boot_store(media, &pm, &core, &store, &recovery);
    assert(recovery.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID);
    assert(recovery.known_transaction_count == TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT);
    assert(store.next_admission_order == 258u);

    assert(store.journal.find(store.journal.context, 201u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(store.journal.find(store.journal.context, 60000u, &entry) == TR2_ERROR_NOT_FOUND);

    assert(command_journal_bounded_slot_select(&core, 200u, &selection) == TR2_OK);
    assert(selection.current_copy == 0u);
    assert(selection.record.generation == 1u);
    assert(selection.record.admission_order == 1u);
    assert(selection.record.entry.transaction_id == 201u);
}

static void assert_new_reserved_after_reboot(FaultMedia *media)
{
    PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandJournalEntry entry;
    CommandJournalBoundedSlotSelection selection;

    power_cycle(media);
    boot_store(media, &pm, &core, &store, &recovery);
    assert(recovery.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID);
    assert(recovery.known_transaction_count == TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT);
    assert(store.next_admission_order == 259u);

    assert(store.journal.find(store.journal.context, 201u, &entry) == TR2_ERROR_NOT_FOUND);
    assert(store.journal.find(store.journal.context, 60000u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);
    assert(entry.request_identity.command_code == COMMAND_CODE_APPLY_CONFIGURATION);

    assert(command_journal_bounded_slot_select(&core, 200u, &selection) == TR2_OK);
    assert(selection.current_copy == 1u);
    assert(selection.record.generation == 2u);
    assert(selection.record.admission_order == 258u);
    assert(selection.record.entry.transaction_id == 60000u);
}

static void test_torn_readmission_keeps_old_completed_victim(void)
{
    const size_t cuts[] = {0u, 1u, 65u, 66u, 67u, 68u, 69u};
    size_t index;

    for (index = 0u; index < sizeof(cuts) / sizeof(cuts[0]); ++index) {
        FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
        CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
        CommandRequest request = make_new_request();
        CommandJournalEntry entry;

        prepare_full_store(&media);
        boot_store(&media, &pm, &core, &store, &recovery);
        assert(store.next_admission_order == 258u);

        media.tear_enabled = true;
        media.tear_after = cuts[index];
        assert(store.journal.reserve(store.journal.context, &request, &entry) ==
               TR2_ERROR_STORAGE);
        assert(store.recovery_required);
        assert(store.next_admission_order == 258u);

        assert_old_victim_after_reboot(&media);
    }
}

static void test_committed_readmission_replaces_victim_and_reconstructs_counter(void)
{
    FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRequest request = make_new_request();
    CommandJournalEntry entry;

    prepare_full_store(&media);
    boot_store(&media, &pm, &core, &store, &recovery);
    assert(store.journal.reserve(store.journal.context, &request, &entry) == TR2_OK);
    assert(entry.transaction_id == 60000u);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);
    assert(store.next_admission_order == 259u);

    assert_new_reserved_after_reboot(&media);
}

static void test_failed_commit_closes_runtime_and_old_durable_victim_wins(void)
{
    FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRequest request = make_new_request();
    CommandJournalEntry entry;

    prepare_full_store(&media);
    boot_store(&media, &pm, &core, &store, &recovery);

    media.fail_commit = true;
    assert(store.journal.reserve(store.journal.context, &request, &entry) ==
           TR2_ERROR_STORAGE);
    assert(store.recovery_required);
    assert(store.next_admission_order == 258u);
    assert(store.journal.find(store.journal.context, 60000u, &entry) ==
           TR2_ERROR_INVALID_STATE);

    assert_old_victim_after_reboot(&media);
}

int main(void)
{
    test_torn_readmission_keeps_old_completed_victim();
    test_committed_readmission_replaces_victim_and_reconstructs_counter();
    test_failed_commit_closes_runtime_and_old_durable_victim_wins();
    return 0;
}
