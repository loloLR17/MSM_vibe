#include <limits.h>
#include <string.h>

#include "tr2/domain/supervision/indicator_calculator.h"

static uint32_t integer_sqrt_u64(uint64_t value)
{
    uint64_t result = 0u;
    uint64_t bit = (uint64_t)1u << 62;

    while (bit > value) {
        bit >>= 2;
    }

    while (bit != 0u) {
        if (value >= result + bit) {
            value -= result + bit;
            result = (result >> 1) + bit;
        } else {
            result >>= 1;
        }
        bit >>= 2;
    }

    return (uint32_t)result;
}

static uint32_t duration_ms_from_window(const AcquisitionWindow *window)
{
    uint64_t duration;

    duration = window->end_monotonic_ms - window->start_monotonic_ms;
    if (duration > UINT32_MAX) {
        return UINT32_MAX;
    }
    return (uint32_t)duration;
}

Tr2Result vibration_indicators_from_window(const AcquisitionWindow *window,
                                           VibrationIndicators *out_indicators)
{
    uint64_t valid_count;

    if (window == NULL || out_indicators == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (window->end_monotonic_ms < window->start_monotonic_ms ||
        window->statistics_overflow) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (window->valid_sample_count == 0u) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    memset(out_indicators, 0, sizeof(*out_indicators));
    valid_count = window->valid_sample_count;

    out_indicators->window_duration_ms = duration_ms_from_window(window);
    out_indicators->valid_sample_count = window->valid_sample_count;
    out_indicators->rms_global_mg = integer_sqrt_u64(
        window->sum_square_vector_mg2 / valid_count);
    out_indicators->peak_global_mg = integer_sqrt_u64(
        window->peak_vector_square_mg2);
    out_indicators->rms_x_mg = integer_sqrt_u64(
        window->sum_square_x_mg2 / valid_count);
    out_indicators->rms_y_mg = integer_sqrt_u64(
        window->sum_square_y_mg2 / valid_count);
    out_indicators->rms_z_mg = integer_sqrt_u64(
        window->sum_square_z_mg2 / valid_count);
    out_indicators->peak_x_mg = window->peak_abs_x_mg;
    out_indicators->peak_y_mg = window->peak_abs_y_mg;
    out_indicators->peak_z_mg = window->peak_abs_z_mg;

    return TR2_OK;
}
