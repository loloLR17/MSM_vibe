#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/command_stop_acquisition.h"

typedef struct {
    CommandJournalEntry entry;
    bool present;
    int step;
    int context_step;
    int started_step;
    int source_stop_step;
    int data_finish_step;
    int close_step;
    CampaignMetadata repository_metadata;
    bool repository_metadata_present;
} TestContext;

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    (void)context;
    (void)offset;
    if (buffer != NULL) {
        memset(buffer, 0, size);
    }
    return TR2_OK;
}

static Tr2Result media_write(void *context,
                             uint32_t offset,
                             const void *buffer,
                             size_t size)
{
    (void)context;
    (void)offset;
    (void)buffer;
    (void)size;
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    (void)context;
    return TR2_OK;
}

static Tr2Result journal_find(void *context, uint16_t transaction_id, CommandJournalEntry *entry)
{
    TestContext *test = context;
    if (!test->present || test->entry.transaction_id != transaction_id) {
        return TR2_ERROR_NOT_FOUND;
    }
    *entry = test->entry;
    return TR2_OK;
}

static Tr2Result journal_reserve(void *context, const CommandRequest *request, CommandJournalEntry *entry)
{
    TestContext *test = context;
    memset(&test->entry, 0, sizeof(test->entry));
    test->entry.transaction_id = request->transaction_id;
    test->entry.request_identity = request->identity;
    test->entry.lifecycle = COMMAND_LIFECYCLE_RESERVED;
    test->present = true;
    *entry = test->entry;
    return TR2_OK;
}

static Tr2Result journal_context(void *context, uint16_t transaction_id,
                                 const CommandRecoveryContext *recovery_context,
                                 CommandJournalEntry *entry)
{
    TestContext *test = context;
    assert(test->entry.transaction_id == transaction_id);
    test->context_step = ++test->step;
    test->entry.has_recovery_context = true;
    test->entry.recovery_context = *recovery_context;
    *entry = test->entry;
    return TR2_OK;
}

static Tr2Result journal_started(void *context, uint16_t transaction_id, CommandJournalEntry *entry)
{
    TestContext *test = context;
    assert(test->entry.transaction_id == transaction_id);
    test->started_step = ++test->step;
    test->entry.lifecycle = COMMAND_LIFECYCLE_STARTED;
    *entry = test->entry;
    return TR2_OK;
}

static Tr2Result journal_complete(void *context, uint16_t transaction_id,
                                  const CommandFinalResult *final_result,
                                  const CommandTerminalTimestamp *terminal_timestamp,
                                  CommandJournalEntry *entry)
{
    TestContext *test = context;
    assert(test->entry.transaction_id == transaction_id);
    test->entry.lifecycle = COMMAND_LIFECYCLE_COMPLETED;
    test->entry.has_final_result = true;
    test->entry.final_result = *final_result;
    test->entry.terminal_timestamp = *terminal_timestamp;
    test->entry.completion_order = 1u;
    *entry = test->entry;
    return TR2_OK;
}

static Tr2Result journal_latest(void *context, CommandJournalEntry *entry)
{
    TestContext *test = context;
    if (!test->present || test->entry.lifecycle != COMMAND_LIFECYCLE_COMPLETED) {
        return TR2_ERROR_NOT_FOUND;
    }
    *entry = test->entry;
    return TR2_OK;
}

static Tr2Result reserve_campaign(void *context, CampaignIdReservation *reservation)
{
    (void)context;
    reservation->valid = true;
    reservation->campaign_id = 77u;
    return TR2_OK;
}

static Tr2Result open_campaign(void *context, const CampaignMetadata *metadata)
{
    TestContext *test = context;
    test->repository_metadata = *metadata;
    test->repository_metadata_present = true;
    return TR2_OK;
}

static Tr2Result close_campaign(void *context, const CampaignMetadata *metadata)
{
    TestContext *test = context;
    test->close_step = ++test->step;
    test->repository_metadata = *metadata;
    test->repository_metadata_present = true;
    return TR2_OK;
}

static Tr2Result repo_recover(void *context, CampaignRepositoryRecoveryResult *result)
{
    (void)context;
    memset(result, 0, sizeof(*result));
    result->status = CAMPAIGN_REPOSITORY_RECOVERY_VALID;
    return TR2_OK;
}

