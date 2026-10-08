#include <assert.h>
#include <limits.h>
#include "iis3dwb_diag_window.h"

static uint32_t u32(const ModbusBlock3Image *image, unsigned index)
{
    return ((uint32_t)image->registers[index] << 16) | image->registers[index + 1];
}

int main(void)
{
    Iis3dwbDiagWindow diag;
    VibrationSample sample = {-300, 400, 0, true, false};
    assert(iis3dwb_diag_window_begin(&diag, 100U) == TR2_OK);
    assert(iis3dwb_diag_window_publish(&diag, 200U) == TR2_ERROR_INVALID_STATE);
    assert(!diag.image_available);
    for (unsigned i = 0; i < TR2_IIS3DWB_DIAG_WINDOW_SAMPLES; ++i) {
        sample.x_mg = (i & 1U) ? 300 : -300;
        assert(iis3dwb_diag_window_append(&diag, &sample) == TR2_OK);
    }
    assert(iis3dwb_diag_window_publish(&diag, 99U) == TR2_ERROR_INVALID_STATE);
    assert(iis3dwb_diag_window_publish(&diag, 350U) == TR2_OK);
    assert(diag.image_available);
    assert(u32(&diag.image, 10) == 250U);
    assert(u32(&diag.image, 12) == 4096U);
    assert(u32(&diag.image, 14) == 500U);
    assert(u32(&diag.image, 16) == 500U);
    assert(u32(&diag.image, 18) == 300U);
    assert(u32(&diag.image, 20) == 400U);
    assert(u32(&diag.image, 22) == 0U);
    assert(u32(&diag.image, 24) == 300U);
    assert(u32(&diag.image, 26) == 400U);
    assert(u32(&diag.image, 28) == 0U);
    assert(!diag.snapshot.threshold_facts_available);
    assert(!diag.snapshot.civil_timestamp_available);
    for (unsigned i = 30; i < 48; ++i) assert(diag.image.registers[i] == 0U);
    assert(iis3dwb_diag_window_append(&diag, &sample) == TR2_ERROR_INVALID_STATE);
    assert(iis3dwb_diag_window_publish(&diag, 400U) == TR2_ERROR_INVALID_STATE);

    assert(iis3dwb_diag_window_begin(&diag, 0U) == TR2_OK);
    sample.valid = false;
    assert(iis3dwb_diag_window_append(&diag, &sample) == TR2_ERROR_NOT_AVAILABLE);
    assert(diag.window.valid_sample_count == 0U && diag.window.source_error);
    assert(iis3dwb_diag_window_publish(&diag, 10U) == TR2_ERROR_INVALID_STATE);

    assert(iis3dwb_diag_window_begin(&diag, 0U) == TR2_OK);
    sample = (VibrationSample){INT32_MIN, 0, 0, true, true};
    assert(iis3dwb_diag_window_append(&diag, &sample) == TR2_OK);
    assert(diag.window.peak_abs_x_mg == 2147483648U);
    assert(diag.window.saturation_observed);
    diag.window.sum_square_x_mg2 = UINT64_MAX;
    assert(iis3dwb_diag_window_append(&diag, &sample) == TR2_ERROR_INVALID_STATE);
    assert(diag.window.statistics_overflow);
    assert(!diag.image_available);

    assert(iis3dwb_diag_window_begin(&diag, 0U) == TR2_OK);
    sample = (VibrationSample){0, 0, 0, true, true};
    for (unsigned i = 0; i < 4096U; ++i) {
        assert(iis3dwb_diag_window_append(&diag, &sample) == TR2_OK);
    }
    assert(iis3dwb_diag_window_publish(&diag, 100U) == TR2_OK);
    assert(u32(&diag.image, 14) == 0U);
    assert((diag.image.registers[1] & 0x20U) == 0U);
    return 0;
}
