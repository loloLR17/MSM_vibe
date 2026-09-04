#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/domain/configuration/configuration.h"

static ConfigurationPayload make_payload(void)
{
    ConfigurationPayload payload = {0};

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

    memcpy(payload.campaign_label, "campaign", 8u);
    memcpy(payload.mission_label, "mission", 7u);

    return payload;
}

int main(void)
{
    const ConfigurationPayload payload = make_payload();
    PreparedConfiguration prepared = {0};
    ValidatedConfiguration validated = {0};
    ActiveConfigurationSnapshot active = {0};

    assert(sizeof(payload.campaign_label) == TR2_CONFIGURATION_LABEL_LENGTH);
    assert(sizeof(payload.mission_label) == TR2_CONFIGURATION_LABEL_LENGTH);

    prepared.generation = 10u;
    prepared.config_id = 42u;
    prepared.supplied_crc = 0x5207CCFCu;
    prepared.payload = payload;

    validated.generation = prepared.generation;
    validated.config_id = prepared.config_id;
    validated.supplied_crc = prepared.supplied_crc;
    validated.payload = prepared.payload;

    active.generation = 11u;
    active.config_id = validated.config_id;
    active.revision_counter = 7u;
    active.payload = validated.payload;

    prepared.payload.sampling_frequency_hz = 1u;
    validated.payload.sampling_frequency_hz = 2u;

    assert(active.config_id == 42u);
    assert(active.revision_counter == 7u);
    assert(active.payload.sampling_frequency_hz == 26667u);
    assert(prepared.payload.sampling_frequency_hz == 1u);
    assert(validated.payload.sampling_frequency_hz == 2u);

    return 0;
}
