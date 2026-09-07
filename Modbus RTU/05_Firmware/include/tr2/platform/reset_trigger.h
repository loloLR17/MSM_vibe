#ifndef TR2_PLATFORM_RESET_TRIGGER_H
#define TR2_PLATFORM_RESET_TRIGGER_H

#include <stdbool.h>
#include <stddef.h>

#include "tr2/common/result.h"

typedef struct {
    void *context;
    Tr2Result (*software_reset)(void *context);
} PlatformResetTrigger;

static inline bool platform_reset_trigger_is_valid(const PlatformResetTrigger *trigger)
{
    return trigger != NULL && trigger->software_reset != NULL;
}

#endif
