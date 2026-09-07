#include <assert.h>
#include <string.h>

#include "tr2/persistence/configuration_record.h"

#define TEST_RECORD_CRC_OFFSET 136u

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

static ActiveConfigurationSnapshot make_snapshot(void)
{
    ActiveConfigurationSnapshot snapshot;
    size_t index;

    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.generation = UINT32_C(0x01020304);
    snapshot.config_id = UINT32_C(0x11223344);
    snapshot.revision_counter = UINT32_C(0x55667788);
    snapshot.payload.sampling_frequency_hz = UINT16_C(0x1234);
    snapshot.payload.axes_enable_mask = UINT16_C(0x0007);
    snapshot.payload.full_scale_code = UINT16_C(0x0002);
    snapshot.payload.acquisition_mode = UINT16_C(0x0003);
    snapshot.payload.window_size_samples = UINT16_C(1024);
    snapshot.payload.indicator_period_ms = UINT16_C(500);
    snapshot.payload.campaign_duration_s = UINT32_C(0x10203040);
    snapshot.payload.storage_mode = UINT16_C(0x0001);
    snapshot.payload.storage_limit_mb = UINT32_C(0x50607080);
    snapshot.payload.supervision_enable_mask = UINT16_C(0x000F);
    snapshot.payload.rms_warn_threshold_mg = UINT16_C(101);
    snapshot.payload.rms_alarm_threshold_mg = UINT16_C(202);
    snapshot.payload.peak_warn_threshold_mg = UINT16_C(303);
    snapshot.payload.peak_alarm_threshold_mg = UINT16_C(404);
    snapshot.payload.threshold_hysteresis_mg = UINT16_C(55);
    snapshot.payload.alarm_hold_time_ms = UINT16_C(600);
    snapshot.payload.campaign_context_id = UINT32_C(0x90A0B0C0);
    snapshot.payload.mission_id = UINT32_C(0x0D0E0F10);

    for (index = 0u; index < TR2_CONFIGURATION_LABEL_LENGTH; ++index) {
        snapshot.payload.campaign_label[index] = (char)('A' + (index % 26u));
        snapshot.payload.mission_label[index] = (char)('a' + (index % 26u));
    }

    snapshot.payload.operating_mode_code = UINT16_C(11);
    snapshot.payload.navigation_zone_code = UINT16_C(22);
    snapshot.payload.load_state_code = UINT16_C(33);
    snapshot.payload.sea_state_code = UINT16_C(44);
    return snapshot;
}

static void assert_payload_equal(const ConfigurationPayload *expected,
                                 const ConfigurationPayload *actual)
{
    assert(expected->sampling_frequency_hz == actual->sampling_frequency_hz);
    assert(expected->axes_enable_mask == actual->axes_enable_mask);
    assert(expected->full_scale_code == actual->full_scale_code);
    assert(expected->acquisition_mode == actual->acquisition_mode);
    assert(expected->window_size_samples == actual->window_size_samples);
    assert(expected->indicator_period_ms == actual->indicator_period_ms);
    assert(expected->campaign_duration_s == actual->campaign_duration_s);
    assert(expected->storage_mode == actual->storage_mode);
    assert(expected->storage_limit_mb == actual->storage_limit_mb);
    assert(expected->supervision_enable_mask == actual->supervision_enable_mask);
    assert(expected->rms_warn_threshold_mg == actual->rms_warn_threshold_mg);
    assert(expected->rms_alarm_threshold_mg == actual->rms_alarm_threshold_mg);
    assert(expected->peak_warn_threshold_mg == actual->peak_warn_threshold_mg);
    assert(expected->peak_alarm_threshold_mg == actual->peak_alarm_threshold_mg);
    assert(expected->threshold_hysteresis_mg == actual->threshold_hysteresis_mg);
    assert(expected->alarm_hold_time_ms == actual->alarm_hold_time_ms);
    assert(expected->campaign_context_id == actual->campaign_context_id);
    assert(expected->mission_id == actual->mission_id);
    assert(memcmp(expected->campaign_label, actual->campaign_label,
                  TR2_CONFIGURATION_LABEL_LENGTH) == 0);
    assert(memcmp(expected->mission_label, actual->mission_label,
                  TR2_CONFIGURATION_LABEL_LENGTH) == 0);
    assert(expected->operating_mode_code == actual->operating_mode_code);
    assert(expected->navigation_zone_code == actual->navigation_zone_code);
    assert(expected->load_state_code == actual->load_state_code);
    assert(expected->sea_state_code == actual->sea_state_code);
}

