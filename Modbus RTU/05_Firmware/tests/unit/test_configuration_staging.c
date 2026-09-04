#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/domain/configuration/configuration_staging.h"

static ConfigurationPayload make_payload(uint16_t sampling_frequency_hz)
{
    ConfigurationPayload payload = {0};

    payload.sampling_frequency_hz = sampling_frequency_hz;
    memcpy(payload.campaign_label, "campaign", 8u);
    memcpy(payload.mission_label, "mission", 7u);

    return payload;
}

int main(void)
{
    ConfigurationStagingService service;
    PreparedConfiguration first_snapshot = {0};
    PreparedConfiguration second_snapshot = {0};
    ConfigurationPayload payload = make_payload(1u);

    configuration_staging_init(&service);

    assert(!configuration_staging_has_prepared(&service));
    assert(!configuration_staging_is_validation_current(&service));
    assert(!configuration_staging_snapshot(&service, &first_snapshot));

    configuration_staging_set_config_id(&service, 42u);
    assert(configuration_staging_has_prepared(&service));
    assert(!configuration_staging_is_validation_current(&service));
    assert(configuration_staging_snapshot(&service, &first_snapshot));
    assert(first_snapshot.config_id == 42u);
    assert(first_snapshot.generation == 1u);

    configuration_staging_set_supplied_crc(&service, 0x5207CCFCu);
    assert(configuration_staging_snapshot(&service, &first_snapshot));
    assert(first_snapshot.supplied_crc == 0x5207CCFCu);
    assert(first_snapshot.generation == 2u);

    configuration_staging_replace_payload(&service, &payload);
    assert(configuration_staging_snapshot(&service, &first_snapshot));
    assert(first_snapshot.payload.sampling_frequency_hz == 1u);
    assert(first_snapshot.generation == 3u);

    configuration_staging_mark_validation_current(&service);
    assert(configuration_staging_is_validation_current(&service));

    payload.sampling_frequency_hz = 65535u;
    configuration_staging_replace_payload(&service, &payload);

    assert(!configuration_staging_is_validation_current(&service));
    assert(configuration_staging_snapshot(&service, &second_snapshot));
    assert(second_snapshot.generation == 4u);
    assert(second_snapshot.payload.sampling_frequency_hz == 65535u);

    assert(first_snapshot.generation == 3u);
    assert(first_snapshot.payload.sampling_frequency_hz == 1u);

    configuration_staging_mark_validation_current(&service);
    assert(configuration_staging_is_validation_current(&service));

    configuration_staging_set_config_id(&service, 43u);
    assert(!configuration_staging_is_validation_current(&service));

    configuration_staging_mark_validation_current(&service);
    assert(configuration_staging_is_validation_current(&service));

    configuration_staging_set_supplied_crc(&service, 0u);
    assert(!configuration_staging_is_validation_current(&service));

    return 0;
}
