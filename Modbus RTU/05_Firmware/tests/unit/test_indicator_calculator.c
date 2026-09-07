#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/domain/supervision/indicator_calculator.h"

static AcquisitionWindow make_window(void)
{
    AcquisitionWindow window;
    memset(&window, 0, sizeof(window));
    window.start_monotonic_ms = 1000u;
    window.end_monotonic_ms = 1125u;
    window.valid_sample_count = 2u;
    window.complete = true;
    return window;
}

int main(void)
{
    AcquisitionWindow window;
    VibrationIndicators indicators;

    /* Two valid samples: (3,4,0) and (0,0,12). */
    window = make_window();
    window.sum_square_x_mg2 = 9u;
    window.sum_square_y_mg2 = 16u;
    window.sum_square_z_mg2 = 144u;
    window.sum_square_vector_mg2 = 169u;
    window.peak_abs_x_mg = 3u;
    window.peak_abs_y_mg = 4u;
    window.peak_abs_z_mg = 12u;
    window.peak_vector_square_mg2 = 144u;

    assert(vibration_indicators_from_window(&window, &indicators) == TR2_OK);
    assert(indicators.window_duration_ms == 125u);
    assert(indicators.valid_sample_count == 2u);
    assert(indicators.rms_global_mg == 9u);
    assert(indicators.peak_global_mg == 12u);
    assert(indicators.rms_x_mg == 2u);
    assert(indicators.rms_y_mg == 2u);
    assert(indicators.rms_z_mg == 8u);
    assert(indicators.peak_x_mg == 3u);
    assert(indicators.peak_y_mg == 4u);
    assert(indicators.peak_z_mg == 12u);

    /* Integer mg representation uses deterministic truncation after sqrt. */
    window = make_window();
    window.valid_sample_count = 1u;
    window.sum_square_x_mg2 = 2u;
    window.sum_square_vector_mg2 = 2u;
    window.peak_abs_x_mg = 1u;
    window.peak_vector_square_mg2 = 2u;
    assert(vibration_indicators_from_window(&window, &indicators) == TR2_OK);
    assert(indicators.rms_x_mg == 1u);
    assert(indicators.rms_global_mg == 1u);
    assert(indicators.peak_global_mg == 1u);

    /* Zero-valued valid samples are legitimate measurements. */
    window = make_window();
    assert(vibration_indicators_from_window(&window, &indicators) == TR2_OK);
    assert(indicators.rms_global_mg == 0u);
    assert(indicators.peak_global_mg == 0u);
    assert(indicators.rms_x_mg == 0u);
    assert(indicators.peak_z_mg == 0u);

    /* No valid sample means no indicator result can be produced. */
    window = make_window();
    window.valid_sample_count = 0u;
    assert(vibration_indicators_from_window(&window, &indicators) == TR2_ERROR_NOT_AVAILABLE);

    /* Accumulator overflow and invalid monotonic ordering are calculation errors. */
    window = make_window();
    window.statistics_overflow = true;
    assert(vibration_indicators_from_window(&window, &indicators) == TR2_ERROR_INVALID_STATE);

    window = make_window();
    window.start_monotonic_ms = 1200u;
    window.end_monotonic_ms = 1100u;
    assert(vibration_indicators_from_window(&window, &indicators) == TR2_ERROR_INVALID_STATE);

    /* A representable B3 duration is saturated rather than wrapping. */
    window = make_window();
    window.valid_sample_count = 1u;
    window.end_monotonic_ms = window.start_monotonic_ms + (uint64_t)UINT32_MAX + 5u;
    assert(vibration_indicators_from_window(&window, &indicators) == TR2_OK);
    assert(indicators.window_duration_ms == UINT32_MAX);

    return 0;
}
