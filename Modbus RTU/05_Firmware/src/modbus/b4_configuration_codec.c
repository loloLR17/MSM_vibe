#include "tr2/modbus/b4_configuration_codec.h"

#include <string.h>

#include "tr2/modbus/codec.h"

static void encode_fixed_ascii_32(const char input[TR2_CONFIGURATION_LABEL_LENGTH],
                                  uint16_t output[TR2_CONFIGURATION_LABEL_LENGTH / 2u])
{
    size_t index;

    for (index = 0u; index < (TR2_CONFIGURATION_LABEL_LENGTH / 2u); ++index) {
        const uint8_t high = (uint8_t)input[index * 2u];
        const uint8_t low = (uint8_t)input[(index * 2u) + 1u];
        output[index] = (uint16_t)(((uint16_t)high << 8u) | (uint16_t)low);
    }
}

static void encode_u32(uint32_t value, uint16_t *registers)
{
    modbus_codec_u32_to_msw_lsw(value, &registers[0], &registers[1]);
}

void tr2_b4_serialize_prepared_payload(const ConfigurationPayload *payload,
                                       uint16_t registers[TR2_B4_PREPARED_REGISTER_COUNT])
{
    memset(registers, 0, TR2_B4_PREPARED_REGISTER_COUNT * sizeof(registers[0]));

    registers[0] = payload->sampling_frequency_hz;
    registers[2] = payload->axes_enable_mask;
    registers[3] = payload->full_scale_code;
    registers[4] = payload->acquisition_mode;
    registers[5] = payload->window_size_samples;
    registers[6] = payload->indicator_period_ms;
    encode_u32(payload->campaign_duration_s, &registers[7]);
    registers[9] = payload->storage_mode;
    encode_u32(payload->storage_limit_mb, &registers[10]);

    registers[24] = payload->supervision_enable_mask;
    registers[25] = payload->rms_warn_threshold_mg;
    registers[26] = payload->rms_alarm_threshold_mg;
    registers[27] = payload->peak_warn_threshold_mg;
    registers[28] = payload->peak_alarm_threshold_mg;
    registers[29] = payload->threshold_hysteresis_mg;
    registers[30] = payload->alarm_hold_time_ms;

    encode_u32(payload->campaign_context_id, &registers[40]);
    encode_u32(payload->mission_id, &registers[42]);
    encode_fixed_ascii_32(payload->campaign_label, &registers[44]);
    encode_fixed_ascii_32(payload->mission_label, &registers[60]);
    registers[76] = payload->operating_mode_code;
    registers[77] = payload->navigation_zone_code;
    registers[78] = payload->load_state_code;
    registers[79] = payload->sea_state_code;
}

void tr2_b4_serialize_active_payload(const ConfigurationPayload *payload,
                                     uint16_t registers[TR2_B4_ACTIVE_REGISTER_COUNT])
{
    memset(registers, 0, TR2_B4_ACTIVE_REGISTER_COUNT * sizeof(registers[0]));

    registers[0] = payload->sampling_frequency_hz;
    registers[1] = payload->axes_enable_mask;
    registers[2] = payload->full_scale_code;
    registers[3] = payload->acquisition_mode;
    registers[4] = payload->window_size_samples;
    registers[5] = payload->indicator_period_ms;
    encode_u32(payload->campaign_duration_s, &registers[6]);
    registers[8] = payload->storage_mode;
    encode_u32(payload->storage_limit_mb, &registers[9]);

    registers[16] = payload->supervision_enable_mask;
    registers[17] = payload->rms_warn_threshold_mg;
    registers[18] = payload->rms_alarm_threshold_mg;
    registers[19] = payload->peak_warn_threshold_mg;
    registers[20] = payload->peak_alarm_threshold_mg;
    registers[21] = payload->threshold_hysteresis_mg;
    registers[22] = payload->alarm_hold_time_ms;

    encode_u32(payload->campaign_context_id, &registers[28]);
    encode_u32(payload->mission_id, &registers[30]);
    encode_fixed_ascii_32(payload->campaign_label, &registers[32]);
    encode_fixed_ascii_32(payload->mission_label, &registers[48]);
    registers[64] = payload->operating_mode_code;
    registers[65] = payload->navigation_zone_code;
    registers[66] = payload->load_state_code;
    registers[67] = payload->sea_state_code;
}

uint32_t tr2_b4_crc32_registers(const uint16_t *registers, size_t register_count)
{
    uint32_t crc = 0xFFFFFFFFu;
    size_t register_index;

    for (register_index = 0u; register_index < register_count; ++register_index) {
        const uint8_t bytes[2] = {
            (uint8_t)(registers[register_index] >> 8u),
            (uint8_t)(registers[register_index] & 0x00FFu)
        };
        size_t byte_index;

        for (byte_index = 0u; byte_index < 2u; ++byte_index) {
            uint8_t bit_index;
            crc ^= (uint32_t)bytes[byte_index];

            for (bit_index = 0u; bit_index < 8u; ++bit_index) {
                if ((crc & 1u) != 0u) {
                    crc = (crc >> 1u) ^ 0xEDB88320u;
                } else {
                    crc >>= 1u;
                }
            }
        }
    }

    return crc ^ 0xFFFFFFFFu;
}

uint32_t tr2_b4_prepared_payload_crc(const ConfigurationPayload *payload)
{
    uint16_t registers[TR2_B4_PREPARED_REGISTER_COUNT];

    tr2_b4_serialize_prepared_payload(payload, registers);
    return tr2_b4_crc32_registers(registers, TR2_B4_PREPARED_REGISTER_COUNT);
}

uint32_t tr2_b4_active_payload_crc(const ConfigurationPayload *payload)
{
    uint16_t registers[TR2_B4_ACTIVE_REGISTER_COUNT];

    tr2_b4_serialize_active_payload(payload, registers);
    return tr2_b4_crc32_registers(registers, TR2_B4_ACTIVE_REGISTER_COUNT);
}
