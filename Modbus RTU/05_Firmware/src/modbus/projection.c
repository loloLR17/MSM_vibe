#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "tr2/modbus/b4_configuration_codec.h"
#include "tr2/modbus/codec.h"
#include "tr2/modbus/projection.h"

#define TR2_B0_CAPABILITIES_MASK UINT16_C(0x000F)
#define TR2_B1_SYSTEM_FLAGS_MASK UINT16_C(0x001F)
#define TR2_B1_SYSTEM_FLAG_TIME_VALID UINT16_C(0x0008)
#define TR2_B1_FAULT_FLAGS_MASK UINT16_C(0x003F)
#define TR2_B1_WARNING_FLAGS_MASK UINT16_C(0x0007)
#define TR2_B2_TIME_FLAGS_MASK UINT16_C(0x00FF)
#define TR2_B2_TIME_FLAG_VALID UINT16_C(0x0001)
#define TR2_B2_TIME_FLAG_SYNC_PERFORMED UINT16_C(0x0002)
#define TR2_B2_TIME_FLAG_PREPARED_AVAILABLE UINT16_C(0x0008)
#define TR2_B2_TIME_STATUS_VALID_NOT_SYNCHRONIZED UINT16_C(2)
#define TR2_B2_TIME_STATUS_SYNCHRONIZED UINT16_C(3)
#define TR2_B3_VALIDITY_FLAG_WINDOW_COMPLETE UINT16_C(0x0008)
#define TR2_B3_VALIDITY_FLAG_SENSOR_NOT_SATURATED UINT16_C(0x0020)
#define TR2_B3_VALIDITY_FLAG_CALC_ERROR UINT16_C(0x0800)
#define TR2_B7_STRUCTURE_VERSION UINT16_C(1)

static bool b4_config_state_is_emittable(uint16_t state)
{
    return state == UINT16_C(0) ||
           state == UINT16_C(1) ||
           state == UINT16_C(2) ||
           state == UINT16_C(4) ||
           state == UINT16_C(5) ||
           state == UINT16_C(6);
}

static uint16_t project_b1_system_flags(const SystemStateSnapshot *system_state,
                                        const TimeSnapshot *time)
{
    uint16_t flags = (uint16_t)(system_state->system_flags & TR2_B1_SYSTEM_FLAGS_MASK);

    flags = (uint16_t)(flags & (uint16_t)~TR2_B1_SYSTEM_FLAG_TIME_VALID);
    if (time != NULL && time->civil_time_usable) {
        flags = (uint16_t)(flags | TR2_B1_SYSTEM_FLAG_TIME_VALID);
    }

    return flags;
}

static uint16_t project_b2_time_status(const TimeSnapshot *snapshot)
{
    if (snapshot->civil_time_usable &&
        snapshot->continuity == TIME_CONTINUITY_PROVEN &&
        snapshot->last_sync_history.state == LAST_SYNC_HISTORY_VALID) {
        return TR2_B2_TIME_STATUS_SYNCHRONIZED;
    }

    if (snapshot->civil_time_usable) {
        return TR2_B2_TIME_STATUS_VALID_NOT_SYNCHRONIZED;
    }

    return snapshot->time_status;
}

static uint16_t project_b2_time_flags(const TimeSnapshot *snapshot)
{
    uint16_t flags = (uint16_t)(snapshot->time_flags & TR2_B2_TIME_FLAGS_MASK);

    flags = (uint16_t)(flags &
                       (uint16_t)~(TR2_B2_TIME_FLAG_VALID |
                                   TR2_B2_TIME_FLAG_SYNC_PERFORMED |
                                   TR2_B2_TIME_FLAG_PREPARED_AVAILABLE));

    if (snapshot->civil_time_usable) {
        flags = (uint16_t)(flags | TR2_B2_TIME_FLAG_VALID);
    }
    if (snapshot->last_sync_history.state == LAST_SYNC_HISTORY_VALID) {
        flags = (uint16_t)(flags | TR2_B2_TIME_FLAG_SYNC_PERFORMED);
    }
    if (snapshot->prepared_time_available) {
        flags = (uint16_t)(flags | TR2_B2_TIME_FLAG_PREPARED_AVAILABLE);
    }

    return flags;
}

static uint16_t project_b3_validity_flags(const SupervisionSnapshot *snapshot)
{
    uint16_t flags = 0u;

    if (snapshot->window_complete) {
        flags = (uint16_t)(flags | TR2_B3_VALIDITY_FLAG_WINDOW_COMPLETE);
    }
    if (!snapshot->saturation_observed) {
        flags = (uint16_t)(flags | TR2_B3_VALIDITY_FLAG_SENSOR_NOT_SATURATED);
    }
    if (snapshot->calculation_error) {
        flags = (uint16_t)(flags | TR2_B3_VALIDITY_FLAG_CALC_ERROR);
    }

    return flags;
}

