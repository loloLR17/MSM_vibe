#include "tr2/application/supervision_service.h"

#include <limits.h>
#include <string.h>

#include "tr2/domain/supervision/indicator_calculator.h"

static uint32_t next_sequence_value(uint32_t current)
{
    return current == UINT32_MAX ? UINT32_MAX : current + 1u;
}

static uint32_t saturating_age_ms(MonotonicTimeMs now_ms,
                                  MonotonicTimeMs value_ms)
{
    const uint64_t age_ms = now_ms - value_ms;
    return age_ms > UINT32_MAX ? UINT32_MAX : (uint32_t)age_ms;
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

Tr2Result supervision_service_bind_temporal_dependencies(
    SupervisionService *service,
    const MonotonicClock *monotonic_clock,
    const TimeService *time_service)
{
    if (service == NULL || monotonic_clock == NULL ||
        monotonic_clock->now_ms == NULL || time_service == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!service->initialized || !time_service->initialized) {
        return TR2_ERROR_INVALID_STATE;
    }

    service->monotonic_clock = monotonic_clock;
    service->time_service = time_service;
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
    TimeSnapshot time_snapshot;
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

    if (service->time_service != NULL &&
        time_service_get_snapshot(service->time_service, &time_snapshot) == TR2_OK &&
        time_snapshot.current_time_available) {
        candidate.civil_timestamp_available = true;
        candidate.civil_timestamp = time_snapshot.current_time;
    }

    service->snapshot = candidate;
    service->has_snapshot = true;
    service->next_calculation_sequence = next_sequence_value(service->next_calculation_sequence);

    return TR2_OK;
}

bool supervision_service_snapshot(const SupervisionService *service,
                                  SupervisionSnapshot *out_snapshot)
{
    MonotonicTimeMs now_ms;

    if (service == NULL || !service->initialized || out_snapshot == NULL ||
        !service->has_snapshot) {
        return false;
    }

    *out_snapshot = service->snapshot;
    if (service->monotonic_clock == NULL || service->monotonic_clock->now_ms == NULL) {
        return true;
    }

    now_ms = service->monotonic_clock->now_ms(service->monotonic_clock->context);
    if (now_ms < out_snapshot->value_monotonic_ms) {
        out_snapshot->value_age_available = false;
        out_snapshot->value_age_ms = 0u;
        return true;
    }

    out_snapshot->value_age_available = true;
    out_snapshot->value_age_ms = saturating_age_ms(now_ms,
                                                   out_snapshot->value_monotonic_ms);
    return true;
}
