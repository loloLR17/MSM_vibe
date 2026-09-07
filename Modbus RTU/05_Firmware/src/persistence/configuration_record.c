#include "tr2/persistence/configuration_record.h"

#include <string.h>

#define TR2_CONFIGURATION_RECORD_MAGIC UINT32_C(0x54523243)
#define TR2_CONFIGURATION_RECORD_HEADER_SIZE 20u
#define TR2_CONFIGURATION_RECORD_PAYLOAD_SIZE 116u
#define TR2_CONFIGURATION_RECORD_CRC_OFFSET 136u

_Static_assert(TR2_CONFIGURATION_RECORD_HEADER_SIZE +
                   TR2_CONFIGURATION_RECORD_PAYLOAD_SIZE +
                   sizeof(uint32_t) ==
               TR2_CONFIGURATION_RECORD_SIZE,
               "configuration record size mismatch");

static void put_u16_be(uint8_t *output, uint16_t value)
{
    output[0] = (uint8_t)(value >> 8u);
    output[1] = (uint8_t)(value & UINT16_C(0x00FF));
}

static void put_u32_be(uint8_t *output, uint32_t value)
{
    output[0] = (uint8_t)(value >> 24u);
    output[1] = (uint8_t)(value >> 16u);
    output[2] = (uint8_t)(value >> 8u);
    output[3] = (uint8_t)(value & UINT32_C(0x000000FF));
}

static uint16_t get_u16_be(const uint8_t *input)
{
    return (uint16_t)(((uint16_t)input[0] << 8u) | (uint16_t)input[1]);
}

static uint32_t get_u32_be(const uint8_t *input)
{
    return ((uint32_t)input[0] << 24u) |
           ((uint32_t)input[1] << 16u) |
           ((uint32_t)input[2] << 8u) |
           (uint32_t)input[3];
}

static uint32_t crc32_bytes(const uint8_t *bytes, size_t byte_count)
{
    uint32_t crc = UINT32_C(0xFFFFFFFF);
    size_t byte_index;

    for (byte_index = 0u; byte_index < byte_count; ++byte_index) {
        uint8_t bit_index;
        crc ^= (uint32_t)bytes[byte_index];

        for (bit_index = 0u; bit_index < 8u; ++bit_index) {
            if ((crc & UINT32_C(1)) != 0u) {
                crc = (crc >> 1u) ^ UINT32_C(0xEDB88320);
            } else {
                crc >>= 1u;
            }
        }
    }

    return crc ^ UINT32_C(0xFFFFFFFF);
}

static void encode_payload(const ConfigurationPayload *payload, uint8_t *output)
{
    size_t offset = 0u;

    put_u16_be(&output[offset], payload->sampling_frequency_hz); offset += 2u;
    put_u16_be(&output[offset], payload->axes_enable_mask); offset += 2u;
    put_u16_be(&output[offset], payload->full_scale_code); offset += 2u;
    put_u16_be(&output[offset], payload->acquisition_mode); offset += 2u;
    put_u16_be(&output[offset], payload->window_size_samples); offset += 2u;
    put_u16_be(&output[offset], payload->indicator_period_ms); offset += 2u;
    put_u32_be(&output[offset], payload->campaign_duration_s); offset += 4u;
    put_u16_be(&output[offset], payload->storage_mode); offset += 2u;
    put_u32_be(&output[offset], payload->storage_limit_mb); offset += 4u;

    put_u16_be(&output[offset], payload->supervision_enable_mask); offset += 2u;
    put_u16_be(&output[offset], payload->rms_warn_threshold_mg); offset += 2u;
    put_u16_be(&output[offset], payload->rms_alarm_threshold_mg); offset += 2u;
    put_u16_be(&output[offset], payload->peak_warn_threshold_mg); offset += 2u;
    put_u16_be(&output[offset], payload->peak_alarm_threshold_mg); offset += 2u;
    put_u16_be(&output[offset], payload->threshold_hysteresis_mg); offset += 2u;
    put_u16_be(&output[offset], payload->alarm_hold_time_ms); offset += 2u;

    put_u32_be(&output[offset], payload->campaign_context_id); offset += 4u;
    put_u32_be(&output[offset], payload->mission_id); offset += 4u;
    memcpy(&output[offset], payload->campaign_label, TR2_CONFIGURATION_LABEL_LENGTH);
    offset += TR2_CONFIGURATION_LABEL_LENGTH;
    memcpy(&output[offset], payload->mission_label, TR2_CONFIGURATION_LABEL_LENGTH);
    offset += TR2_CONFIGURATION_LABEL_LENGTH;
    put_u16_be(&output[offset], payload->operating_mode_code); offset += 2u;
    put_u16_be(&output[offset], payload->navigation_zone_code); offset += 2u;
    put_u16_be(&output[offset], payload->load_state_code); offset += 2u;
    put_u16_be(&output[offset], payload->sea_state_code); offset += 2u;

    (void)offset;
}

