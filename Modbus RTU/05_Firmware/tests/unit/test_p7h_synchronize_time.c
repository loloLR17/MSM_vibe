#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/command_synchronize_time.h"

#define FAKE_CAPACITY 2u

typedef struct {
    CommandJournalEntry entries[FAKE_CAPACITY];
    size_t count;
    uint32_t next_completion_order;
} FakeJournalContext;

typedef struct {
    Tr2CivilTimestamp current_time;
    WallClockReadResult read_result;
    Tr2Result set_result;
    uint32_t set_calls;
} TestWallClock;

typedef struct {
    MonotonicTimeMs now_ms;
} TestMonotonicClock;

typedef struct {
    uint8_t durable[64u];
    uint8_t staged[64u];
} TestMedia;

static CommandJournalEntry *find_stored(FakeJournalContext *fake, uint16_t transaction_id)
{
    size_t index;
    for (index = 0u; index < fake->count; ++index) {
        if (fake->entries[index].transaction_id == transaction_id) {
            return &fake->entries[index];
        }
    }
    return NULL;
}

static Tr2Result fake_find(void *context, uint16_t transaction_id, CommandJournalEntry *entry)
{
    CommandJournalEntry *stored = find_stored((FakeJournalContext *)context, transaction_id);
    if (stored == NULL) {
        return TR2_ERROR_NOT_FOUND;
    }
    *entry = *stored;
    return TR2_OK;
}

static Tr2Result fake_reserve(void *context, const CommandRequest *request, CommandJournalEntry *entry)
{
    FakeJournalContext *fake = (FakeJournalContext *)context;
    CommandJournalEntry *stored;
    if (fake->count >= FAKE_CAPACITY) {
        return TR2_ERROR_STORAGE;
    }
    stored = &fake->entries[fake->count++];
    memset(stored, 0, sizeof(*stored));
    stored->transaction_id = request->transaction_id;
    stored->request_identity = request->identity;
    stored->lifecycle = COMMAND_LIFECYCLE_RESERVED;
    *entry = *stored;
    return TR2_OK;
}

static Tr2Result fake_set_context(void *context, uint16_t transaction_id,
                                  const CommandRecoveryContext *recovery_context,
                                  CommandJournalEntry *entry)
{
    CommandJournalEntry *stored = find_stored((FakeJournalContext *)context, transaction_id);
    if (stored == NULL || stored->lifecycle != COMMAND_LIFECYCLE_RESERVED) {
        return TR2_ERROR_INVALID_STATE;
    }
    stored->has_recovery_context = true;
    stored->recovery_context = *recovery_context;
    *entry = *stored;
    return TR2_OK;
}

static Tr2Result fake_mark_started(void *context, uint16_t transaction_id, CommandJournalEntry *entry)
{
    CommandJournalEntry *stored = find_stored((FakeJournalContext *)context, transaction_id);
    if (stored == NULL || stored->lifecycle != COMMAND_LIFECYCLE_RESERVED) {
        return TR2_ERROR_INVALID_STATE;
    }
    stored->lifecycle = COMMAND_LIFECYCLE_STARTED;
    *entry = *stored;
    return TR2_OK;
}

static Tr2Result fake_complete(void *context, uint16_t transaction_id,
                               const CommandFinalResult *final_result,
                               const CommandTerminalTimestamp *terminal_timestamp,
                               CommandJournalEntry *entry)
{
    FakeJournalContext *fake = (FakeJournalContext *)context;
    CommandJournalEntry *stored = find_stored(fake, transaction_id);
    if (stored == NULL || stored->lifecycle == COMMAND_LIFECYCLE_COMPLETED) {
        return TR2_ERROR_INVALID_STATE;
    }
    stored->lifecycle = COMMAND_LIFECYCLE_COMPLETED;
    stored->has_final_result = true;
    stored->final_result = *final_result;
    stored->terminal_timestamp = *terminal_timestamp;
    stored->completion_order = fake->next_completion_order++;
    *entry = *stored;
    return TR2_OK;
}

static Tr2Result fake_latest_completed(void *context, CommandJournalEntry *entry)
{
    FakeJournalContext *fake = (FakeJournalContext *)context;
    size_t index;
    bool found = false;
    for (index = 0u; index < fake->count; ++index) {
        if (fake->entries[index].lifecycle == COMMAND_LIFECYCLE_COMPLETED &&
            (!found || fake->entries[index].completion_order > entry->completion_order)) {
            *entry = fake->entries[index];
            found = true;
        }
    }
    return found ? TR2_OK : TR2_ERROR_NOT_FOUND;
}

static WallClockReadResult wall_read(void *context, Tr2CivilTimestamp *timestamp)
{
    TestWallClock *wall = (TestWallClock *)context;
    *timestamp = wall->current_time;
    return wall->read_result;
}

static Tr2Result wall_set(void *context, Tr2CivilTimestamp timestamp)
{
    TestWallClock *wall = (TestWallClock *)context;
    wall->set_calls++;
    if (wall->set_result != TR2_OK) {
        return wall->set_result;
    }
    wall->current_time = timestamp;
    return TR2_OK;
}

static MonotonicTimeMs monotonic_now(void *context)
{
    return ((TestMonotonicClock *)context)->now_ms;
}

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    if ((size_t)offset + size > sizeof(media->durable)) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(buffer, &media->durable[offset], size);
    return TR2_OK;
}

