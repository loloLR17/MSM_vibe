#include "tr2/domain/configuration/configuration_staging.h"

#include <string.h>

static void configuration_staging_touch(ConfigurationStagingService *service)
{
    service->has_prepared = true;
    service->validation_current = false;
    service->prepared.generation += 1u;
}

void configuration_staging_init(ConfigurationStagingService *service)
{
    memset(service, 0, sizeof(*service));
}

bool configuration_staging_has_prepared(const ConfigurationStagingService *service)
{
    return service->has_prepared;
}

bool configuration_staging_snapshot(
    const ConfigurationStagingService *service,
    PreparedConfiguration *out_snapshot)
{
    if (!service->has_prepared) {
        return false;
    }

    *out_snapshot = service->prepared;
    return true;
}

void configuration_staging_set_config_id(
    ConfigurationStagingService *service,
    uint32_t config_id)
{
    service->prepared.config_id = config_id;
    configuration_staging_touch(service);
}

void configuration_staging_set_supplied_crc(
    ConfigurationStagingService *service,
    uint32_t supplied_crc)
{
    service->prepared.supplied_crc = supplied_crc;
    configuration_staging_touch(service);
}

void configuration_staging_replace_payload(
    ConfigurationStagingService *service,
    const ConfigurationPayload *payload)
{
    service->prepared.payload = *payload;
    configuration_staging_touch(service);
}

void configuration_staging_mark_validation_current(ConfigurationStagingService *service)
{
    service->validation_current = service->has_prepared;
}

bool configuration_staging_is_validation_current(
    const ConfigurationStagingService *service)
{
    return service->validation_current;
}
