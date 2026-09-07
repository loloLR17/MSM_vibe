#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/acquisition_service.h"
#include "tr2/persistence/configuration_store.h"
#include "tr2/persistence/persistent_storage_core.h"
#include "tr2/platform/persistent_media.h"

#define TEST_MEDIA_SIZE 2048u
#define TEST_SAMPLE_CAPACITY 8u

typedef struct {
    uint8_t bytes[TEST_MEDIA_SIZE];
} FakeMediaContext;

typedef struct {
    MonotonicTimeMs now_ms;
} FakeClockContext;

typedef struct {
    VibrationSourceConfiguration last_configuration;
    VibrationSample samples[TEST_SAMPLE_CAPACITY];
    size_t sample_count;
    size_t read_index;
    Tr2Result configure_result;
    Tr2Result start_result;
    Tr2Result read_result;
    Tr2Result stop_result;
    uint32_t configure_calls;
    uint32_t start_calls;
    uint32_t read_calls;
    uint32_t stop_calls;
} FakeVibrationContext;

static Tr2Result fake_media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    FakeMediaContext *media = (FakeMediaContext *)context;
    if (buffer == NULL || offset > TEST_MEDIA_SIZE || size > TEST_MEDIA_SIZE - offset) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memcpy(buffer, &media->bytes[offset], size);
    return TR2_OK;
}

