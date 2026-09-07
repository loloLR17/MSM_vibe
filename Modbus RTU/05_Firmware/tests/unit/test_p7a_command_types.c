#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "tr2/domain/command/command.h"

static void test_v1_command_codes_and_transaction_id_domain(void)
{
    assert(COMMAND_CODE_NONE == 0);
    assert(COMMAND_CODE_APPLY_CONFIGURATION == 1);
    assert(COMMAND_CODE_SYNCHRONIZE_TIME == 2);
    assert(COMMAND_CODE_START_ACQUISITION == 3);
    assert(COMMAND_CODE_STOP_ACQUISITION == 4);
    assert(COMMAND_CODE_SOFTWARE_RESET == 10);
    assert(COMMAND_CODE_RESET_STATISTICS == 11);

    assert(!command_transaction_id_is_valid(TR2_COMMAND_TRANSACTION_ID_INVALID));
    assert(command_transaction_id_is_valid(1u));
    assert(command_transaction_id_is_valid(UINT16_MAX));
}

static void test_canonical_request_identity_excludes_transaction_id(void)
{
    CommandRequest first;
    CommandRequest retry;

    memset(&first, 0, sizeof(first));
    memset(&retry, 0, sizeof(retry));

    first.transaction_id = 10u;
    first.identity.command_code = COMMAND_CODE_START_ACQUISITION;
    first.identity.param1 = 1u;
    first.identity.param2 = 2u;
    first.identity.param3 = 0x12345678u;
    first.identity.confirm_key = TR2_COMMAND_CONFIRM_KEY_NONE;

    retry = first;
    retry.transaction_id = 11u;

    assert(command_request_identity_equal(&first.identity, &retry.identity));
    assert(first.transaction_id != retry.transaction_id);
}

static void test_canonical_request_identity_detects_content_change(void)
{
    CommandRequestIdentity reference;
    CommandRequestIdentity changed;

    memset(&reference, 0, sizeof(reference));
    reference.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    reference.param1 = 0u;
    reference.param2 = 0u;
    reference.param3 = 0u;
    reference.confirm_key = TR2_COMMAND_CONFIRM_KEY_NONE;

    changed = reference;
    assert(command_request_identity_equal(&reference, &changed));

    changed.param3 = 1u;
    assert(!command_request_identity_equal(&reference, &changed));

    changed = reference;
    changed.confirm_key = TR2_COMMAND_CONFIRM_KEY_VALID;
    assert(!command_request_identity_equal(&reference, &changed));

    assert(!command_request_identity_equal(NULL, &reference));
    assert(!command_request_identity_equal(&reference, NULL));
}

static void test_internal_lifecycle_is_distinct_from_v1_status(void)
{
    assert(COMMAND_LIFECYCLE_RESERVED == 0);
    assert(COMMAND_LIFECYCLE_STARTED == 1);
    assert(COMMAND_LIFECYCLE_COMPLETED == 2);

    assert(!command_status_is_final(COMMAND_STATUS_NONE));
    assert(!command_status_is_final(COMMAND_STATUS_RECEIVED));
    assert(!command_status_is_final(COMMAND_STATUS_ACCEPTED));
    assert(!command_status_is_final(COMMAND_STATUS_RUNNING));
    assert(command_status_is_final(COMMAND_STATUS_SUCCESS));
    assert(command_status_is_final(COMMAND_STATUS_REFUSED));
    assert(command_status_is_final(COMMAND_STATUS_FAILED));
    assert(command_status_is_final(COMMAND_STATUS_UNKNOWN));
    assert(command_status_is_final(COMMAND_STATUS_NOT_ALLOWED));
}

static void test_last_snapshot_timestamp_absence_is_explicit(void)
{
    CommandSnapshot snapshot;

    memset(&snapshot, 0, sizeof(snapshot));

    assert(!snapshot.last.present);
    assert(!snapshot.last.terminal_timestamp.available);
    assert(snapshot.last.terminal_timestamp.value == 0u);

    snapshot.last.present = true;
    snapshot.last.command_code = COMMAND_CODE_SYNCHRONIZE_TIME;
    snapshot.last.transaction_id = 42u;
    snapshot.last.final_result.status = COMMAND_STATUS_SUCCESS;
    snapshot.last.final_result.result_code = COMMAND_RESULT_SUCCESS;
    snapshot.last.terminal_timestamp.available = true;
    snapshot.last.terminal_timestamp.value = 123456u;

    assert(snapshot.last.present);
    assert(snapshot.last.terminal_timestamp.available);
    assert(snapshot.last.terminal_timestamp.value == 123456u);
}

static void test_reconciliation_outcomes_do_not_define_new_v1_codes(void)
{
    CommandReconciliationOutcome outcome = COMMAND_RECONCILIATION_INDETERMINATE;

    assert(outcome == COMMAND_RECONCILIATION_INDETERMINATE);
    assert(COMMAND_RESULT_ACTIVE_CONFIGURATION_INVALID == 22);
}

int main(void)
{
    test_v1_command_codes_and_transaction_id_domain();
    test_canonical_request_identity_excludes_transaction_id();
    test_canonical_request_identity_detects_content_change();
    test_internal_lifecycle_is_distinct_from_v1_status();
    test_last_snapshot_timestamp_absence_is_explicit();
    test_reconciliation_outcomes_do_not_define_new_v1_codes();
    return 0;
}
