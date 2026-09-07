#include <stddef.h>
#include <string.h>

#include "tr2/application/diagnostic_service.h"

static bool active_fault_input_valid(const DiagnosticActiveFault *faults, size_t fault_count)
{
    size_t index;
    size_t other;

    if (fault_count > TR2_DIAGNOSTIC_ACK_TRACKING_CAPACITY ||
        (fault_count > 0u && faults == NULL)) {
        return false;
    }

    for (index = 0u; index < fault_count; ++index) {
        if (faults[index].code == 0u) {
            return false;
        }
        for (other = index + 1u; other < fault_count; ++other) {
            if (faults[index].code == faults[other].code) {
                return false;
            }
        }
    }

    return true;
}

static const DiagnosticFaultAcknowledgement *find_active_fault(
    const DiagnosticService *service,
    uint16_t fault_code)
{
    size_t index;

    for (index = 0u; index < service->active_fault_count; ++index) {
        if (service->active_faults[index].active &&
            service->active_faults[index].code == fault_code) {
            return &service->active_faults[index];
        }
    }
    return NULL;
}

static DiagnosticFaultAcknowledgement *find_active_fault_mutable(
    DiagnosticService *service,
    uint16_t fault_code)
{
    size_t index;

    for (index = 0u; index < service->active_fault_count; ++index) {
        if (service->active_faults[index].active &&
            service->active_faults[index].code == fault_code) {
            return &service->active_faults[index];
        }
    }
    return NULL;
}

Tr2Result diagnostic_service_init(DiagnosticService *service)
{
    if (service == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(service, 0, sizeof(*service));
    service->initialized = true;
    service->next_generation = 1u;
    return TR2_OK;
}

bool diagnostic_service_is_initialized(const DiagnosticService *service)
{
    return service != NULL && service->initialized;
}

Tr2Result diagnostic_service_publish_facts(DiagnosticService *service,
                                           const DiagnosticFacts *facts)
{
    if (service == NULL || facts == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!service->initialized) {
        return TR2_ERROR_INVALID_STATE;
    }

    service->snapshot.generation = service->next_generation;
    service->snapshot.facts = *facts;
    service->next_generation++;
    service->has_snapshot = true;
    return TR2_OK;
}

Tr2Result diagnostic_service_restore_last_fault(DiagnosticService *service,
                                                const DiagnosticLastFault *last_fault)
{
    if (service == NULL || last_fault == NULL || !last_fault->present) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!service->initialized) {
        return TR2_ERROR_INVALID_STATE;
    }

    if (!service->has_snapshot) {
        memset(&service->snapshot, 0, sizeof(service->snapshot));
        service->snapshot.facts.health = DIAGNOSTIC_HEALTH_OK;
        service->snapshot.facts.selftest.state = DIAGNOSTIC_SELFTEST_NEVER_RUN;
    }
    service->snapshot.facts.last_fault = *last_fault;
    service->snapshot.generation = service->next_generation++;
    service->has_snapshot = true;
    return TR2_OK;
}

Tr2Result diagnostic_service_publish_active_faults(DiagnosticService *service,
                                                   const DiagnosticActiveFault *faults,
                                                   size_t fault_count)
{
    DiagnosticFaultAcknowledgement next[TR2_DIAGNOSTIC_ACK_TRACKING_CAPACITY];
    size_t index;

    if (service == NULL || !active_fault_input_valid(faults, fault_count)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!service->initialized) {
        return TR2_ERROR_INVALID_STATE;
    }

    memset(next, 0, sizeof(next));
    for (index = 0u; index < fault_count; ++index) {
        const DiagnosticFaultAcknowledgement *previous =
            find_active_fault(service, faults[index].code);

        next[index].active = true;
        next[index].code = faults[index].code;
        next[index].acknowledgeable = faults[index].acknowledgeable;
        next[index].acknowledged = previous != NULL &&
                                   previous->acknowledgeable &&
                                   faults[index].acknowledgeable &&
                                   previous->acknowledged;
    }

    memset(service->active_faults, 0, sizeof(service->active_faults));
    if (fault_count > 0u) {
        memcpy(service->active_faults, next,
               fault_count * sizeof(service->active_faults[0]));
    }
    service->active_fault_count = fault_count;
    return TR2_OK;
}

bool diagnostic_service_fault_acknowledgement(
    const DiagnosticService *service,
    uint16_t fault_code,
    DiagnosticFaultAcknowledgement *out_state)
{
    const DiagnosticFaultAcknowledgement *state;

    if (service == NULL || out_state == NULL || !service->initialized || fault_code == 0u) {
        return false;
    }

    state = find_active_fault(service, fault_code);
    if (state == NULL) {
        return false;
    }

    *out_state = *state;
    return true;
}

size_t diagnostic_service_acknowledgeable_fault_count(const DiagnosticService *service)
{
    size_t index;
    size_t count = 0u;

    if (service == NULL || !service->initialized) {
        return 0u;
    }

    for (index = 0u; index < service->active_fault_count; ++index) {
        if (service->active_faults[index].active &&
            service->active_faults[index].acknowledgeable) {
            ++count;
        }
    }
    return count;
}

Tr2Result diagnostic_service_acknowledge_fault(DiagnosticService *service,
                                               uint16_t fault_code)
{
    DiagnosticFaultAcknowledgement *state;

    if (service == NULL || fault_code == 0u) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!service->initialized) {
        return TR2_ERROR_INVALID_STATE;
    }

    state = find_active_fault_mutable(service, fault_code);
    if (state == NULL) {
        return TR2_ERROR_NOT_FOUND;
    }
    if (!state->acknowledgeable) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    state->acknowledged = true;
    return TR2_OK;
}

Tr2Result diagnostic_service_acknowledge_all(DiagnosticService *service)
{
    size_t index;
    bool found = false;

    if (service == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!service->initialized) {
        return TR2_ERROR_INVALID_STATE;
    }

    for (index = 0u; index < service->active_fault_count; ++index) {
        if (service->active_faults[index].active &&
            service->active_faults[index].acknowledgeable) {
            service->active_faults[index].acknowledged = true;
            found = true;
        }
    }

    return found ? TR2_OK : TR2_ERROR_NOT_AVAILABLE;
}

bool diagnostic_service_snapshot(const DiagnosticService *service,
                                 DiagnosticSnapshot *out_snapshot)
{
    if (service == NULL || out_snapshot == NULL || !service->initialized ||
        !service->has_snapshot) {
        return false;
    }

    *out_snapshot = service->snapshot;
    return true;
}
