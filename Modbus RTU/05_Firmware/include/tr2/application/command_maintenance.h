#ifndef TR2_APPLICATION_COMMAND_MAINTENANCE_H
#define TR2_APPLICATION_COMMAND_MAINTENANCE_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/application/command_engine.h"
#include "tr2/application/maintenance_service.h"

Tr2Result command_enter_maintenance_execute(
    CommandEngine *engine,
    MaintenanceService *maintenance_service,
    bool acquisition_active,
    uint16_t transaction_id,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry);

Tr2Result command_exit_maintenance_execute(
    CommandEngine *engine,
    MaintenanceService *maintenance_service,
    uint16_t transaction_id,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry);

#endif
