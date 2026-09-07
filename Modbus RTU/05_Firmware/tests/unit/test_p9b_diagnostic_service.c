#include <assert.h>
#include <string.h>

#include "tr2/application/command_acknowledge_fault.h"
#include "tr2/application/diagnostic_service.h"

typedef struct {
    CommandJournalEntry entry;
    CommandJournalEntry latest;
    bool present;
    bool latest_present;
    unsigned int started_count;
} TestJournalContext;

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

static Tr2Result journal_find(void *context,
                              uint16_t transaction_id,
                              CommandJournalEntry *entry)
{
    TestJournalContext *test = (TestJournalContext *)context;

    if (!test->present || test->entry.transaction_id != transaction_id) {
        return TR2_ERROR_NOT_FOUND;
    }
    *entry = test->entry;
    return TR2_OK;
}

static Tr2Result journal_reserve(void *context,
                                 const CommandRequest *request,
                                 CommandJournalEntry *entry)
{
    TestJournalContext *test = (TestJournalContext *)context;

    memset(&test->entry, 0, sizeof(test->entry));
    test->entry.transaction_id = request->transaction_id;
    test->entry.request_identity = request->identity;
    test->entry.lifecycle = COMMAND_LIFECYCLE_RESERVED;
    test->present = true;
    *entry = test->entry;
    return TR2_OK;
}

static Tr2Result journal_started(void *context,
                                 uint16_t transaction_id,
                                 CommandJournalEntry *entry)
{
    TestJournalContext *test = (TestJournalContext *)context;

    assert(test->present);
    assert(test->entry.transaction_id == transaction_id);
    test->entry.lifecycle = COMMAND_LIFECYCLE_STARTED;
    test->started_count++;
    *entry = test->entry;
    return TR2_OK;
}

static Tr2Result journal_complete(void *context,
                                  uint16_t transaction_id,
                                  const CommandFinalResult *final_result,
                                  const CommandTerminalTimestamp *terminal_timestamp,
                                  CommandJournalEntry *entry)
{
    TestJournalContext *test = (TestJournalContext *)context;

    assert(test->present);
    assert(test->entry.transaction_id == transaction_id);
    test->entry.lifecycle = COMMAND_LIFECYCLE_COMPLETED;
    test->entry.has_final_result = true;
    test->entry.final_result = *final_result;
    test->entry.terminal_timestamp = *terminal_timestamp;
    test->entry.completion_order = 1u;
    test->latest = test->entry;
    test->latest_present = true;
    *entry = test->entry;
    return TR2_OK;
}

static Tr2Result journal_latest(void *context, CommandJournalEntry *entry)
{
    TestJournalContext *test = (TestJournalContext *)context;

    if (!test->latest_present) {
        return TR2_ERROR_NOT_FOUND;
    }
    *entry = test->latest;
    return TR2_OK;
}

static void init_engine(TestJournalContext *context,
                        CommandJournal *journal,
                        CommandEngine *engine)
{
    memset(context, 0, sizeof(*context));
    memset(journal, 0, sizeof(*journal));
    journal->context = context;
    journal->find = journal_find;
    journal->reserve = journal_reserve;
    journal->mark_started = journal_started;
    journal->complete = journal_complete;
    journal->latest_completed = journal_latest;
    assert(command_engine_init(engine, journal) == TR2_OK);
}

static void test_snapshot_publication(void)
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
}

static void test_acknowledgement_state_tracks_active_occurrence(void)
{
    DiagnosticService service;
    DiagnosticActiveFault active[2];
    DiagnosticFaultAcknowledgement state;

    assert(diagnostic_service_init(&service) == TR2_OK);
    active[0].code = UINT16_C(0x1201);
    active[0].acknowledgeable = true;
    active[1].code = UINT16_C(0x1202);
    active[1].acknowledgeable = false;

    assert(diagnostic_service_publish_active_faults(&service, active, 2u) == TR2_OK);
    assert(diagnostic_service_acknowledgeable_fault_count(&service) == 1u);
    assert(diagnostic_service_fault_acknowledgement(&service, active[0].code, &state));
    assert(state.active);
    assert(state.acknowledgeable);
    assert(!state.acknowledged);
    assert(diagnostic_service_acknowledge_fault(&service, active[0].code) == TR2_OK);
    assert(diagnostic_service_fault_acknowledgement(&service, active[0].code, &state));
    assert(state.acknowledged);

    assert(diagnostic_service_publish_active_faults(&service, active, 2u) == TR2_OK);
    assert(diagnostic_service_fault_acknowledgement(&service, active[0].code, &state));
    assert(state.acknowledged);

    assert(diagnostic_service_publish_active_faults(&service, &active[1], 1u) == TR2_OK);
    assert(!diagnostic_service_fault_acknowledgement(&service, active[0].code, &state));
    assert(diagnostic_service_publish_active_faults(&service, active, 2u) == TR2_OK);
    assert(diagnostic_service_fault_acknowledgement(&service, active[0].code, &state));
    assert(!state.acknowledged);

    assert(diagnostic_service_acknowledge_fault(&service, active[1].code) ==
           TR2_ERROR_NOT_AVAILABLE);
    assert(diagnostic_service_acknowledge_all(&service) == TR2_OK);
    assert(diagnostic_service_fault_acknowledgement(&service, active[0].code, &state));
    assert(state.acknowledged);
    assert(diagnostic_service_fault_acknowledgement(&service, active[1].code, &state));
    assert(!state.acknowledged);
}

