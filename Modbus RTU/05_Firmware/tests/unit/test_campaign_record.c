#include <assert.h>
#include <string.h>

#include "tr2/persistence/campaign_record.h"

#define TEST_RECORD_CRC_OFFSET 248u

static uint32_t test_crc32_bytes(const uint8_t *bytes, size_t byte_count)
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

static void test_put_u32_be(uint8_t *output, uint32_t value)
{
    output[0] = (uint8_t)(value >> 24u);
    output[1] = (uint8_t)(value >> 16u);
    output[2] = (uint8_t)(value >> 8u);
    output[3] = (uint8_t)(value & UINT32_C(0x000000FF));
}

static void refresh_crc(uint8_t *record)
{
    test_put_u32_be(&record[TEST_RECORD_CRC_OFFSET],
                    test_crc32_bytes(record, TEST_RECORD_CRC_OFFSET));
}

static CampaignMetadata make_metadata(void)
{
    CampaignMetadata metadata;
    size_t index;

    memset(&metadata, 0, sizeof(metadata));
    metadata.campaign_id = UINT32_C(0x01020304);
    metadata.mission_id = UINT32_C(0x11223344);
    metadata.start_timestamp.available = true;
    metadata.start_timestamp.value = UINT32_C(0x55667788);
    metadata.end_timestamp.available = true;
    metadata.end_timestamp.value = UINT32_C(0x10203040);
    metadata.duration.available = true;
    metadata.duration.seconds = UINT32_C(123456);
    metadata.durable_data_size_bytes = UINT64_C(0x0102030405060708);
    metadata.lifecycle_state = CAMPAIGN_LIFECYCLE_CLOSED;
    metadata.data_integrity = CAMPAIGN_DATA_INTEGRITY_COMPLETE;
    metadata.historical_context.configuration_generation = UINT32_C(7);
    metadata.historical_context.configuration_id = UINT32_C(42);
    metadata.historical_context.configuration_revision_counter = UINT32_C(3);
    metadata.historical_context.configuration_payload.sampling_frequency_hz = UINT16_C(26667);
    metadata.historical_context.configuration_payload.axes_enable_mask = UINT16_C(7);
    metadata.historical_context.configuration_payload.window_size_samples = UINT16_C(8192);
    metadata.historical_context.configuration_payload.mission_id = metadata.mission_id;

    for (index = 0u; index < TR2_CAMPAIGN_LABEL_LENGTH; ++index) {
        metadata.campaign_label[index] = (char)('A' + (index % 26u));
        metadata.mission_label[index] = (char)('a' + (index % 26u));
        metadata.historical_context.configuration_payload.campaign_label[index] =
            metadata.campaign_label[index];
        metadata.historical_context.configuration_payload.mission_label[index] =
            metadata.mission_label[index];
    }
    return metadata;
}

static void test_round_trip_preserves_critical_metadata(void)
{
    CampaignMetadata input = make_metadata();
    CampaignMetadata output;
    uint8_t record[TR2_CAMPAIGN_RECORD_SIZE];

    memset(&output, 0xA5, sizeof(output));
    assert(tr2_campaign_record_encode(&input, record, sizeof(record)) == TR2_OK);
    assert(tr2_campaign_record_decode(record, sizeof(record), &output) == TR2_OK);
    assert(output.campaign_id == input.campaign_id);
    assert(output.mission_id == input.mission_id);
    assert(output.start_timestamp.available &&
           output.start_timestamp.value == input.start_timestamp.value);
    assert(output.end_timestamp.available &&
           output.end_timestamp.value == input.end_timestamp.value);
    assert(output.duration.available && output.duration.seconds == input.duration.seconds);
    assert(output.durable_data_size_bytes == input.durable_data_size_bytes);
    assert(output.lifecycle_state == input.lifecycle_state);
    assert(output.data_integrity == input.data_integrity);
    assert(memcmp(output.campaign_label, input.campaign_label,
                  TR2_CAMPAIGN_LABEL_LENGTH) == 0);
    assert(memcmp(output.mission_label, input.mission_label,
                  TR2_CAMPAIGN_LABEL_LENGTH) == 0);
    assert(output.historical_context.configuration_generation ==
           input.historical_context.configuration_generation);
    assert(output.historical_context.configuration_id ==
           input.historical_context.configuration_id);
    assert(output.historical_context.configuration_revision_counter ==
           input.historical_context.configuration_revision_counter);
    assert(memcmp(&output.historical_context.configuration_payload,
                  &input.historical_context.configuration_payload,
                  sizeof(ConfigurationPayload)) == 0);
}

