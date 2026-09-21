#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/command_journal_bounded_admission.h"

typedef struct {
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE];
    bool fail_read;
} TestMedia;

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
    if ((size_t)offset + size > sizeof(media->bytes)) return TR2_ERROR_STORAGE;
    memcpy(&media->bytes[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    (void)context;
    return TR2_OK;
}

static void init_core(TestMedia *media, PersistentMedia *pm, PersistentStorageCore *core)
{
    memset(media, 0xFF, sizeof(*media));
    media->fail_read = false;
    pm->context = media;
    pm->read = media_read;
    pm->write = media_write;
    pm->commit = media_commit;
    assert(persistent_storage_core_init(core, pm) == TR2_OK);
}

static CommandRequestIdentity identity(uint16_t code, uint16_t param1)
{
    CommandRequestIdentity value;
    memset(&value, 0, sizeof(value));
    value.command_code = code;
    value.param1 = param1;
    return value;
}

static CommandJournalBoundedRecord record(uint16_t id, uint32_t admission,
                                          CommandLifecycleState lifecycle,
                                          uint32_t generation)
{
    CommandJournalBoundedRecord value;
    memset(&value, 0, sizeof(value));
    value.generation = generation;
    value.admission_order = admission;
    value.entry.transaction_id = id;
    value.entry.request_identity = identity(COMMAND_CODE_APPLY_CONFIGURATION, id);
    value.entry.lifecycle = lifecycle;
    if (lifecycle == COMMAND_LIFECYCLE_COMPLETED) {
        value.entry.has_final_result = true;
        value.entry.final_result.status = COMMAND_STATUS_SUCCESS;
        value.entry.completion_order = admission;
    }
    return value;
}

static void put_record(TestMedia *media, size_t slot,
                       const CommandJournalBoundedRecord *value)
{
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE];
    uint32_t offset;
    assert(command_journal_bounded_slot_offset(slot, 0u, &offset) == TR2_OK);
    assert(tr2_command_journal_bounded_record_encode(value, bytes, sizeof(bytes)) == TR2_OK);
    memcpy(&media->bytes[offset], bytes, sizeof(bytes));
}

static void fill_completed(TestMedia *media)
{
    size_t slot;
    for (slot = 0u; slot < TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT; ++slot) {
        CommandJournalBoundedRecord value =
            record((uint16_t)(slot + 1u), (uint32_t)(slot + 1u),
                   COMMAND_LIFECYCLE_COMPLETED, 1u);
        put_record(media, slot, &value);
    }
}

static void test_empty_chooses_slot_zero(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedAdmissionPlan plan;
    CommandRequestIdentity request = identity(COMMAND_CODE_SELFTEST, 1u);
    init_core(&media, &pm, &core);
    assert(command_journal_bounded_admission_plan(&core, 65535u, &request, &plan) == TR2_OK);
    assert(plan.kind == COMMAND_JOURNAL_BOUNDED_ADMISSION_ADMIT_EMPTY);
    assert(plan.has_logical_slot && plan.logical_slot == 0u);
    assert(!plan.has_current);
}

static void test_first_empty_is_deterministic(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedAdmissionPlan plan;
    CommandRequestIdentity request = identity(COMMAND_CODE_SELFTEST, 2u);
    CommandJournalBoundedRecord a = record(1u, 1u, COMMAND_LIFECYCLE_COMPLETED, 1u);
    CommandJournalBoundedRecord b = record(2u, 2u, COMMAND_LIFECYCLE_COMPLETED, 1u);
    init_core(&media, &pm, &core);
    put_record(&media, 0u, &a);
    put_record(&media, 2u, &b);
    assert(command_journal_bounded_admission_plan(&core, 500u, &request, &plan) == TR2_OK);
    assert(plan.kind == COMMAND_JOURNAL_BOUNDED_ADMISSION_ADMIT_EMPTY);
    assert(plan.logical_slot == 1u);
}

static void test_existing_retry_and_collision(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedAdmissionPlan plan;
    CommandJournalBoundedRecord existing =
        record(65535u, 10u, COMMAND_LIFECYCLE_COMPLETED, 4u);
    CommandRequestIdentity same = existing.entry.request_identity;
    CommandRequestIdentity different = same;
    different.param2 = 99u;
    init_core(&media, &pm, &core);
    put_record(&media, 37u, &existing);

    assert(command_journal_bounded_admission_plan(&core, 65535u, &same, &plan) == TR2_OK);
    assert(plan.kind == COMMAND_JOURNAL_BOUNDED_ADMISSION_RETRY_EXISTING);
    assert(plan.logical_slot == 37u && plan.has_current);

    assert(command_journal_bounded_admission_plan(&core, 65535u, &different, &plan) == TR2_OK);
    assert(plan.kind == COMMAND_JOURNAL_BOUNDED_ADMISSION_COLLISION_EXISTING);
    assert(plan.logical_slot == 37u && plan.has_current);
}

