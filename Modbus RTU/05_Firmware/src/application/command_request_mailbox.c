#include <stddef.h>
#include <string.h>

#include "tr2/application/command_request_mailbox.h"

static void capture_request(const CommandRequestMailbox *mailbox,
                            CommandRequest *captured_request)
{
    captured_request->transaction_id = mailbox->transaction_id;
    captured_request->identity.command_code = mailbox->command_code;
    captured_request->identity.param1 = mailbox->param1;
    captured_request->identity.param2 = mailbox->param2;
    captured_request->identity.param3 = mailbox->param3;
    captured_request->identity.confirm_key = mailbox->confirm_key;
}

void command_request_mailbox_init(CommandRequestMailbox *mailbox)
{
    if (mailbox == NULL) {
        return;
    }

    memset(mailbox, 0, sizeof(*mailbox));
}

void command_request_mailbox_clear_fields(CommandRequestMailbox *mailbox)
{
    if (mailbox == NULL) {
        return;
    }

    mailbox->command_code = 0u;
    mailbox->transaction_id = TR2_COMMAND_TRANSACTION_ID_INVALID;
    mailbox->param1 = 0u;
    mailbox->param2 = 0u;
    mailbox->param3 = 0u;
    mailbox->confirm_key = TR2_COMMAND_CONFIRM_KEY_NONE;
}

CommandMailboxSubmitResult command_request_mailbox_write_control(
    CommandRequestMailbox *mailbox,
    uint16_t control,
    CommandRequest *captured_request)
{
    bool previous_submit;
    bool requested_submit;

    if (mailbox == NULL) {
        return COMMAND_MAILBOX_NO_SUBMISSION;
    }

    previous_submit = (mailbox->control & COMMAND_REQUEST_CONTROL_SUBMIT) != 0u;
    requested_submit = (control & COMMAND_REQUEST_CONTROL_SUBMIT) != 0u;
    mailbox->control = control & COMMAND_REQUEST_CONTROL_ALLOWED_MASK;

    if ((mailbox->control & COMMAND_REQUEST_CONTROL_CLEAR_REQUEST_FIELDS) != 0u) {
        command_request_mailbox_clear_fields(mailbox);
        mailbox->control &= (uint16_t)~COMMAND_REQUEST_CONTROL_CLEAR_REQUEST_FIELDS;
    }

    if (!requested_submit || previous_submit) {
        return COMMAND_MAILBOX_NO_SUBMISSION;
    }

    mailbox->control &= (uint16_t)~COMMAND_REQUEST_CONTROL_SUBMIT;

    if (mailbox->command_code == COMMAND_CODE_NONE) {
        return COMMAND_MAILBOX_SUBMISSION_INVALID_CODE;
    }

    if (!command_transaction_id_is_valid(mailbox->transaction_id)) {
        return COMMAND_MAILBOX_SUBMISSION_INVALID_TRANSACTION_ID;
    }

    if (captured_request == NULL) {
        return COMMAND_MAILBOX_NO_SUBMISSION;
    }

    capture_request(mailbox, captured_request);
    return COMMAND_MAILBOX_SUBMISSION_CAPTURED;
}

bool command_request_mailbox_cancel_requested(const CommandRequestMailbox *mailbox)
{
    if (mailbox == NULL) {
        return false;
    }

    return (mailbox->control & COMMAND_REQUEST_CONTROL_CANCEL_REQUEST) != 0u;
}
