#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/command_software_reset.h"
#include "tr2/persistence/boot_intent_store.h"
#include "tr2/persistence/persistent_storage_core.h"
#include "tr2/platform/reset_trigger.h"

typedef struct {
    uint8_t bytes[64];
    uint32_t read_calls;
    uint32_t write_calls;
    uint32_t commit_calls;
    Tr2Result read_result;
    Tr2Result write_result;
    Tr2Result commit_result;
} FakeMediaContext;

typedef struct {
    uint8_t durable[64];
    uint8_t staged[64];
    uint32_t commit_calls;
    uint32_t fail_commit_call;
} PowerLossMediaContext;

typedef struct {
    uint32_t calls;
    Tr2Result result;
} FakeResetContext;

static Tr2Result fake_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    FakeMediaContext *fake = (FakeMediaContext *)context;

    assert(fake != NULL);
    fake->read_calls += 1u;
    if (fake->read_result != TR2_OK) {
        return fake->read_result;
    }
    if ((size_t)offset + size > sizeof(fake->bytes)) {
        return TR2_ERROR_STORAGE;
    }

    memcpy(buffer, &fake->bytes[offset], size);
    return TR2_OK;
}

static Tr2Result fake_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    FakeMediaContext *fake = (FakeMediaContext *)context;

    assert(fake != NULL);
    fake->write_calls += 1u;
    if (fake->write_result != TR2_OK) {
        return fake->write_result;
    }
    if ((size_t)offset + size > sizeof(fake->bytes)) {
        return TR2_ERROR_STORAGE;
    }

    memcpy(&fake->bytes[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result fake_commit(void *context)
{
    FakeMediaContext *fake = (FakeMediaContext *)context;

    assert(fake != NULL);
    fake->commit_calls += 1u;
    return fake->commit_result;
}

static Tr2Result power_loss_read(void *context,
                                 uint32_t offset,
                                 void *buffer,
                                 size_t size)
{
    PowerLossMediaContext *media = (PowerLossMediaContext *)context;

    if (media == NULL || buffer == NULL ||
        (size_t)offset + size > sizeof(media->durable)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memcpy(buffer, &media->durable[offset], size);
    return TR2_OK;
}

static Tr2Result power_loss_write(void *context,
                                  uint32_t offset,
                                  const void *buffer,
                                  size_t size)
{
    PowerLossMediaContext *media = (PowerLossMediaContext *)context;

    if (media == NULL || buffer == NULL ||
        (size_t)offset + size > sizeof(media->staged)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memcpy(&media->staged[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result power_loss_commit(void *context)
{
    PowerLossMediaContext *media = (PowerLossMediaContext *)context;

    if (media == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    ++media->commit_calls;
    if (media->fail_commit_call != 0u &&
        media->commit_calls == media->fail_commit_call) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(media->durable, media->staged, sizeof(media->durable));
    return TR2_OK;
}

static void power_loss_media_init(PowerLossMediaContext *media)
{
    memset(media, 0, sizeof(*media));
    memset(media->durable, 0xFF, sizeof(media->durable));
    memcpy(media->staged, media->durable, sizeof(media->staged));
}

static void power_loss_media_reboot(PowerLossMediaContext *media)
{
    memcpy(media->staged, media->durable, sizeof(media->staged));
    media->fail_commit_call = 0u;
}

static Tr2Result fake_software_reset(void *context)
{
    FakeResetContext *fake = (FakeResetContext *)context;
    assert(fake != NULL);
    fake->calls += 1u;
    return fake->result;
}

static CommandJournalEntry started_software_reset_entry(uint16_t transaction_id)
{
    CommandJournalEntry entry;

    memset(&entry, 0, sizeof(entry));
    entry.transaction_id = transaction_id;
    entry.request_identity.command_code = COMMAND_CODE_SOFTWARE_RESET;
    entry.request_identity.confirm_key = TR2_COMMAND_CONFIRM_KEY_VALID;
    entry.lifecycle = COMMAND_LIFECYCLE_STARTED;
    entry.has_recovery_context = true;
    entry.recovery_context.kind = COMMAND_RECOVERY_CONTEXT_BOOT_INTENT;
    entry.recovery_context.value1 = transaction_id;
    return entry;
}

static void test_boot_intent_store(void)
{
    PersistentStorageCore core = {0};
    FakeMediaContext fake = {{0}, 0u, 0u, 0u, TR2_OK, TR2_OK, TR2_OK};
    PersistentMedia media = {&fake, fake_read, fake_write, fake_commit};
    BootIntentStore store;
    BootIntentRecoveryResult recovery;
    BootIntent intent = boot_intent_software_reset(42u);

    assert(persistent_storage_core_init(&core, &media) == TR2_OK);
    assert(boot_intent_store_init(&store, &core, 16u) == TR2_OK);
    assert(boot_intent_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == BOOT_INTENT_RECOVERY_EMPTY);

    assert(boot_intent_store_commit(&store, &intent) == TR2_OK);
    assert(boot_intent_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == BOOT_INTENT_RECOVERY_VALID);
    assert(recovery.intent.kind == BOOT_INTENT_SOFTWARE_RESET);
    assert(recovery.intent.transaction_id == 42u);

    fake.bytes[25] ^= UINT8_C(0x01);
    assert(boot_intent_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == BOOT_INTENT_RECOVERY_CORRUPTED);

    memset(&fake.bytes[16], 0, TR2_BOOT_INTENT_RECORD_SIZE);
    assert(boot_intent_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == BOOT_INTENT_RECOVERY_EMPTY);

    assert(boot_intent_store_commit(&store, &intent) == TR2_OK);
    assert(boot_intent_store_clear(&store) == TR2_OK);
    assert(boot_intent_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == BOOT_INTENT_RECOVERY_EMPTY);
}

static void test_reset_trigger_contract(void)
{
    FakeResetContext state = {0u, TR2_OK};
    PlatformResetTrigger trigger = {&state, fake_software_reset};

    assert(platform_reset_trigger_is_valid(&trigger));
    assert(trigger.software_reset(trigger.context) == TR2_OK);
    assert(state.calls == 1u);
    state.result = TR2_ERROR_INTERNAL;
    assert(trigger.software_reset(trigger.context) == TR2_ERROR_INTERNAL);
    assert(state.calls == 2u);
}

static void test_p9k_boot_intent_commit_cut_recovers_empty(void)
{
    PowerLossMediaContext media;
    PersistentMedia backend;
    PersistentStorageCore storage;
    PersistentStorageCore reboot_storage;
    BootIntentStore store;
    BootIntentStore reboot_store;
    BootIntentRecoveryResult recovery;
    BootIntent intent = boot_intent_software_reset(51u);
    CommandJournalEntry entry = started_software_reset_entry(51u);

    power_loss_media_init(&media);
    backend.context = &media;
    backend.read = power_loss_read;
    backend.write = power_loss_write;
    backend.commit = power_loss_commit;
    assert(persistent_storage_core_init(&storage, &backend) == TR2_OK);
    assert(boot_intent_store_init(&store, &storage, 16u) == TR2_OK);

    media.fail_commit_call = 1u;
    assert(boot_intent_store_commit(&store, &intent) == TR2_ERROR_STORAGE);
    assert(boot_intent_store_recovery_required(&store));

    power_loss_media_reboot(&media);
    assert(persistent_storage_core_init(&reboot_storage, &backend) == TR2_OK);
    assert(boot_intent_store_init(&reboot_store, &reboot_storage, 16u) == TR2_OK);
    assert(boot_intent_store_recover(&reboot_store, &recovery) == TR2_OK);
    assert(recovery.status == BOOT_INTENT_RECOVERY_EMPTY);
    assert(command_software_reset_reconcile(&entry,
                                            &recovery,
                                            RESET_CAUSE_SOFTWARE) ==
           COMMAND_RECONCILIATION_INDETERMINATE);
}

static void test_p9k_durable_intent_power_on_never_proves_software_reset(void)
{
    PowerLossMediaContext media;
    PersistentMedia backend;
    PersistentStorageCore storage;
    PersistentStorageCore reboot_storage;
    BootIntentStore store;
    BootIntentStore reboot_store;
    BootIntentRecoveryResult recovery;
    BootIntent intent = boot_intent_software_reset(52u);
    CommandJournalEntry entry = started_software_reset_entry(52u);

    power_loss_media_init(&media);
    backend.context = &media;
    backend.read = power_loss_read;
    backend.write = power_loss_write;
    backend.commit = power_loss_commit;
    assert(persistent_storage_core_init(&storage, &backend) == TR2_OK);
    assert(boot_intent_store_init(&store, &storage, 16u) == TR2_OK);
    assert(boot_intent_store_commit(&store, &intent) == TR2_OK);

    power_loss_media_reboot(&media);
    assert(persistent_storage_core_init(&reboot_storage, &backend) == TR2_OK);
    assert(boot_intent_store_init(&reboot_store, &reboot_storage, 16u) == TR2_OK);
    assert(boot_intent_store_recover(&reboot_store, &recovery) == TR2_OK);
    assert(recovery.status == BOOT_INTENT_RECOVERY_VALID);
    assert(recovery.intent.transaction_id == 52u);

    assert(command_software_reset_reconcile(&entry,
                                            &recovery,
                                            RESET_CAUSE_POWER_ON) ==
           COMMAND_RECONCILIATION_INDETERMINATE);
    assert(command_software_reset_reconcile(&entry,
                                            &recovery,
                                            RESET_CAUSE_BROWNOUT) ==
           COMMAND_RECONCILIATION_INDETERMINATE);
    assert(command_software_reset_reconcile(&entry,
                                            &recovery,
                                            RESET_CAUSE_WATCHDOG) ==
           COMMAND_RECONCILIATION_INDETERMINATE);
    assert(command_software_reset_reconcile(&entry,
                                            &recovery,
                                            RESET_CAUSE_SOFTWARE) ==
           COMMAND_RECONCILIATION_TERMINAL_EFFECT_PROVEN);
}

static void test_p9k_corrupt_or_mismatched_intent_never_proves_reset(void)
{
    PowerLossMediaContext media;
    PersistentMedia backend;
    PersistentStorageCore storage;
    BootIntentStore store;
    BootIntentRecoveryResult recovery;
    BootIntent intent = boot_intent_software_reset(53u);
    CommandJournalEntry entry = started_software_reset_entry(53u);

    power_loss_media_init(&media);
    backend.context = &media;
    backend.read = power_loss_read;
    backend.write = power_loss_write;
    backend.commit = power_loss_commit;
    assert(persistent_storage_core_init(&storage, &backend) == TR2_OK);
    assert(boot_intent_store_init(&store, &storage, 16u) == TR2_OK);
    assert(boot_intent_store_commit(&store, &intent) == TR2_OK);

    media.durable[25] ^= UINT8_C(0x01);
    power_loss_media_reboot(&media);
    assert(boot_intent_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == BOOT_INTENT_RECOVERY_CORRUPTED);
    assert(command_software_reset_reconcile(&entry,
                                            &recovery,
                                            RESET_CAUSE_SOFTWARE) ==
           COMMAND_RECONCILIATION_INDETERMINATE);

    recovery.status = BOOT_INTENT_RECOVERY_VALID;
    recovery.intent = boot_intent_software_reset(54u);
    assert(command_software_reset_reconcile(&entry,
                                            &recovery,
                                            RESET_CAUSE_SOFTWARE) ==
           COMMAND_RECONCILIATION_INDETERMINATE);
}

static void test_p9k_reserved_journal_dominates_stale_boot_intent(void)
{
    BootIntentRecoveryResult recovery;
    CommandJournalEntry entry;

    memset(&entry, 0, sizeof(entry));
    entry.transaction_id = 55u;
    entry.request_identity.command_code = COMMAND_CODE_SOFTWARE_RESET;
    entry.request_identity.confirm_key = TR2_COMMAND_CONFIRM_KEY_VALID;
    entry.lifecycle = COMMAND_LIFECYCLE_RESERVED;
    entry.has_recovery_context = true;
    entry.recovery_context.kind = COMMAND_RECOVERY_CONTEXT_BOOT_INTENT;
    entry.recovery_context.value1 = 55u;

    recovery.status = BOOT_INTENT_RECOVERY_VALID;
    recovery.intent = boot_intent_software_reset(55u);

    assert(command_software_reset_reconcile(&entry,
                                            &recovery,
                                            RESET_CAUSE_SOFTWARE) ==
           COMMAND_RECONCILIATION_INDETERMINATE);
}

int main(void)
{
    PersistentStorageCore core = { 0 };
    FakeMediaContext fake = { { 0 }, 0u, 0u, 0u, TR2_OK, TR2_OK, TR2_OK };
    PersistentMedia media = { &fake, fake_read, fake_write, fake_commit };
    PersistentMedia incomplete_media = { &fake, fake_read, fake_write, NULL };
    uint8_t write_data[4] = { 1u, 2u, 3u, 4u };
    uint8_t read_data[4] = { 0 };

    assert(!persistent_storage_core_is_initialized(&core));
    assert(persistent_storage_core_read(&core, 0u, read_data, sizeof(read_data)) == TR2_ERROR_INVALID_STATE);
    assert(persistent_storage_core_write(&core, 0u, write_data, sizeof(write_data)) == TR2_ERROR_INVALID_STATE);
    assert(persistent_storage_core_commit(&core) == TR2_ERROR_INVALID_STATE);

    assert(persistent_storage_core_init(NULL, &media) == TR2_ERROR_INVALID_ARGUMENT);
    assert(persistent_storage_core_init(&core, NULL) == TR2_ERROR_INVALID_ARGUMENT);
    assert(persistent_storage_core_init(&core, &incomplete_media) == TR2_ERROR_INVALID_ARGUMENT);

    assert(persistent_storage_core_init(&core, &media) == TR2_OK);
    assert(persistent_storage_core_is_initialized(&core));

    assert(persistent_storage_core_write(&core, 8u, write_data, sizeof(write_data)) == TR2_OK);
    assert(fake.write_calls == 1u);
    assert(fake.commit_calls == 0u);

    assert(persistent_storage_core_read(&core, 8u, read_data, sizeof(read_data)) == TR2_OK);
    assert(fake.read_calls == 1u);
    assert(memcmp(write_data, read_data, sizeof(write_data)) == 0);
    assert(fake.commit_calls == 0u);

    assert(persistent_storage_core_commit(&core) == TR2_OK);
    assert(fake.commit_calls == 1u);

    assert(persistent_storage_core_write(&core, 0u, NULL, 1u) == TR2_ERROR_INVALID_ARGUMENT);
    assert(persistent_storage_core_read(&core, 0u, NULL, 1u) == TR2_ERROR_INVALID_ARGUMENT);
    assert(persistent_storage_core_write(&core, 0u, NULL, 0u) == TR2_OK);
    assert(persistent_storage_core_read(&core, 0u, NULL, 0u) == TR2_OK);
    assert(fake.write_calls == 1u);
    assert(fake.read_calls == 1u);

    fake.write_result = TR2_ERROR_UNAVAILABLE;
    assert(persistent_storage_core_write(&core, 0u, write_data, sizeof(write_data)) == TR2_ERROR_UNAVAILABLE);
    assert(fake.write_calls == 2u);

    fake.read_result = TR2_ERROR_CORRUPTED;
    assert(persistent_storage_core_read(&core, 0u, read_data, sizeof(read_data)) == TR2_ERROR_CORRUPTED);
    assert(fake.read_calls == 2u);

    fake.commit_result = TR2_ERROR_STORAGE;
    assert(persistent_storage_core_commit(&core) == TR2_ERROR_STORAGE);
    assert(fake.commit_calls == 2u);

    test_boot_intent_store();
    test_reset_trigger_contract();
    test_p9k_boot_intent_commit_cut_recovers_empty();
    test_p9k_durable_intent_power_on_never_proves_software_reset();
    test_p9k_corrupt_or_mismatched_intent_never_proves_reset();
    test_p9k_reserved_journal_dominates_stale_boot_intent();
    return 0;
}