static void decode_payload(const uint8_t *input, ConfigurationPayload *payload)
{
    size_t offset = 0u;

    memset(payload, 0, sizeof(*payload));

    payload->sampling_frequency_hz = get_u16_be(&input[offset]); offset += 2u;
    payload->axes_enable_mask = get_u16_be(&input[offset]); offset += 2u;
    payload->full_scale_code = get_u16_be(&input[offset]); offset += 2u;
    payload->acquisition_mode = get_u16_be(&input[offset]); offset += 2u;
    payload->window_size_samples = get_u16_be(&input[offset]); offset += 2u;
    payload->indicator_period_ms = get_u16_be(&input[offset]); offset += 2u;
    payload->campaign_duration_s = get_u32_be(&input[offset]); offset += 4u;
    payload->storage_mode = get_u16_be(&input[offset]); offset += 2u;
    payload->storage_limit_mb = get_u32_be(&input[offset]); offset += 4u;

    payload->supervision_enable_mask = get_u16_be(&input[offset]); offset += 2u;
    payload->rms_warn_threshold_mg = get_u16_be(&input[offset]); offset += 2u;
    payload->rms_alarm_threshold_mg = get_u16_be(&input[offset]); offset += 2u;
    payload->peak_warn_threshold_mg = get_u16_be(&input[offset]); offset += 2u;
    payload->peak_alarm_threshold_mg = get_u16_be(&input[offset]); offset += 2u;
    payload->threshold_hysteresis_mg = get_u16_be(&input[offset]); offset += 2u;
    payload->alarm_hold_time_ms = get_u16_be(&input[offset]); offset += 2u;

    payload->campaign_context_id = get_u32_be(&input[offset]); offset += 4u;
    payload->mission_id = get_u32_be(&input[offset]); offset += 4u;
    memcpy(payload->campaign_label, &input[offset], TR2_CONFIGURATION_LABEL_LENGTH);
    offset += TR2_CONFIGURATION_LABEL_LENGTH;
    memcpy(payload->mission_label, &input[offset], TR2_CONFIGURATION_LABEL_LENGTH);
    offset += TR2_CONFIGURATION_LABEL_LENGTH;
    payload->operating_mode_code = get_u16_be(&input[offset]); offset += 2u;
    payload->navigation_zone_code = get_u16_be(&input[offset]); offset += 2u;
    payload->load_state_code = get_u16_be(&input[offset]); offset += 2u;
    payload->sea_state_code = get_u16_be(&input[offset]); offset += 2u;

    (void)offset;
}

Tr2Result tr2_configuration_record_encode(const ActiveConfigurationSnapshot *snapshot,
                                          uint8_t *record,
                                          size_t record_size)
{
    uint32_t crc;

    if (snapshot == NULL || record == NULL || record_size != TR2_CONFIGURATION_RECORD_SIZE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(record, 0, record_size);
    put_u32_be(&record[0], TR2_CONFIGURATION_RECORD_MAGIC);
    put_u16_be(&record[4], TR2_CONFIGURATION_RECORD_FORMAT_VERSION);
    put_u16_be(&record[6], (uint16_t)TR2_CONFIGURATION_RECORD_SIZE);
    put_u32_be(&record[8], snapshot->generation);
    put_u32_be(&record[12], snapshot->config_id);
    put_u32_be(&record[16], snapshot->revision_counter);
    encode_payload(&snapshot->payload, &record[TR2_CONFIGURATION_RECORD_HEADER_SIZE]);

    crc = crc32_bytes(record, TR2_CONFIGURATION_RECORD_CRC_OFFSET);
    put_u32_be(&record[TR2_CONFIGURATION_RECORD_CRC_OFFSET], crc);
    return TR2_OK;
}

Tr2Result tr2_configuration_record_decode(const uint8_t *record,
                                          size_t record_size,
                                          ActiveConfigurationSnapshot *snapshot)
{
    uint32_t stored_crc;
    uint32_t computed_crc;

    if (record == NULL || snapshot == NULL || record_size != TR2_CONFIGURATION_RECORD_SIZE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    if (get_u32_be(&record[0]) != TR2_CONFIGURATION_RECORD_MAGIC) {
        return TR2_ERROR_CORRUPTED;
    }

    if (get_u16_be(&record[6]) != (uint16_t)TR2_CONFIGURATION_RECORD_SIZE) {
        return TR2_ERROR_CORRUPTED;
    }

    stored_crc = get_u32_be(&record[TR2_CONFIGURATION_RECORD_CRC_OFFSET]);
    computed_crc = crc32_bytes(record, TR2_CONFIGURATION_RECORD_CRC_OFFSET);
    if (stored_crc != computed_crc) {
        return TR2_ERROR_CORRUPTED;
    }

    if (get_u16_be(&record[4]) != TR2_CONFIGURATION_RECORD_FORMAT_VERSION) {
        return TR2_ERROR_UNSUPPORTED;
    }

    memset(snapshot, 0, sizeof(*snapshot));
    snapshot->generation = get_u32_be(&record[8]);
    snapshot->config_id = get_u32_be(&record[12]);
    snapshot->revision_counter = get_u32_be(&record[16]);
    decode_payload(&record[TR2_CONFIGURATION_RECORD_HEADER_SIZE], &snapshot->payload);
    return TR2_OK;
}
