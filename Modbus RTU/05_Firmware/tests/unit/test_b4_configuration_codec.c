#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/modbus/b4_configuration_codec.h"

static ConfigurationPayload make_zero_payload(void)
{
    ConfigurationPayload payload;
    memset(&payload, 0, sizeof(payload));
    return payload;
}

static void test_normative_prepared_crc_vector(void)
{
    ConfigurationPayload payload = make_zero_payload();

    payload.sampling_frequency_hz = 26667u;
    payload.axes_enable_mask = 0x0007u;
    payload.full_scale_code = 2u;
    payload.acquisition_mode = 1u;
    payload.window_size_samples = 32768u;
    payload.indicator_period_ms = 5000u;
    payload.campaign_duration_s = 3600u;
    payload.storage_mode = 1u;
    payload.storage_limit_mb = 512u;

    payload.supervision_enable_mask = 1u;
    payload.rms_warn_threshold_mg = 100u;
    payload.rms_alarm_threshold_mg = 200u;
    payload.peak_warn_threshold_mg = 300u;
    payload.peak_alarm_threshold_mg = 400u;
    payload.threshold_hysteresis_mg = 20u;
    payload.alarm_hold_time_ms = 500u;

    payload.campaign_context_id = 1u;
    payload.mission_id = 2u;
    payload.operating_mode_code = 1u;
    payload.navigation_zone_code = 2u;
    payload.load_state_code = 3u;
    payload.sea_state_code = 1u;

    assert(tr2_b4_prepared_payload_crc(&payload) == 0x5207CCFCu);
}

static void test_neutral_active_crc_vector(void)
{
    const ConfigurationPayload payload = make_zero_payload();

    assert(tr2_b4_active_payload_crc(&payload) == 0x177C92D9u);
}

static void test_prepared_serialization_positions(void)
{
    ConfigurationPayload payload = make_zero_payload();
    uint16_t registers[TR2_B4_PREPARED_REGISTER_COUNT];

    payload.campaign_duration_s = 0x12345678u;
    payload.storage_limit_mb = 0x89ABCDEFu;
    payload.campaign_context_id = 0x10203040u;
    payload.mission_id = 0x50607080u;
    payload.campaign_label[0] = 'A';
    payload.campaign_label[1] = 'B';
    payload.mission_label[30] = 'Y';
    payload.mission_label[31] = 'Z';

    tr2_b4_serialize_prepared_payload(&payload, registers);

    assert(registers[1] == 0u);
    assert(registers[7] == 0x1234u);
    assert(registers[8] == 0x5678u);
    assert(registers[10] == 0x89ABu);
    assert(registers[11] == 0xCDEFu);
    assert(registers[40] == 0x1020u);
    assert(registers[41] == 0x3040u);
    assert(registers[42] == 0x5060u);
    assert(registers[43] == 0x7080u);
    assert(registers[44] == 0x4142u);
    assert(registers[75] == 0x595Au);
    assert(registers[80] == 0u);
    assert(registers[83] == 0u);
}

static void test_active_serialization_is_compact_and_reserved_zero(void)
{
    ConfigurationPayload payload = make_zero_payload();
    uint16_t registers[TR2_B4_ACTIVE_REGISTER_COUNT];
    size_t index;

    payload.sampling_frequency_hz = 26667u;
    payload.axes_enable_mask = 7u;
    payload.supervision_enable_mask = 0x55AAu;
    payload.campaign_context_id = 0x01020304u;
    payload.operating_mode_code = 9u;

    tr2_b4_serialize_active_payload(&payload, registers);

    assert(registers[0] == 26667u);
    assert(registers[1] == 7u);
    assert(registers[16] == 0x55AAu);
    assert(registers[28] == 0x0102u);
    assert(registers[29] == 0x0304u);
    assert(registers[64] == 9u);

    for (index = 11u; index <= 15u; ++index) {
        assert(registers[index] == 0u);
    }
    for (index = 23u; index <= 27u; ++index) {
        assert(registers[index] == 0u);
    }
    for (index = 68u; index <= 75u; ++index) {
        assert(registers[index] == 0u);
    }
}

int main(void)
{
    test_normative_prepared_crc_vector();
    test_neutral_active_crc_vector();
    test_prepared_serialization_positions();
    test_active_serialization_is_compact_and_reserved_zero();
    return 0;
}
