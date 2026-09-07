#include "tr2/application/supervision_service.h"

#include <limits.h>
#include <string.h>

#include "tr2/domain/supervision/indicator_calculator.h"

static uint32_t next_sequence_value(uint32_t current)
{
    return current == UINT32_MAX ? UINT32_MAX : current + 1u;
}

Tr2Result supervision_service_init(SupervisionService *service)
{
    if (service == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(service, 0, sizeof(*service));
    service->initialized = true;
    service->next_calculation_sequence = 1u;
    return TR2_OK;
}

bool supervision_service_is_initialized(const SupervisionService *service)
{
    return service != NULL && service->initialized;
}

Tr2Result supervision_service_publish_window(SupervisionService *service,
                                             const AcquisitionWindow *window)
{
    SupervisionSnapshot candidate;
    VibrationIndicators indicators;
    Tr2Result result;

    if (service == NULL || window == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!service->initialized) {
        return TR2_ERROR_INVALID_STATE;
    }

    result = vibration_indicators_from_window(window, &indicators);
    if (result != TR2_OK) {
        return result;
    }

    memset(&candidate, 0, sizeof(candidate));
    candidate.configuration = window->configuration;
    candidate.calculation_sequence = service->next_calculation_sequence;
    candidate.window_duration_ms = indicators.window_duration_ms;
    candidate.valid_sample_count = indicators.valid_sample_count;
    candidate.rms_global_mg = indicators.rms_global_mg;
    candidate.peak_global_mg = indicators.peak_global_mg;
    candidate.rms_x_mg = indicators.rms_x_mg;
    candidate.rms_y_mg = indicators.rms_y_mg;
    candidate.rms_z_mg = indicators.rms_z_mg;
    candidate.peak_x_mg = indicators.peak_x_mg;
    candidate.peak_y_mg = indicators.peak_y_mg;
    candidate.peak_z_mg = indicators.peak_z_mg;
    candidate.values_available = true;
    candidate.window_complete = window->complete;
    candidate.saturation_observed = window->saturation_observed;
    candidate.calculation_error = false;
    candidate.value_monotonic_ms = window->end_monotonic_ms;
    candidate.civil_timestamp_available = false;

    service->snapshot = candidate;
    service->has_snapshot = true;
    service->next_calculation_sequence = next_sequence_value(service->next_calculation_sequence);

    return TR2_OK;
}

bool supervision_service_snapshot(const SupervisionService *service,
                                  SupervisionSnapshot *out_snapshot)
{
    if (service == NULL || !service->initialized || out_snapshot == NULL ||
        !service->has_snapshot) {
        return false;
    }

    *out_snapshot = service->snapshot;
    return true;
}