static void test_round_trip(void)
{
    ActiveConfigurationSnapshot input = make_snapshot();
    ActiveConfigurationSnapshot output;
    uint8_t record[TR2_CONFIGURATION_RECORD_SIZE];

    memset(&output, 0xA5, sizeof(output));
    assert(tr2_configuration_record_encode(&input, record, sizeof(record)) == TR2_OK);
    assert(tr2_configuration_record_decode(record, sizeof(record), &output) == TR2_OK);
    assert(input.generation == output.generation);
    assert(input.config_id == output.config_id);
    assert(input.revision_counter == output.revision_counter);
    assert_payload_equal(&input.payload, &output.payload);
}

static void test_deterministic_header_and_big_endian_layout(void)
{
    ActiveConfigurationSnapshot input = make_snapshot();
    uint8_t record[TR2_CONFIGURATION_RECORD_SIZE];

    assert(tr2_configuration_record_encode(&input, record, sizeof(record)) == TR2_OK);
    assert(record[0] == (uint8_t)'T');
    assert(record[1] == (uint8_t)'R');
    assert(record[2] == (uint8_t)'2');
    assert(record[3] == (uint8_t)'C');
    assert(record[4] == UINT8_C(0x00));
    assert(record[5] == UINT8_C(0x01));
    assert(record[6] == UINT8_C(0x00));
    assert(record[7] == UINT8_C(0x8C));
    assert(record[8] == UINT8_C(0x01));
    assert(record[9] == UINT8_C(0x02));
    assert(record[10] == UINT8_C(0x03));
    assert(record[11] == UINT8_C(0x04));
    assert(record[12] == UINT8_C(0x11));
    assert(record[13] == UINT8_C(0x22));
    assert(record[14] == UINT8_C(0x33));
    assert(record[15] == UINT8_C(0x44));
    assert(record[16] == UINT8_C(0x55));
    assert(record[17] == UINT8_C(0x66));
    assert(record[18] == UINT8_C(0x77));
    assert(record[19] == UINT8_C(0x88));
    assert(record[20] == UINT8_C(0x12));
    assert(record[21] == UINT8_C(0x34));
}

static void test_crc_detects_payload_and_version_corruption(void)
{
    ActiveConfigurationSnapshot input = make_snapshot();
    ActiveConfigurationSnapshot output;
    uint8_t record[TR2_CONFIGURATION_RECORD_SIZE];

    assert(tr2_configuration_record_encode(&input, record, sizeof(record)) == TR2_OK);
    record[50] ^= UINT8_C(0x01);
    assert(tr2_configuration_record_decode(record, sizeof(record), &output) == TR2_ERROR_CORRUPTED);

    assert(tr2_configuration_record_encode(&input, record, sizeof(record)) == TR2_OK);
    record[5] = UINT8_C(0x02);
    assert(tr2_configuration_record_decode(record, sizeof(record), &output) == TR2_ERROR_CORRUPTED);
}

static void test_magic_length_and_supported_version_status(void)
{
    ActiveConfigurationSnapshot input = make_snapshot();
    ActiveConfigurationSnapshot output;
    uint8_t record[TR2_CONFIGURATION_RECORD_SIZE];
    uint32_t crc;

    assert(tr2_configuration_record_encode(&input, record, sizeof(record)) == TR2_OK);
    record[0] ^= UINT8_C(0x01);
    assert(tr2_configuration_record_decode(record, sizeof(record), &output) == TR2_ERROR_CORRUPTED);

    assert(tr2_configuration_record_encode(&input, record, sizeof(record)) == TR2_OK);
    record[7] = UINT8_C(0x8B);
    assert(tr2_configuration_record_decode(record, sizeof(record), &output) == TR2_ERROR_CORRUPTED);

    assert(tr2_configuration_record_encode(&input, record, sizeof(record)) == TR2_OK);
    record[5] = UINT8_C(0x02);
    crc = test_crc32_bytes(record, TEST_RECORD_CRC_OFFSET);
    test_put_u32_be(&record[TEST_RECORD_CRC_OFFSET], crc);
    assert(tr2_configuration_record_decode(record, sizeof(record), &output) == TR2_ERROR_UNSUPPORTED);
}

static void test_invalid_arguments(void)
{
    ActiveConfigurationSnapshot input = make_snapshot();
    ActiveConfigurationSnapshot output;
    uint8_t record[TR2_CONFIGURATION_RECORD_SIZE];

    assert(tr2_configuration_record_encode(NULL, record, sizeof(record)) == TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_configuration_record_encode(&input, NULL, sizeof(record)) == TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_configuration_record_encode(&input, record, sizeof(record) - 1u) == TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_configuration_record_decode(NULL, sizeof(record), &output) == TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_configuration_record_decode(record, sizeof(record), NULL) == TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_configuration_record_decode(record, sizeof(record) - 1u, &output) == TR2_ERROR_INVALID_ARGUMENT);
}

int main(void)
{
    test_round_trip();
    test_deterministic_header_and_big_endian_layout();
    test_crc_detects_payload_and_version_corruption();
    test_magic_length_and_supported_version_status();
    test_invalid_arguments();
    return 0;
}
