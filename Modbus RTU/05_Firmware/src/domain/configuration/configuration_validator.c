#include "tr2/domain/configuration/configuration_validator.h"

#include <stddef.h>

static bool window_size_is_valid(uint16_t value)
{
    return value == 4096u || value == 8192u || value == 16384u || value == 32768u;
}

static bool indicator_period_is_valid(uint16_t value)
{
    return value == 2000u || value == 5000u || value == 10000u ||
           value == 30000u || value == 60000u;
}

static bool static_constraints_are_valid(const PreparedConfiguration *prepared)
{
    const ConfigurationPayload *payload = &prepared->payload;

    if (prepared->config_id == 0u ||
        payload->campaign_context_id == 0u ||
        payload->mission_id == 0u) {
        return false;
    }

    if (payload->sampling_frequency_hz != 26667u) {
        return false;
    }

    if (payload->axes_enable_mask == 0u || payload->axes_enable_mask > 0x0007u) {
        return false;
    }

    if (payload->full_scale_code > 3u || payload->acquisition_mode != 1u) {
        return false;
    }

    if (!window_size_is_valid(payload->window_size_samples) ||
        !indicator_period_is_valid(payload->indicator_period_ms)) {
        return false;
    }

    if (payload->campaign_duration_s < 60u || payload->campaign_duration_s > 604800u) {
        return false;
    }

    if (payload->storage_mode != 1u || payload->storage_limit_mb == 0u) {
        return false;
    }

    if ((uint64_t)payload->indicator_period_ms * (uint64_t)payload->sampling_frequency_hz <
        1000u * (uint64_t)payload->window_size_samples) {
        return false;
    }

    return true;
}

ConfigurationValidationResult configuration_validate(
    const PreparedConfiguration *prepared,
    const ConfigurationValidationEnvironment *environment,
    ValidatedConfiguration *out_validated)
{
    ConfigurationValidationResult result = {CONFIGURATION_VALIDATION_INVALID};

    if (!static_constraints_are_valid(prepared)) {
        return result;
    }

    if (!environment->storage_capacity_known) {
        result.status = CONFIGURATION_VALIDATION_ENVIRONMENT_NOT_CHARACTERIZED;
        return result;
    }

    if (prepared->payload.storage_limit_mb > environment->usable_storage_capacity_mb) {
        return result;
    }

    if (out_validated != NULL) {
        out_validated->generation = prepared->generation;
        out_validated->config_id = prepared->config_id;
        out_validated->supplied_crc = prepared->supplied_crc;
        out_validated->payload = prepared->payload;
    }

    result.status = CONFIGURATION_VALIDATION_VALID;
    return result;
}
