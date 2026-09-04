#ifndef TR2_DOMAIN_TIME_SERVICE_H
#define TR2_DOMAIN_TIME_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/platform/wall_clock.h"

typedef struct {
    uint32_t generation;
    bool current_time_available;
    Tr2CivilTimestamp current_time;
    bool prepared_time_available;
    Tr2CivilTimestamp prepared_time;
    uint16_t prepared_time_status;
} TimeSnapshot;

typedef struct {
    const WallClock *wall_clock;
    uint32_t generation;
    bool prepared_time_available;
    Tr2CivilTimestamp prepared_time;
    uint16_t prepared_time_status;
    bool initialized;
} TimeService;

Tr2Result time_service_init(TimeService *service, const WallClock *wall_clock);
Tr2Result time_service_get_snapshot(const TimeService *service, TimeSnapshot *snapshot);
Tr2Result time_service_prepare_time(TimeService *service, Tr2CivilTimestamp prepared_time);

#endif
