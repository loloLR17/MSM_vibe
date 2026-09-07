#ifndef TR2_PLATFORM_VIBRATION_SOURCE_H
#define TR2_PLATFORM_VIBRATION_SOURCE_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"

typedef struct {
    uint16_t sampling_frequency_hz;
    uint16_t axes_enable_mask;
    uint16_t full_scale_code;
} VibrationSourceConfiguration;

typedef struct {
    int32_t x_mg;
    int32_t y_mg;
    int32_t z_mg;
    bool valid;
    bool saturated;
} VibrationSample;

typedef struct {
    void *context;
    Tr2Result (*configure)(void *context, const VibrationSourceConfiguration *configuration);
    Tr2Result (*start)(void *context);
    Tr2Result (*read_sample)(void *context, VibrationSample *sample);
    Tr2Result (*stop)(void *context);
} VibrationSource;

#endif
