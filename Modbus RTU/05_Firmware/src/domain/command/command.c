#include "tr2/domain/command/command.h"

bool command_transaction_id_is_valid(uint16_t transaction_id)
{
    return transaction_id != TR2_COMMAND_TRANSACTION_ID_INVALID;
}

bool command_request_identity_equal(const CommandRequestIdentity *left,
                                    const CommandRequestIdentity *right)
{
    if (left == NULL || right == NULL) {
        return false;
    }

    return left->command_code == right->command_code &&
           left->param1 == right->param1 &&
           left->param2 == right->param2 &&
           left->param3 == right->param3 &&
           left->confirm_key == right->confirm_key;
}

bool command_status_is_final(uint16_t status)
{
    return status == COMMAND_STATUS_SUCCESS ||
           status == COMMAND_STATUS_REFUSED ||
           status == COMMAND_STATUS_FAILED ||
           status == COMMAND_STATUS_UNKNOWN ||
           status == COMMAND_STATUS_NOT_ALLOWED;
}
