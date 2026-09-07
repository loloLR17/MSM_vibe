#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "tr2/domain/acquisition/acquisition.h"
#include "tr2/domain/supervision/supervision.h"
#include "tr2/platform/vibration_source.h"

typedef struct {
    VibrationSourceConfiguration configuration;
    bool configured;
    bool running;
    size_t next_sample;
    VibrationSample samples[2];
} FakeVibrationSource;

static Tr2Result fake_configure(void *context,
                                const VibrationSourceConfiguration *configuration)
{
    FakeVibrationSource *fake = (FakeVibrationSource *)context;
    if (fake == NULL || configuration == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    fake->configuration = *configuration;
    fake->configured = true;
    fake->next_sample = 0u;
    return TR2_OK;
}

static Tr2Result fake_start(void *context)
{
    FakeVibrationSource *fake = (FakeVibrationSource *)context;
    if (fake == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!fake->configured || fake->running) {
        return TR2_ERROR_INVALID_STATE;
    }
    fake->running = true;
    return TR2_OK;
}

static Tr2Result fake_read_sample(void *context, VibrationSample *sample)
{
    FakeVibrationSource *fake = (FakeVibrationSource *)context;
    if (fake == NULL || sample == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!fake->running) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (fake->next_sample >= 2u) {
        return TR2_ERROR_NOT_AVAILABLE;
    }
    *sample = fake->samples[fake->next_sample];
    fake->next_sample++;
    return TR2_OK;
}

static Tr2Result fake_stop(void *context)
{
    FakeVibrationSource *fake = (FakeVibrationSource *)context;
    if (fake == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!fake->running) {
        return TR2_ERROR_INVALID_STATE;
    }
    fake->running = false;
    return TR2_OK;
}

static void test_vibration_source_contract(void)
{
    FakeVibrationSource fake;
    VibrationSource source;
    VibrationSourceConfiguration configuration = {26667u, 0x0007u, 2u};
    VibrationSample sample;

    memset(&fake, 0, sizeof(fake));
    fake.samples[0] = (VibrationSample){100, -200, 300, true, false};
    fake.samples[1] = (VibrationSample){-400, 500, -600, true, true};

    source.context = &fake;
    source.configure = fake_configure;
    source.start = fake_start;
    source.read_sample = fake_read_sample;
    source.stop = fake_stop;

    assert(source.start(source.context) == TR2_ERROR_INVALID_STATE);
    assert(source.configure(source.context, &configuration) == TR2_OK);
    assert(fake.configuration.sampling_frequency_hz == 26667u);
    assert(fake.configuration.axes_enable_mask == 0x0007u);
    assert(fake.configuration.full_scale_code == 2u);
    assert(source.start(source.context) == TR2_OK);

    assert(source.read_sample(source.context, &sample) == TR2_OK);
    assert(sample.x_mg == 100);
    assert(sample.y_mg == -200);
    assert(sample.z_mg == 300);
    assert(sample.valid);
    assert(!sample.saturated);

    assert(source.read_sample(source.context, &sample) == TR2_OK);
    assert(sample.x_mg == -400);
    assert(sample.y_mg == 500);
    assert(sample.z_mg == -600);
    assert(sample.valid);
    assert(sample.saturated);

    assert(source.read_sample(source.context, &sample) == TR2_ERROR_NOT_AVAILABLE);
    assert(source.stop(source.context) == TR2_OK);
    assert(source.read_sample(source.context, &sample) == TR2_ERROR_INVALID_STATE);
}

static void test_acquisition_context_is_a_value_snapshot(void)
{
    ActiveConfigurationSnapshot active;
    AcquisitionWindow window;

    memset(&active, 0, sizeof(active));
    memset(&window, 0, sizeof(window));

    active.generation = 7u;
    active.config_id = 42u;
    active.revision_counter = 3u;
    active.payload.sampling_frequency_hz = 26667u;
    active.payload.window_size_samples = 8192u;
    active.payload.rms_warn_threshold_mg = 100u;

    window.configuration.generation = active.generation;
    window.configuration.config_id = active.config_id;
    window.configuration.revision_counter = active.revision_counter;
    window.configuration.payload = active.payload;

    active.generation = 8u;
    active.config_id = 43u;
    active.revision_counter = 4u;
    active.payload.window_size_samples = 16384u;
    active.payload.rms_warn_threshold_mg = 200u;

    assert(window.configuration.generation == 7u);
    assert(window.configuration.config_id == 42u);
    assert(window.configuration.revision_counter == 3u);
    assert(window.configuration.payload.window_size_samples == 8192u);
    assert(window.configuration.payload.rms_warn_threshold_mg == 100u);
}

static void test_supervision_contract_has_no_implicit_civil_timestamp(void)
{
    SupervisionSnapshot snapshot;

    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.values_available = true;
    snapshot.value_monotonic_ms = 1234u;

    assert(snapshot.values_available);
    assert(snapshot.value_monotonic_ms == 1234u);
    assert(!snapshot.civil_timestamp_available);
    assert(snapshot.civil_timestamp == 0u);
}

int main(void)
{
    test_vibration_source_contract();
    test_acquisition_context_is_a_value_snapshot();
    test_supervision_contract_has_no_implicit_civil_timestamp();
    return 0;
}
