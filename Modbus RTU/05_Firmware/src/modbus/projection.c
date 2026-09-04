#include <stddef.h>
#include "tr2/modbus/codec.h"
#include "tr2/modbus/projection.h"

#define TR2_B0_CAPABILITIES_MASK UINT16_C(0x000F)

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

#undef TR2_B0_CAPABILITIES_MASK
