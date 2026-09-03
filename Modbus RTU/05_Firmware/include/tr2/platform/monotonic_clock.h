#ifndef TR2_PLATFORM_MONOTONIC_CLOCK_H
#define TR2_PLATFORM_MONOTONIC_CLOCK_H

#include <stdint.h>

typedef uint64_t MonotonicTimeMs;

typedef struct {
    void *context;
    MonotonicTimeMs (*now_ms)(void *context);
} MonotonicClock;

#endif
