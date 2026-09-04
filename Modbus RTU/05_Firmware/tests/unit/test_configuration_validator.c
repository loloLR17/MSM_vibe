#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/domain/configuration/configuration_validator.h"

static PreparedConfiguration make_valid_prepared(void)
{
    PreparedConfiguration prepared = {0};

    prepared.generation = 7u;
    prepared.config_id = 42u;
    prepared.supplied_crc = 0x12345678u;
    prepared.payload.sampling_frequency_hz = 26667u;
    prepared.payload.axes_enable_mask = 0x0007u;
    prepared.payload.full_scale_code = 3u;
    prepared.payload.acquisition_mode = 1u;
    prepared.payload.window_size_samples = 32768u;
    prepared.payload.indicator_period_ms = 2000u;
    prepared.payload.campaign_duration_s = 604800u;
    prepared.payload.storage_mode = 1u;
    prepared.payload.storage_limit_mb = 100u;
    prepared.payload.campaign_context_id = 11u;
    prepared.payload.mission_id = 12u;

    return prepared;
}

static void expect_invalid(
    const PreparedConfiguration *prepared,
    const ConfigurationValidationEnvironment *environment)
{
    ValidatedConfiguration validated = {0};
    ConfigurationValidationResult result =
        configuration_validate(prepared, environment, &validated);

    assert(result.status == CONFIGURATION_VALIDATION_INVALID);
}

int main(void)
{
    ConfigurationValidationEnvironment environment = {true, 1000u};
    PreparedConfiguration prepared = make_valid_prepared();
    ValidatedConfiguration validated = {0};
    ConfigurationValidationResult result;

    result = configuration_validate(&prepared, &environment, &validated);
    assert(result.status == CONFIGURATION_VALIDATION_VALID);
    assert(validated.generation == prepared.generation);
    assert(validated.config_id == prepared.config_id);
    assert(validated.supplied_crc == prepared.supplied_crc);
    assert(validated.payload.storage_limit_mb == prepared.payload.storage_limit_mb);

    prepared.config_id = 0u;
    expect_invalid(&prepared, &environment);
    prepared = make_valid_prepared();
    prepared.payload.campaign_context_id = 0u;
    expect_invalid(&prepared, &environment);
    prepared = make_valid_prepared();
    prepared.payload.mission_id = 0u;
    expect_invalid(&prepared, &environment);

    prepared = make_valid_prepared();
    prepared.payload.sampling_frequency_hz = 26666u;
    expect_invalid(&prepared, &environment);
    prepared = make_valid_prepared();
    prepared.payload.axes_enable_mask = 0u;
    expect_invalid(&prepared, &environment);
    prepared = make_valid_prepared();
    prepared.payload.axes_enable_mask = 0x0008u;
    expect_invalid(&prepared, &environment);
    prepared = make_valid_prepared();
    prepared.payload.full_scale_code = 4u;
    expect_invalid(&prepared, &environment);
    prepared = make_valid_prepared();
    prepared.payload.acquisition_mode = 0u;
    expect_invalid(&prepared, &environment);

    prepared = make_valid_prepared();
    prepared.payload.window_size_samples = 4095u;
    expect_invalid(&prepared, &environment);
    prepared = make_valid_prepared();
    prepared.payload.indicator_period_ms = 1999u;
    expect_invalid(&prepared, &environment);
    prepared = make_valid_prepared();
    prepared.payload.campaign_duration_s = 59u;
    expect_invalid(&prepared, &environment);
    prepared = make_valid_prepared();
    prepared.payload.campaign_duration_s = 604801u;
    expect_invalid(&prepared, &environment);
    prepared = make_valid_prepared();
    prepared.payload.storage_mode = 0u;
    expect_invalid(&prepared, &environment);
    prepared = make_valid_prepared();
    prepared.payload.storage_limit_mb = 0u;
    expect_invalid(&prepared, &environment);

    prepared = make_valid_prepared();
    prepared.payload.storage_limit_mb = 1001u;
    expect_invalid(&prepared, &environment);

    prepared = make_valid_prepared();
    environment.storage_capacity_known = false;
    result = configuration_validate(&prepared, &environment, &validated);
    assert(result.status == CONFIGURATION_VALIDATION_ENVIRONMENT_NOT_CHARACTERIZED);

    environment.storage_capacity_known = true;
    environment.usable_storage_capacity_mb = 100u;
    result = configuration_validate(&prepared, &environment, &validated);
    assert(result.status == CONFIGURATION_VALIDATION_VALID);

    /* V1 leaves these business domains/relations NOT_DEFINED: they must not be rejected. */
    prepared.payload.supervision_enable_mask = 0xFFFFu;
    prepared.payload.rms_warn_threshold_mg = 60000u;
    prepared.payload.rms_alarm_threshold_mg = 1u;
    prepared.payload.peak_warn_threshold_mg = 65000u;
    prepared.payload.peak_alarm_threshold_mg = 2u;
    prepared.payload.threshold_hysteresis_mg = 65535u;
    prepared.payload.alarm_hold_time_ms = 65535u;
    prepared.payload.operating_mode_code = 65535u;
    prepared.payload.navigation_zone_code = 65535u;
    prepared.payload.load_state_code = 65535u;
    prepared.payload.sea_state_code = 65535u;
    result = configuration_validate(&prepared, &environment, &validated);
    assert(result.status == CONFIGURATION_VALIDATION_VALID);

    /* Prove every V1 window/period pair satisfies the frozen cross-constraint. */
    {
        static const uint16_t windows[] = {4096u, 8192u, 16384u, 32768u};
        static const uint16_t periods[] = {2000u, 5000u, 10000u, 30000u, 60000u};
        size_t wi;
        size_t pi;

        for (wi = 0u; wi < sizeof(windows) / sizeof(windows[0]); ++wi) {
            for (pi = 0u; pi < sizeof(periods) / sizeof(periods[0]); ++pi) {
                prepared = make_valid_prepared();
                prepared.payload.window_size_samples = windows[wi];
                prepared.payload.indicator_period_ms = periods[pi];
                result = configuration_validate(&prepared, &environment, &validated);
                assert(result.status == CONFIGURATION_VALIDATION_VALID);
            }
        }
    }

    return 0;
}
