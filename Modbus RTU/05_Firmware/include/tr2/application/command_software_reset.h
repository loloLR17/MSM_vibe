#ifndef TR2_APPLICATION_COMMAND_SOFTWARE_RESET_H
#define TR2_APPLICATION_COMMAND_SOFTWARE_RESET_H

#include <stdbool.h>

#include "tr2/application/command_engine.h"
#include "tr2/persistence/boot_intent_store.h"
#include "tr2/platform/reset_cause_provider.h"
#include "tr2/platform/reset_trigger.h"

Tr2Result command_software_reset_execute(
    CommandEngine *engine,
    BootIntentStore *boot_intent_store,
    const PlatformResetTrigger *reset_trigger,
    bool acquisition_active,
    bool critical_operation_active,
    uint16_t transaction_id,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry);

CommandReconciliationOutcome command_software_reset_reconcile(
    const CommandJournalEntry *entry,
    const BootIntentRecoveryResult *boot_intent,
    ResetCause reset_cause);

#endif
