#include <string.h>

#include "tr2/application/system_command_dispatcher.h"
#include "tr2/application/command_apply_configuration.h"
#include "tr2/application/command_synchronize_time.h"
#include "tr2/application/command_policy.h"

static void view(SystemCommandDispatcher *dispatcher, uint16_t code, uint16_t id,
                 uint16_t status, uint16_t result)
{
    dispatcher->has_view = true;
    dispatcher->view_code = code;
    dispatcher->view_transaction_id = id;
    dispatcher->view_result = (CommandFinalResult){status, result, 0u};
    ++dispatcher->generation;
}

static CommandTerminalTimestamp terminal_time(SystemRuntime *runtime)
{
    TimeSnapshot snapshot;
    CommandTerminalTimestamp timestamp = {false, 0u};
    if (time_service_get_snapshot(&runtime->time_service, &snapshot) == TR2_OK &&
        snapshot.current_time_available && snapshot.civil_time_usable) {
        timestamp.available = true;
        timestamp.value = snapshot.current_time;
    }
    return timestamp;
}

Tr2Result system_command_dispatcher_init(SystemCommandDispatcher *dispatcher,
    SystemRuntime *runtime, ConfigurationWorkflow *workflow)
{
    if (dispatcher == NULL || !system_runtime_is_ready_for_modbus(runtime) ||
        !runtime->command_runtime_available ||
        (workflow != NULL && (workflow->staging == NULL ||
         workflow->integrity.compute_prepared_crc == NULL || workflow->activation.commit == NULL)))
        return TR2_ERROR_INVALID_ARGUMENT;
    memset(dispatcher, 0, sizeof(*dispatcher));
    dispatcher->runtime = runtime;
    dispatcher->configuration_workflow = workflow;
    return TR2_OK;
}

