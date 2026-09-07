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
    bool complete;
    bool saturation_observed;
    bool source_error;
} AcquisitionWindow;

#endif
