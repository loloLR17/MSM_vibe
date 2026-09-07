#ifndef TR2_APPLICATION_COMMAND_STOP_ACQUISITION_H
#define TR2_APPLICATION_COMMAND_STOP_ACQUISITION_H

#include "tr2/application/campaign_service.h"
#include "tr2/application/command_engine.h"

Tr2Result command_stop_acquisition_execute(
    CommandEngine *engine,
    CampaignService *campaign_service,
    SupervisionService *supervision_service,
    uint16_t transaction_id,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry);

CommandReconciliationOutcome command_stop_acquisition_reconcile(
    const CommandJournalEntry *entry,
    const CampaignRepository *repository);

#endif
