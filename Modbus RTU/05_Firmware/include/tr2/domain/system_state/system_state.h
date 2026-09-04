#ifndef TR2_DOMAIN_SYSTEM_STATE_H
#define TR2_DOMAIN_SYSTEM_STATE_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"

typedef struct {
    uint32_t generation;
    uint16_t system_status;
    uint16_t system_flags;
    uint16_t fault_flags;
    uint16_t warning_flags;
    uint32_t uptime_s;
    uint16_t last_reset_cause;
    int16_t internal_temp_dC;
    uint16_t cpu_load_percent;
    uint16_t memory_usage_percent;
    uint16_t storage_status;
    uint16_t storage_usage_percent;
    uint16_t acquisition_state;
    uint32_t active_campaign_id;
    uint16_t error_code;
    uint16_t warning_code;
} SystemStateSnapshot;

typedef struct {
    SystemStateSnapshot snapshot;
    bool initialized;
} SystemStateService;

Tr2Result system_state_service_init(SystemStateService *service,
                                    const SystemStateSnapshot *snapshot);
Tr2Result system_state_service_get_snapshot(const SystemStateService *service,
                                            SystemStateSnapshot *snapshot);

#endif
