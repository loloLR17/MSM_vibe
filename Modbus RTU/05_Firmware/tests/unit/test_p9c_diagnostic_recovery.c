#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/diagnostic_service.h"
#include "tr2/application/system_state_aggregator.h"
#include "tr2/persistence/diagnostic_history_store.h"

#define TEST_OFFSET UINT32_C(5)
#define TEST_MEDIA_SIZE (TEST_OFFSET + TR2_DIAGNOSTIC_HISTORY_RECORD_SIZE + 4u)

typedef struct {
    uint8_t durable[TEST_MEDIA_SIZE];
    uint8_t staged[TEST_MEDIA_SIZE];
    bool fail_read;
} TestMedia;

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    if (media->fail_read || (size_t)offset + size > TEST_MEDIA_SIZE) return TR2_ERROR_STORAGE;
    memcpy(buffer, &media->durable[offset], size);
    return TR2_OK;
}

static Tr2Result media_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    if ((size_t)offset + size > TEST_MEDIA_SIZE) return TR2_ERROR_STORAGE;
    memcpy(&media->staged[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    TestMedia *media = (TestMedia *)context;
    memcpy(media->durable, media->staged, sizeof(media->durable));
    return TR2_OK;
}

int main(void)
{
    TestMedia media;
    PersistentMedia backend;
    PersistentStorageCore storage;
    DiagnosticHistoryStore store = {0};
    DiagnosticHistoryRecoveryResult recovery;
    DiagnosticLastFault fault = {true, UINT16_C(0x1234), true, UINT32_C(0x01020304)};
    DiagnosticService diagnostic_service;
    DiagnosticSnapshot diagnostic;
    SystemStateAggregator aggregator;
    SystemStateAggregationInput input;
    SystemStateSnapshot system;

    memset(&media, 0xFF, sizeof(media));
    media.fail_read = false;
    backend.context = &media;
    backend.read = media_read;
    backend.write = media_write;
    backend.commit = media_commit;
    assert(persistent_storage_core_init(&storage, &backend) == TR2_OK);
    assert(diagnostic_history_store_init(&store, &storage, TEST_OFFSET) == TR2_OK);
    assert(diagnostic_history_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == DIAGNOSTIC_HISTORY_RECOVERY_EMPTY);
    assert(diagnostic_history_store_commit_last_fault(&store, &fault) == TR2_OK);
    assert(diagnostic_history_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == DIAGNOSTIC_HISTORY_RECOVERY_VALID);
    assert(recovery.last_fault.present);
    assert(recovery.last_fault.code == fault.code);
    assert(recovery.last_fault.timestamp_available);
    assert(recovery.last_fault.timestamp == fault.timestamp);

    assert(diagnostic_service_init(&diagnostic_service) == TR2_OK);
    assert(diagnostic_service_restore_last_fault(&diagnostic_service, &recovery.last_fault) == TR2_OK);
    assert(diagnostic_service_snapshot(&diagnostic_service, &diagnostic));
    assert(diagnostic.facts.last_fault.present);
    assert(diagnostic.facts.last_fault.code == fault.code);
    assert(!diagnostic.facts.active_conditions.storage_fault);

    media.durable[TEST_OFFSET + 8u] ^= UINT8_C(0x01);
    assert(diagnostic_history_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == DIAGNOSTIC_HISTORY_RECOVERY_CORRUPTED);
    assert(!recovery.last_fault.present);
    media.fail_read = true;
    assert(diagnostic_history_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == DIAGNOSTIC_HISTORY_RECOVERY_UNAVAILABLE);

    memset(&diagnostic, 0, sizeof(diagnostic));
    diagnostic.generation = 7u;
    diagnostic.facts.health = DIAGNOSTIC_HEALTH_DEGRADED;
    diagnostic.facts.active_conditions.storage_fault = true;
    diagnostic.facts.active_conditions.temperature_out_of_range = true;
    diagnostic.facts.internal_temperature_available = true;
    diagnostic.facts.internal_temp_dC = 425;

    memset(&input, 0, sizeof(input));
    input.ready = true;
    input.active_configuration_valid = true;
    input.time_valid = true;
    input.storage_available = false;
    input.uptime_s = 123u;
    input.last_reset_cause = 1u;
    input.storage_status = 3u;
    input.diagnostic = &diagnostic;

    assert(system_state_aggregator_init(&aggregator) == TR2_OK);
    assert(system_state_aggregator_build(&aggregator, &input, &system) == TR2_OK);
    assert(system.generation == 1u);
    assert(system.system_status == 3u);
    assert((system.system_flags & UINT16_C(0x0001)) != 0u);
    assert((system.system_flags & UINT16_C(0x0004)) != 0u);
    assert((system.system_flags & UINT16_C(0x0008)) != 0u);
    assert((system.system_flags & UINT16_C(0x0010)) == 0u);
    assert(system.fault_flags == UINT16_C(0x0004));
    assert(system.warning_flags == UINT16_C(0x0004));
    assert(system.internal_temp_dC == 425);
    assert(system.uptime_s == 123u);
    assert(system.last_reset_cause == 1u);

    return 0;
}