static uint16_t project_b7_fault_flags(const DiagnosticActiveConditions *conditions)
{
    uint16_t flags = 0u;

    if (conditions->sensor_fault) flags |= UINT16_C(0x0001);
    if (conditions->acquisition_fault) flags |= UINT16_C(0x0002);
    if (conditions->memory_fault) flags |= UINT16_C(0x0004);
    if (conditions->storage_fault) flags |= UINT16_C(0x0008);
    if (conditions->time_fault) flags |= UINT16_C(0x0010);
    if (conditions->configuration_fault) flags |= UINT16_C(0x0020);
    if (conditions->firmware_fault) flags |= UINT16_C(0x0040);
    if (conditions->overcurrent_fault) flags |= UINT16_C(0x0080);
    if (conditions->temperature_out_of_range) flags |= UINT16_C(0x0100);
    if (conditions->internal_communication_fault) flags |= UINT16_C(0x0200);

    return flags;
}

Tr2Result modbus_project_b0(const IdentitySnapshot *snapshot, ModbusBlock0Image *output)
{
    ModbusBlock0Image candidate = { { 0u }, 0u };

    if (snapshot == NULL || output == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    modbus_codec_u32_to_msw_lsw(snapshot->device_id,
                                &candidate.registers[0],
                                &candidate.registers[1]);
    candidate.registers[2] = snapshot->hardware_version;
    candidate.registers[3] = snapshot->firmware_version_major;
    candidate.registers[4] = snapshot->firmware_version_minor;
    candidate.registers[5] = snapshot->firmware_version_patch;
    candidate.registers[6] = snapshot->protocol_version;
    candidate.registers[7] = (uint16_t)(snapshot->device_capabilities & TR2_B0_CAPABILITIES_MASK);

    if (!modbus_codec_ascii_fixed_encode(snapshot->serial_number,
                                         TR2_IDENTITY_SERIAL_LENGTH,
                                         &candidate.registers[8],
                                         8u)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    if (!modbus_codec_ascii_fixed_encode(snapshot->manufacturer,
                                         TR2_IDENTITY_MANUFACTURER_LENGTH,
                                         &candidate.registers[16],
                                         4u)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    candidate.registers[20] = 0u;
    candidate.source_generation = snapshot->generation;
    *output = candidate;
    return TR2_OK;
}

Tr2Result modbus_project_b1(const ModbusBlock1ProjectionSource *source,
                            ModbusBlock1Image *output)
{
    ModbusBlock1Image candidate = { { 0u }, 0u };
    const SystemStateSnapshot *snapshot;

    if (source == NULL || source->system_state == NULL || output == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    snapshot = source->system_state;
    candidate.registers[0] = snapshot->system_status;
    candidate.registers[1] = project_b1_system_flags(snapshot, source->time);
    candidate.registers[2] = (uint16_t)(snapshot->fault_flags & TR2_B1_FAULT_FLAGS_MASK);
    candidate.registers[3] = (uint16_t)(snapshot->warning_flags & TR2_B1_WARNING_FLAGS_MASK);
    modbus_codec_u32_to_msw_lsw(snapshot->uptime_s,
                                &candidate.registers[4],
                                &candidate.registers[5]);
    candidate.registers[6] = snapshot->last_reset_cause;
    candidate.registers[7] = modbus_codec_i16_to_register(snapshot->internal_temp_dC);
    candidate.registers[8] = snapshot->cpu_load_percent;
    candidate.registers[9] = snapshot->memory_usage_percent;
    candidate.registers[10] = snapshot->storage_status;
    candidate.registers[11] = snapshot->storage_usage_percent;
    candidate.registers[12] = snapshot->acquisition_state;
    modbus_codec_u32_to_msw_lsw(snapshot->active_campaign_id,
                                &candidate.registers[13],
                                &candidate.registers[14]);
    candidate.registers[15] = snapshot->error_code;
    candidate.registers[16] = snapshot->warning_code;
    candidate.registers[17] = 0u;
    candidate.registers[18] = 0u;
    candidate.registers[19] = 0u;
    candidate.source_generation = snapshot->generation;

    *output = candidate;
    return TR2_OK;
}

Tr2Result modbus_project_b2(const TimeSnapshot *snapshot, ModbusBlock2Image *output)
{
    ModbusBlock2Image candidate = { { 0u }, 0u };

    if (snapshot == NULL || output == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!snapshot->synchronization_facts_available || !snapshot->current_time_available) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    candidate.registers[0] = project_b2_time_status(snapshot);
    candidate.registers[1] = project_b2_time_flags(snapshot);
    modbus_codec_u32_to_msw_lsw(snapshot->current_time,
                                &candidate.registers[2],
                                &candidate.registers[3]);
    modbus_codec_u32_to_msw_lsw(snapshot->last_sync_time,
                                &candidate.registers[4],
                                &candidate.registers[5]);
    modbus_codec_u32_to_msw_lsw(snapshot->time_since_sync_s,
                                &candidate.registers[6],
                                &candidate.registers[7]);
    modbus_codec_u32_to_msw_lsw(snapshot->prepared_time,
                                &candidate.registers[8],
                                &candidate.registers[9]);
    candidate.registers[10] = snapshot->prepared_time_status;
    candidate.registers[11] = snapshot->time_accuracy_ms;
    candidate.registers[12] = modbus_codec_i16_to_register(snapshot->drift_ppm);
    candidate.registers[13] = snapshot->sync_source;
    candidate.registers[14] = 0u;
    candidate.registers[15] = 0u;
    candidate.source_generation = snapshot->generation;

    *output = candidate;
    return TR2_OK;
}

Tr2Result modbus_project_b3(const SupervisionSnapshot *snapshot, ModbusBlock3Image *output)
{
    ModbusBlock3Image candidate = { { 0u }, 0u };

    if (snapshot == NULL || output == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!snapshot->values_available) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    candidate.registers[1] = project_b3_validity_flags(snapshot);

    if (snapshot->civil_timestamp_available) {
        modbus_codec_u32_to_msw_lsw(snapshot->civil_timestamp,
                                    &candidate.registers[4],
                                    &candidate.registers[5]);
    }
    if (snapshot->value_age_available) {
        modbus_codec_u32_to_msw_lsw(snapshot->value_age_ms,
                                    &candidate.registers[6],
                                    &candidate.registers[7]);
    }

    modbus_codec_u32_to_msw_lsw(snapshot->calculation_sequence,
                                &candidate.registers[8],
                                &candidate.registers[9]);
    modbus_codec_u32_to_msw_lsw(snapshot->window_duration_ms,
                                &candidate.registers[10],
                                &candidate.registers[11]);
    modbus_codec_u32_to_msw_lsw(snapshot->valid_sample_count,
                                &candidate.registers[12],
                                &candidate.registers[13]);
    modbus_codec_u32_to_msw_lsw(snapshot->rms_global_mg,
                                &candidate.registers[14],
                                &candidate.registers[15]);
    modbus_codec_u32_to_msw_lsw(snapshot->peak_global_mg,
                                &candidate.registers[16],
                                &candidate.registers[17]);
    modbus_codec_u32_to_msw_lsw(snapshot->rms_x_mg,
                                &candidate.registers[18],
                                &candidate.registers[19]);
    modbus_codec_u32_to_msw_lsw(snapshot->rms_y_mg,
                                &candidate.registers[20],
                                &candidate.registers[21]);
    modbus_codec_u32_to_msw_lsw(snapshot->rms_z_mg,
                                &candidate.registers[22],
                                &candidate.registers[23]);
    modbus_codec_u32_to_msw_lsw(snapshot->peak_x_mg,
                                &candidate.registers[24],
                                &candidate.registers[25]);
    modbus_codec_u32_to_msw_lsw(snapshot->peak_y_mg,
                                &candidate.registers[26],
                                &candidate.registers[27]);
    modbus_codec_u32_to_msw_lsw(snapshot->peak_z_mg,
                                &candidate.registers[28],
                                &candidate.registers[29]);

    candidate.source_calculation_sequence = snapshot->calculation_sequence;
    *output = candidate;
    return TR2_OK;
}

Tr2Result modbus_project_b4(const ModbusBlock4ProjectionSource *source, ModbusBlock4Image *output)
{
    ModbusBlock4Image candidate = { { 0u } };
    uint16_t payload_registers[TR2_B4_PREPARED_REGISTER_COUNT];

    if (source == NULL || output == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!b4_config_state_is_emittable(source->config_state)) {
        return TR2_ERROR_INVALID_STATE;
    }

    candidate.registers[0] = source->config_structure_version;
    candidate.registers[1] = source->config_capabilities_mask;
    candidate.registers[6] = source->config_state;
    candidate.registers[7] = source->config_error_code;

    if (source->prepared != NULL) {
        modbus_codec_u32_to_msw_lsw(source->prepared->config_id,
                                    &candidate.registers[2],
                                    &candidate.registers[3]);
        modbus_codec_u32_to_msw_lsw(source->prepared->supplied_crc,
                                    &candidate.registers[8],
                                    &candidate.registers[9]);
        tr2_b4_serialize_prepared_payload(&source->prepared->payload, payload_registers);
        memcpy(&candidate.registers[16],
               payload_registers,
               TR2_B4_PREPARED_REGISTER_COUNT * sizeof(payload_registers[0]));
    }

    if (source->active != NULL) {
        uint16_t active_registers[TR2_B4_ACTIVE_REGISTER_COUNT];
        const uint32_t active_crc = tr2_b4_active_payload_crc(&source->active->payload);

        modbus_codec_u32_to_msw_lsw(source->active->config_id,
                                    &candidate.registers[4],
                                    &candidate.registers[5]);
        modbus_codec_u32_to_msw_lsw(active_crc,
                                    &candidate.registers[10],
                                    &candidate.registers[11]);
        modbus_codec_u32_to_msw_lsw(source->active->revision_counter,
                                    &candidate.registers[12],
                                    &candidate.registers[13]);
        tr2_b4_serialize_active_payload(&source->active->payload, active_registers);
        memcpy(&candidate.registers[100],
               active_registers,
               TR2_B4_ACTIVE_REGISTER_COUNT * sizeof(active_registers[0]));
    } else {
        const ConfigurationPayload neutral_payload = { 0 };
        const uint32_t neutral_crc = tr2_b4_active_payload_crc(&neutral_payload);

        modbus_codec_u32_to_msw_lsw(neutral_crc,
                                    &candidate.registers[10],
                                    &candidate.registers[11]);
    }

    *output = candidate;
    return TR2_OK;
}

Tr2Result modbus_project_b7(const ModbusBlock7ProjectionSource *source,
                            ModbusBlock7Image *output)
{
    ModbusBlock7Image candidate = { { 0u }, 0u };
    const DiagnosticFacts *facts;

    if (source == NULL || source->diagnostic == NULL || output == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    facts = &source->diagnostic->facts;
    candidate.registers[0] = TR2_B7_STRUCTURE_VERSION;
    candidate.registers[1] = (uint16_t)facts->health;
    candidate.registers[2] = project_b7_fault_flags(&facts->active_conditions);
    if (facts->last_fault.present) {
        candidate.registers[3] = facts->last_fault.code;
        if (facts->last_fault.timestamp_available) {
            modbus_codec_u32_to_msw_lsw(facts->last_fault.timestamp,
                                        &candidate.registers[4],
                                        &candidate.registers[5]);
        }
    }
    candidate.registers[6] = (uint16_t)facts->selftest.state;
    candidate.registers[7] = facts->selftest.result_code;
    candidate.registers[8] = facts->selftest.detail;
    modbus_codec_u32_to_msw_lsw(source->uptime_s,
                                &candidate.registers[9],
                                &candidate.registers[10]);
    candidate.registers[11] = source->reset_cause;
    if (facts->internal_temperature_available) {
        candidate.registers[12] = modbus_codec_i16_to_register(facts->internal_temp_dC);
    }
    if (facts->supply_voltage_available) {
        candidate.registers[13] = facts->supply_voltage_mV;
    }
    candidate.registers[14] = 0u;
    candidate.registers[15] = 0u;
    candidate.source_generation = source->diagnostic->generation;

    *output = candidate;
    return TR2_OK;
}

#undef TR2_B0_CAPABILITIES_MASK
#undef TR2_B1_SYSTEM_FLAGS_MASK
#undef TR2_B1_SYSTEM_FLAG_TIME_VALID
#undef TR2_B1_FAULT_FLAGS_MASK
#undef TR2_B1_WARNING_FLAGS_MASK
#undef TR2_B2_TIME_FLAGS_MASK
#undef TR2_B2_TIME_FLAG_VALID
#undef TR2_B2_TIME_FLAG_SYNC_PERFORMED
#undef TR2_B2_TIME_FLAG_PREPARED_AVAILABLE
#undef TR2_B2_TIME_STATUS_VALID_NOT_SYNCHRONIZED
#undef TR2_B2_TIME_STATUS_SYNCHRONIZED
#undef TR2_B3_VALIDITY_FLAG_WINDOW_COMPLETE
#undef TR2_B3_VALIDITY_FLAG_SENSOR_NOT_SATURATED
#undef TR2_B3_VALIDITY_FLAG_CALC_ERROR
#undef TR2_B7_STRUCTURE_VERSION