static void test_unavailable_temporal_values_are_canonicalized(void)
{
    CampaignMetadata input = make_metadata();
    CampaignMetadata output;
    uint8_t record[TR2_CAMPAIGN_RECORD_SIZE];

    input.start_timestamp.available = false;
    input.start_timestamp.value = UINT32_C(99);
    input.end_timestamp.available = false;
    input.end_timestamp.value = UINT32_C(88);
    input.duration.available = false;
    input.duration.seconds = UINT32_C(77);

    assert(tr2_campaign_record_encode(&input, record, sizeof(record)) == TR2_OK);
    assert(tr2_campaign_record_decode(record, sizeof(record), &output) == TR2_OK);
    assert(!output.start_timestamp.available && output.start_timestamp.value == 0u);
    assert(!output.end_timestamp.available && output.end_timestamp.value == 0u);
    assert(!output.duration.available && output.duration.seconds == 0u);
}

static void test_header_crc_and_version_are_distinguished(void)
{
    CampaignMetadata input = make_metadata();
    CampaignMetadata output;
    uint8_t record[TR2_CAMPAIGN_RECORD_SIZE];

    assert(tr2_campaign_record_encode(&input, record, sizeof(record)) == TR2_OK);
    assert(record[0] == (uint8_t)'T');
    assert(record[1] == (uint8_t)'R');
    assert(record[2] == (uint8_t)'2');
    assert(record[3] == (uint8_t)'M');
    assert(record[4] == UINT8_C(0x00) && record[5] == UINT8_C(0x01));
    assert(record[6] == UINT8_C(0x00) && record[7] == UINT8_C(0xFC));

    record[50] ^= UINT8_C(1);
    assert(tr2_campaign_record_decode(record, sizeof(record), &output) == TR2_ERROR_CORRUPTED);

    assert(tr2_campaign_record_encode(&input, record, sizeof(record)) == TR2_OK);
    record[5] = UINT8_C(2);
    refresh_crc(record);
    assert(tr2_campaign_record_decode(record, sizeof(record), &output) == TR2_ERROR_UNSUPPORTED);
}

static void test_structural_validation_rejects_invalid_metadata(void)
{
    CampaignMetadata input = make_metadata();
    CampaignMetadata output;
    uint8_t record[TR2_CAMPAIGN_RECORD_SIZE];

    input.campaign_id = TR2_CAMPAIGN_ID_INVALID;
    assert(tr2_campaign_record_encode(&input, record, sizeof(record)) == TR2_ERROR_INVALID_ARGUMENT);

    input = make_metadata();
    assert(tr2_campaign_record_encode(&input, record, sizeof(record)) == TR2_OK);
    record[17] |= UINT8_C(0x80);
    refresh_crc(record);
    assert(tr2_campaign_record_decode(record, sizeof(record), &output) == TR2_ERROR_CORRUPTED);

    assert(tr2_campaign_record_encode(&input, record, sizeof(record)) == TR2_OK);
    record[19] = UINT8_C(0x7F);
    refresh_crc(record);
    assert(tr2_campaign_record_decode(record, sizeof(record), &output) == TR2_ERROR_CORRUPTED);

    assert(tr2_campaign_record_encode(&input, record, sizeof(record)) == TR2_OK);
    record[21] = UINT8_C(0x7F);
    refresh_crc(record);
    assert(tr2_campaign_record_decode(record, sizeof(record), &output) == TR2_ERROR_CORRUPTED);
}

static void test_nested_configuration_record_integrity_is_preserved(void)
{
    CampaignMetadata input = make_metadata();
    CampaignMetadata output;
    uint8_t record[TR2_CAMPAIGN_RECORD_SIZE];

    assert(tr2_campaign_record_encode(&input, record, sizeof(record)) == TR2_OK);
    record[120] ^= UINT8_C(1);
    refresh_crc(record);
    assert(tr2_campaign_record_decode(record, sizeof(record), &output) == TR2_ERROR_CORRUPTED);
}

static void test_invalid_arguments(void)
{
    CampaignMetadata input = make_metadata();
    CampaignMetadata output;
    uint8_t record[TR2_CAMPAIGN_RECORD_SIZE];

    assert(tr2_campaign_record_encode(NULL, record, sizeof(record)) == TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_campaign_record_encode(&input, NULL, sizeof(record)) == TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_campaign_record_encode(&input, record, sizeof(record) - 1u) == TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_campaign_record_decode(NULL, sizeof(record), &output) == TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_campaign_record_decode(record, sizeof(record), NULL) == TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_campaign_record_decode(record, sizeof(record) - 1u, &output) == TR2_ERROR_INVALID_ARGUMENT);
}

int main(void)
{
    test_round_trip_preserves_critical_metadata();
    test_unavailable_temporal_values_are_canonicalized();
    test_header_crc_and_version_are_distinguished();
    test_structural_validation_rejects_invalid_metadata();
    test_nested_configuration_record_integrity_is_preserved();
    test_invalid_arguments();
    return 0;
}