static Tr2Result get_campaign(void *context, CampaignId campaign_id, CampaignMetadata *metadata)
{
    TestContext *test = context;
    if (!test->repository_metadata_present ||
        test->repository_metadata.campaign_id != campaign_id) {
        return TR2_ERROR_NOT_FOUND;
    }
    *metadata = test->repository_metadata;
    return TR2_OK;
}

static Tr2Result data_begin(void *context, CampaignId campaign_id)
{
    (void)context;
    return campaign_id == 77u ? TR2_OK : TR2_ERROR_INVALID_ARGUMENT;
}

static Tr2Result data_finish(void *context, CampaignId campaign_id)
{
    TestContext *test = context;
    if (campaign_id != 77u) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    test->data_finish_step = ++test->step;
    return TR2_OK;
}

static Tr2Result data_recover(void *context, CampaignId campaign_id,
                              CampaignDataRecoveryResult *result)
{
    (void)context;
    if (campaign_id != 77u) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memset(result, 0, sizeof(*result));
    result->status = CAMPAIGN_DATA_RECOVERY_VALID;
    result->durable_prefix_bytes = 123u;
    return TR2_OK;
}

static MonotonicTimeMs now_ms(void *context)
{
    (void)context;
    return 100u;
}

static Tr2Result source_configure(void *context, const VibrationSourceConfiguration *configuration)
{
    (void)context;
    (void)configuration;
    return TR2_OK;
}

static Tr2Result source_start(void *context)
{
    (void)context;
    return TR2_OK;
}

static Tr2Result source_read(void *context, VibrationSample *sample)
{
    (void)context;
    memset(sample, 0, sizeof(*sample));
    return TR2_OK;
}

static Tr2Result source_stop(void *context)
{
    TestContext *test = context;
    test->source_stop_step = ++test->step;
    return TR2_OK;
}

static void init_journal(TestContext *test, CommandJournal *journal, CommandEngine *engine)
{
    memset(journal, 0, sizeof(*journal));
    journal->context = test;
    journal->find = journal_find;
    journal->reserve = journal_reserve;
    journal->set_recovery_context = journal_context;
    journal->mark_started = journal_started;
    journal->complete = journal_complete;
    journal->latest_completed = journal_latest;
    assert(command_engine_init(engine, journal) == TR2_OK);
}

static void init_campaign_service(TestContext *test,
                                  ConfigurationService *configuration,
                                  AcquisitionService *acquisition,
                                  CampaignRepository *repository,
                                  CampaignDataStore *data_store,
                                  CampaignService *campaign_service,
                                  MonotonicClock *clock,
                                  VibrationSource *source)
{
    static PersistentMedia media;
    static PersistentStorageCore storage;
    static ConfigurationStore configuration_store;

    memset(&media, 0, sizeof(media));
    media.context = test;
    media.read = media_read;
    media.write = media_write;
    media.commit = media_commit;
    assert(persistent_storage_core_init(&storage, &media) == TR2_OK);
    assert(configuration_store_init(&configuration_store, &storage) == TR2_OK);
    assert(configuration_service_init(configuration, &configuration_store) == TR2_OK);
    configuration->has_active = true;
    configuration->active.generation = 3u;
    configuration->active.config_id = 4u;
    configuration->active.payload.sampling_frequency_hz = 1000u;
    configuration->active.payload.axes_enable_mask = 7u;
    configuration->active.payload.window_size_samples = 8u;

    memset(clock, 0, sizeof(*clock));
    clock->context = test;
    clock->now_ms = now_ms;
    memset(source, 0, sizeof(*source));
    source->context = test;
    source->configure = source_configure;
    source->start = source_start;
    source->read_sample = source_read;
    source->stop = source_stop;
    assert(acquisition_service_init(acquisition, configuration, clock, source) == TR2_OK);

    memset(repository, 0, sizeof(*repository));
    repository->context = test;
    repository->reserve_campaign_id = reserve_campaign;
    repository->open_campaign = open_campaign;
    repository->close_campaign = close_campaign;
    repository->get_campaign_by_id = get_campaign;
    repository->recover = repo_recover;

    memset(data_store, 0, sizeof(*data_store));
    data_store->context = test;
    data_store->begin_campaign = data_begin;
    data_store->finish_campaign = data_finish;
    data_store->recover_campaign = data_recover;

    assert(campaign_service_init(campaign_service, configuration, acquisition,
                                 repository, data_store) == TR2_OK);
    assert(campaign_service_start_reserved(campaign_service, 77u) == TR2_OK);
}

