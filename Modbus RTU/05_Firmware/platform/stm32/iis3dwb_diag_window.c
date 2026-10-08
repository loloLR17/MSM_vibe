#include "iis3dwb_diag_window.h"

#include <limits.h>
#include <string.h>

static uint32_t absolute_mg(int32_t value)
{
    return value < 0 ? (uint32_t)(-(int64_t)value) : (uint32_t)value;
}

Tr2Result iis3dwb_diag_window_begin(Iis3dwbDiagWindow *diag, MonotonicTimeMs now)
{
    if (diag == NULL) return TR2_ERROR_INVALID_ARGUMENT;
    memset(diag, 0, sizeof(*diag));
    diag->window.configuration.payload.sampling_frequency_hz = 26667U;
    diag->window.configuration.payload.axes_enable_mask = 7U;
    diag->window.configuration.payload.full_scale_code = 0U;
    diag->window.configuration.payload.window_size_samples = TR2_IIS3DWB_DIAG_WINDOW_SAMPLES;
    diag->window.start_monotonic_ms = now;
    return supervision_service_init(&diag->supervision);
}

Tr2Result iis3dwb_diag_window_append(Iis3dwbDiagWindow *diag,
                                    const VibrationSample *sample)
{
    uint32_t x, y, z;
    uint64_t sx, sy, sz, sv;
    AcquisitionWindow *w;
    if (diag == NULL || sample == NULL) return TR2_ERROR_INVALID_ARGUMENT;
    w = &diag->window;
    if (!diag->supervision.initialized || diag->image_available ||
        w->statistics_overflow || w->source_error ||
        w->acquired_sample_count >= TR2_IIS3DWB_DIAG_WINDOW_SAMPLES) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (!sample->valid) {
        w->source_error = true;
        return TR2_ERROR_NOT_AVAILABLE;
    }
    x = absolute_mg(sample->x_mg);
    y = absolute_mg(sample->y_mg);
    z = absolute_mg(sample->z_mg);
    sx = (uint64_t)x * x;
    sy = (uint64_t)y * y;
    sz = (uint64_t)z * z;
    /* Three int32 magnitudes squared fit in uint64; accumulated sums may not. */
    sv = sx + sy + sz;
    if (w->sum_square_x_mg2 > UINT64_MAX - sx ||
        w->sum_square_y_mg2 > UINT64_MAX - sy ||
        w->sum_square_z_mg2 > UINT64_MAX - sz ||
        w->sum_square_vector_mg2 > UINT64_MAX - sv) {
        w->statistics_overflow = true;
        return TR2_ERROR_INVALID_STATE;
    }
    w->sum_square_x_mg2 += sx;
    w->sum_square_y_mg2 += sy;
    w->sum_square_z_mg2 += sz;
    w->sum_square_vector_mg2 += sv;
    if (x > w->peak_abs_x_mg) w->peak_abs_x_mg = x;
    if (y > w->peak_abs_y_mg) w->peak_abs_y_mg = y;
    if (z > w->peak_abs_z_mg) w->peak_abs_z_mg = z;
    if (sv > w->peak_vector_square_mg2) w->peak_vector_square_mg2 = sv;
    ++w->acquired_sample_count;
    ++w->valid_sample_count;
    w->saturation_observed |= sample->saturated;
    return TR2_OK;
}

Tr2Result iis3dwb_diag_window_publish(Iis3dwbDiagWindow *diag, MonotonicTimeMs now)
{
    Tr2Result result;
    if (diag == NULL) return TR2_ERROR_INVALID_ARGUMENT;
    if (diag->image_available || diag->window.source_error ||
        diag->window.statistics_overflow ||
        diag->window.valid_sample_count != TR2_IIS3DWB_DIAG_WINDOW_SAMPLES ||
        now < diag->window.start_monotonic_ms) {
        return TR2_ERROR_INVALID_STATE;
    }
    diag->window.end_monotonic_ms = now;
    diag->window.complete = true;
    result = supervision_service_publish_window(&diag->supervision, &diag->window);
    if (result != TR2_OK) return result;
    if (!supervision_service_snapshot(&diag->supervision, &diag->snapshot)) {
        return TR2_ERROR_INTERNAL;
    }
    /* No configured thresholds, civil clock or campaign authority in this harness. */
    diag->snapshot.threshold_facts_available = false;
    memset(&diag->snapshot.threshold_facts, 0, sizeof(diag->snapshot.threshold_facts));
    result = modbus_project_b3(&diag->snapshot, &diag->image);
    diag->image_available = result == TR2_OK;
    return result;
}
