#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/campaign_service.h"
#include "tr2/application/supervision_service.h"

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
    window.configuration.payload.axes_enable_mask = 0x0007u;
    window.configuration.payload.window_size_samples = 2u;
    window.start_monotonic_ms = start_ms;
    window.end_monotonic_ms = end_ms;
    window.acquired_sample_count = 2u;
    window.valid_sample_count = 2u;

    /* Samples equivalent to (3,4,0) and (0,0,12). */
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
    SupervisionService service;
    SupervisionService bridge_service;
    SupervisionSnapshot first;
    SupervisionSnapshot second;
    SupervisionSnapshot held;
    AcquisitionWindow window_a;
    AcquisitionWindow window_b;
    AcquisitionWindow invalid_window;
    CampaignAcquisitionStep step;

    memset(&service, 0xA5, sizeof(service));
    assert(supervision_service_init(&service) == TR2_OK);
    assert(supervision_service_is_initialized(&service));
    assert(!supervision_service_snapshot(&service, &first));

    window_a = make_window(7u, 42u, 1000u, 1125u);
    window_a.saturation_observed = true;
    assert(supervision_service_publish_window(&service, &window_a) == TR2_OK);
    assert(supervision_service_snapshot(&service, &first));

    assert(first.configuration.generation == 7u);
    assert(first.configuration.config_id == 42u);
    assert(first.configuration.revision_counter == 17u);
    assert(first.calculation_sequence == 1u);
    assert(first.window_duration_ms == 125u);
    assert(first.valid_sample_count == 2u);
    assert(first.rms_global_mg == 9u);
    assert(first.peak_global_mg == 12u);
    assert(first.rms_x_mg == 2u);
    assert(first.rms_y_mg == 2u);
    assert(first.rms_z_mg == 8u);
    assert(first.peak_x_mg == 3u);
    assert(first.peak_y_mg == 4u);
    assert(first.peak_z_mg == 12u);
    assert(first.values_available);
    assert(first.window_complete);
    assert(first.saturation_observed);
    assert(!first.calculation_error);
    assert(first.value_monotonic_ms == 1125u);
    assert(!first.civil_timestamp_available);
    assert(first.civil_timestamp == 0u);

    window_b = make_window(8u, 43u, 2000u, 2200u);
    window_b.sum_square_x_mg2 = 50u;
    window_b.sum_square_y_mg2 = 50u;
    window_b.sum_square_z_mg2 = 0u;
    window_b.sum_square_vector_mg2 = 100u;
    window_b.peak_abs_x_mg = 5u;
    window_b.peak_abs_y_mg = 5u;
    window_b.peak_abs_z_mg = 0u;
    window_b.peak_vector_square_mg2 = 50u;
    assert(supervision_service_publish_window(&service, &window_b) == TR2_OK);
    assert(supervision_service_snapshot(&service, &second));
    assert(second.configuration.generation == 8u);
    assert(second.configuration.config_id == 43u);
    assert(second.calculation_sequence == 2u);
    assert(second.window_duration_ms == 200u);
    assert(second.rms_global_mg == 7u);
    assert(second.peak_global_mg == 7u);
    assert(second.rms_x_mg == 5u);
    assert(second.rms_y_mg == 5u);
    assert(second.rms_z_mg == 0u);
    assert(second.value_monotonic_ms == 2200u);
    assert(!second.saturation_observed);

    invalid_window = make_window(9u, 99u, 3000u, 3100u);
    invalid_window.valid_sample_count = 0u;
    invalid_window.sum_square_x_mg2 = 0u;
    invalid_window.sum_square_y_mg2 = 0u;
    invalid_window.sum_square_z_mg2 = 0u;
    invalid_window.sum_square_vector_mg2 = 0u;
    assert(supervision_service_publish_window(&service, &invalid_window) != TR2_OK);
    assert(supervision_service_snapshot(&service, &held));
    assert(memcmp(&held, &second, sizeof(held)) == 0);

    window_a = make_window(10u, 44u, 4000u, 4100u);
    assert(supervision_service_publish_window(&service, &window_a) == TR2_OK);
    assert(supervision_service_snapshot(&service, &held));
    assert(held.calculation_sequence == 3u);

    service.next_calculation_sequence = UINT32_MAX;
    window_a = make_window(11u, 45u, 5000u, 5100u);
    assert(supervision_service_publish_window(&service, &window_a) == TR2_OK);
    assert(supervision_service_snapshot(&service, &held));
    assert(held.calculation_sequence == UINT32_MAX);
    window_a.configuration.generation = 12u;
    assert(supervision_service_publish_window(&service, &window_a) == TR2_OK);
    assert(supervision_service_snapshot(&service, &held));
    assert(held.calculation_sequence == UINT32_MAX);

    /* P8-E: only a completed acquisition window may feed supervision. */
    assert(supervision_service_init(&bridge_service) == TR2_OK);
    memset(&step, 0, sizeof(step));
    step.kind = CAMPAIGN_ACQUISITION_STEP_SAMPLE_READ;
    assert(campaign_service_publish_supervision_step(&bridge_service, &step) ==
           TR2_ERROR_INVALID_STATE);
    assert(!supervision_service_snapshot(&bridge_service, &held));

    step.kind = CAMPAIGN_ACQUISITION_STEP_WINDOW_COMPLETED;
    step.window = make_window(21u, 46u, 6000u, 6125u);
    assert(campaign_service_publish_supervision_step(&bridge_service, &step) == TR2_OK);
    assert(supervision_service_snapshot(&bridge_service, &held));
    assert(held.configuration.generation == 21u);
    assert(held.configuration.config_id == 46u);
    assert(held.configuration.revision_counter == 31u);
    assert(held.value_monotonic_ms == 6125u);
    assert(held.window_complete);

    second = held;
    step.window.configuration.generation = 99u;
    step.window.valid_sample_count = 0u;
    step.window.sum_square_x_mg2 = 0u;
    step.window.sum_square_y_mg2 = 0u;
    step.window.sum_square_z_mg2 = 0u;
    step.window.sum_square_vector_mg2 = 0u;
    assert(campaign_service_publish_supervision_step(&bridge_service, &step) ==
           TR2_ERROR_NOT_AVAILABLE);
    assert(supervision_service_snapshot(&bridge_service, &held));
    assert(memcmp(&held, &second, sizeof(held)) == 0);

    assert(campaign_service_publish_supervision_step(NULL, &step) ==
           TR2_ERROR_INVALID_ARGUMENT);
    assert(campaign_service_publish_supervision_step(&bridge_service, NULL) ==
           TR2_ERROR_INVALID_ARGUMENT);

    return 0;
}
