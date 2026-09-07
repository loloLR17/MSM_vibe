#include <assert.h>
#include <string.h>

#include "tr2/application/command_request_mailbox.h"

static void fill_valid_request(CommandRequestMailbox *mailbox)
{
    mailbox->command_code = COMMAND_CODE_START_ACQUISITION;
    mailbox->transaction_id = 42u;
    mailbox->param1 = 11u;
    mailbox->param2 = 12u;
    mailbox->param3 = 0x12345678u;
    mailbox->confirm_key = TR2_COMMAND_CONFIRM_KEY_VALID;
}

static void test_init_and_clear_are_volatile_only(void)
{
    CommandRequestMailbox mailbox;

    memset(&mailbox, 0xA5, sizeof(mailbox));
    command_request_mailbox_init(&mailbox);
    assert(mailbox.command_code == 0u);
    assert(mailbox.transaction_id == 0u);
    assert(mailbox.control == 0u);

    fill_valid_request(&mailbox);
    mailbox.control = COMMAND_REQUEST_CONTROL_CANCEL_REQUEST;
    command_request_mailbox_clear_fields(&mailbox);

    assert(mailbox.command_code == 0u);
    assert(mailbox.transaction_id == 0u);
    assert(mailbox.param1 == 0u);
    assert(mailbox.param2 == 0u);
    assert(mailbox.param3 == 0u);
    assert(mailbox.confirm_key == 0u);
    assert(mailbox.control == COMMAND_REQUEST_CONTROL_CANCEL_REQUEST);
}

static void test_submit_captures_immutable_request_and_auto_clears_submit(void)
{
    CommandRequestMailbox mailbox;
    CommandRequest captured;

    command_request_mailbox_init(&mailbox);
    memset(&captured, 0, sizeof(captured));
    fill_valid_request(&mailbox);

    assert(command_request_mailbox_write_control(
               &mailbox,
               COMMAND_REQUEST_CONTROL_SUBMIT,
               &captured) == COMMAND_MAILBOX_SUBMISSION_CAPTURED);
    assert((mailbox.control & COMMAND_REQUEST_CONTROL_SUBMIT) == 0u);
    assert(captured.transaction_id == 42u);
    assert(captured.identity.command_code == COMMAND_CODE_START_ACQUISITION);
    assert(captured.identity.param1 == 11u);
    assert(captured.identity.param2 == 12u);
    assert(captured.identity.param3 == 0x12345678u);
    assert(captured.identity.confirm_key == TR2_COMMAND_CONFIRM_KEY_VALID);

    mailbox.transaction_id = 43u;
    mailbox.param3 = 0u;
    assert(captured.transaction_id == 42u);
    assert(captured.identity.param3 == 0x12345678u);
}

static void test_submit_requires_nonzero_code_and_valid_transaction_id(void)
{
    CommandRequestMailbox mailbox;
    CommandRequest captured;

    command_request_mailbox_init(&mailbox);
    memset(&captured, 0, sizeof(captured));
    mailbox.transaction_id = 1u;
    assert(command_request_mailbox_write_control(
               &mailbox,
               COMMAND_REQUEST_CONTROL_SUBMIT,
               &captured) == COMMAND_MAILBOX_SUBMISSION_INVALID_CODE);

    mailbox.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    mailbox.transaction_id = 0u;
    assert(command_request_mailbox_write_control(
               &mailbox,
               COMMAND_REQUEST_CONTROL_SUBMIT,
               &captured) == COMMAND_MAILBOX_SUBMISSION_INVALID_TRANSACTION_ID);
}

static void test_control_clear_and_cancel_are_explicit(void)
{
    CommandRequestMailbox mailbox;
    CommandRequest captured;

    command_request_mailbox_init(&mailbox);
    memset(&captured, 0, sizeof(captured));
    fill_valid_request(&mailbox);

    assert(command_request_mailbox_write_control(
               &mailbox,
               COMMAND_REQUEST_CONTROL_CLEAR_REQUEST_FIELDS,
               &captured) == COMMAND_MAILBOX_NO_SUBMISSION);
    assert(mailbox.command_code == 0u);
    assert(mailbox.transaction_id == 0u);
    assert((mailbox.control & COMMAND_REQUEST_CONTROL_CLEAR_REQUEST_FIELDS) == 0u);

    assert(command_request_mailbox_write_control(
               &mailbox,
               COMMAND_REQUEST_CONTROL_CANCEL_REQUEST,
               &captured) == COMMAND_MAILBOX_NO_SUBMISSION);
    assert(command_request_mailbox_cancel_requested(&mailbox));
}

static void test_reserved_control_bits_are_not_retained_by_mailbox(void)
{
    CommandRequestMailbox mailbox;

    command_request_mailbox_init(&mailbox);
    assert(command_request_mailbox_write_control(&mailbox, 0xFFF8u, NULL) ==
           COMMAND_MAILBOX_NO_SUBMISSION);
    assert(mailbox.control == 0u);
}

int main(void)
{
    test_init_and_clear_are_volatile_only();
    test_submit_captures_immutable_request_and_auto_clears_submit();
    test_submit_requires_nonzero_code_and_valid_transaction_id();
    test_control_clear_and_cancel_are_explicit();
    test_reserved_control_bits_are_not_retained_by_mailbox();
    return 0;
}