static Tr2Result fake_media_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    FakeMediaContext *media = (FakeMediaContext *)context;
    if (buffer == NULL || offset > TEST_MEDIA_SIZE || size > TEST_MEDIA_SIZE - offset) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memcpy(&media->bytes[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result fake_media_commit(void *context)
{
    (void)context;
    return TR2_OK;
}

static MonotonicTimeMs fake_now_ms(void *context)
{
    return ((FakeClockContext *)context)->now_ms;
}

static Tr2Result fake_vibration_configure(
    void *context,
    const VibrationSourceConfiguration *configuration)
{
    FakeVibrationContext *source = (FakeVibrationContext *)context;
    source->configure_calls++;
    if (configuration == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    source->last_configuration = *configuration;
    return source->configure_result;
}

static Tr2Result fake_vibration_start(void *context)
{
    FakeVibrationContext *source = (FakeVibrationContext *)context;
    source->start_calls++;
    return source->start_result;
}

static Tr2Result fake_vibration_read(void *context, VibrationSample *sample)
{
    FakeVibrationContext *source = (FakeVibrationContext *)context;
    source->read_calls++;
    if (sample == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (source->read_result != TR2_OK) {
        return source->read_result;
    }
    if (source->read_index >= source->sample_count) {
        return TR2_ERROR_NOT_AVAILABLE;
    }
    *sample = source->samples[source->read_index++];
    return TR2_OK;
}

static Tr2Result fake_vibration_stop(void *context)
{
    FakeVibrationContext *source = (FakeVibrationContext *)context;
    source->stop_calls++;
    return source->stop_result;
}

static void initialize_configuration_service(ConfigurationService *service,
                                             ConfigurationStore *store,
                                             PersistentStorageCore *storage,
                                             PersistentMedia *media,
                                             FakeMediaContext *media_context)
{
    memset(media_context, 0, sizeof(*media_context));
    media->context = media_context;
    media->read = fake_media_read;
    media->write = fake_media_write;
    media->commit = fake_media_commit;

    assert(persistent_storage_core_init(storage, media) == TR2_OK);
    assert(configuration_store_init(store, storage) == TR2_OK);
    assert(configuration_service_init(service, store) == TR2_OK);
}

static ActiveConfigurationSnapshot make_active(uint32_t generation,
                                               uint32_t config_id,
                                               uint32_t revision,
                                               uint16_t window_size)
{
    ActiveConfigurationSnapshot active;
    memset(&active, 0, sizeof(active));
    active.generation = generation;
    active.config_id = config_id;
    active.revision_counter = revision;
    active.payload.sampling_frequency_hz = 26667u;
    active.payload.axes_enable_mask = 0x0007u;
    active.payload.full_scale_code = 2u;
    active.payload.window_size_samples = window_size;
    active.payload.indicator_period_ms = 5000u;
    active.payload.rms_warn_threshold_mg = 100u;
    active.payload.rms_alarm_threshold_mg = 200u;
    return active;
}

static void publish_active(ConfigurationService *service,
                           const ActiveConfigurationSnapshot *active)
{
    service->active = *active;
    service->has_active = true;
}

static void initialize_source(FakeVibrationContext *context, VibrationSource *source)
{
    memset(context, 0, sizeof(*context));
    context->configure_result = TR2_OK;
    context->start_result = TR2_OK;
    context->read_result = TR2_OK;
    context->stop_result = TR2_OK;

    source->context = context;
    source->configure = fake_vibration_configure;
    source->start = fake_vibration_start;
    source->read_sample = fake_vibration_read;
    source->stop = fake_vibration_stop;
}

int main(void)
{
    FakeMediaContext media_context;
    PersistentMedia media;
    PersistentStorageCore storage;
    ConfigurationStore store;
    ConfigurationService configuration_service;
    FakeClockContext clock_context = { 1000u };
    MonotonicClock clock = { &clock_context, fake_now_ms };
    FakeVibrationContext source_context;
    VibrationSource source;
    AcquisitionService acquisition;
    ActiveConfigurationSnapshot active_a;
    ActiveConfigurationSnapshot active_b;
    AcquisitionWindow window;
    VibrationSample sample;

    memset(&media, 0, sizeof(media));
    memset(&storage, 0, sizeof(storage));
    memset(&store, 0, sizeof(store));
    memset(&configuration_service, 0, sizeof(configuration_service));
    initialize_configuration_service(&configuration_service,
                                     &store,
                                     &storage,
                                     &media,
                                     &media_context);
    initialize_source(&source_context, &source);

    assert(acquisition_service_init(&acquisition,
                                    &configuration_service,
                                    &clock,
                                    &source) == TR2_OK);
    assert(acquisition_service_is_initialized(&acquisition));
    assert(!acquisition_service_window_active(&acquisition));

    assert(acquisition_service_begin_window(&acquisition) == TR2_ERROR_NOT_AVAILABLE);
    assert(source_context.configure_calls == 0u);
    assert(source_context.start_calls == 0u);

    active_a = make_active(10u, 100u, 7u, 2u);
    publish_active(&configuration_service, &active_a);

    source_context.samples[0] = (VibrationSample){ 10, -20, 30, true, false };
    source_context.samples[1] = (VibrationSample){ 40, 50, -60, false, true };
    source_context.sample_count = 2u;

    assert(acquisition_service_begin_window(&acquisition) == TR2_OK);
    assert(acquisition_service_window_active(&acquisition));
    assert(source_context.configure_calls == 1u);
    assert(source_context.start_calls == 1u);
    assert(source_context.last_configuration.sampling_frequency_hz == 26667u);
    assert(source_context.last_configuration.axes_enable_mask == 0x0007u);
    assert(source_context.last_configuration.full_scale_code == 2u);
    assert(acquisition.current_window.configuration.generation == 10u);
    assert(acquisition.current_window.configuration.config_id == 100u);
    assert(acquisition.current_window.configuration.revision_counter == 7u);
    assert(acquisition.current_window.start_monotonic_ms == 1000u);

    active_b = make_active(11u, 101u, 8u, 1u);
    active_b.payload.axes_enable_mask = 0x0001u;
    active_b.payload.full_scale_code = 3u;
    publish_active(&configuration_service, &active_b);

    assert(acquisition_service_read_sample(&acquisition, &sample) == TR2_OK);
    assert(sample.x_mg == 10);
    assert(acquisition_service_read_sample(&acquisition, &sample) == TR2_OK);
    assert(sample.z_mg == -60);
    assert(acquisition.current_window.configuration.generation == 10u);
    assert(acquisition.current_window.configuration.payload.window_size_samples == 2u);
    assert(acquisition.current_window.acquired_sample_count == 2u);
    assert(acquisition.current_window.valid_sample_count == 1u);
    assert(acquisition.current_window.saturation_observed);
    assert(!acquisition.current_window.statistics_overflow);
    assert(acquisition.current_window.sum_square_x_mg2 == 100u);
    assert(acquisition.current_window.sum_square_y_mg2 == 400u);
    assert(acquisition.current_window.sum_square_z_mg2 == 900u);
    assert(acquisition.current_window.sum_square_vector_mg2 == 1400u);
    assert(acquisition.current_window.peak_abs_x_mg == 10u);
    assert(acquisition.current_window.peak_abs_y_mg == 20u);
    assert(acquisition.current_window.peak_abs_z_mg == 30u);
    assert(acquisition.current_window.peak_vector_square_mg2 == 1400u);
    assert(acquisition_service_read_sample(&acquisition, &sample) == TR2_ERROR_INVALID_STATE);

    clock_context.now_ms = 1125u;
    assert(acquisition_service_end_window(&acquisition, &window) == TR2_OK);
    assert(!acquisition_service_window_active(&acquisition));
    assert(window.complete);
    assert(!window.source_error);
    assert(window.configuration.generation == 10u);
    assert(window.configuration.config_id == 100u);
    assert(window.start_monotonic_ms == 1000u);
    assert(window.end_monotonic_ms == 1125u);
    assert(source_context.stop_calls == 1u);

    source_context.read_index = 0u;
    source_context.sample_count = 1u;
    assert(acquisition_service_begin_window(&acquisition) == TR2_OK);
    assert(acquisition.current_window.configuration.generation == 11u);
    assert(acquisition.current_window.configuration.config_id == 101u);
    assert(acquisition.current_window.configuration.revision_counter == 8u);
    assert(acquisition.current_window.configuration.payload.window_size_samples == 1u);
    assert(source_context.last_configuration.axes_enable_mask == 0x0001u);
    assert(source_context.last_configuration.full_scale_code == 3u);
    assert(acquisition_service_begin_window(&acquisition) == TR2_ERROR_INVALID_STATE);

    assert(acquisition_service_read_sample(&acquisition, &sample) == TR2_OK);
    clock_context.now_ms = 1200u;
    assert(acquisition_service_end_window(&acquisition, &window) == TR2_OK);
    assert(window.complete);
    assert(window.configuration.generation == 11u);

    active_a.payload.window_size_samples = 2u;
    publish_active(&configuration_service, &active_a);
    source_context.read_index = 0u;
    source_context.sample_count = 2u;
    assert(acquisition_service_begin_window(&acquisition) == TR2_OK);
    assert(acquisition_service_read_sample(&acquisition, &sample) == TR2_OK);
    clock_context.now_ms = 1300u;
    assert(acquisition_service_end_window(&acquisition, &window) == TR2_OK);
    assert(!window.complete);
    assert(window.acquired_sample_count == 1u);

    source_context.read_result = TR2_ERROR_UNAVAILABLE;
    assert(acquisition_service_begin_window(&acquisition) == TR2_OK);
    assert(acquisition_service_read_sample(&acquisition, &sample) == TR2_ERROR_UNAVAILABLE);
    clock_context.now_ms = 1400u;
    assert(acquisition_service_end_window(&acquisition, &window) == TR2_OK);
    assert(window.source_error);
    assert(!window.complete);
    assert(window.acquired_sample_count == 0u);

    return 0;
}
