#include <limits.h>
#include "tr2/domain/acquisition/acquisition.h"
#include "tr2/platform/vibration_source.h"

static uint32_t sample_abs_mg(int32_t value)
{
    if (value >= 0) {
        return (uint32_t)value;
    }
    return (uint32_t)(-(int64_t)value);
}

static bool checked_add_u64(uint64_t *accumulator, uint64_t value)
{
    if (*accumulator > UINT64_MAX - value) {
        return false;
    }
    *accumulator += value;
    return true;
}

bool acquisition_window_accumulate_valid_sample(AcquisitionWindow *window,
                                                       const VibrationSample *sample)
{
    uint32_t abs_x;
    uint32_t abs_y;
    uint32_t abs_z;
    uint64_t square_x;
    uint64_t square_y;
    uint64_t square_z;
    uint64_t vector_square;

    abs_x = sample_abs_mg(sample->x_mg);
    abs_y = sample_abs_mg(sample->y_mg);
    abs_z = sample_abs_mg(sample->z_mg);
    square_x = (uint64_t)abs_x * (uint64_t)abs_x;
    square_y = (uint64_t)abs_y * (uint64_t)abs_y;
    square_z = (uint64_t)abs_z * (uint64_t)abs_z;

    if (square_x > UINT64_MAX - square_y ||
        square_x + square_y > UINT64_MAX - square_z) {
        return false;
    }
    vector_square = square_x + square_y + square_z;

    if (!checked_add_u64(&window->sum_square_x_mg2, square_x) ||
        !checked_add_u64(&window->sum_square_y_mg2, square_y) ||
        !checked_add_u64(&window->sum_square_z_mg2, square_z) ||
        !checked_add_u64(&window->sum_square_vector_mg2, vector_square)) {
        return false;
    }

    if (abs_x > window->peak_abs_x_mg) {
        window->peak_abs_x_mg = abs_x;
    }
    if (abs_y > window->peak_abs_y_mg) {
        window->peak_abs_y_mg = abs_y;
    }
    if (abs_z > window->peak_abs_z_mg) {
        window->peak_abs_z_mg = abs_z;
    }
    if (vector_square > window->peak_vector_square_mg2) {
        window->peak_vector_square_mg2 = vector_square;
    }

    return true;
}
