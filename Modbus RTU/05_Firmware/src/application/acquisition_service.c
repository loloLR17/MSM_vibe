#include <limits.h>
#include <string.h>

#include "tr2/application/acquisition_service.h"

static bool acquisition_service_dependencies_valid(
    ConfigurationService *configuration_service,
    const MonotonicClock *monotonic_clock,
    VibrationSource *vibration_source)
{
    return configuration_service != NULL &&
           monotonic_clock != NULL &&
           monotonic_clock->now_ms != NULL &&
           vibration_source != NULL &&
           vibration_source->configure != NULL &&
           vibration_source->start != NULL &&
           vibration_source->read_sample != NULL &&
           vibration_source->stop != NULL;
}

static AcquisitionConfigurationContext acquisition_configuration_from_active(
    const ActiveConfigurationSnapshot *active)
{
    AcquisitionConfigurationContext context;

    context.generation = active->generation;
    context.config_id = active->config_id;
    context.revision_counter = active->revision_counter;
    context.payload = active->payload;

    return context;
}

static VibrationSourceConfiguration vibration_source_configuration_from_context(
    const AcquisitionConfigurationContext *context)
{
    VibrationSourceConfiguration configuration;

    configuration.sampling_frequency_hz = context->payload.sampling_frequency_hz;
    configuration.axes_enable_mask = context->payload.axes_enable_mask;
    configuration.full_scale_code = context->payload.full_scale_code;

    return configuration;
}

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

static bool acquisition_window_accumulate_valid_sample(AcquisitionWindow *window,
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

Tr2Result acquisition_service_init(AcquisitionService *service,
                                   ConfigurationService *configuration_service,
                                   const MonotonicClock *monotonic_clock,
                                   VibrationSource *vibration_source)
{
    if (service == NULL ||
        !acquisition_service_dependencies_valid(configuration_service,
                                                monotonic_clock,
                                                vibration_source)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(service, 0, sizeof(*service));
    service->configuration_service = configuration_service;
    service->monotonic_clock = monotonic_clock;
    service->vibration_source = vibration_source;
    service->initialized = true;

    return TR2_OK;
}

bool acquisition_service_is_initialized(const AcquisitionService *service)
{
    return service != NULL && service->initialized;
}

bool acquisition_service_window_active(const AcquisitionService *service)
{
    return service != NULL && service->initialized && service->window_active;
}

Tr2Result acquisition_service_begin_window(AcquisitionService *service)
{
    ActiveConfigurationSnapshot active;
    VibrationSourceConfiguration source_configuration;
    Tr2Result result;

    if (service == NULL || !service->initialized) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (service->window_active) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (!configuration_service_active_snapshot(service->configuration_service, &active)) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    memset(&service->current_window, 0, sizeof(service->current_window));
    service->current_window.configuration = acquisition_configuration_from_active(&active);
    source_configuration = vibration_source_configuration_from_context(
        &service->current_window.configuration);

    result = service->vibration_source->configure(service->vibration_source->context,
                                                   &source_configuration);
    if (result != TR2_OK) {
        return result;
    }

    result = service->vibration_source->start(service->vibration_source->context);
    if (result != TR2_OK) {
        return result;
    }

    service->source_started = true;
    service->current_window.start_monotonic_ms =
        service->monotonic_clock->now_ms(service->monotonic_clock->context);
    service->window_active = true;

    return TR2_OK;
}

Tr2Result acquisition_service_read_sample(AcquisitionService *service,
                                          VibrationSample *out_sample)
{
    Tr2Result result;

    if (service == NULL || out_sample == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!service->initialized || !service->window_active || !service->source_started) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (service->current_window.acquired_sample_count >=
        service->current_window.configuration.payload.window_size_samples) {
        return TR2_ERROR_INVALID_STATE;
    }

    result = service->vibration_source->read_sample(service->vibration_source->context,
                                                     out_sample);
    if (result != TR2_OK) {
        service->current_window.source_error = true;
        return result;
    }

    service->current_window.acquired_sample_count++;
    if (out_sample->valid) {
        service->current_window.valid_sample_count++;
        if (!service->current_window.statistics_overflow &&
            !acquisition_window_accumulate_valid_sample(&service->current_window,
                                                        out_sample)) {
            service->current_window.statistics_overflow = true;
        }
    }
    if (out_sample->saturated) {
        service->current_window.saturation_observed = true;
    }

    return TR2_OK;
}

Tr2Result acquisition_service_end_window(AcquisitionService *service,
                                         AcquisitionWindow *out_window)
{
    Tr2Result stop_result;

    if (service == NULL || out_window == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!service->initialized || !service->window_active || !service->source_started) {
        return TR2_ERROR_INVALID_STATE;
    }

    stop_result = service->vibration_source->stop(service->vibration_source->context);
    if (stop_result != TR2_OK) {
        service->current_window.source_error = true;
    }

    service->current_window.end_monotonic_ms =
        service->monotonic_clock->now_ms(service->monotonic_clock->context);
    service->current_window.complete =
        !service->current_window.source_error &&
        service->current_window.acquired_sample_count ==
            service->current_window.configuration.payload.window_size_samples;

    *out_window = service->current_window;
    service->window_active = false;
    service->source_started = false;

    return stop_result;
}