static Tr2Result submit_request(SystemCommandDispatcher *dispatcher,
                                const CommandRequest *request)
{
    SystemRuntime *runtime = dispatcher->runtime;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    CommandTerminalTimestamp timestamp;
    CommandRequestPolicyResult policy;
    Tr2Result result;
    uint16_t code = request->identity.command_code;

    /* ID zero creates no persistent identity, even for an invalid code. */
    if (!command_transaction_id_is_valid(request->transaction_id)) {
        view(dispatcher, code, 0u, COMMAND_STATUS_REFUSED,
             COMMAND_RESULT_INVALID_TRANSACTION_ID);
        return TR2_OK;
    }
    if (code == COMMAND_CODE_NONE) {
        view(dispatcher, code, request->transaction_id, COMMAND_STATUS_UNKNOWN,
             COMMAND_RESULT_UNKNOWN_COMMAND);
        return TR2_OK;
    }

    /* Resolve retry/collision before checking today's platform capabilities. */
    result = runtime->command_engine.journal->find(
        runtime->command_engine.journal->context, request->transaction_id, &entry);
    if (result == TR2_OK) {
        if (!command_request_identity_equal(&entry.request_identity, &request->identity)) {
            /* V1 collision wire mapping is unspecified: no invented business code. */
            return TR2_ERROR_INVALID_STATE;
        }
        if (entry.has_final_result) {
            view(dispatcher, code, request->transaction_id,
                 entry.final_result.status, entry.final_result.result_code);
            dispatcher->view_result.result_detail = entry.final_result.result_detail;
        } else {
            dispatcher->has_view = false;
        }
        return TR2_OK;
    }
    if (result != TR2_ERROR_NOT_FOUND) return result;
    if (command_engine_has_active_transaction(&runtime->command_engine)) {
        view(dispatcher, code, request->transaction_id, COMMAND_STATUS_REFUSED,
             COMMAND_RESULT_COMMAND_ALREADY_RUNNING);
        return TR2_OK;
    }
    if ((code == COMMAND_CODE_APPLY_CONFIGURATION &&
         dispatcher->configuration_workflow == NULL) ||
        (code == COMMAND_CODE_SELFTEST && (runtime->deps.selftest_executor == NULL ||
         runtime->deps.selftest_executor->run_standard == NULL)) ||
        (code == COMMAND_CODE_SOFTWARE_RESET &&
         !platform_reset_trigger_is_valid(runtime->deps.reset_trigger)) ||
        code == COMMAND_CODE_RESET_STATISTICS) {
        return TR2_ERROR_NOT_AVAILABLE;
    }
    dispatcher->has_view = false;
    timestamp = terminal_time(runtime);
    policy = command_request_policy_validate(request);
    /* Persist valid new identities before functional validation, including refusals. */
    if (policy != COMMAND_REQUEST_POLICY_VALID || code > COMMAND_CODE_RESET_STATISTICS ||
        code == COMMAND_CODE_APPLY_CONFIGURATION || code == COMMAND_CODE_SYNCHRONIZE_TIME) {
        result = command_engine_admit(&runtime->command_engine, request, &admission);
        if (result != TR2_OK) return result;
        if (admission.kind != COMMAND_ADMISSION_NEW) return TR2_ERROR_INVALID_STATE;
        if (policy != COMMAND_REQUEST_POLICY_VALID || code > COMMAND_CODE_RESET_STATISTICS) {
            CommandFinalResult refusal = {
                code > COMMAND_CODE_RESET_STATISTICS ? COMMAND_STATUS_UNKNOWN : COMMAND_STATUS_REFUSED,
                code > COMMAND_CODE_RESET_STATISTICS ? COMMAND_RESULT_UNKNOWN_COMMAND :
                policy == COMMAND_REQUEST_POLICY_CONFIRMATION_MISSING
                    ? COMMAND_RESULT_CONFIRMATION_MISSING : COMMAND_RESULT_INVALID_PARAMETER,
                0u};
            result = command_engine_complete(&runtime->command_engine, request->transaction_id,
                                             &refusal, &timestamp, &entry);
        } else if (code == COMMAND_CODE_APPLY_CONFIGURATION) {
            ConfigurationWorkflow *workflow = dispatcher->configuration_workflow;
            uint16_t refusal_code = COMMAND_RESULT_SUCCESS;
            if (campaign_service_acquisition_running(&runtime->campaign_service)) {
                refusal_code = COMMAND_RESULT_ACQUISITION_RUNNING;
            } else {
                bool prepared = configuration_staging_has_prepared(workflow->staging);
                ConfigurationValidationResult validation = configuration_workflow_validate(
                    workflow, runtime->deps.configuration_validation_environment);
                if (!prepared) refusal_code = COMMAND_RESULT_PREPARED_CONFIGURATION_INCOMPLETE;
                else if (validation.status == CONFIGURATION_VALIDATION_INVALID)
                    refusal_code = COMMAND_RESULT_INVALID_CONFIGURATION;
                else if (validation.status == CONFIGURATION_VALIDATION_ENVIRONMENT_NOT_CHARACTERIZED)
                    refusal_code = COMMAND_RESULT_INCOMPATIBLE_STATE;
            }
            if (refusal_code != COMMAND_RESULT_SUCCESS) {
                CommandFinalResult refusal = {COMMAND_STATUS_REFUSED, refusal_code, 0u};
                result = command_engine_complete(&runtime->command_engine, request->transaction_id,
                                                  &refusal, &timestamp, &entry);
            } else {
                result = command_apply_configuration_execute_bound(&runtime->command_engine,
                    workflow, request->transaction_id, &timestamp, &entry);
            }
        } else {
            result = command_synchronize_time_execute_bound(&runtime->command_engine,
                &runtime->time_service, request->transaction_id, UINT16_C(1),
                &timestamp, &entry);
        }
    } else if (code == COMMAND_CODE_START_ACQUISITION || code == COMMAND_CODE_STOP_ACQUISITION) {
        result = system_runtime_execute_acquisition_command(runtime, request, &timestamp,
                                                            &admission, &entry);
    } else {
        result = system_runtime_execute_p9_command(runtime, request, &timestamp, &admission, &entry);
    }
    if (result == TR2_OK && entry.has_final_result) {
        view(dispatcher, code, request->transaction_id,
             entry.final_result.status, entry.final_result.result_code);
        dispatcher->view_result.result_detail = entry.final_result.result_detail;
    }
    /* Failure remains observable; never fabricate terminal success after failed durability. */
    return result;
}

