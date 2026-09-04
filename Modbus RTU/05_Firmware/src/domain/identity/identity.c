#include <stddef.h>
#include "tr2/domain/identity/identity.h"

Tr2Result identity_service_init(IdentityService *service, const IdentitySnapshot *snapshot)
{
    if (service == NULL || snapshot == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    service->snapshot = *snapshot;
    service->initialized = true;
    return TR2_OK;
}

Tr2Result identity_service_get_snapshot(const IdentityService *service, IdentitySnapshot *snapshot)
{
    if (service == NULL || snapshot == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!service->initialized) {
        return TR2_ERROR_INVALID_STATE;
    }

    *snapshot = service->snapshot;
    return TR2_OK;
}
