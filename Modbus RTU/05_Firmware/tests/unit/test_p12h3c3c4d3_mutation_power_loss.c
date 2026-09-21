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
    request.transaction_id = 101u;
    request.identity.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    return request;
}

static void prepare_reserved(FaultMedia *media)
{
    PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRequest request = make_request();
    CommandJournalEntry entry;

    media_init(media);
    boot_store(media, &pm, &core, &store, &recovery);
    assert(store.journal.reserve(store.journal.context, &request, &entry) == TR2_OK);
}

static void assert_reserved_no_context_after_reboot(FaultMedia *media)
{
    PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandJournalEntry entry;
    CommandJournalBoundedSlotSelection selection;

    power_cycle(media);
    boot_store(media, &pm, &core, &store, &recovery);
    assert(recovery.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID);
    assert(recovery.known_transaction_count == 1u);
    assert(store.next_admission_order == 2u);
    assert(store.journal.find(store.journal.context, 101u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);
    assert(!entry.has_recovery_context);
    assert(command_journal_bounded_slot_select(&core, 0u, &selection) == TR2_OK);
    assert(selection.status == COMMAND_JOURNAL_BOUNDED_SLOT_VALID);
    assert(selection.record.generation == 1u);
    assert(selection.record.admission_order == 1u);
}

static void assert_reserved_with_context_after_reboot(FaultMedia *media)
{
    PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandJournalEntry entry;
    CommandJournalBoundedSlotSelection selection;

    power_cycle(media);
    boot_store(media, &pm, &core, &store, &recovery);
    assert(recovery.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID);
    assert(store.journal.find(store.journal.context, 101u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);
    assert(entry.has_recovery_context);
    assert(entry.recovery_context.kind == COMMAND_RECOVERY_CONTEXT_CONFIGURATION);
    assert(entry.recovery_context.value1 == 0x12345678u);
    assert(command_journal_bounded_slot_select(&core, 0u, &selection) == TR2_OK);
    assert(selection.record.generation == 2u);
    assert(selection.record.admission_order == 1u);
}

static void test_torn_recovery_context_keeps_previous_copy(void)
{
    const size_t cuts[] = {0u, 1u, 65u, 66u, 67u, 68u, 69u};
    size_t index;

    for (index = 0u; index < sizeof(cuts) / sizeof(cuts[0]); ++index) {
        FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
        CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
        CommandRecoveryContext context;
        CommandJournalEntry entry;

        prepare_reserved(&media);
        boot_store(&media, &pm, &core, &store, &recovery);
        memset(&context, 0, sizeof(context));
        context.kind = COMMAND_RECOVERY_CONTEXT_CONFIGURATION;
        context.value1 = 0x12345678u;

        media.tear_enabled = true;
        media.tear_after = cuts[index];
        assert(store.journal.set_recovery_context(
                   store.journal.context, 101u, &context, &entry) == TR2_ERROR_STORAGE);
        assert(store.recovery_required);
        assert_reserved_no_context_after_reboot(&media);
    }
}

static void test_committed_recovery_context_selects_new_copy(void)
{
    FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRecoveryContext context;
    CommandJournalEntry entry;

    prepare_reserved(&media);
    boot_store(&media, &pm, &core, &store, &recovery);
    memset(&context, 0, sizeof(context));
    context.kind = COMMAND_RECOVERY_CONTEXT_CONFIGURATION;
    context.value1 = 0x12345678u;
    assert(store.journal.set_recovery_context(
               store.journal.context, 101u, &context, &entry) == TR2_OK);
    assert_reserved_with_context_after_reboot(&media);
}

static void prepare_reserved_with_context(FaultMedia *media)
{
    PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRecoveryContext context;
    CommandJournalEntry entry;

    prepare_reserved(media);
    boot_store(media, &pm, &core, &store, &recovery);
    memset(&context, 0, sizeof(context));
    context.kind = COMMAND_RECOVERY_CONTEXT_CONFIGURATION;
    context.value1 = 0x12345678u;
    assert(store.journal.set_recovery_context(
               store.journal.context, 101u, &context, &entry) == TR2_OK);
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
    assert(store.journal.find(store.journal.context, 101u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_STARTED);
    assert(entry.has_recovery_context);
    assert(entry.recovery_context.value1 == 0x12345678u);
    assert(command_journal_bounded_slot_select(&core, 0u, &selection) == TR2_OK);
    assert(selection.record.generation == 3u);
    assert(selection.record.admission_order == 1u);
}

static void test_torn_mark_started_keeps_context_copy(void)
{
    const size_t cuts[] = {0u, 1u, 65u, 66u, 67u, 68u, 69u};
    size_t index;

    for (index = 0u; index < sizeof(cuts) / sizeof(cuts[0]); ++index) {
        FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
        CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
        CommandJournalEntry entry;

        prepare_reserved_with_context(&media);
        boot_store(&media, &pm, &core, &store, &recovery);

        media.tear_enabled = true;
        media.tear_after = cuts[index];
        assert(store.journal.mark_started(store.journal.context, 101u, &entry) ==
               TR2_ERROR_STORAGE);
        assert(store.recovery_required);
        assert_reserved_with_context_after_reboot(&media);
    }
}

static void test_committed_mark_started_selects_generation_three(void)
{
    FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandJournalEntry entry;

    prepare_reserved_with_context(&media);
    boot_store(&media, &pm, &core, &store, &recovery);
    assert(store.journal.mark_started(store.journal.context, 101u, &entry) == TR2_OK);
    assert_started_after_reboot(&media);
}

int main(void)
{
    test_torn_recovery_context_keeps_previous_copy();
    test_committed_recovery_context_selects_new_copy();
    test_torn_mark_started_keeps_context_copy();
    test_committed_mark_started_selects_generation_three();
    return 0;
}
