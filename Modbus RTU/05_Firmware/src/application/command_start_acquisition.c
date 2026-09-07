#include <stddef.h>
#include <string.h>

#include "tr2/application/command_start_acquisition.h"

static CommandFinalResult final_result(uint16_t status, uint16_t result_code)
{
    CommandFinalResult result;
    memset(&result, 0, sizeof(result));
    result.status = status;
    result.result_code = result_code;
    return result;
}

Tr2Result command_start_acquisition_execute(
    CommandEngine *engine,
    CampaignService *campaign_service,
    uint16_t transaction_id,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry)
{
    CommandJournalEntry current;
    CommandRecoveryContext context;
    CommandFinalResult result;
    CampaignId campaign_id;
    Tr2Result operation_result;

    if (engine == NULL || campaign_service == NULL || terminal_timestamp == NULL ||
        entry == NULL || !command_transaction_id_is_valid(transaction_id) ||
        !command_engine_has_active_transaction(engine) ||
        command_engine_active_transaction_id(engine) != transaction_id) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    operation_result = engine->journal->find(engine->journal->context,
                                             transaction_id,
                                             &current);
    if (operation_result != TR2_OK) {
        return operation_result;
    }
    if (current.request_identity.command_code != COMMAND_CODE_START_ACQUISITION ||
        current.lifecycle != COMMAND_LIFECYCLE_RESERVED) {
        return TR2_ERROR_INVALID_STATE;
    }

    if (campaign_service_campaign_open(campaign_service)) {
        result = final_result(COMMAND_STATUS_REFUSED, COMMAND_RESULT_ACQUISITION_RUNNING);
        return command_engine_complete(engine, transaction_id, &result,
                                       terminal_timestamp, entry);
    }
    if (!configuration_service_active_snapshot(campaign_service->configuration_service,
                                               &(ActiveConfigurationSnapshot){0})) {
        result = final_result(COMMAND_STATUS_REFUSED,
                              COMMAND_RESULT_ACTIVE_CONFIGURATION_INVALID);
        return command_engine_complete(engine, transaction_id, &result,
                                       terminal_timestamp, entry);
    }

    operation_result = campaign_service_reserve_start_id(campaign_service, &campaign_id);
    if (operation_result != TR2_OK) {
        return operation_result;
    }

    memset(&context, 0, sizeof(context));
    context.kind = COMMAND_RECOVERY_CONTEXT_START_CAMPAIGN;
    context.value1 = campaign_id;

    if (engine->journal->set_recovery_context == NULL) {
        return TR2_ERROR_INVALID_STATE;
    }
    operation_result = engine->journal->set_recovery_context(engine->journal->context,
                                                              transaction_id,
                                                              &context,
                                                              entry);
    if (operation_result != TR2_OK) {
        return operation_result;
    }

    operation_result = command_engine_mark_started(engine, transaction_id, entry);
    if (operation_result != TR2_OK) {
        return operation_result;
    }

    operation_result = campaign_service_start_reserved(campaign_service, campaign_id);
    if (operation_result != TR2_OK) {
        return operation_result;
    }

    result = final_result(COMMAND_STATUS_SUCCESS, COMMAND_RESULT_SUCCESS);
    return command_engine_complete(engine, transaction_id, &result,
                                   terminal_timestamp, entry);
}

CommandReconciliationOutcome command_start_acquisition_reconcile(
    const CommandJournalEntry *entry,
    const CampaignRepository *repository)
{
    CampaignMetadata metadata;
    Tr2Result result;

    if (entry == NULL || repository == NULL || repository->get_campaign_by_id == NULL ||
        entry->lifecycle != COMMAND_LIFECYCLE_STARTED ||
        !entry->has_recovery_context ||
        entry->recovery_context.kind != COMMAND_RECOVERY_CONTEXT_START_CAMPAIGN ||
        entry->recovery_context.value1 == TR2_CAMPAIGN_ID_INVALID) {
        return COMMAND_RECONCILIATION_INDETERMINATE;
    }

    memset(&metadata, 0, sizeof(metadata));
    result = repository->get_campaign_by_id(repository->context,
                                             entry->recovery_context.value1,
                                             &metadata);
    if (result == TR2_OK) {
        return metadata.campaign_id == entry->recovery_context.value1
                   ? COMMAND_RECONCILIATION_TERMINAL_EFFECT_PROVEN
                   : COMMAND_RECONCILIATION_INDETERMINATE;
    }
    if (result == TR2_ERROR_NOT_FOUND) {
        return COMMAND_RECONCILIATION_ABSENCE_PROVEN;
    }
    return COMMAND_RECONCILIATION_INDETERMINATE;
}