Tr2Result system_command_dispatcher_submit(void *context,
    CommandMailboxSubmitResult submission, const CommandRequest *request)
{
    SystemCommandDispatcher *dispatcher = context;
    Tr2Result result = TR2_OK;
    CommandSnapshot snapshot;
    if (dispatcher == NULL || !system_runtime_is_ready_for_modbus(dispatcher->runtime))
        return TR2_ERROR_INVALID_STATE;
    if (submission != COMMAND_MAILBOX_NO_SUBMISSION) {
        if (request == NULL) return TR2_ERROR_INVALID_ARGUMENT;
        result = submit_request(dispatcher, request);
    }
    if (command_policy_consume_cancel_request(&dispatcher->runtime->command_mailbox)
        == COMMAND_CANCEL_NOT_CANCELLABLE) {
        /* V1 defines no successful cancellation. Preserve active authority and history. */
        if (command_engine_snapshot(&dispatcher->runtime->command_engine, &snapshot) != TR2_OK)
            return TR2_ERROR_INTERNAL;
        view(dispatcher, snapshot.active_command_code, snapshot.active_transaction_id,
             snapshot.status, COMMAND_RESULT_NOT_CANCELLABLE);
        if (snapshot.status == COMMAND_STATUS_NONE)
            dispatcher->view_result.status = COMMAND_STATUS_REFUSED;
    }
    return result;
}

Tr2Result system_command_dispatcher_snapshot(void *context, CommandSnapshot *snapshot)
{
    SystemCommandDispatcher *dispatcher = context;
    CommandEngineFlagsSource flags = {0};
    TimeSnapshot time;
    ActiveConfigurationSnapshot configuration;
    Tr2Result result;
    if (dispatcher == NULL || snapshot == NULL ||
        !system_runtime_is_ready_for_modbus(dispatcher->runtime)) return TR2_ERROR_INVALID_STATE;
    result = command_engine_snapshot(&dispatcher->runtime->command_engine, snapshot);
    if (result != TR2_OK) return result;
    snapshot->generation += dispatcher->generation;
    if (dispatcher->has_view) {
        snapshot->active_command_code = dispatcher->view_code;
        snapshot->active_transaction_id = dispatcher->view_transaction_id;
        snapshot->status = dispatcher->view_result.status;
        snapshot->result_code = dispatcher->view_result.result_code;
        snapshot->result_detail = dispatcher->view_result.result_detail;
    }
    flags.running = command_engine_has_active_transaction(&dispatcher->runtime->command_engine);
    flags.ready = !flags.running;
    flags.maintenance_active = maintenance_service_active(&dispatcher->runtime->maintenance_service);
    flags.acquisition_active = campaign_service_acquisition_running(&dispatcher->runtime->campaign_service);
    flags.active_configuration_valid = configuration_service_active_snapshot(
        &dispatcher->runtime->configuration_service, &configuration);
    flags.prepared_sync_available = time_service_get_snapshot(&dispatcher->runtime->time_service, &time)
        == TR2_OK && time.prepared_time_available;
    flags.prepared_configuration_available = dispatcher->configuration_workflow != NULL &&
        configuration_staging_has_prepared(dispatcher->configuration_workflow->staging);
    flags.command_logging_performed = snapshot->last.present;
    snapshot->engine_flags = command_engine_flags_project(&flags);
    return TR2_OK;
}

void system_command_dispatcher_bind(SystemCommandDispatcher *dispatcher,
    ModbusSystemServerBinding *binding)
{
    if (dispatcher == NULL || binding == NULL) return;
    binding->runtime = dispatcher->runtime;
    binding->configuration_workflow = dispatcher->configuration_workflow;
    binding->command_submit_context = dispatcher;
    binding->command_submit = system_command_dispatcher_submit;
    binding->command_snapshot = system_command_dispatcher_snapshot;
}