static void test_acknowledge_command_preserves_active_cause_and_barrier(void)
{
    TestJournalContext context;
    CommandJournal journal;
    CommandEngine engine;
    CommandRequest request;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandTerminalTimestamp timestamp = {false, 0u};
    DiagnosticService service;
    DiagnosticFacts facts = make_facts();
    DiagnosticSnapshot snapshot;
    DiagnosticActiveFault active[2];
    DiagnosticFaultAcknowledgement state;

    init_engine(&context, &journal, &engine);
    assert(diagnostic_service_init(&service) == TR2_OK);
    assert(diagnostic_service_publish_facts(&service, &facts) == TR2_OK);
    active[0].code = UINT16_C(0x1234);
    active[0].acknowledgeable = true;
    active[1].code = UINT16_C(0x4321);
    active[1].acknowledgeable = false;
    assert(diagnostic_service_publish_active_faults(&service, active, 2u) == TR2_OK);

    memset(&request, 0, sizeof(request));
    request.transaction_id = 501u;
    request.identity.command_code = COMMAND_CODE_ACKNOWLEDGE_FAULT;
    request.identity.param1 = active[0].code;
    assert(command_engine_admit(&engine, &request, &admission) == TR2_OK);
    assert(admission.kind == COMMAND_ADMISSION_NEW);
    assert(command_acknowledge_fault_execute(&engine, &service, 501u,
                                             &timestamp, &entry) == TR2_OK);
    assert(context.started_count == 1u);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(!entry.has_recovery_context);
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(entry.final_result.result_code == COMMAND_RESULT_SUCCESS);
    assert(entry.final_result.result_detail == active[0].code);
    assert(diagnostic_service_fault_acknowledgement(&service, active[0].code, &state));
    assert(state.acknowledged);
    assert(diagnostic_service_snapshot(&service, &snapshot));
    assert(snapshot.facts.active_conditions.storage_fault);

    memset(&request, 0, sizeof(request));
    request.transaction_id = 502u;
    request.identity.command_code = COMMAND_CODE_ACKNOWLEDGE_FAULT;
    request.identity.param1 = active[1].code;
    assert(command_engine_admit(&engine, &request, &admission) == TR2_OK);
    assert(command_acknowledge_fault_execute(&engine, &service, 502u,
                                             &timestamp, &entry) == TR2_OK);
    assert(context.started_count == 1u);
    assert(entry.final_result.status == COMMAND_STATUS_REFUSED);
    assert(entry.final_result.result_code == COMMAND_RESULT_FAULT_NOT_ACKNOWLEDGEABLE);
    assert(entry.final_result.result_detail == active[1].code);
    assert(diagnostic_service_fault_acknowledgement(&service, active[1].code, &state));
    assert(!state.acknowledged);

    active[0].acknowledgeable = true;
    assert(diagnostic_service_publish_active_faults(&service, active, 2u) == TR2_OK);
    memset(&request, 0, sizeof(request));
    request.transaction_id = 503u;
    request.identity.command_code = COMMAND_CODE_ACKNOWLEDGE_FAULT;
    request.identity.param2 = 1u;
    assert(command_engine_admit(&engine, &request, &admission) == TR2_OK);
    assert(command_acknowledge_fault_execute(&engine, &service, 503u,
                                             &timestamp, &entry) == TR2_OK);
    assert(context.started_count == 2u);
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(entry.final_result.result_detail == 0u);
    assert(diagnostic_service_fault_acknowledgement(&service, active[0].code, &state));
    assert(state.acknowledged);
    assert(diagnostic_service_fault_acknowledgement(&service, active[1].code, &state));
    assert(!state.acknowledged);
}

int main(void)
{
    test_snapshot_publication();
    test_acknowledgement_state_tracks_active_occurrence();
    test_acknowledge_command_preserves_active_cause_and_barrier();
    return 0;
}
