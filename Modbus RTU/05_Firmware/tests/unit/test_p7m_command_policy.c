#include <assert.h>
#include <string.h>

#include "tr2/application/command_policy.h"
#include "tr2/modbus/projection.h"

static CommandRequest request(uint16_t code)
{
    CommandRequest value;
    memset(&value, 0, sizeof(value));
    value.transaction_id = 77u;
    value.identity.command_code = code;
    return value;
}

static void test_engine_flags_are_v1_bounded_and_non_cancellable(void)
{
    CommandEngineFlagsSource source = {0};
    uint16_t flags;

    source.ready = true;
    source.running = true;
    source.confirmation_required = true;
    source.maintenance_active = true;
    source.acquisition_active = true;
    source.critical_fault_active = true;
    source.active_configuration_valid = true;
    source.prepared_sync_available = true;
    source.prepared_configuration_available = true;
    source.command_logging_performed = true;

    flags = command_engine_flags_project(&source);
    assert((flags & COMMAND_ENGINE_FLAG_READY) != 0u);
    assert((flags & COMMAND_ENGINE_FLAG_RUNNING) != 0u);
    assert((flags & COMMAND_ENGINE_FLAG_CONFIRMATION_REQUIRED) != 0u);
    assert((flags & COMMAND_ENGINE_FLAG_MAINTENANCE_ACTIVE) != 0u);
    assert((flags & COMMAND_ENGINE_FLAG_ACQUISITION_ACTIVE) != 0u);
    assert((flags & COMMAND_ENGINE_FLAG_CRITICAL_FAULT_ACTIVE) != 0u);
    assert((flags & COMMAND_ENGINE_FLAG_ACTIVE_CONFIGURATION_VALID) != 0u);
    assert((flags & COMMAND_ENGINE_FLAG_PREPARED_SYNC_AVAILABLE) != 0u);
    assert((flags & COMMAND_ENGINE_FLAG_PREPARED_CONFIG_AVAILABLE) != 0u);
    assert((flags & COMMAND_ENGINE_FLAG_CANCELLATION_SUPPORTED) == 0u);
    assert((flags & COMMAND_ENGINE_FLAG_COMMAND_LOGGING_PERFORMED) != 0u);
    assert((flags & (uint16_t)~COMMAND_ENGINE_FLAG_V1_ALLOWED_MASK) == 0u);
}

