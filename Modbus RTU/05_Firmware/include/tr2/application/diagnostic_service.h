#ifndef TR2_APPLICATION_DIAGNOSTIC_SERVICE_H
#define TR2_APPLICATION_DIAGNOSTIC_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/diagnostic/diagnostic.h"

typedef struct {
    bool initialized;
    bool has_snapshot;
    uint32_t next_generation;
    DiagnosticSnapshot snapshot;
} DiagnosticService;

Tr2Result diagnostic_service_init(DiagnosticService *service);

bool diagnostic_service_is_initialized(const DiagnosticService *service);

Tr2Result diagnostic_service_publish_facts(DiagnosticService *service,
                                           const DiagnosticFacts *facts);

Tr2Result diagnostic_service_restore_last_fault(DiagnosticService *service,
                                                const DiagnosticLastFault *last_fault);

bool diagnostic_service_snapshot(const DiagnosticService *service,
                                 DiagnosticSnapshot *out_snapshot);

#endif
