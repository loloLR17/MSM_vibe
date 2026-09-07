#ifndef TR2_APPLICATION_SUPERVISION_SERVICE_H
#define TR2_APPLICATION_SUPERVISION_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/acquisition/acquisition.h"
#include "tr2/domain/supervision/supervision.h"

typedef struct {
    bool initialized;
    bool has_snapshot;
    uint32_t next_calculation_sequence;
    SupervisionSnapshot snapshot;
} SupervisionService;

Tr2Result supervision_service_init(SupervisionService *service);

bool supervision_service_is_initialized(const SupervisionService *service);

Tr2Result supervision_service_publish_window(SupervisionService *service,
                                             const AcquisitionWindow *window);

bool supervision_service_snapshot(const SupervisionService *service,
                                  SupervisionSnapshot *out_snapshot);

#endif
