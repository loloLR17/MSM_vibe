#ifndef TR2_PLATFORM_WALL_CLOCK_H
#define TR2_PLATFORM_WALL_CLOCK_H

#include <stdint.h>
#include "tr2/common/result.h"

typedef uint32_t Tr2CivilTimestamp;

typedef enum {
    WALL_CLOCK_OK = 0,
    WALL_CLOCK_INVALID,
    WALL_CLOCK_UNAVAILABLE
} WallClockReadResult;

typedef struct {
    void *context;
    WallClockReadResult (*read)(void *context, Tr2CivilTimestamp *timestamp);
    Tr2Result (*set)(void *context, Tr2CivilTimestamp timestamp);
} WallClock;

#endif