static Tr2Result media_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    if ((size_t)offset + size > sizeof(media->staged)) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(&media->staged[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    TestMedia *media = (TestMedia *)context;
    memcpy(media->durable, media->staged, sizeof(media->durable));
    return TR2_OK;
}

static void init_journal(FakeJournalContext *fake, CommandJournal *journal)
{
    memset(fake, 0, sizeof(*fake));
    fake->next_completion_order = 1u;
    memset(journal, 0, sizeof(*journal));
    journal->context = fake;
    journal->find = fake_find;
    journal->reserve = fake_reserve;
    journal->set_recovery_context = fake_set_context;
    journal->mark_started = fake_mark_started;
    journal->complete = fake_complete;
    journal->latest_completed = fake_latest_completed;
}

static CommandRequest request(uint16_t transaction_id)
{
    CommandRequest value;
    memset(&value, 0, sizeof(value));
    value.transaction_id = transaction_id;
    value.identity.command_code = COMMAND_CODE_SYNCHRONIZE_TIME;
    return value;
}

static void init_time_service(TimeService *service, TestWallClock *wall,
                              TestMonotonicClock *monotonic, TestMedia *media,
                              WallClock *wall_clock, MonotonicClock *monotonic_clock,
                              PersistentMedia *persistent_media,
                              PersistentStorageCore *storage, TimeHistoryStore *history_store)
{
    TimeHistoryRecoveryResult recovery;
    memset(media, UINT8_C(0xFF), sizeof(*media));
    *wall_clock = (WallClock){ wall, wall_read, wall_set };
    *monotonic_clock = (MonotonicClock){ monotonic, monotonic_now };
    *persistent_media = (PersistentMedia){ media, media_read, media_write, media_commit };
    assert(persistent_storage_core_init(storage, persistent_media) == TR2_OK);
    assert(time_history_store_init(history_store, storage, UINT32_C(8)) == TR2_OK);
    assert(time_history_store_recover(history_store, &recovery) == TR2_OK);
    assert(time_service_init(service, wall_clock) == TR2_OK);
    assert(time_service_bind_synchronization_dependencies(service, monotonic_clock, history_store) == TR2_OK);
}

static void test_absent_prepared_time_is_refused_without_started(void)
{
    FakeJournalContext fake;
    CommandJournal journal;
    CommandEngine engine;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandRequest command = request(201u);
    CommandTerminalTimestamp timestamp = { false, 0u };
    TestWallClock wall = { 100u, WALL_CLOCK_OK, TR2_OK, 0u };
    TestMonotonicClock monotonic = { 1000u };
    TestMedia media;
    WallClock wall_clock;
    MonotonicClock monotonic_clock;
    PersistentMedia persistent_media;
    PersistentStorageCore storage;
    TimeHistoryStore history_store;
    TimeService time_service;
    CommandJournalStore journal_store;

    init_journal(&fake, &journal);
    assert(command_engine_init(&engine, &journal) == TR2_OK);
    assert(command_engine_admit(&engine, &command, &admission) == TR2_OK);
    memset(&journal_store, 0, sizeof(journal_store));
    init_time_service(&time_service, &wall, &monotonic, &media, &wall_clock,
                      &monotonic_clock, &persistent_media, &storage, &history_store);

    assert(command_synchronize_time_execute(&engine, &journal_store, &time_service, 201u, 7u,
                                            &timestamp, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(entry.final_result.status == COMMAND_STATUS_REFUSED);
    assert(entry.final_result.result_code == COMMAND_RESULT_PREPARED_TIME_ABSENT);
    assert(wall.set_calls == 0u);
}

static void test_success_and_reconciliation(void)
{
    FakeJournalContext fake;
    CommandJournal journal;
    CommandEngine engine;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandRequest command = request(202u);
    CommandTerminalTimestamp timestamp = { true, 1234u };
    TestWallClock wall = { 100u, WALL_CLOCK_OK, TR2_OK, 0u };
    TestMonotonicClock monotonic = { 1000u };
    TestMedia media;
    WallClock wall_clock;
    MonotonicClock monotonic_clock;
    PersistentMedia persistent_media;
    PersistentStorageCore storage;
    TimeHistoryStore history_store;
    TimeService time_service;
    CommandJournalStore journal_store;

    init_journal(&fake, &journal);
    assert(command_engine_init(&engine, &journal) == TR2_OK);
    assert(command_engine_admit(&engine, &command, &admission) == TR2_OK);
    memset(&journal_store, 0, sizeof(journal_store));
    journal_store.initialized = true;
    journal_store.journal = journal;
    journal_store.journal.set_recovery_context = fake_set_context;
    engine.journal = &journal_store.journal;
    init_time_service(&time_service, &wall, &monotonic, &media, &wall_clock,
                      &monotonic_clock, &persistent_media, &storage, &history_store);
    assert(time_service_prepare_time(&time_service, 2000u) == TR2_OK);

    assert(command_synchronize_time_execute(&engine, &journal_store, &time_service, 202u, 9u,
                                            &timestamp, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(entry.has_recovery_context);
    assert(entry.recovery_context.kind == COMMAND_RECOVERY_CONTEXT_TIME_SYNC);
    assert(entry.recovery_context.value1 == 2000u);
    assert(entry.recovery_context.value2 == 9u);
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(wall.current_time == 2000u);

    entry.lifecycle = COMMAND_LIFECYCLE_STARTED;
    entry.has_final_result = false;
    entry.completion_order = 0u;
    assert(command_synchronize_time_reconcile(&entry, &time_service) ==
           COMMAND_RECONCILIATION_TERMINAL_EFFECT_PROVEN);
}

int main(void)
{
    test_absent_prepared_time_is_refused_without_started();
    test_success_and_reconciliation();
    return 0;
}
