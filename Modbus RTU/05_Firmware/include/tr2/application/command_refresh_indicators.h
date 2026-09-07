#ifndef TR2_APPLICATION_COMMAND_REFRESH_INDICATORS_H
#define TR2_APPLICATION_COMMAND_REFRESH_INDICATORS_H

#include <stdint.h>

#include "tr2/application/command_engine.h"
#include "tr2/application/diagnostic_service.h"
#include "tr2/application/system_state_aggregator.h"
#include "tr2/common/result.h"

Tr2Result command_refresh_indicators_execute(
    CommandEngine *engine,
    DiagnosticService *diagnostic_service,
    SystemStateAggregator *aggregator,
    const SystemStateRefreshSource *refresh_source,
    uint16_t transaction_id,
    const CommandTerminalTimestamp *terminal_timestamp,
    DiagnosticSnapshot *diagnostic_snapshot,
    SystemStateSnapshot *system_snapshot,
    CommandJournalEntry *entry);

#endif
