#ifndef TR2_APPLICATION_COMMAND_APPLY_CONFIGURATION_H
#define TR2_APPLICATION_COMMAND_APPLY_CONFIGURATION_H

#include "tr2/application/command_engine.h"
#include "tr2/application/configuration_service.h"
#include "tr2/application/configuration_workflow.h"
#include "tr2/persistence/command_journal_store.h"

Tr2Result command_apply_configuration_execute(
    CommandEngine *engine,
    CommandJournalStore *journal_store,
    ConfigurationWorkflow *workflow,
    uint16_t transaction_id,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry);

CommandReconciliationOutcome command_apply_configuration_reconcile(
    const CommandJournalEntry *entry,
    const ConfigurationService *configuration_service);

#endif
