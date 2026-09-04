#include <stddef.h>
#include "tr2/modbus/codec.h"
#include "tr2/modbus/projection.h"

#define TR2_B0_CAPABILITIES_MASK UINT16_C(0x000F)
#define TR2_B1_SYSTEM_FLAGS_MASK UINT16_C(0x001F)
#define TR2_B1_FAULT_FLAGS_MASK UINT16_C(0x003F)
#define TR2_B1_WARNING_FLAGS_MASK UINT16_C(0x0007)

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

#undef TR2_B0_CAPABILITIES_MASK
#undef TR2_B1_SYSTEM_FLAGS_MASK
#undef TR2_B1_FAULT_FLAGS_MASK
#undef TR2_B1_WARNING_FLAGS_MASK
