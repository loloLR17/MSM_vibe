#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "tr2/modbus/b4_configuration_codec.h"
#include "tr2/modbus/codec.h"
#include "tr2/modbus/projection.h"

#define TR2_B0_CAPABILITIES_MASK UINT16_C(0x000F)
#define TR2_B1_SYSTEM_FLAGS_MASK UINT16_C(0x001F)
#define TR2_B1_FAULT_FLAGS_MASK UINT16_C(0x003F)
#define TR2_B1_WARNING_FLAGS_MASK UINT16_C(0x0007)
#define TR2_B2_TIME_FLAGS_MASK UINT16_C(0x00FF)

static bool b4_config_state_is_emittable(uint16_t state)
{
    return state == UINT16_C(0) ||
           state == UINT16_C(1) ||
           state == UINT16_C(2) ||
           state == UINT16_C(4) ||
           state == UINT16_C(5) ||
           state == UINT16_C(6);
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

Tr2Result modbus_project_b1(const SystemStateSnapshot *snapshot, ModbusBlock1Image *output)
{
    ModbusBlock1Image candidate = { { 0u }, 0u };

    if (snapshot == NULL || output == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    candidate.registers[0] = snapshot->system_status;
    candidate.registers[1] = (uint16_t)(snapshot->system_flags & TR2_B1_SYSTEM_FLAGS_MASK);
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

    candidate.registers[0] = snapshot->time_status;
    candidate.registers[1] = (uint16_t)(snapshot->time_flags & TR2_B2_TIME_FLAGS_MASK);
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

#undef TR2_B0_CAPABILITIES_MASK
#undef TR2_B1_SYSTEM_FLAGS_MASK
#undef TR2_B1_FAULT_FLAGS_MASK
#undef TR2_B1_WARNING_FLAGS_MASK
#undef TR2_B2_TIME_FLAGS_MASK
