#ifndef TR2_APPLICATION_CONFIGURATION_SERVICE_H
#define TR2_APPLICATION_CONFIGURATION_SERVICE_H

#include <stdbool.h>

#include "tr2/common/result.h"
#include "tr2/domain/configuration/configuration.h"
#include "tr2/domain/configuration/configuration_validator.h"
#include "tr2/persistence/configuration_store.h"

typedef struct {
    ConfigurationStore *store;
    bool initialized;
    bool has_active;
    ActiveConfigurationSnapshot active;
    bool has_recovery_status;
    ConfigurationRecoveryStatus recovery_status;
} ConfigurationService;

Tr2Result configuration_service_init(ConfigurationService *service,
                                     ConfigurationStore *store);

bool configuration_service_is_initialized(const ConfigurationService *service);

Tr2Result configuration_service_recover(
    ConfigurationService *service,
    const ConfigurationValidationEnvironment *environment);

Tr2Result configuration_service_commit_candidate(
    ConfigurationService *service,
    const ActiveConfigurationSnapshot *candidate);

bool configuration_service_active_snapshot(
    const ConfigurationService *service,
    ActiveConfigurationSnapshot *out_snapshot);

bool configuration_service_recovery_status(
    const ConfigurationService *service,
    ConfigurationRecoveryStatus *out_status);

#endif
