#ifndef TR2_DOMAIN_SYSTEM_MODE_H
#define TR2_DOMAIN_SYSTEM_MODE_H

#include <stdint.h>

typedef enum {
    SYSTEM_MODE_NORMAL = 0,
    SYSTEM_MODE_MAINTENANCE = 1
} SystemMode;

typedef struct {
    uint32_t generation;
    SystemMode mode;
} SystemModeSnapshot;

#endif
