#ifndef TR2_APPLICATION_MAINTENANCE_SERVICE_H
#define TR2_APPLICATION_MAINTENANCE_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/system_mode/system_mode.h"

typedef struct {
    bool initialized;
    uint32_t generation;
    SystemMode mode;
} MaintenanceService;

Tr2Result maintenance_service_init(MaintenanceService *service);
bool maintenance_service_is_initialized(const MaintenanceService *service);
bool maintenance_service_active(const MaintenanceService *service);
Tr2Result maintenance_service_enter(MaintenanceService *service);
Tr2Result maintenance_service_exit(MaintenanceService *service);
bool maintenance_service_snapshot(const MaintenanceService *service,
                                  SystemModeSnapshot *snapshot);

#endif
