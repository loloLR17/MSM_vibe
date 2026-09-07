#include <stddef.h>
#include <string.h>

#include "tr2/application/diagnostic_service.h"

Tr2Result diagnostic_service_init(DiagnosticService *service)
{
    if (service == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(service, 0, sizeof(*service));
    service->initialized = true;
    service->next_generation = 1u;
    return TR2_OK;
}

bool diagnostic_service_is_initialized(const DiagnosticService *service)
{
    return service != NULL && service->initialized;
}

Tr2Result diagnostic_service_publish_facts(DiagnosticService *service,
                                           const DiagnosticFacts *facts)
{
    if (service == NULL || facts == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!service->initialized) {
        return TR2_ERROR_INVALID_STATE;
    }

    service->snapshot.generation = service->next_generation;
    service->snapshot.facts = *facts;
    service->next_generation++;
    service->has_snapshot = true;
    return TR2_OK;
}

bool diagnostic_service_snapshot(const DiagnosticService *service,
                                 DiagnosticSnapshot *out_snapshot)
{
    if (service == NULL || out_snapshot == NULL || !service->initialized ||
        !service->has_snapshot) {
        return false;
    }

    *out_snapshot = service->snapshot;
    return true;
}
