#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/campaign_service.h"

typedef struct {
    int step;
    int reserve_step;
    int open_step;
    int data_begin_step;
    int source_start_step;
    int source_stop_step;
    int data_finish_step;
    int close_step;
    Tr2Result source_start_result;
    CampaignMetadata opened;
    CampaignMetadata closed;
} TestContext;

static MonotonicTimeMs fake_now(void *context)
{
    (void)context;
    return UINT64_C(1000);
}

static Tr2Result fake_configure(void *context,
                                const VibrationSourceConfiguration *configuration)
{
    (void)context;
    (void)configuration;
    return TR2_OK;
}

static Tr2Result fake_source_start(void *context)
{
    TestContext *test = context;
    test->source_start_step = ++test->step;
    return test->source_start_result;
}

static Tr2Result fake_read(void *context, VibrationSample *sample)
{
    (void)context;
    memset(sample, 0, sizeof(*sample));
    return TR2_OK;
}

static Tr2Result fake_source_stop(void *context)
{
    TestContext *test = context;
    test->source_stop_step = ++test->step;
    return TR2_OK;
}

static Tr2Result fake_reserve(void *context, CampaignIdReservation *reservation)
{
    TestContext *test = context;
    test->reserve_step = ++test->step;
    reservation->campaign_id = 7u;
    reservation->valid = true;
    return TR2_OK;
}

static Tr2Result fake_open(void *context, const CampaignMetadata *metadata)
{
    TestContext *test = context;
    test->open_step = ++test->step;
    test->opened = *metadata;
    return TR2_OK;
}

static Tr2Result fake_close(void *context, const CampaignMetadata *metadata)
{
    TestContext *test = context;
    test->close_step = ++test->step;
    test->closed = *metadata;
    return TR2_OK;
}

static Tr2Result fake_repo_recover(void *context,
                                   CampaignRepositoryRecoveryResult *result)
{
    (void)context;
    memset(result, 0, sizeof(*result));
    result->status = CAMPAIGN_REPOSITORY_RECOVERY_EMPTY;
    return TR2_OK;
}

static Tr2Result fake_data_begin(void *context, CampaignId campaign_id)
{
    TestContext *test = context;
    assert(campaign_id == 7u);
    test->data_begin_step = ++test->step;
    return TR2_OK;
}

static Tr2Result fake_data_finish(void *context, CampaignId campaign_id)
{
    TestContext *test = context;
    assert(campaign_id == 7u);
    test->data_finish_step = ++test->step;
    return TR2_OK;
}

static Tr2Result fake_data_recover(void *context,
                                   CampaignId campaign_id,
                                   CampaignDataRecoveryResult *result)
{
    (void)context;
    assert(campaign_id == 7u);
    memset(result, 0, sizeof(*result));
    result->status = CAMPAIGN_DATA_RECOVERY_VALID;
    result->durable_prefix_bytes = UINT64_C(123);
    return TR2_OK;
}

static void build_dependencies(TestContext *test,
                               ConfigurationService *configuration,
                               AcquisitionService *acquisition,
                               CampaignRepository *repository,
                               CampaignDataStore *data_store,
                               MonotonicClock *clock,
                               VibrationSource *source)
{
    memset(configuration, 0, sizeof(*configuration));
    configuration->initialized = true;
    configuration->has_active = true;
    configuration->active.generation = 3u;
    configuration->active.config_id = 4u;
    configuration->active.revision_counter = 5u;
    configuration->active.payload.sampling_frequency_hz = 1000u;
    configuration->active.payload.axes_enable_mask = 7u;
    configuration->active.payload.window_size_samples = 8u;
    configuration->active.payload.mission_id = 42u;
    memcpy(configuration->active.payload.campaign_label, "campaign-A", 10u);
    memcpy(configuration->active.payload.mission_label, "mission-A", 9u);

    clock->context = test;
    clock->now_ms = fake_now;
    source->context = test;
    source->configure = fake_configure;
    source->start = fake_source_start;
    source->read_sample = fake_read;
    source->stop = fake_source_stop;
    assert(acquisition_service_init(acquisition,
                                    configuration,
                                    clock,
                                    source) == TR2_OK);

    memset(repository, 0, sizeof(*repository));
    repository->context = test;
    repository->reserve_campaign_id = fake_reserve;
    repository->open_campaign = fake_open;
    repository->close_campaign = fake_close;
    repository->recover = fake_repo_recover;

    memset(data_store, 0, sizeof(*data_store));
    data_store->context = test;
    data_store->begin_campaign = fake_data_begin;
    data_store->finish_campaign = fake_data_finish;
    data_store->recover_campaign = fake_data_recover;
}

