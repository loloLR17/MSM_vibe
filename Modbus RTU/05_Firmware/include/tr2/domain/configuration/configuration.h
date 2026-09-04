#ifndef TR2_DOMAIN_CONFIGURATION_H
#define TR2_DOMAIN_CONFIGURATION_H

#include <stdint.h>

#define TR2_CONFIGURATION_LABEL_LENGTH 32u

typedef struct {
    uint16_t sampling_frequency_hz;
    uint16_t axes_enable_mask;
    uint16_t full_scale_code;
    uint16_t acquisition_mode;
    uint16_t window_size_samples;
    uint16_t indicator_period_ms;
    uint32_t campaign_duration_s;
    uint16_t storage_mode;
    uint32_t storage_limit_mb;

    uint16_t supervision_enable_mask;
    uint16_t rms_warn_threshold_mg;
    uint16_t rms_alarm_threshold_mg;
    uint16_t peak_warn_threshold_mg;
    uint16_t peak_alarm_threshold_mg;
    uint16_t threshold_hysteresis_mg;
    uint16_t alarm_hold_time_ms;

    uint32_t campaign_context_id;
    uint32_t mission_id;
    char campaign_label[TR2_CONFIGURATION_LABEL_LENGTH];
    char mission_label[TR2_CONFIGURATION_LABEL_LENGTH];
    uint16_t operating_mode_code;
    uint16_t navigation_zone_code;
    uint16_t load_state_code;
    uint16_t sea_state_code;
} ConfigurationPayload;

typedef struct PreparedConfiguration {
    uint32_t generation;
    uint32_t config_id;
    uint32_t supplied_crc;
    ConfigurationPayload payload;
} PreparedConfiguration;

typedef struct ValidatedConfiguration {
    uint32_t generation;
    uint32_t config_id;
    uint32_t supplied_crc;
    ConfigurationPayload payload;
} ValidatedConfiguration;

typedef struct ActiveConfigurationSnapshot {
    uint32_t generation;
    uint32_t config_id;
    uint32_t revision_counter;
    ConfigurationPayload payload;
} ActiveConfigurationSnapshot;

#endif
