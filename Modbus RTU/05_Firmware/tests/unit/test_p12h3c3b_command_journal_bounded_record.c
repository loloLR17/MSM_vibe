#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/command_journal_bounded_record.h"

static CommandJournalBoundedRecord make_completed_record(void)
{
    CommandJournalBoundedRecord record;

    memset(&record, 0, sizeof(record));
    record.generation = 7u;
    record.admission_order = 19u;
    record.entry.transaction_id = UINT16_MAX;
    record.entry.request_identity.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    record.entry.request_identity.param1 = 11u;
    record.entry.request_identity.param2 = 22u;
    record.entry.request_identity.param3 = UINT32_C(0x12345678);
    record.entry.request_identity.confirm_key = TR2_COMMAND_CONFIRM_KEY_VALID;
    record.entry.lifecycle = COMMAND_LIFECYCLE_COMPLETED;
    record.entry.has_recovery_context = true;
    record.entry.recovery_context.kind = COMMAND_RECOVERY_CONTEXT_CONFIGURATION;
    record.entry.recovery_context.value1 = 1u;
    record.entry.recovery_context.value2 = 2u;
    record.entry.recovery_context.value3 = 3u;
    record.entry.has_final_result = true;
    record.entry.final_result.status = COMMAND_STATUS_SUCCESS;
    record.entry.final_result.result_code = COMMAND_RESULT_SUCCESS;
    record.entry.final_result.result_detail = 17u;
    record.entry.terminal_timestamp.available = true;
    record.entry.terminal_timestamp.value = 123456u;
    record.entry.completion_order = 23u;
    return record;
}

static void test_round_trip_preserves_record(void)
{
    CommandJournalBoundedRecord source = make_completed_record();
    CommandJournalBoundedRecord decoded;
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE];

    assert(tr2_command_journal_bounded_record_encode(&source, bytes, sizeof(bytes)) == TR2_OK);
    assert(tr2_command_journal_bounded_record_decode(bytes, sizeof(bytes), &decoded) == TR2_OK);
    assert(decoded.generation == source.generation);
    assert(decoded.admission_order == source.admission_order);
    assert(decoded.entry.transaction_id == source.entry.transaction_id);
    assert(command_request_identity_equal(&decoded.entry.request_identity,
                                          &source.entry.request_identity));
    assert(decoded.entry.lifecycle == source.entry.lifecycle);
    assert(decoded.entry.has_recovery_context == source.entry.has_recovery_context);
    assert(decoded.entry.recovery_context.kind == source.entry.recovery_context.kind);
    assert(decoded.entry.recovery_context.value1 == source.entry.recovery_context.value1);
    assert(decoded.entry.recovery_context.value2 == source.entry.recovery_context.value2);
    assert(decoded.entry.recovery_context.value3 == source.entry.recovery_context.value3);
    assert(decoded.entry.has_final_result == source.entry.has_final_result);
    assert(decoded.entry.final_result.status == source.entry.final_result.status);
    assert(decoded.entry.final_result.result_code == source.entry.final_result.result_code);
    assert(decoded.entry.final_result.result_detail == source.entry.final_result.result_detail);
    assert(decoded.entry.terminal_timestamp.available == source.entry.terminal_timestamp.available);
    assert(decoded.entry.terminal_timestamp.value == source.entry.terminal_timestamp.value);
    assert(decoded.entry.completion_order == source.entry.completion_order);
}

static void test_encode_rejects_zero_generation_and_admission_order(void)
{
    CommandJournalBoundedRecord record = make_completed_record();
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE];

    record.generation = 0u;
    assert(tr2_command_journal_bounded_record_encode(&record, bytes, sizeof(bytes)) ==
           TR2_ERROR_INVALID_ARGUMENT);

    record = make_completed_record();
    record.admission_order = 0u;
    assert(tr2_command_journal_bounded_record_encode(&record, bytes, sizeof(bytes)) ==
           TR2_ERROR_INVALID_ARGUMENT);
}

static void test_decode_rejects_crc_corruption(void)
{
    CommandJournalBoundedRecord source = make_completed_record();
    CommandJournalBoundedRecord decoded;
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE];

    assert(tr2_command_journal_bounded_record_encode(&source, bytes, sizeof(bytes)) == TR2_OK);
    bytes[22] ^= UINT8_C(0x01);
    assert(tr2_command_journal_bounded_record_decode(bytes, sizeof(bytes), &decoded) ==
           TR2_ERROR_CORRUPTED);
}

static void test_decode_reports_unsupported_version_with_valid_crc(void)
{
    CommandJournalBoundedRecord source = make_completed_record();
    CommandJournalBoundedRecord decoded;
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE];
    uint8_t bit;

    assert(tr2_command_journal_bounded_record_encode(&source, bytes, sizeof(bytes)) == TR2_OK);
    bytes[4] = 0u;
    bytes[5] = 2u;

    {
        uint32_t crc = UINT32_C(0xFFFFFFFF);
        size_t index;
        for (index = 0u; index < 66u; ++index) {
            crc ^= (uint32_t)bytes[index];
            for (bit = 0u; bit < 8u; ++bit) {
                crc = (crc & UINT32_C(1)) != 0u
                          ? (crc >> 1u) ^ UINT32_C(0xEDB88320)
                          : crc >> 1u;
            }
        }
        crc ^= UINT32_C(0xFFFFFFFF);
        bytes[66] = (uint8_t)(crc >> 24u);
        bytes[67] = (uint8_t)(crc >> 16u);
        bytes[68] = (uint8_t)(crc >> 8u);
        bytes[69] = (uint8_t)crc;
    }

    assert(tr2_command_journal_bounded_record_decode(bytes, sizeof(bytes), &decoded) ==
           TR2_ERROR_UNSUPPORTED);
}

static void test_invalid_sizes_are_rejected(void)
{
    CommandJournalBoundedRecord record = make_completed_record();
    CommandJournalBoundedRecord decoded;
    uint8_t bytes[TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE];

    assert(tr2_command_journal_bounded_record_encode(
               &record, bytes, TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE - 1u) ==
           TR2_ERROR_INVALID_ARGUMENT);
    assert(tr2_command_journal_bounded_record_decode(
               bytes, TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE - 1u, &decoded) ==
           TR2_ERROR_INVALID_ARGUMENT);
}

int main(void)
{
    test_round_trip_preserves_record();
    test_encode_rejects_zero_generation_and_admission_order();
    test_decode_rejects_crc_corruption();
    test_decode_reports_unsupported_version_with_valid_crc();
    test_invalid_sizes_are_rejected();
    return 0;
}
