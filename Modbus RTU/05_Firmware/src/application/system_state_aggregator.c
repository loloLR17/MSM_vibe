#include "tr2/application/system_state_aggregator.h"

#include <stddef.h>
#include <string.h>

#define SYSTEM_STATUS_NOMINAL UINT16_C(1)
#define SYSTEM_STATUS_DEGRADED UINT16_C(2)
#define SYSTEM_STATUS_FAULT UINT16_C(3)
#define SYSTEM_FLAG_READY UINT16_C(0x0001)
#define SYSTEM_FLAG_ACQUISITION_ACTIVE UINT16_C(0x0002)
#define SYSTEM_FLAG_CONFIG_VALID UINT16_C(0x0004)
#define SYSTEM_FLAG_TIME_VALID UINT16_C(0x0008)
#define SYSTEM_FLAG_STORAGE_AVAILABLE UINT16_C(0x0010)
#define FAULT_SENSOR UINT16_C(0x0001)
#define FAULT_ACQUISITION UINT16_C(0x0002)
#define FAULT_STORAGE UINT16_C(0x0004)
#define FAULT_TIME UINT16_C(0x0008)
#define FAULT_CONFIG UINT16_C(0x0010)
#define FAULT_INTERNAL UINT16_C(0x0020)
#define WARNING_TEMPERATURE UINT16_C(0x0004)

Tr2Result system_state_aggregator_init(SystemStateAggregator *aggregator)
{
    if (aggregator == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memset(aggregator, 0, sizeof(*aggregator));
    aggregator->initialized = true;
    aggregator->next_generation = 1u;
    return TR2_OK;
}

Tr2Result system_state_aggregator_build(SystemStateAggregator *aggregator,
                                       const SystemStateAggregationInput *input,
                                       SystemStateSnapshot *snapshot)
{
    const DiagnosticFacts *diagnostic;
    uint16_t system_flags = 0u;
    uint16_t fault_flags = 0u;
    uint16_t warning_flags = 0u;

    if (aggregator == NULL || input == NULL || snapshot == NULL || input->diagnostic == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!aggregator->initialized) {
        return TR2_ERROR_INVALID_STATE;
    }

    diagnostic = &input->diagnostic->facts;
    if (input->ready) system_flags |= SYSTEM_FLAG_READY;
    if (input->acquisition_active) system_flags |= SYSTEM_FLAG_ACQUISITION_ACTIVE;
    if (input->active_configuration_valid) system_flags |= SYSTEM_FLAG_CONFIG_VALID;
    if (input->time_valid) system_flags |= SYSTEM_FLAG_TIME_VALID;
    if (input->storage_available) system_flags |= SYSTEM_FLAG_STORAGE_AVAILABLE;

    if (diagnostic->active_conditions.sensor_fault) fault_flags |= FAULT_SENSOR;
    if (diagnostic->active_conditions.acquisition_fault) fault_flags |= FAULT_ACQUISITION;
    if (diagnostic->active_conditions.storage_fault) fault_flags |= FAULT_STORAGE;
    if (diagnostic->active_conditions.time_fault) fault_flags |= FAULT_TIME;
    if (diagnostic->active_conditions.configuration_fault) fault_flags |= FAULT_CONFIG;
    if (diagnostic->active_conditions.memory_fault || diagnostic->active_conditions.firmware_fault ||
        diagnostic->active_conditions.internal_communication_fault) fault_flags |= FAULT_INTERNAL;
    if (diagnostic->active_conditions.temperature_out_of_range) warning_flags |= WARNING_TEMPERATURE;

    memset(snapshot, 0, sizeof(*snapshot));
    snapshot->generation = aggregator->next_generation++;
    snapshot->system_status = fault_flags != 0u || diagnostic->health == DIAGNOSTIC_HEALTH_CRITICAL
                                  ? SYSTEM_STATUS_FAULT
                                  : (warning_flags != 0u || diagnostic->health == DIAGNOSTIC_HEALTH_WARNING ||
                                     diagnostic->health == DIAGNOSTIC_HEALTH_DEGRADED)
                                        ? SYSTEM_STATUS_DEGRADED
                                        : SYSTEM_STATUS_NOMINAL;
    snapshot->system_flags = system_flags;
    snapshot->fault_flags = fault_flags;
    snapshot->warning_flags = warning_flags;
    snapshot->uptime_s = input->uptime_s;
    snapshot->last_reset_cause = input->last_reset_cause;
    snapshot->internal_temp_dC = diagnostic->internal_temperature_available ? diagnostic->internal_temp_dC : 0;
    snapshot->cpu_load_percent = input->cpu_load_percent;
    snapshot->memory_usage_percent = input->memory_usage_percent;
    snapshot->storage_status = input->storage_status;
    snapshot->storage_usage_percent = input->storage_usage_percent;
    snapshot->acquisition_state = input->acquisition_state;
    snapshot->active_campaign_id = input->active_campaign_id;
    snapshot->error_code = input->error_code;
    snapshot->warning_code = input->warning_code;
    return TR2_OK;
}