static void test_known_parameter_and_confirmation_protections(void)
{
    CommandRequest value = request(COMMAND_CODE_APPLY_CONFIGURATION);
    assert(command_request_policy_validate(&value) == COMMAND_REQUEST_POLICY_VALID);

    value.identity.param2 = 1u;
    assert(command_request_policy_validate(&value) ==
           COMMAND_REQUEST_POLICY_INVALID_PARAMETER);

    value = request(COMMAND_CODE_START_ACQUISITION);
    value.identity.param3 = 1u;
    assert(command_request_policy_validate(&value) ==
           COMMAND_REQUEST_POLICY_INVALID_PARAMETER);

    value = request(COMMAND_CODE_SELFTEST);
    assert(command_request_policy_validate(&value) == COMMAND_REQUEST_POLICY_VALID);
    value.identity.param1 = 1u;
    assert(command_request_policy_validate(&value) ==
           COMMAND_REQUEST_POLICY_INVALID_PARAMETER);

    value = request(COMMAND_CODE_REFRESH_INDICATORS);
    assert(command_request_policy_validate(&value) == COMMAND_REQUEST_POLICY_VALID);
    value.identity.param1 = 1u;
    assert(command_request_policy_validate(&value) ==
           COMMAND_REQUEST_POLICY_INVALID_PARAMETER);

    value = request(COMMAND_CODE_ENTER_MAINTENANCE);
    assert(command_request_policy_validate(&value) == COMMAND_REQUEST_POLICY_VALID);
    value.identity.param1 = 1u;
    assert(command_request_policy_validate(&value) ==
           COMMAND_REQUEST_POLICY_INVALID_PARAMETER);

    value = request(COMMAND_CODE_EXIT_MAINTENANCE);
    assert(command_request_policy_validate(&value) == COMMAND_REQUEST_POLICY_VALID);
    value.identity.param3 = 1u;
    assert(command_request_policy_validate(&value) ==
           COMMAND_REQUEST_POLICY_INVALID_PARAMETER);

    value = request(COMMAND_CODE_ACKNOWLEDGE_FAULT);
    value.identity.param1 = UINT16_C(0x1234);
    assert(command_request_policy_validate(&value) == COMMAND_REQUEST_POLICY_VALID);
    value.identity.param1 = 0u;
    assert(command_request_policy_validate(&value) ==
           COMMAND_REQUEST_POLICY_INVALID_PARAMETER);
    value.identity.param2 = 1u;
    assert(command_request_policy_validate(&value) == COMMAND_REQUEST_POLICY_VALID);
    value.identity.param1 = UINT16_C(0x1234);
    assert(command_request_policy_validate(&value) ==
           COMMAND_REQUEST_POLICY_INVALID_PARAMETER);
    value.identity.param1 = 0u;
    value.identity.param2 = 2u;
    assert(command_request_policy_validate(&value) ==
           COMMAND_REQUEST_POLICY_INVALID_PARAMETER);
    value.identity.param2 = 1u;
    value.identity.param3 = 1u;
    assert(command_request_policy_validate(&value) ==
           COMMAND_REQUEST_POLICY_INVALID_PARAMETER);

    value = request(COMMAND_CODE_SYNCHRONIZE_TIME);
    value.identity.param1 = 9u;
    assert(command_request_policy_validate(&value) == COMMAND_REQUEST_POLICY_VALID);

    value = request(COMMAND_CODE_SOFTWARE_RESET);
    assert(command_request_policy_validate(&value) ==
           COMMAND_REQUEST_POLICY_CONFIRMATION_MISSING);
    value.identity.confirm_key = TR2_COMMAND_CONFIRM_KEY_VALID;
    assert(command_request_policy_validate(&value) == COMMAND_REQUEST_POLICY_VALID);
    value.identity.param1 = 1u;
    assert(command_request_policy_validate(&value) ==
           COMMAND_REQUEST_POLICY_INVALID_PARAMETER);
    value.identity.param1 = 0u;
    value.identity.param3 = UINT32_C(1);
    assert(command_request_policy_validate(&value) ==
           COMMAND_REQUEST_POLICY_INVALID_PARAMETER);

    value = request(COMMAND_CODE_RESET_STATISTICS);
    value.identity.confirm_key = TR2_COMMAND_CONFIRM_KEY_VALID;
    assert(command_request_policy_validate(&value) == COMMAND_REQUEST_POLICY_VALID);
}

static void test_cancel_is_consumed_without_cancelling_business_effect(void)
{
    CommandRequestMailbox mailbox;
    command_request_mailbox_init(&mailbox);
    mailbox.control = COMMAND_REQUEST_CONTROL_CANCEL_REQUEST;

    assert(command_policy_consume_cancel_request(&mailbox) ==
           COMMAND_CANCEL_NOT_CANCELLABLE);
    assert(!command_request_mailbox_cancel_requested(&mailbox));
    assert(command_policy_consume_cancel_request(&mailbox) == COMMAND_CANCEL_NONE);
}

static void test_b5_projection_masks_reserved_engine_flag_bits(void)
{
    CommandRequestMailbox mailbox;
    CommandSnapshot snapshot;
    ModbusBlock5ProjectionSource source;
    ModbusBlock5Image image;

    command_request_mailbox_init(&mailbox);
    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.engine_flags = UINT16_C(0xFFFF);
    source.mailbox = &mailbox;
    source.snapshot = &snapshot;

    assert(modbus_project_b5(&source, &image) == TR2_OK);
    assert(image.registers[13] == COMMAND_ENGINE_FLAG_V1_ALLOWED_MASK);
}

int main(void)
{
    test_engine_flags_are_v1_bounded_and_non_cancellable();
    test_known_parameter_and_confirmation_protections();
    test_cancel_is_consumed_without_cancelling_business_effect();
    test_b5_projection_masks_reserved_engine_flag_bits();
    return 0;
}
