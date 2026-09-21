#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/command_journal_bounded_record.h"
#include "tr2/persistence/command_journal_bounded_slot.h"
#include "tr2/persistence/command_journal_bounded_store.h"

typedef enum {
    COMMIT_FAILURE_KEEP_WORKING = 0,
    COMMIT_FAILURE_ROLLBACK_WORKING
} CommitFailureMode;

typedef struct {
    uint8_t durable[TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE];
    uint8_t working[TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE];
    size_t tear_after;
    bool tear_enabled;
    bool fail_commit;
    CommitFailureMode commit_failure_mode;
} FaultMedia;

static void media_init(FaultMedia *media)
{
    memset(media->durable, 0xFF, sizeof(media->durable));
    memcpy(media->working, media->durable, sizeof(media->working));
    media->tear_after = 0u;
    media->tear_enabled = false;
    media->fail_commit = false;
    media->commit_failure_mode = COMMIT_FAILURE_KEEP_WORKING;
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
    if (media->fail_commit) {
        if (media->commit_failure_mode == COMMIT_FAILURE_ROLLBACK_WORKING) {
            memcpy(media->working, media->durable, sizeof(media->working));
        }
        return TR2_ERROR_STORAGE;
    }
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

static CommandRequest make_request(uint16_t id)
{
    CommandRequest request;
    memset(&request, 0, sizeof(request));
    request.transaction_id = id;
    request.identity.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    return request;
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

static void assert_empty_after_reboot(FaultMedia *media)
{
    PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandJournalEntry entry;

    power_cycle(media);
    boot_store(media, &pm, &core, &store, &recovery);
    assert(recovery.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_EMPTY);
    assert(recovery.known_transaction_count == 0u);
    assert(store.next_admission_order == 1u);
    assert(store.journal.find(store.journal.context, 100u, &entry) == TR2_ERROR_NOT_FOUND);
}

static void assert_reserved_after_reboot(FaultMedia *media)
{
    PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandJournalEntry entry;

    power_cycle(media);
    boot_store(media, &pm, &core, &store, &recovery);
    assert(recovery.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID);
    assert(recovery.known_transaction_count == 1u);
    assert(store.next_admission_order == 2u);
    assert(store.journal.find(store.journal.context, 100u, &entry) == TR2_OK);
    assert(entry.transaction_id == 100u);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);
}

static void test_torn_initial_admission_is_not_durable(void)
{
    const size_t cuts[] = {
        0u,
        1u,
        65u,
        66u,
        67u,
        68u,
        69u
    };
    size_t index;

    for (index = 0u; index < sizeof(cuts) / sizeof(cuts[0]); ++index) {
        FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
        CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
        CommandRequest request = make_request(100u);
        CommandJournalEntry entry;

        media_init(&media);
        boot_store(&media, &pm, &core, &store, &recovery);
        assert(recovery.status == COMMAND_JOURNAL_BOUNDED_RECOVERY_EMPTY);

        media.tear_enabled = true;
        media.tear_after = cuts[index];
        assert(store.journal.reserve(store.journal.context, &request, &entry) ==
               TR2_ERROR_STORAGE);
        assert(store.recovery_required);
        assert(store.next_admission_order == 1u);

        assert_empty_after_reboot(&media);
    }
}

static void test_successful_initial_admission_survives_reboot(void)
{
    FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRequest request = make_request(100u);
    CommandJournalEntry entry;

    media_init(&media);
    boot_store(&media, &pm, &core, &store, &recovery);
    assert(store.journal.reserve(store.journal.context, &request, &entry) == TR2_OK);
    assert(store.next_admission_order == 2u);
    assert_reserved_after_reboot(&media);
}

static void test_failed_commit_rollback_leaves_empty_after_reboot(void)
{
    FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRequest request = make_request(100u);
    CommandJournalEntry entry;

    media_init(&media);
    boot_store(&media, &pm, &core, &store, &recovery);
    media.fail_commit = true;
    media.commit_failure_mode = COMMIT_FAILURE_ROLLBACK_WORKING;
    assert(store.journal.reserve(store.journal.context, &request, &entry) ==
           TR2_ERROR_STORAGE);
    assert(store.recovery_required);
    assert(store.next_admission_order == 1u);
    assert_empty_after_reboot(&media);
}

static void test_failed_commit_readable_but_not_durable_leaves_empty_after_reboot(void)
{
    FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedStore store; CommandJournalBoundedRecoveryResult recovery;
    CommandRequest request = make_request(100u);
    CommandJournalEntry entry;

    media_init(&media);
    boot_store(&media, &pm, &core, &store, &recovery);
    media.fail_commit = true;
    media.commit_failure_mode = COMMIT_FAILURE_KEEP_WORKING;
    assert(store.journal.reserve(store.journal.context, &request, &entry) ==
           TR2_ERROR_STORAGE);
    assert(store.recovery_required);
    assert(store.next_admission_order == 1u);

    /* The failed write may still be readable before power loss, but runtime is closed. */
    assert(store.journal.find(store.journal.context, 100u, &entry) ==
           TR2_ERROR_INVALID_STATE);
    assert_empty_after_reboot(&media);
}

int main(void)
{
    test_torn_initial_admission_is_not_durable();
    test_successful_initial_admission_survives_reboot();
    test_failed_commit_rollback_leaves_empty_after_reboot();
    test_failed_commit_readable_but_not_durable_leaves_empty_after_reboot();
    return 0;
}
