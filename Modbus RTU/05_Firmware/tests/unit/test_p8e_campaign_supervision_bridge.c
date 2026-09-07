#include <assert.h>
#include <string.h>

#include "tr2/application/campaign_service.h"

static AcquisitionWindow make_window(uint32_t generation,
                                     uint32_t config_id,
                                     MonotonicTimeMs start_ms,
                                     MonotonicTimeMs end_ms)
{
    AcquisitionWindow window;

    memset(&window, 0, sizeof(window));
    window.configuration.generation = generation;
    window.configuration.config_id = config_id;
    window.configuration.revision_counter = generation + 10u;
    window.configuration.payload.sampling_frequency_hz = 26667u;
    window.configuration.payload.axes_enable_mask = 7u;
    window.configuration.payload.window_size_samples = 2u;
    window.configuration.payload.rms_warn_threshold_mg = 100u;
    window.configuration.payload.rms_alarm_threshold_mg = 200u;
    window.configuration.payload.peak_warn_threshold_mg = 300u;
    window.configuration.payload.peak_alarm_threshold_mg = 400u;
    window.start_monotonic_ms = start_ms;
    window.end_monotonic_ms = end_ms;
    window.acquired_sample_count = 2u;
    window.valid_sample_count = 2u;
    window.sum_square_x_mg2 = 9u;
    window.sum_square_y_mg2 = 16u;
    window.sum_square_z_mg2 = 144u;
    window.sum_square_vector_mg2 = 169u;
    window.peak_abs_x_mg = 3u;
    window.peak_abs_y_mg = 4u;
    window.peak_abs_z_mg = 12u;
    window.peak_vector_square_mg2 = 144u;
    window.complete = true;
    return window;
}

int main(void)
{
    SupervisionService supervision;
    SupervisionSnapshot snapshot;
    SupervisionSnapshot held;
    CampaignAcquisitionStep step;

    assert(supervision_service_init(&supervision) == TR2_OK);

    memset(&step, 0, sizeof(step));
    step.kind = CAMPAIGN_ACQUISITION_STEP_SAMPLE_READ;
    assert(campaign_service_publish_supervision_step(&supervision, &step) ==
           TR2_ERROR_INVALID_STATE);
    assert(!supervision_service_snapshot(&supervision, &snapshot));

    memset(&step, 0, sizeof(step));
    step.kind = CAMPAIGN_ACQUISITION_STEP_WINDOW_COMPLETED;
    step.window = make_window(21u, 42u, 1000u, 1125u);
    assert(campaign_service_publish_supervision_step(&supervision, &step) == TR2_OK);
    assert(supervision_service_snapshot(&supervision, &snapshot));
    assert(snapshot.configuration.generation == 21u);
    assert(snapshot.configuration.config_id == 42u);
    assert(snapshot.configuration.revision_counter == 31u);
    assert(snapshot.window_complete);
    assert(snapshot.value_monotonic_ms == 1125u);
    assert(snapshot.valid_sample_count == 2u);
    assert(snapshot.rms_global_mg == 9u);
    assert(snapshot.peak_global_mg == 12u);

    held = snapshot;
    step.window.configuration.generation = 99u;
    step.window.valid_sample_count = 0u;
    step.window.sum_square_x_mg2 = 0u;
    step.window.sum_square_y_mg2 = 0u;
    step.window.sum_square_z_mg2 = 0u;
    step.window.sum_square_vector_mg2 = 0u;
    assert(campaign_service_publish_supervision_step(&supervision, &step) ==
           TR2_ERROR_NOT_AVAILABLE);
    assert(supervision_service_snapshot(&supervision, &snapshot));
    assert(memcmp(&snapshot, &held, sizeof(snapshot)) == 0);

    assert(campaign_service_publish_supervision_step(NULL, &step) ==
           TR2_ERROR_INVALID_ARGUMENT);
    assert(campaign_service_publish_supervision_step(&supervision, NULL) ==
           TR2_ERROR_INVALID_ARGUMENT);

    return 0;
}
