#include <assert.h>
#include <string.h>

#include "tr2/application/diagnostic_service.h"

static DiagnosticFacts make_facts(void)
{
    DiagnosticFacts facts;

    memset(&facts, 0, sizeof(facts));
    facts.health = DIAGNOSTIC_HEALTH_DEGRADED;
    facts.active_conditions.storage_fault = true;
    facts.active_conditions.temperature_out_of_range = true;
    facts.last_fault.present = true;
    facts.last_fault.code = 0x1234u;
    facts.last_fault.timestamp_available = false;
    facts.last_fault.timestamp = 0u;
    facts.selftest.state = DIAGNOSTIC_SELFTEST_NEVER_RUN;
    facts.selftest.result_code = 0u;
    facts.selftest.detail = 0u;
    facts.internal_temperature_available = true;
    facts.internal_temp_dC = 423;
    facts.supply_voltage_available = true;
    facts.supply_voltage_mV = 3295u;

    return facts;
}

int main(void)
{
    DiagnosticService service;
    DiagnosticService uninitialized;
    DiagnosticFacts first_facts;
    DiagnosticFacts second_facts;
    DiagnosticSnapshot first;
    DiagnosticSnapshot held;
    DiagnosticSnapshot second;

    assert(diagnostic_service_init(NULL) == TR2_ERROR_INVALID_ARGUMENT);

    memset(&service, 0xA5, sizeof(service));
    assert(diagnostic_service_init(&service) == TR2_OK);
    assert(diagnostic_service_is_initialized(&service));
    assert(!diagnostic_service_snapshot(&service, &first));
    assert(diagnostic_service_publish_facts(NULL, &first_facts) ==
           TR2_ERROR_INVALID_ARGUMENT);
    assert(diagnostic_service_publish_facts(&service, NULL) ==
           TR2_ERROR_INVALID_ARGUMENT);

    memset(&uninitialized, 0, sizeof(uninitialized));
    first_facts = make_facts();
    assert(diagnostic_service_publish_facts(&uninitialized, &first_facts) ==
           TR2_ERROR_INVALID_STATE);
    assert(!diagnostic_service_snapshot(&uninitialized, &first));

    assert(diagnostic_service_publish_facts(&service, &first_facts) == TR2_OK);
    assert(diagnostic_service_snapshot(&service, &first));
    assert(first.generation == 1u);
    assert(first.facts.health == DIAGNOSTIC_HEALTH_DEGRADED);
    assert(first.facts.active_conditions.storage_fault);
    assert(first.facts.active_conditions.temperature_out_of_range);
    assert(!first.facts.active_conditions.sensor_fault);
    assert(first.facts.last_fault.present);
    assert(first.facts.last_fault.code == 0x1234u);
    assert(!first.facts.last_fault.timestamp_available);
    assert(first.facts.selftest.state == DIAGNOSTIC_SELFTEST_NEVER_RUN);
    assert(first.facts.internal_temperature_available);
    assert(first.facts.internal_temp_dC == 423);
    assert(first.facts.supply_voltage_available);
    assert(first.facts.supply_voltage_mV == 3295u);

    held = first;
    second_facts = first_facts;
    second_facts.health = DIAGNOSTIC_HEALTH_CRITICAL;
    second_facts.active_conditions.firmware_fault = true;
    second_facts.last_fault.code = 0xBEEFu;
    second_facts.last_fault.timestamp_available = true;
    second_facts.last_fault.timestamp = 123456u;
    second_facts.selftest.state = DIAGNOSTIC_SELFTEST_PASSED;
    second_facts.selftest.result_code = 77u;
    second_facts.selftest.detail = 88u;

    assert(diagnostic_service_publish_facts(&service, &second_facts) == TR2_OK);
    assert(diagnostic_service_snapshot(&service, &second));
    assert(second.generation == 2u);
    assert(second.facts.health == DIAGNOSTIC_HEALTH_CRITICAL);
    assert(second.facts.active_conditions.firmware_fault);
    assert(second.facts.last_fault.code == 0xBEEFu);
    assert(second.facts.last_fault.timestamp_available);
    assert(second.facts.last_fault.timestamp == 123456u);
    assert(second.facts.selftest.state == DIAGNOSTIC_SELFTEST_PASSED);
    assert(second.facts.selftest.result_code == 77u);
    assert(second.facts.selftest.detail == 88u);

    assert(held.generation == 1u);
    assert(held.facts.last_fault.code == 0x1234u);
    assert(!held.facts.active_conditions.firmware_fault);

    assert(!diagnostic_service_snapshot(NULL, &second));
    assert(!diagnostic_service_snapshot(&service, NULL));

    return 0;
}
