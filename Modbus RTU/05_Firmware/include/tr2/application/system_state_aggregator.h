#ifndef TR2_APPLICATION_SYSTEM_STATE_AGGREGATOR_H
#define TR2_APPLICATION_SYSTEM_STATE_AGGREGATOR_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/application/diagnostic_service.h"
#include "tr2/common/result.h"
#include "tr2/domain/diagnostic/diagnostic.h"
#include "tr2/domain/system_state/system_state.h"

typedef struct {
    bool ready;
    bool acquisition_active;
    bool active_configuration_valid;
    bool time_valid;
    bool storage_available;
    uint32_t uptime_s;
    uint16_t last_reset_cause;
    uint16_t cpu_load_percent;
    uint16_t memory_usage_percent;
    uint16_t storage_status;
    uint16_t storage_usage_percent;
    uint16_t acquisition_state;
    uint32_t active_campaign_id;
    uint16_t error_code;
    uint16_t warning_code;
    const DiagnosticSnapshot *diagnostic;
} SystemStateAggregationInput;

typedef Tr2Result (*SystemStateRefreshCollectFn)(
    void *context,
    DiagnosticFacts *diagnostic_facts,
    SystemStateAggregationInput *aggregation_input);

typedef struct {
    void *context;
    SystemStateRefreshCollectFn collect;
} SystemStateRefreshSource;

typedef struct {
    bool initialized;
    uint32_t next_generation;
} SystemStateAggregator;

Tr2Result system_state_aggregator_init(SystemStateAggregator *aggregator);
Tr2Result system_state_aggregator_build(SystemStateAggregator *aggregator,
                                       const SystemStateAggregationInput *input,
                                       SystemStateSnapshot *snapshot);
Tr2Result system_state_aggregator_refresh(
    SystemStateAggregator *aggregator,
    DiagnosticService *diagnostic_service,
    const SystemStateRefreshSource *source,
    DiagnosticSnapshot *diagnostic_snapshot,
    SystemStateSnapshot *system_snapshot);

#endif
