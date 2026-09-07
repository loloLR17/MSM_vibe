#ifndef TR2_APPLICATION_DIAGNOSTIC_SERVICE_H
#define TR2_APPLICATION_DIAGNOSTIC_SERVICE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/diagnostic/diagnostic.h"

typedef struct {
    bool initialized;
    bool has_snapshot;
    uint32_t next_generation;
    DiagnosticSnapshot snapshot;
    size_t active_fault_count;
    DiagnosticFaultAcknowledgement active_faults[TR2_DIAGNOSTIC_ACK_TRACKING_CAPACITY];
} DiagnosticService;

Tr2Result diagnostic_service_init(DiagnosticService *service);

bool diagnostic_service_is_initialized(const DiagnosticService *service);

Tr2Result diagnostic_service_publish_facts(DiagnosticService *service,
                                           const DiagnosticFacts *facts);

Tr2Result diagnostic_service_restore_last_fault(DiagnosticService *service,
                                                const DiagnosticLastFault *last_fault);

Tr2Result diagnostic_service_publish_active_faults(DiagnosticService *service,
                                                   const DiagnosticActiveFault *faults,
                                                   size_t fault_count);

bool diagnostic_service_fault_acknowledgement(
    const DiagnosticService *service,
    uint16_t fault_code,
    DiagnosticFaultAcknowledgement *out_state);

size_t diagnostic_service_acknowledgeable_fault_count(const DiagnosticService *service);

Tr2Result diagnostic_service_acknowledge_fault(DiagnosticService *service,
                                               uint16_t fault_code);

Tr2Result diagnostic_service_acknowledge_all(DiagnosticService *service);

bool diagnostic_service_snapshot(const DiagnosticService *service,
                                 DiagnosticSnapshot *out_snapshot);

#endif
