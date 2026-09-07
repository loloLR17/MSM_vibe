#include <stddef.h>

#include "tr2/application/command_policy.h"

static bool request_parameters_must_be_zero(uint16_t command_code)
{
    return command_code == COMMAND_CODE_APPLY_CONFIGURATION ||
           command_code == COMMAND_CODE_START_ACQUISITION ||
           command_code == COMMAND_CODE_STOP_ACQUISITION ||
           command_code == COMMAND_CODE_SELFTEST ||
           command_code == COMMAND_CODE_REFRESH_INDICATORS ||
           command_code == COMMAND_CODE_ENTER_MAINTENANCE ||
           command_code == COMMAND_CODE_EXIT_MAINTENANCE;
}

static bool acknowledge_fault_parameters_valid(const CommandRequestIdentity *identity)
{
    if (identity->param3 != 0u) {
        return false;
    }
    if (identity->param2 == 0u) {
        return identity->param1 != 0u;
    }
    if (identity->param2 == 1u) {
        return identity->param1 == 0u;
    }
    return false;
}

static bool protected_command(uint16_t command_code)
{
    return command_code == COMMAND_CODE_SOFTWARE_RESET ||
           command_code == COMMAND_CODE_RESET_STATISTICS;
}

uint16_t command_engine_flags_project(const CommandEngineFlagsSource *source)
{
    uint16_t flags = 0u;

    if (source == NULL) {
        return 0u;
    }
    if (source->ready) {
        flags |= COMMAND_ENGINE_FLAG_READY;
    }
    if (source->running) {
        flags |= COMMAND_ENGINE_FLAG_RUNNING;
    }
    if (source->confirmation_required) {
        flags |= COMMAND_ENGINE_FLAG_CONFIRMATION_REQUIRED;
    }
    if (source->maintenance_active) {
        flags |= COMMAND_ENGINE_FLAG_MAINTENANCE_ACTIVE;
    }
    if (source->acquisition_active) {
        flags |= COMMAND_ENGINE_FLAG_ACQUISITION_ACTIVE;
    }
    if (source->critical_fault_active) {
        flags |= COMMAND_ENGINE_FLAG_CRITICAL_FAULT_ACTIVE;
    }
    if (source->active_configuration_valid) {
        flags |= COMMAND_ENGINE_FLAG_ACTIVE_CONFIGURATION_VALID;
    }
    if (source->prepared_sync_available) {
        flags |= COMMAND_ENGINE_FLAG_PREPARED_SYNC_AVAILABLE;
    }
    if (source->prepared_configuration_available) {
        flags |= COMMAND_ENGINE_FLAG_PREPARED_CONFIG_AVAILABLE;
    }
    if (source->command_logging_performed) {
        flags |= COMMAND_ENGINE_FLAG_COMMAND_LOGGING_PERFORMED;
    }

    return flags & COMMAND_ENGINE_FLAG_V1_ALLOWED_MASK;
}

CommandRequestPolicyResult command_request_policy_validate(const CommandRequest *request)
{
    if (request == NULL || !command_transaction_id_is_valid(request->transaction_id) ||
        request->identity.command_code == COMMAND_CODE_NONE) {
        return COMMAND_REQUEST_POLICY_INVALID_PARAMETER;
    }

    if (request_parameters_must_be_zero(request->identity.command_code) &&
        (request->identity.param1 != 0u || request->identity.param2 != 0u ||
         request->identity.param3 != 0u)) {
        return COMMAND_REQUEST_POLICY_INVALID_PARAMETER;
    }

    if (request->identity.command_code == COMMAND_CODE_ACKNOWLEDGE_FAULT &&
        !acknowledge_fault_parameters_valid(&request->identity)) {
        return COMMAND_REQUEST_POLICY_INVALID_PARAMETER;
    }

    if (protected_command(request->identity.command_code) &&
        request->identity.confirm_key != TR2_COMMAND_CONFIRM_KEY_VALID) {
        return COMMAND_REQUEST_POLICY_CONFIRMATION_MISSING;
    }

    return COMMAND_REQUEST_POLICY_VALID;
}

CommandCancelResult command_policy_consume_cancel_request(CommandRequestMailbox *mailbox)
{
    if (mailbox == NULL ||
        (mailbox->control & COMMAND_REQUEST_CONTROL_CANCEL_REQUEST) == 0u) {
        return COMMAND_CANCEL_NONE;
    }

    mailbox->control &= (uint16_t)~COMMAND_REQUEST_CONTROL_CANCEL_REQUEST;
    return COMMAND_CANCEL_NOT_CANCELLABLE;
}
