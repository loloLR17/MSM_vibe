#ifndef TR2_DOMAIN_SUPERVISION_H
#define TR2_DOMAIN_SUPERVISION_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/domain/acquisition/acquisition.h"
#include "tr2/platform/wall_clock.h"

typedef struct {
    AcquisitionConfigurationContext configuration;
    uint32_t calculation_sequence;
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

    bool values_available;
    bool window_complete;
    bool saturation_observed;
    bool calculation_error;

    MonotonicTimeMs value_monotonic_ms;
    bool value_age_available;
    uint32_t value_age_ms;
    bool civil_timestamp_available;
    Tr2CivilTimestamp civil_timestamp;
} SupervisionSnapshot;

#endif
