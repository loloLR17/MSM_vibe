#ifndef TR2_APPLICATION_COMMAND_POLICY_H
#define TR2_APPLICATION_COMMAND_POLICY_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/application/command_request_mailbox.h"
#include "tr2/domain/command/command.h"

typedef struct {
    bool ready;
    bool running;
    bool confirmation_required;
    bool maintenance_active;
    bool acquisition_active;
    bool critical_fault_active;
    bool active_configuration_valid;
    bool prepared_sync_available;
    bool prepared_configuration_available;
    bool cancellation_supported;
    bool command_logging_performed;
} CommandEngineFlagsSource;

typedef enum {
    COMMAND_REQUEST_POLICY_VALID = 0,
    COMMAND_REQUEST_POLICY_INVALID_PARAMETER,
    COMMAND_REQUEST_POLICY_CONFIRMATION_MISSING
} CommandRequestPolicyResult;

typedef enum {
    COMMAND_CANCEL_NONE = 0,
    COMMAND_CANCEL_NOT_CANCELLABLE
} CommandCancelResult;

#define COMMAND_ENGINE_FLAG_READY                    UINT16_C(0x0001)
#define COMMAND_ENGINE_FLAG_RUNNING                  UINT16_C(0x0002)
#define COMMAND_ENGINE_FLAG_CONFIRMATION_REQUIRED    UINT16_C(0x0004)
#define COMMAND_ENGINE_FLAG_MAINTENANCE_ACTIVE       UINT16_C(0x0008)
#define COMMAND_ENGINE_FLAG_ACQUISITION_ACTIVE       UINT16_C(0x0010)
#define COMMAND_ENGINE_FLAG_CRITICAL_FAULT_ACTIVE    UINT16_C(0x0020)
#define COMMAND_ENGINE_FLAG_ACTIVE_CONFIGURATION_VALID UINT16_C(0x0040)
#define COMMAND_ENGINE_FLAG_PREPARED_SYNC_AVAILABLE  UINT16_C(0x0080)
#define COMMAND_ENGINE_FLAG_PREPARED_CONFIG_AVAILABLE UINT16_C(0x0100)
#define COMMAND_ENGINE_FLAG_CANCELLATION_SUPPORTED   UINT16_C(0x0200)
#define COMMAND_ENGINE_FLAG_COMMAND_LOGGING_PERFORMED UINT16_C(0x0400)
#define COMMAND_ENGINE_FLAG_V1_ALLOWED_MASK          UINT16_C(0x07FF)

uint16_t command_engine_flags_project(const CommandEngineFlagsSource *source);
CommandRequestPolicyResult command_request_policy_validate(const CommandRequest *request);
CommandCancelResult command_policy_consume_cancel_request(CommandRequestMailbox *mailbox);

#endif
