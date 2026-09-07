#include "tr2/application/system_runtime.h"

#include <string.h>

#define TR2_B1_STORAGE_AVAILABLE UINT16_C(1)
#define TR2_B1_ACQUISITION_STOPPED UINT16_C(0)
#define TR2_B1_ACQUISITION_RUNNING UINT16_C(1)

static uint16_t status_reset_cause_from_platform(ResetCause cause)
{
    switch (cause) {
    case RESET_CAUSE_POWER_ON: return UINT16_C(1);
    case RESET_CAUSE_SOFTWARE: return UINT16_C(2);
    case RESET_CAUSE_WATCHDOG: return UINT16_C(3);
    case RESET_CAUSE_BROWNOUT: return UINT16_C(4);
    case RESET_CAUSE_EXTERNAL: return UINT16_C(5);
    case RESET_CAUSE_UNKNOWN:
    default: return UINT16_C(0);
    }
}

Tr2Result system_runtime_build_boot_status_images(SystemRuntime *runtime)
{
    DiagnosticSnapshot diagnostic;
    SystemStateAggregationInput input;
    ActiveConfigurationSnapshot active_configuration;
    ModbusBlock1ProjectionSource b1_source;
    ModbusBlock7ProjectionSource b7_source;
    TimeSnapshot time_snapshot;
    const bool acquisition_active = runtime != NULL &&
        campaign_service_acquisition_running(&runtime->campaign_service);
    Tr2Result result;

    if (runtime == NULL || !runtime->initialized ||
        !runtime->p9_authorities_available || !runtime->fg_runtime_available) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (!diagnostic_service_snapshot(&runtime->diagnostic_service, &diagnostic)) {
        return TR2_ERROR_INTERNAL;
    }

    memset(&input, 0, sizeof(input));
    input.ready = true;
    input.acquisition_active = acquisition_active;
    input.active_configuration_valid = configuration_service_active_snapshot(
        &runtime->configuration_service, &active_configuration);
    if (time_service_get_snapshot(&runtime->time_service, &time_snapshot) == TR2_OK) {
        runtime->time_snapshot = time_snapshot;
        runtime->time_snapshot_available = true;
        input.time_valid = time_snapshot.civil_time_usable;
    }
    input.storage_available = true;
    input.uptime_s = (uint32_t)(runtime->deps.monotonic_clock->now_ms(
        runtime->deps.monotonic_clock->context) / UINT64_C(1000));
    input.last_reset_cause = status_reset_cause_from_platform(runtime->boot_context.reset_cause);
    input.storage_status = TR2_B1_STORAGE_AVAILABLE;
    input.acquisition_state = acquisition_active
                                  ? TR2_B1_ACQUISITION_RUNNING
                                  : TR2_B1_ACQUISITION_STOPPED;
    if (campaign_service_campaign_open(&runtime->campaign_service)) {
        input.active_campaign_id = runtime->campaign_service.active_metadata.campaign_id;
    }
    input.diagnostic = &diagnostic;

    result = system_state_aggregator_build(&runtime->system_state_aggregator,
                                           &input,
                                           &runtime->system_state_snapshot);
    if (result != TR2_OK) return result;

    runtime->diagnostic_snapshot = diagnostic;
    runtime->system_state_snapshot_available = true;

    memset(&b1_source, 0, sizeof(b1_source));
    b1_source.system_state = &runtime->system_state_snapshot;
    b1_source.time = runtime->time_snapshot_available ? &runtime->time_snapshot : NULL;
    result = modbus_project_b1(&b1_source, &runtime->b1_image);
    if (result != TR2_OK) {
        runtime->b1_image_available = false;
        return result;
    }
    runtime->b1_image_available = true;

    memset(&b7_source, 0, sizeof(b7_source));
    b7_source.diagnostic = &runtime->diagnostic_snapshot;
    b7_source.uptime_s = runtime->system_state_snapshot.uptime_s;
    b7_source.reset_cause = runtime->system_state_snapshot.last_reset_cause;
    result = modbus_project_b7(&b7_source, &runtime->b7_image);
    if (result != TR2_OK) {
        runtime->b7_image_available = false;
        return result;
    }
    runtime->b7_image_available = true;
    return TR2_OK;
}

#undef TR2_B1_ACQUISITION_RUNNING
#undef TR2_B1_ACQUISITION_STOPPED
#undef TR2_B1_STORAGE_AVAILABLE
