#include <stddef.h>
#include "tr2/domain/system_state/system_state.h"

Tr2Result system_state_service_init(SystemStateService *service,
                                    const SystemStateSnapshot *snapshot)
{
    if (service == NULL || snapshot == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    service->snapshot = *snapshot;
    service->initialized = true;
    return TR2_OK;
}

Tr2Result system_state_service_get_snapshot(const SystemStateService *service,
                                            SystemStateSnapshot *snapshot)
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
