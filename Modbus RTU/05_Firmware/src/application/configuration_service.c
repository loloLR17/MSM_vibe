#include "tr2/application/configuration_service.h"

#include <string.h>

Tr2Result configuration_service_init(ConfigurationService *service,
                                     ConfigurationStore *store)
{
    if (service == NULL || store == NULL ||
        !configuration_store_is_initialized(store)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(service, 0, sizeof(*service));
    service->store = store;
    service->initialized = true;
    return TR2_OK;
}

bool configuration_service_is_initialized(const ConfigurationService *service)
{
    return service != NULL && service->initialized && service->store != NULL &&
           configuration_store_is_initialized(service->store);
}

Tr2Result configuration_service_recover(
    ConfigurationService *service,
    const ConfigurationValidationEnvironment *environment)
{
    ConfigurationRecoveryResult recovery;
    Tr2Result result;

    if (!configuration_service_is_initialized(service)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (environment == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = configuration_store_recover(service->store, environment, &recovery);
    if (result != TR2_OK) {
        return result;
    }

    service->recovery_status = recovery.status;
    service->has_recovery_status = true;
    if (recovery.status == CONFIGURATION_RECOVERY_VALID && recovery.has_snapshot) {
        service->active = recovery.snapshot;
        service->has_active = true;
    } else {
        memset(&service->active, 0, sizeof(service->active));
        service->has_active = false;
    }

    return TR2_OK;
}

Tr2Result configuration_service_commit_candidate(
    ConfigurationService *service,
    const ActiveConfigurationSnapshot *candidate)
{
    Tr2Result result;

    if (!configuration_service_is_initialized(service)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (candidate == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = configuration_store_commit(service->store, candidate);
    if (result != TR2_OK) {
        return result;
    }

    service->active = *candidate;
    service->has_active = true;
    service->recovery_status = CONFIGURATION_RECOVERY_VALID;
    service->has_recovery_status = true;
    return TR2_OK;
}

bool configuration_service_active_snapshot(
    const ConfigurationService *service,
    ActiveConfigurationSnapshot *out_snapshot)
{
    if (!configuration_service_is_initialized(service) || out_snapshot == NULL ||
        !service->has_active) {
        return false;
    }

    *out_snapshot = service->active;
    return true;
}

bool configuration_service_recovery_status(
    const ConfigurationService *service,
    ConfigurationRecoveryStatus *out_status)
{
    if (!configuration_service_is_initialized(service) || out_status == NULL ||
        !service->has_recovery_status) {
        return false;
    }

    *out_status = service->recovery_status;
    return true;
}