static void test_start_stop_freezes_context_and_orders_durability(void)
{
    TestContext test = {0};
    ConfigurationService configuration;
    AcquisitionService acquisition;
    CampaignRepository repository;
    CampaignDataStore data_store;
    MonotonicClock clock;
    VibrationSource source;
    CampaignService service;
    CampaignId id;
    CampaignMetadata closed;

    test.source_start_result = TR2_OK;
    build_dependencies(&test,
                       &configuration,
                       &acquisition,
                       &repository,
                       &data_store,
                       &clock,
                       &source);
    assert(campaign_service_init(&service,
                                 &configuration,
                                 &acquisition,
                                 &repository,
                                 &data_store) == TR2_OK);

    assert(campaign_service_start(&service, &id) == TR2_OK);
    assert(id == 7u);
    assert(test.reserve_step < test.open_step);
    assert(test.open_step < test.data_begin_step);
    assert(test.data_begin_step < test.source_start_step);
    assert(test.opened.lifecycle_state == CAMPAIGN_LIFECYCLE_OPEN);
    assert(test.opened.mission_id == 42u);
    assert(test.opened.historical_context.configuration_generation == 3u);
    assert(test.opened.historical_context.configuration_id == 4u);
    assert(test.opened.historical_context.configuration_revision_counter == 5u);
    assert(!test.opened.start_timestamp.available);
    assert(campaign_service_campaign_open(&service));
    assert(campaign_service_acquisition_running(&service));

    configuration.active.payload.mission_id = 99u;
    configuration.active.generation = 99u;

    assert(campaign_service_stop(&service, &closed) == TR2_OK);
    assert(test.source_stop_step < test.data_finish_step);
    assert(test.data_finish_step < test.close_step);
    assert(closed.lifecycle_state == CAMPAIGN_LIFECYCLE_CLOSED);
    assert(closed.mission_id == 42u);
    assert(closed.historical_context.configuration_generation == 3u);
    assert(closed.durable_data_size_bytes == UINT64_C(123));
    assert(!closed.end_timestamp.available);
    assert(!closed.duration.available);
    assert(closed.data_integrity == CAMPAIGN_DATA_INTEGRITY_UNKNOWN);
    assert(!campaign_service_campaign_open(&service));
    assert(!campaign_service_acquisition_running(&service));
}

static void test_start_without_active_configuration_is_rejected(void)
{
    TestContext test = {0};
    ConfigurationService configuration;
    AcquisitionService acquisition;
    CampaignRepository repository;
    CampaignDataStore data_store;
    MonotonicClock clock;
    VibrationSource source;
    CampaignService service;
    CampaignId id = 99u;

    test.source_start_result = TR2_OK;
    build_dependencies(&test,
                       &configuration,
                       &acquisition,
                       &repository,
                       &data_store,
                       &clock,
                       &source);
    configuration.has_active = false;
    assert(campaign_service_init(&service,
                                 &configuration,
                                 &acquisition,
                                 &repository,
                                 &data_store) == TR2_OK);
    assert(campaign_service_start(&service, &id) == TR2_ERROR_NOT_AVAILABLE);
    assert(id == TR2_CAMPAIGN_ID_INVALID);
    assert(test.reserve_step == 0);
}

static void test_partial_start_remains_closable(void)
{
    TestContext test = {0};
    ConfigurationService configuration;
    AcquisitionService acquisition;
    CampaignRepository repository;
    CampaignDataStore data_store;
    MonotonicClock clock;
    VibrationSource source;
    CampaignService service;
    CampaignId id;
    CampaignMetadata closed;

    test.source_start_result = TR2_ERROR_UNAVAILABLE;
    build_dependencies(&test,
                       &configuration,
                       &acquisition,
                       &repository,
                       &data_store,
                       &clock,
                       &source);
    assert(campaign_service_init(&service,
                                 &configuration,
                                 &acquisition,
                                 &repository,
                                 &data_store) == TR2_OK);

    assert(campaign_service_start(&service, &id) == TR2_ERROR_UNAVAILABLE);
    assert(id == TR2_CAMPAIGN_ID_INVALID);
    assert(campaign_service_campaign_open(&service));
    assert(!campaign_service_acquisition_running(&service));
    assert(test.open_step != 0);
    assert(test.data_begin_step != 0);

    assert(campaign_service_stop(&service, &closed) == TR2_OK);
    assert(closed.campaign_id == 7u);
    assert(closed.lifecycle_state == CAMPAIGN_LIFECYCLE_CLOSED);
    assert(test.source_stop_step == 0);
    assert(test.data_finish_step != 0);
    assert(test.close_step != 0);
}

int main(void)
{
    test_start_stop_freezes_context_and_orders_durability();
    test_start_without_active_configuration_is_rejected();
    test_partial_start_remains_closable();
    return 0;
}
