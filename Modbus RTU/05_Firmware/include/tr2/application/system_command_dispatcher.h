#ifndef TR2_APPLICATION_SYSTEM_COMMAND_DISPATCHER_H
#define TR2_APPLICATION_SYSTEM_COMMAND_DISPATCHER_H

#include "tr2/application/modbus_system_server.h"

/* Borrowed authorities must outlive this object. Single main-loop owner.
 * Transient refusals never overwrite the durable journal or its last result. */
typedef struct {
    SystemRuntime *runtime;
    ConfigurationWorkflow *configuration_workflow;
    bool has_view;
    uint16_t view_code;
    uint16_t view_transaction_id;
    CommandFinalResult view_result;
    uint32_t generation;
} SystemCommandDispatcher;

Tr2Result system_command_dispatcher_init(SystemCommandDispatcher *dispatcher,
    SystemRuntime *runtime, ConfigurationWorkflow *configuration_workflow);
Tr2Result system_command_dispatcher_submit(void *context,
    CommandMailboxSubmitResult submission, const CommandRequest *request);
Tr2Result system_command_dispatcher_snapshot(void *context, CommandSnapshot *snapshot);
void system_command_dispatcher_bind(SystemCommandDispatcher *dispatcher,
    ModbusSystemServerBinding *binding);

#endif
