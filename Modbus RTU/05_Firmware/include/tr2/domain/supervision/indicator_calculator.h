#ifndef TR2_DOMAIN_SUPERVISION_INDICATOR_CALCULATOR_H
#define TR2_DOMAIN_SUPERVISION_INDICATOR_CALCULATOR_H

#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/acquisition/acquisition.h"

typedef struct {
    uint32_t window_duration_ms;
    uint32_t valid_sample_count;
    uint32_t rms_global_mg;
    uint32_t peak_global_mg;
    uint32_t rms_x_mg;
    uint32_t rms_y_mg;
    uint32_t rms_z_mg;
    uint32_t peak_x_mg;
    uint32_t peak_y_mg;
    uint32_t peak_z_mg;
} VibrationIndicators;

Tr2Result vibration_indicators_from_window(const AcquisitionWindow *window,
                                           VibrationIndicators *out_indicators);

#endif