static void test_full_evicts_oldest_completed(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedAdmissionPlan plan;
    CommandRequestIdentity request = identity(COMMAND_CODE_SELFTEST, 3u);
    CommandJournalBoundedRecord oldest;
    init_core(&media, &pm, &core);
    fill_completed(&media);
    oldest = record(100u, 1u, COMMAND_LIFECYCLE_COMPLETED, 1u);
    put_record(&media, 200u, &oldest);
    assert(command_journal_bounded_admission_plan(&core, 60000u, &request, &plan) == TR2_OK);
    assert(plan.kind == COMMAND_JOURNAL_BOUNDED_ADMISSION_EVICT_COMPLETED);
    assert(plan.logical_slot == 200u);
    assert(plan.current.record.admission_order == 1u);
}

static void test_nonterminal_never_evicted(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedAdmissionPlan plan;
    CommandRequestIdentity request = identity(COMMAND_CODE_SELFTEST, 4u);
    CommandJournalBoundedRecord reserved;
    CommandJournalBoundedRecord started;
    init_core(&media, &pm, &core);
    fill_completed(&media);
    reserved = record(1000u, 1u, COMMAND_LIFECYCLE_RESERVED, 1u);
    started = record(1001u, 2u, COMMAND_LIFECYCLE_STARTED, 1u);
    put_record(&media, 0u, &reserved);
    put_record(&media, 1u, &started);
    assert(command_journal_bounded_admission_plan(&core, 60001u, &request, &plan) == TR2_OK);
    assert(plan.kind == COMMAND_JOURNAL_BOUNDED_ADMISSION_EVICT_COMPLETED);
    assert(plan.logical_slot == 2u);
}

static void test_generation_max_victim_is_skipped(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedAdmissionPlan plan;
    CommandRequestIdentity request = identity(COMMAND_CODE_SELFTEST, 5u);
    CommandJournalBoundedRecord immutable;
    init_core(&media, &pm, &core);
    fill_completed(&media);
    immutable = record(1u, 1u, COMMAND_LIFECYCLE_COMPLETED, UINT32_MAX);
    put_record(&media, 0u, &immutable);
    assert(command_journal_bounded_admission_plan(&core, 60002u, &request, &plan) == TR2_OK);
    assert(plan.kind == COMMAND_JOURNAL_BOUNDED_ADMISSION_EVICT_COMPLETED);
    assert(plan.logical_slot == 1u);
}

static void test_no_mutable_completed_means_no_capacity(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedAdmissionPlan plan;
    CommandRequestIdentity request = identity(COMMAND_CODE_SELFTEST, 6u);
    size_t slot;
    init_core(&media, &pm, &core);
    for (slot = 0u; slot < TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT; ++slot) {
        CommandJournalBoundedRecord value =
            record((uint16_t)(slot + 1u), (uint32_t)(slot + 1u),
                   COMMAND_LIFECYCLE_COMPLETED, UINT32_MAX);
        put_record(&media, slot, &value);
    }
    assert(command_journal_bounded_admission_plan(&core, 60003u, &request, &plan) == TR2_OK);
    assert(plan.kind == COMMAND_JOURNAL_BOUNDED_ADMISSION_NO_CAPACITY);
    assert(!plan.has_logical_slot && !plan.has_current);
}

static void test_invalid_id_and_media_failure(void)
{
    TestMedia media; PersistentMedia pm; PersistentStorageCore core;
    CommandJournalBoundedAdmissionPlan plan;
    CommandRequestIdentity request = identity(COMMAND_CODE_SELFTEST, 7u);
    init_core(&media, &pm, &core);
    assert(command_journal_bounded_admission_plan(&core, 0u, &request, &plan) ==
           TR2_ERROR_INVALID_ARGUMENT);
    media.fail_read = true;
    assert(command_journal_bounded_admission_plan(&core, 1u, &request, &plan) ==
           TR2_ERROR_UNAVAILABLE);
}

int main(void)
{
    test_empty_chooses_slot_zero();
    test_first_empty_is_deterministic();
    test_existing_retry_and_collision();
    test_full_evicts_oldest_completed();
    test_nonterminal_never_evicted();
    test_generation_max_victim_is_skipped();
    test_no_mutable_completed_means_no_capacity();
    test_invalid_id_and_media_failure();
    return 0;
}