static void test_stop_orders_barrier_before_effect_and_reconciles(void)
{
    TestContext test = {0};
    CommandJournal journal;
    CommandEngine engine;
    CommandRequest request = {0};
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandTerminalTimestamp timestamp = { false, 0u };
    ConfigurationService configuration;
    AcquisitionService acquisition;
    CampaignRepository repository;
    CampaignDataStore data_store;
    CampaignService campaign_service;
    MonotonicClock clock;
    VibrationSource source;

    init_journal(&test, &journal, &engine);
    init_campaign_service(&test, &configuration, &acquisition, &repository,
                          &data_store, &campaign_service, &clock, &source);
    test.step = 0;
    test.context_step = 0;
    test.started_step = 0;
    test.source_stop_step = 0;
    test.data_finish_step = 0;
    test.close_step = 0;

    request.transaction_id = 401u;
    request.identity.command_code = COMMAND_CODE_STOP_ACQUISITION;
    assert(command_engine_admit(&engine, &request, &admission) == TR2_OK);
    assert(command_stop_acquisition_execute(&engine, &campaign_service, 401u,
                                            &timestamp, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(entry.has_recovery_context);
    assert(entry.recovery_context.kind == COMMAND_RECOVERY_CONTEXT_STOP_CAMPAIGN);
    assert(entry.recovery_context.value1 == 77u);
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(test.context_step < test.started_step);
    assert(test.started_step < test.source_stop_step);
    assert(test.source_stop_step < test.data_finish_step);
    assert(test.data_finish_step < test.close_step);
    assert(!campaign_service_campaign_open(&campaign_service));

    entry.lifecycle = COMMAND_LIFECYCLE_STARTED;
    entry.has_final_result = false;
    assert(command_stop_acquisition_reconcile(&entry, &repository) ==
           COMMAND_RECONCILIATION_TERMINAL_EFFECT_PROVEN);
    test.repository_metadata.lifecycle_state = CAMPAIGN_LIFECYCLE_OPEN;
    assert(command_stop_acquisition_reconcile(&entry, &repository) ==
           COMMAND_RECONCILIATION_ABSENCE_PROVEN);
    test.repository_metadata_present = false;
    assert(command_stop_acquisition_reconcile(&entry, &repository) ==
           COMMAND_RECONCILIATION_INDETERMINATE);
}

static void test_stop_without_campaign_is_terminal_refusal(void)
{
    TestContext test = {0};
    CommandJournal journal;
    CommandEngine engine;
    CommandRequest request = {0};
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandTerminalTimestamp timestamp = { false, 0u };
    ConfigurationService configuration;
    AcquisitionService acquisition;
    CampaignRepository repository;
    CampaignDataStore data_store;
    CampaignService campaign_service;
    MonotonicClock clock;
    VibrationSource source;

    init_journal(&test, &journal, &engine);
    init_campaign_service(&test, &configuration, &acquisition, &repository,
                          &data_store, &campaign_service, &clock, &source);
    campaign_service.campaign_open = false;
    campaign_service.acquisition_window_started = false;
    campaign_service.data_store_started = false;
    memset(&campaign_service.active_metadata, 0, sizeof(campaign_service.active_metadata));

    request.transaction_id = 402u;
    request.identity.command_code = COMMAND_CODE_STOP_ACQUISITION;
    assert(command_engine_admit(&engine, &request, &admission) == TR2_OK);
    assert(command_stop_acquisition_execute(&engine, &campaign_service, 402u,
                                            &timestamp, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(entry.final_result.status == COMMAND_STATUS_REFUSED);
    assert(entry.final_result.result_code == COMMAND_RESULT_ACQUISITION_NOT_ACTIVE);
    assert(test.context_step == 0);
    assert(test.started_step == 0);
}

int main(void)
{
    test_stop_orders_barrier_before_effect_and_reconciles();
    test_stop_without_campaign_is_terminal_refusal();
    return 0;
}
