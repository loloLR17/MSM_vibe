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
    int checkpoint_step;
    int data_finish_step;
    int close_step;
    uint32_t source_read_calls;
    uint32_t data_append_calls;
    uint32_t data_checkpoint_calls;
    uint32_t data_recover_calls;
    size_t last_append_size;
    uint8_t appended_records[2][TR2_CAMPAIGN_SAMPLE_RECORD_SIZE];
    bool fail_data_recover_once;
    Tr2Result source_start_result;
    Tr2Result source_read_result;
    Tr2Result data_append_result;
    Tr2Result data_checkpoint_result;
    CampaignMetadata opened;
    CampaignMetadata closed;
    PersistentMedia media;
    PersistentStorageCore storage;
    ConfigurationStore configuration_store;
} TestContext;

static Tr2Result fake_media_read(void *context,
                                 uint32_t offset,
                                 void *buffer,
                                 size_t size)
{
    (void)context;
    (void)offset;
    if (buffer != NULL) {
        memset(buffer, 0, size);
    }
    return TR2_OK;
}

static Tr2Result fake_media_write(void *context,
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

static Tr2Result fake_media_commit(void *context)
{
    (void)context;
    return TR2_OK;
}

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
    TestContext *test = context;

    test->source_read_calls++;
    if (test->source_read_result != TR2_OK) {
        return test->source_read_result;
    }
    if (sample == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(sample, 0, sizeof(*sample));
    sample->x_mg = (int32_t)test->source_read_calls;
    sample->y_mg = -(int32_t)test->source_read_calls;
    sample->z_mg = (int32_t)(10u * test->source_read_calls);
    sample->valid = test->source_read_calls != 2u;
    sample->saturated = test->source_read_calls == 2u;
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

static Tr2Result fake_data_append(void *context,
                                  CampaignId campaign_id,
                                  const uint8_t *data,
                                  size_t size)
{
    TestContext *test = context;
    uint32_t index = test->data_append_calls;

    assert(campaign_id == 7u);
    assert(data != NULL);
    assert(size == TR2_CAMPAIGN_SAMPLE_RECORD_SIZE);
    test->data_append_calls++;
    test->last_append_size = size;
    if (index < 2u) {
        memcpy(test->appended_records[index], data, size);
    }
    return test->data_append_result;
}

static Tr2Result fake_data_checkpoint(void *context, CampaignId campaign_id)
{
    TestContext *test = context;
    assert(campaign_id == 7u);
    test->data_checkpoint_calls++;
    test->checkpoint_step = ++test->step;
    return test->data_checkpoint_result;
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
    TestContext *test = context;
    assert(campaign_id == 7u);
    test->data_recover_calls++;
    if (test->fail_data_recover_once) {
        test->fail_data_recover_once = false;
        return TR2_ERROR_STORAGE;
    }
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
    memset(&test->media, 0, sizeof(test->media));
    test->media.context = test;
    test->media.read = fake_media_read;
    test->media.write = fake_media_write;
    test->media.commit = fake_media_commit;
    assert(persistent_storage_core_init(&test->storage, &test->media) == TR2_OK);
    assert(configuration_store_init(&test->configuration_store,
                                    &test->storage) == TR2_OK);
    assert(configuration_service_init(configuration,
                                      &test->configuration_store) == TR2_OK);

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
    data_store->append = fake_data_append;
    data_store->checkpoint = fake_data_checkpoint;
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
    build_dependencies(&test, &configuration, &acquisition, &repository, &data_store, &clock, &source);
    assert(campaign_service_init(&service, &configuration, &acquisition, &repository, &data_store) == TR2_OK);

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
    assert(test.source_stop_step < test.checkpoint_step);
    assert(test.checkpoint_step < test.data_finish_step);
    assert(test.data_finish_step < test.close_step);
    assert(test.data_checkpoint_calls == 1u);
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

static void test_live_driver_appends_records_and_checkpoints_window(void)
{
    static const uint8_t expected_first[TR2_CAMPAIGN_SAMPLE_RECORD_SIZE] = {
        0x01u, 0x00u, 0x00u, 0x00u,
        0xFFu, 0xFFu, 0xFFu, 0xFFu,
        0x0Au, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u
    };
    static const uint8_t expected_second[TR2_CAMPAIGN_SAMPLE_RECORD_SIZE] = {
        0x02u, 0x00u, 0x00u, 0x00u,
        0xFEu, 0xFFu, 0xFFu, 0xFFu,
        0x14u, 0x00u, 0x00u, 0x00u,
        0x02u, 0x00u, 0x00u, 0x00u
    };
    TestContext test = {0};
    ConfigurationService configuration;
    AcquisitionService acquisition;
    CampaignRepository repository;
    CampaignDataStore data_store;
    MonotonicClock clock;
    VibrationSource source;
    CampaignService service;
    CampaignAcquisitionStep step;
    CampaignId id;
    CampaignMetadata closed;

    test.source_start_result = TR2_OK;
    test.source_read_result = TR2_OK;
    build_dependencies(&test, &configuration, &acquisition, &repository, &data_store, &clock, &source);
    configuration.active.payload.window_size_samples = 2u;
    assert(campaign_service_init(&service, &configuration, &acquisition, &repository, &data_store) == TR2_OK);
    assert(campaign_service_drive_acquisition_step(&service, &step) == TR2_ERROR_INVALID_STATE);
    assert(campaign_service_start(&service, &id) == TR2_OK);

    assert(campaign_service_drive_acquisition_step(&service, &step) == TR2_OK);
    assert(step.kind == CAMPAIGN_ACQUISITION_STEP_SAMPLE_READ);
    assert(step.storage_result == TR2_OK);
    assert(step.sample.x_mg == 1);
    assert(step.sample.y_mg == -1);
    assert(step.sample.z_mg == 10);
    assert(step.sample.valid);
    assert(!step.sample.saturated);
    assert(test.data_append_calls == 1u);
    assert(test.last_append_size == TR2_CAMPAIGN_SAMPLE_RECORD_SIZE);
    assert(memcmp(test.appended_records[0], expected_first, sizeof(expected_first)) == 0);
    assert(campaign_service_acquisition_running(&service));

    assert(campaign_service_drive_acquisition_step(&service, &step) == TR2_OK);
    assert(step.kind == CAMPAIGN_ACQUISITION_STEP_SAMPLE_READ);
    assert(step.storage_result == TR2_OK);
    assert(!step.sample.valid);
    assert(step.sample.saturated);
    assert(test.data_append_calls == 2u);
    assert(memcmp(test.appended_records[1], expected_second, sizeof(expected_second)) == 0);
    assert(campaign_service_acquisition_running(&service));

    assert(campaign_service_drive_acquisition_step(&service, &step) == TR2_OK);
    assert(step.kind == CAMPAIGN_ACQUISITION_STEP_WINDOW_COMPLETED);
    assert(step.source_result == TR2_OK);
    assert(step.stop_result == TR2_OK);
    assert(step.storage_result == TR2_OK);
    assert(step.window.complete);
    assert(!step.window.source_error);
    assert(step.window.acquired_sample_count == 2u);
    assert(step.window.valid_sample_count == 1u);
    assert(step.window.saturation_observed);
    assert(!campaign_service_acquisition_running(&service));
    assert(test.source_stop_step < test.checkpoint_step);
    assert(test.data_checkpoint_calls == 1u);

    assert(campaign_service_stop(&service, &closed) == TR2_OK);
    assert(test.data_checkpoint_calls == 1u);
    assert(closed.lifecycle_state == CAMPAIGN_LIFECYCLE_CLOSED);
}

static void test_live_driver_closes_and_checkpoints_on_source_error(void)
{
    TestContext test = {0};
    ConfigurationService configuration;
    AcquisitionService acquisition;
    CampaignRepository repository;
    CampaignDataStore data_store;
    MonotonicClock clock;
    VibrationSource source;
    CampaignService service;
    CampaignAcquisitionStep step;
    CampaignId id;
    CampaignMetadata closed;
    int stop_step_after_error;

    test.source_start_result = TR2_OK;
    test.source_read_result = TR2_ERROR_UNAVAILABLE;
    build_dependencies(&test, &configuration, &acquisition, &repository, &data_store, &clock, &source);
    configuration.active.payload.window_size_samples = 2u;
    assert(campaign_service_init(&service, &configuration, &acquisition, &repository, &data_store) == TR2_OK);
    assert(campaign_service_start(&service, &id) == TR2_OK);

    assert(campaign_service_drive_acquisition_step(&service, &step) == TR2_ERROR_UNAVAILABLE);
    assert(step.kind == CAMPAIGN_ACQUISITION_STEP_WINDOW_COMPLETED);
    assert(step.source_result == TR2_ERROR_UNAVAILABLE);
    assert(step.stop_result == TR2_OK);
    assert(step.storage_result == TR2_OK);
    assert(step.window.source_error);
    assert(!step.window.complete);
    assert(step.window.acquired_sample_count == 0u);
    assert(!campaign_service_acquisition_running(&service));
    assert(test.source_read_calls == 1u);
    assert(test.data_append_calls == 0u);
    assert(test.data_checkpoint_calls == 1u);
    assert(test.source_stop_step < test.checkpoint_step);
    stop_step_after_error = test.source_stop_step;
    assert(stop_step_after_error != 0);

    assert(campaign_service_stop(&service, &closed) == TR2_OK);
    assert(test.source_stop_step == stop_step_after_error);
    assert(test.data_checkpoint_calls == 1u);
    assert(closed.lifecycle_state == CAMPAIGN_LIFECYCLE_CLOSED);
}

static void test_append_failure_is_exposed_without_fabricating_durability(void)
{
    TestContext test = {0};
    ConfigurationService configuration;
    AcquisitionService acquisition;
    CampaignRepository repository;
    CampaignDataStore data_store;
    MonotonicClock clock;
    VibrationSource source;
    CampaignService service;
    CampaignAcquisitionStep step;
    CampaignId id;

    test.source_start_result = TR2_OK;
    test.source_read_result = TR2_OK;
    test.data_append_result = TR2_ERROR_STORAGE;
    build_dependencies(&test, &configuration, &acquisition, &repository, &data_store, &clock, &source);
    configuration.active.payload.window_size_samples = 2u;
    assert(campaign_service_init(&service, &configuration, &acquisition, &repository, &data_store) == TR2_OK);
    assert(campaign_service_start(&service, &id) == TR2_OK);

    assert(campaign_service_drive_acquisition_step(&service, &step) == TR2_ERROR_STORAGE);
    assert(step.kind == CAMPAIGN_ACQUISITION_STEP_SAMPLE_READ);
    assert(step.source_result == TR2_OK);
    assert(step.storage_result == TR2_ERROR_STORAGE);
    assert(test.source_read_calls == 1u);
    assert(test.data_append_calls == 1u);
    assert(test.data_checkpoint_calls == 0u);
    assert(acquisition.current_window.acquired_sample_count == 1u);
    assert(campaign_service_acquisition_running(&service));
}

static void test_checkpoint_failure_blocks_supervision_boundary(void)
{
    TestContext test = {0};
    ConfigurationService configuration;
    AcquisitionService acquisition;
    CampaignRepository repository;
    CampaignDataStore data_store;
    MonotonicClock clock;
    VibrationSource source;
    CampaignService service;
    CampaignAcquisitionStep step;
    CampaignId id;
    SupervisionService supervision;

    test.source_start_result = TR2_OK;
    test.source_read_result = TR2_OK;
    test.data_checkpoint_result = TR2_ERROR_STORAGE;
    build_dependencies(&test, &configuration, &acquisition, &repository, &data_store, &clock, &source);
    configuration.active.payload.window_size_samples = 1u;
    assert(campaign_service_init(&service, &configuration, &acquisition, &repository, &data_store) == TR2_OK);
    assert(supervision_service_init(&supervision) == TR2_OK);
    assert(campaign_service_start(&service, &id) == TR2_OK);
    assert(campaign_service_drive_acquisition_step(&service, &step) == TR2_OK);

    assert(campaign_service_drive_acquisition_step(&service, &step) == TR2_ERROR_STORAGE);
    assert(step.kind == CAMPAIGN_ACQUISITION_STEP_WINDOW_COMPLETED);
    assert(step.storage_result == TR2_ERROR_STORAGE);
    assert(test.data_checkpoint_calls == 1u);
    assert(!campaign_service_acquisition_running(&service));
    assert(campaign_service_publish_supervision_step(&supervision, &step) == TR2_ERROR_INVALID_STATE);
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
    build_dependencies(&test, &configuration, &acquisition, &repository, &data_store, &clock, &source);
    configuration.has_active = false;
    assert(campaign_service_init(&service, &configuration, &acquisition, &repository, &data_store) == TR2_OK);
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
    build_dependencies(&test, &configuration, &acquisition, &repository, &data_store, &clock, &source);
    assert(campaign_service_init(&service, &configuration, &acquisition, &repository, &data_store) == TR2_OK);

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
    assert(test.data_checkpoint_calls == 0u);
    assert(test.data_finish_step != 0);
    assert(test.close_step != 0);
}

static void test_stop_retries_data_recovery_without_refinishing(void)
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
    int finish_step_before_retry;

    test.source_start_result = TR2_OK;
    test.fail_data_recover_once = true;
    build_dependencies(&test, &configuration, &acquisition, &repository, &data_store, &clock, &source);
    assert(campaign_service_init(&service, &configuration, &acquisition, &repository, &data_store) == TR2_OK);
    assert(campaign_service_start(&service, &id) == TR2_OK);

    assert(campaign_service_stop(&service, &closed) == TR2_ERROR_STORAGE);
    assert(campaign_service_campaign_open(&service));
    assert(!campaign_service_acquisition_running(&service));
    assert(!service.data_store_started);
    assert(service.data_store_recovery_pending);
    assert(test.data_checkpoint_calls == 1u);
    assert(test.data_recover_calls == 1u);
    finish_step_before_retry = test.data_finish_step;

    assert(campaign_service_stop(&service, &closed) == TR2_OK);
    assert(test.data_finish_step == finish_step_before_retry);
    assert(test.data_checkpoint_calls == 1u);
    assert(test.data_recover_calls == 2u);
    assert(closed.durable_data_size_bytes == UINT64_C(123));
    assert(closed.lifecycle_state == CAMPAIGN_LIFECYCLE_CLOSED);
    assert(!closed.end_timestamp.available);
    assert(!closed.duration.available);
    assert(!campaign_service_campaign_open(&service));
}

int main(void)
{
    test_start_stop_freezes_context_and_orders_durability();
    test_live_driver_appends_records_and_checkpoints_window();
    test_live_driver_closes_and_checkpoints_on_source_error();
    test_append_failure_is_exposed_without_fabricating_durability();
    test_checkpoint_failure_blocks_supervision_boundary();
    test_start_without_active_configuration_is_rejected();
    test_partial_start_remains_closable();
    test_stop_retries_data_recovery_without_refinishing();
    return 0;
}
