#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

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

static CommandRequest make_request(void)
{
    CommandRequest request;
    memset(&request, 0, sizeof(request));
    request.transaction_id = 102u;
    request.identity.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    return request;
}

static void prepare_started(FaultMedia *media)
{
    PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRequest request = make_request();
    CommandRecoveryContext context;
    CommandJournalEntry entry;

    media_init(media);
    boot_store(media, &pm, &core, &store, &recovery);
    assert(store.journal.reserve(store.journal.context, &request, &entry) == TR2_OK);

    memset(&context, 0, sizeof(context));
    context.kind = COMMAND_RECOVERY_CONTEXT_CONFIGURATION;
    context.value1 = 0xABCDEF01u;
    assert(store.journal.set_recovery_context(
               store.journal.context, 102u, &context, &entry) == TR2_OK);
    assert(store.journal.mark_started(store.journal.context, 102u, &entry) == TR2_OK);
}

static void make_terminal(CommandFinalResult *result,
                          CommandTerminalTimestamp *timestamp)
{
    memset(result, 0, sizeof(*result));
    result->status = COMMAND_STATUS_SUCCESS;
    result->result_code = COMMAND_RESULT_SUCCESS;
    result->result_detail = 0x55AAu;

    memset(timestamp, 0, sizeof(*timestamp));
    timestamp->available = true;
    timestamp->value = 0x12345678u;
}

static void assert_started_after_reboot(FaultMedia *media)
{
    PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandJournalEntry entry;
    CommandJournalBoundedSlotSelection selection;

    power_cycle(media);
    boot_store(media, &pm, &core, &store, &recovery);
    assert(recovery.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID);
    assert(store.next_admission_order == 2u);
    assert(store.next_completion_order == 1u);
    assert(store.journal.find(store.journal.context, 102u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_STARTED);
    assert(entry.has_recovery_context);
    assert(!entry.has_final_result);
    assert(entry.completion_order == 0u);
    assert(command_journal_bounded_slot_select(&core, 0u, &selection) == TR2_OK);
    assert(selection.record.generation == 3u);
    assert(selection.record.admission_order == 1u);
}

static void assert_completed_after_reboot(FaultMedia *media)
{
    PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandJournalEntry entry;
    CommandJournalEntry latest;
    CommandJournalBoundedSlotSelection selection;

    power_cycle(media);
    boot_store(media, &pm, &core, &store, &recovery);
    assert(recovery.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID);
    assert(store.next_admission_order == 2u);
    assert(store.next_completion_order == 2u);

    assert(store.journal.find(store.journal.context, 102u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(entry.has_recovery_context);
    assert(entry.has_final_result);
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(entry.final_result.result_code == COMMAND_RESULT_SUCCESS);
    assert(entry.final_result.result_detail == 0x55AAu);
    assert(entry.terminal_timestamp.available);
    assert(entry.terminal_timestamp.value == 0x12345678u);
    assert(entry.completion_order == 1u);

    assert(store.journal.latest_completed(store.journal.context, &latest) == TR2_OK);
    assert(latest.transaction_id == 102u);
    assert(latest.completion_order == 1u);

    assert(command_journal_bounded_slot_select(&core, 0u, &selection) == TR2_OK);
    assert(selection.record.generation == 4u);
    assert(selection.record.admission_order == 1u);
}

static void test_torn_completion_keeps_started_copy_and_counter(void)
{
    const size_t cuts[] = {0u, 1u, 65u, 66u, 67u, 68u, 69u};
    size_t index;

    for (index = 0u; index < sizeof(cuts) / sizeof(cuts[0]); ++index) {
        FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
        CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
        CommandFinalResult result; CommandTerminalTimestamp timestamp;
        CommandJournalEntry entry;

        prepare_started(&media);
        boot_store(&media, &pm, &core, &store, &recovery);
        make_terminal(&result, &timestamp);

        media.tear_enabled = true;
        media.tear_after = cuts[index];
        assert(store.journal.complete(
                   store.journal.context, 102u, &result, &timestamp, &entry) ==
               TR2_ERROR_STORAGE);
        assert(store.recovery_required);
        assert(store.next_completion_order == 1u);

        assert_started_after_reboot(&media);
    }
}

static void test_committed_completion_survives_and_reconstructs_counter(void)
{
    FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandFinalResult result; CommandTerminalTimestamp timestamp;
    CommandJournalEntry entry;

    prepare_started(&media);
    boot_store(&media, &pm, &core, &store, &recovery);
    make_terminal(&result, &timestamp);

    assert(store.journal.complete(
               store.journal.context, 102u, &result, &timestamp, &entry) == TR2_OK);
    assert(store.next_completion_order == 2u);
    assert_completed_after_reboot(&media);
}

static void test_failed_commit_closes_runtime_and_old_durable_copy_wins(void)
{
    FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandFinalResult result; CommandTerminalTimestamp timestamp;
    CommandJournalEntry entry;

    prepare_started(&media);
    boot_store(&media, &pm, &core, &store, &recovery);
    make_terminal(&result, &timestamp);

    media.fail_commit = true;
    assert(store.journal.complete(
               store.journal.context, 102u, &result, &timestamp, &entry) ==
           TR2_ERROR_STORAGE);
    assert(store.recovery_required);
    assert(store.next_completion_order == 1u);
    assert(store.journal.find(store.journal.context, 102u, &entry) ==
           TR2_ERROR_INVALID_STATE);

    assert_started_after_reboot(&media);
}

int main(void)
{
    test_torn_completion_keeps_started_copy_and_counter();
    test_committed_completion_survives_and_reconstructs_counter();
    test_failed_commit_closes_runtime_and_old_durable_copy_wins();
    return 0;
}
