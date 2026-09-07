#ifndef TR2_APPLICATION_COMMAND_SYNCHRONIZE_TIME_H
#define TR2_APPLICATION_COMMAND_SYNCHRONIZE_TIME_H

#include <stdint.h>

#include "tr2/application/command_engine.h"
#include "tr2/domain/time/time_service.h"
#include "tr2/persistence/command_journal_store.h"

Tr2Result command_synchronize_time_execute(
    CommandEngine *engine,
    CommandJournalStore *journal_store,
    TimeService *time_service,
    uint16_t transaction_id,
    uint16_t sync_source,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry);

CommandReconciliationOutcome command_synchronize_time_reconcile(
    const CommandJournalEntry *entry,
    const TimeService *time_service);

#endif
