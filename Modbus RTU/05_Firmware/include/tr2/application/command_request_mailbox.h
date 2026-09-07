#ifndef TR2_APPLICATION_COMMAND_REQUEST_MAILBOX_H
#define TR2_APPLICATION_COMMAND_REQUEST_MAILBOX_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/domain/command/command.h"

typedef struct {
    uint16_t command_code;
    uint16_t transaction_id;
    uint16_t param1;
    uint16_t param2;
    uint32_t param3;
    uint16_t confirm_key;
    uint16_t control;
} CommandRequestMailbox;

typedef enum {
    COMMAND_MAILBOX_NO_SUBMISSION = 0,
    COMMAND_MAILBOX_SUBMISSION_CAPTURED = 1,
    COMMAND_MAILBOX_SUBMISSION_INVALID_CODE = 2,
    COMMAND_MAILBOX_SUBMISSION_INVALID_TRANSACTION_ID = 3
} CommandMailboxSubmitResult;

#define COMMAND_REQUEST_CONTROL_SUBMIT 0x0001u
#define COMMAND_REQUEST_CONTROL_CANCEL_REQUEST 0x0002u
#define COMMAND_REQUEST_CONTROL_CLEAR_REQUEST_FIELDS 0x0004u
#define COMMAND_REQUEST_CONTROL_ALLOWED_MASK 0x0007u

void command_request_mailbox_init(CommandRequestMailbox *mailbox);
void command_request_mailbox_clear_fields(CommandRequestMailbox *mailbox);
CommandMailboxSubmitResult command_request_mailbox_write_control(
    CommandRequestMailbox *mailbox,
    uint16_t control,
    CommandRequest *captured_request);
bool command_request_mailbox_cancel_requested(const CommandRequestMailbox *mailbox);

#endif
