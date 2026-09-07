#include "tr2/persistence/campaign_record.h"

#include <string.h>

#include "tr2/persistence/configuration_record.h"

#define TR2_CAMPAIGN_RECORD_MAGIC UINT32_C(0x5452324D)
#define TR2_CAMPAIGN_RECORD_HEADER_SIZE 8u
#define TR2_CAMPAIGN_RECORD_FLAGS_OFFSET 16u
#define TR2_CAMPAIGN_RECORD_START_OFFSET 24u
#define TR2_CAMPAIGN_RECORD_END_OFFSET 28u
#define TR2_CAMPAIGN_RECORD_DURATION_OFFSET 32u
#define TR2_CAMPAIGN_RECORD_DATA_SIZE_OFFSET 36u
#define TR2_CAMPAIGN_RECORD_CAMPAIGN_LABEL_OFFSET 44u
#define TR2_CAMPAIGN_RECORD_MISSION_LABEL_OFFSET 76u
#define TR2_CAMPAIGN_RECORD_CONFIGURATION_OFFSET 108u
#define TR2_CAMPAIGN_RECORD_CRC_OFFSET 248u

#define TR2_CAMPAIGN_RECORD_FLAG_START_AVAILABLE UINT16_C(0x0001)
#define TR2_CAMPAIGN_RECORD_FLAG_END_AVAILABLE UINT16_C(0x0002)
#define TR2_CAMPAIGN_RECORD_FLAG_DURATION_AVAILABLE UINT16_C(0x0004)
#define TR2_CAMPAIGN_RECORD_KNOWN_FLAGS UINT16_C(0x0007)

_Static_assert(TR2_CAMPAIGN_RECORD_CONFIGURATION_OFFSET +
                   TR2_CONFIGURATION_RECORD_SIZE ==
               TR2_CAMPAIGN_RECORD_CRC_OFFSET,
               "campaign record payload size mismatch");
_Static_assert(TR2_CAMPAIGN_RECORD_CRC_OFFSET + sizeof(uint32_t) ==
                   TR2_CAMPAIGN_RECORD_SIZE,
               "campaign record size mismatch");

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

