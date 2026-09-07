#ifndef TR2_APPLICATION_COMMAND_ACKNOWLEDGE_FAULT_H
#define TR2_APPLICATION_COMMAND_ACKNOWLEDGE_FAULT_H

#include <stdint.h>

#include "tr2/application/command_engine.h"
#include "tr2/application/diagnostic_service.h"
#include "tr2/common/result.h"

Tr2Result command_acknowledge_fault_execute(
    CommandEngine *engine,
    DiagnosticService *diagnostic_service,
    uint16_t transaction_id,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry);

#endif
