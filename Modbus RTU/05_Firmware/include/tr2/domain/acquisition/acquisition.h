#ifndef TR2_DOMAIN_ACQUISITION_H
#define TR2_DOMAIN_ACQUISITION_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/domain/configuration/configuration.h"
#include "tr2/platform/monotonic_clock.h"

typedef struct {
    uint32_t generation;
    uint32_t config_id;
    uint32_t revision_counter;
    ConfigurationPayload payload;
} AcquisitionConfigurationContext;

typedef struct {
    AcquisitionConfigurationContext configuration;
    MonotonicTimeMs start_monotonic_ms;
    MonotonicTimeMs end_monotonic_ms;
    uint32_t acquired_sample_count;
    uint32_t valid_sample_count;

    uint64_t sum_square_x_mg2;
    uint64_t sum_square_y_mg2;
    uint64_t sum_square_z_mg2;
    uint64_t sum_square_vector_mg2;
    uint32_t peak_abs_x_mg;
    uint32_t peak_abs_y_mg;
    uint32_t peak_abs_z_mg;
    uint64_t peak_vector_square_mg2;

    bool complete;
    bool saturation_observed;
    bool source_error;
    bool statistics_overflow;
} AcquisitionWindow;

#endif
