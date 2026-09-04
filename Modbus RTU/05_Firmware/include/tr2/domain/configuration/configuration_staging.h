#ifndef TR2_DOMAIN_CONFIGURATION_STAGING_H
#define TR2_DOMAIN_CONFIGURATION_STAGING_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/domain/configuration/configuration.h"

typedef struct {
    bool has_prepared;
    bool validation_current;
    PreparedConfiguration prepared;
} ConfigurationStagingService;

void configuration_staging_init(ConfigurationStagingService *service);

bool configuration_staging_has_prepared(const ConfigurationStagingService *service);
bool configuration_staging_snapshot(
    const ConfigurationStagingService *service,
    PreparedConfiguration *out_snapshot);

void configuration_staging_set_config_id(
    ConfigurationStagingService *service,
    uint32_t config_id);
void configuration_staging_set_supplied_crc(
    ConfigurationStagingService *service,
    uint32_t supplied_crc);
void configuration_staging_replace_payload(
    ConfigurationStagingService *service,
    const ConfigurationPayload *payload);

void configuration_staging_mark_validation_current(ConfigurationStagingService *service);
bool configuration_staging_is_validation_current(
    const ConfigurationStagingService *service);

#endif