static void put_u64_be(uint8_t *output, uint64_t value)
{
    output[0] = (uint8_t)(value >> 56u);
    output[1] = (uint8_t)(value >> 48u);
    output[2] = (uint8_t)(value >> 40u);
    output[3] = (uint8_t)(value >> 32u);
    output[4] = (uint8_t)(value >> 24u);
    output[5] = (uint8_t)(value >> 16u);
    output[6] = (uint8_t)(value >> 8u);
    output[7] = (uint8_t)(value & UINT64_C(0x00000000000000FF));
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

static uint64_t get_u64_be(const uint8_t *input)
{
    return ((uint64_t)input[0] << 56u) |
           ((uint64_t)input[1] << 48u) |
           ((uint64_t)input[2] << 40u) |
           ((uint64_t)input[3] << 32u) |
           ((uint64_t)input[4] << 24u) |
           ((uint64_t)input[5] << 16u) |
           ((uint64_t)input[6] << 8u) |
           (uint64_t)input[7];
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

static bool lifecycle_is_valid(uint16_t value)
{
    return value == (uint16_t)CAMPAIGN_LIFECYCLE_OPEN ||
           value == (uint16_t)CAMPAIGN_LIFECYCLE_CLOSED;
}

static bool integrity_is_valid(uint16_t value)
{
    return value <= (uint16_t)CAMPAIGN_DATA_INTEGRITY_CORRUPTED;
}

static void historical_context_to_snapshot(const CampaignHistoricalContext *context,
                                           ActiveConfigurationSnapshot *snapshot)
{
    snapshot->generation = context->configuration_generation;
    snapshot->config_id = context->configuration_id;
    snapshot->revision_counter = context->configuration_revision_counter;
    snapshot->payload = context->configuration_payload;
}

static void snapshot_to_historical_context(const ActiveConfigurationSnapshot *snapshot,
                                           CampaignHistoricalContext *context)
{
    context->configuration_generation = snapshot->generation;
    context->configuration_id = snapshot->config_id;
    context->configuration_revision_counter = snapshot->revision_counter;
    context->configuration_payload = snapshot->payload;
}

Tr2Result tr2_campaign_record_encode(const CampaignMetadata *metadata,
                                     uint8_t *record,
                                     size_t record_size)
{
    ActiveConfigurationSnapshot snapshot;
    uint16_t flags = 0u;
    Tr2Result result;

    if (metadata == NULL || record == NULL || record_size != TR2_CAMPAIGN_RECORD_SIZE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (metadata->campaign_id == TR2_CAMPAIGN_ID_INVALID ||
        !lifecycle_is_valid((uint16_t)metadata->lifecycle_state) ||
        !integrity_is_valid((uint16_t)metadata->data_integrity)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(record, 0, record_size);
    put_u32_be(&record[0], TR2_CAMPAIGN_RECORD_MAGIC);
    put_u16_be(&record[4], TR2_CAMPAIGN_RECORD_FORMAT_VERSION);
    put_u16_be(&record[6], (uint16_t)TR2_CAMPAIGN_RECORD_SIZE);
    put_u32_be(&record[8], metadata->campaign_id);
    put_u32_be(&record[12], metadata->mission_id);

    if (metadata->start_timestamp.available) {
        flags |= TR2_CAMPAIGN_RECORD_FLAG_START_AVAILABLE;
        put_u32_be(&record[TR2_CAMPAIGN_RECORD_START_OFFSET], metadata->start_timestamp.value);
    }
    if (metadata->end_timestamp.available) {
        flags |= TR2_CAMPAIGN_RECORD_FLAG_END_AVAILABLE;
        put_u32_be(&record[TR2_CAMPAIGN_RECORD_END_OFFSET], metadata->end_timestamp.value);
    }
    if (metadata->duration.available) {
        flags |= TR2_CAMPAIGN_RECORD_FLAG_DURATION_AVAILABLE;
        put_u32_be(&record[TR2_CAMPAIGN_RECORD_DURATION_OFFSET], metadata->duration.seconds);
    }
    put_u16_be(&record[TR2_CAMPAIGN_RECORD_FLAGS_OFFSET], flags);
    put_u16_be(&record[18], (uint16_t)metadata->lifecycle_state);
    put_u16_be(&record[20], (uint16_t)metadata->data_integrity);
    put_u64_be(&record[TR2_CAMPAIGN_RECORD_DATA_SIZE_OFFSET], metadata->durable_data_size_bytes);
    memcpy(&record[TR2_CAMPAIGN_RECORD_CAMPAIGN_LABEL_OFFSET], metadata->campaign_label,
           TR2_CAMPAIGN_LABEL_LENGTH);
    memcpy(&record[TR2_CAMPAIGN_RECORD_MISSION_LABEL_OFFSET], metadata->mission_label,
           TR2_CAMPAIGN_LABEL_LENGTH);

    historical_context_to_snapshot(&metadata->historical_context, &snapshot);
    result = tr2_configuration_record_encode(
        &snapshot, &record[TR2_CAMPAIGN_RECORD_CONFIGURATION_OFFSET],
        TR2_CONFIGURATION_RECORD_SIZE);
    if (result != TR2_OK) {
        return result;
    }

    put_u32_be(&record[TR2_CAMPAIGN_RECORD_CRC_OFFSET],
               crc32_bytes(record, TR2_CAMPAIGN_RECORD_CRC_OFFSET));
    return TR2_OK;
}

Tr2Result tr2_campaign_record_decode(const uint8_t *record,
                                     size_t record_size,
                                     CampaignMetadata *metadata)
{
    ActiveConfigurationSnapshot snapshot;
    uint16_t flags;
    uint16_t lifecycle;
    uint16_t integrity;
    uint32_t stored_crc;
    Tr2Result result;

    if (record == NULL || metadata == NULL || record_size != TR2_CAMPAIGN_RECORD_SIZE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (get_u32_be(&record[0]) != TR2_CAMPAIGN_RECORD_MAGIC ||
        get_u16_be(&record[6]) != (uint16_t)TR2_CAMPAIGN_RECORD_SIZE) {
        return TR2_ERROR_CORRUPTED;
    }

    stored_crc = get_u32_be(&record[TR2_CAMPAIGN_RECORD_CRC_OFFSET]);
    if (stored_crc != crc32_bytes(record, TR2_CAMPAIGN_RECORD_CRC_OFFSET)) {
        return TR2_ERROR_CORRUPTED;
    }
    if (get_u16_be(&record[4]) != TR2_CAMPAIGN_RECORD_FORMAT_VERSION) {
        return TR2_ERROR_UNSUPPORTED;
    }

    flags = get_u16_be(&record[TR2_CAMPAIGN_RECORD_FLAGS_OFFSET]);
    lifecycle = get_u16_be(&record[18]);
    integrity = get_u16_be(&record[20]);
    if ((flags & (uint16_t)~TR2_CAMPAIGN_RECORD_KNOWN_FLAGS) != 0u ||
        get_u16_be(&record[22]) != 0u ||
        get_u32_be(&record[8]) == TR2_CAMPAIGN_ID_INVALID ||
        !lifecycle_is_valid(lifecycle) || !integrity_is_valid(integrity)) {
        return TR2_ERROR_CORRUPTED;
    }

    result = tr2_configuration_record_decode(
        &record[TR2_CAMPAIGN_RECORD_CONFIGURATION_OFFSET],
        TR2_CONFIGURATION_RECORD_SIZE, &snapshot);
    if (result != TR2_OK) {
        return result;
    }

    memset(metadata, 0, sizeof(*metadata));
    metadata->campaign_id = get_u32_be(&record[8]);
    metadata->mission_id = get_u32_be(&record[12]);
    metadata->start_timestamp.available =
        (flags & TR2_CAMPAIGN_RECORD_FLAG_START_AVAILABLE) != 0u;
    metadata->end_timestamp.available =
        (flags & TR2_CAMPAIGN_RECORD_FLAG_END_AVAILABLE) != 0u;
    metadata->duration.available =
        (flags & TR2_CAMPAIGN_RECORD_FLAG_DURATION_AVAILABLE) != 0u;
    if (metadata->start_timestamp.available) {
        metadata->start_timestamp.value = get_u32_be(&record[TR2_CAMPAIGN_RECORD_START_OFFSET]);
    }
    if (metadata->end_timestamp.available) {
        metadata->end_timestamp.value = get_u32_be(&record[TR2_CAMPAIGN_RECORD_END_OFFSET]);
    }
    if (metadata->duration.available) {
        metadata->duration.seconds = get_u32_be(&record[TR2_CAMPAIGN_RECORD_DURATION_OFFSET]);
    }
    metadata->durable_data_size_bytes = get_u64_be(&record[TR2_CAMPAIGN_RECORD_DATA_SIZE_OFFSET]);
    metadata->lifecycle_state = (CampaignLifecycleState)lifecycle;
    metadata->data_integrity = (CampaignDataIntegrity)integrity;
    memcpy(metadata->campaign_label, &record[TR2_CAMPAIGN_RECORD_CAMPAIGN_LABEL_OFFSET],
           TR2_CAMPAIGN_LABEL_LENGTH);
    memcpy(metadata->mission_label, &record[TR2_CAMPAIGN_RECORD_MISSION_LABEL_OFFSET],
           TR2_CAMPAIGN_LABEL_LENGTH);
    snapshot_to_historical_context(&snapshot, &metadata->historical_context);
    return TR2_OK;
}
