#include "tr2/persistence/command_journal_record.h"

#include <string.h>

#define TR2_COMMAND_JOURNAL_RECORD_MAGIC UINT32_C(0x5452324A)
#define TR2_COMMAND_JOURNAL_RECORD_CRC_OFFSET 46u

static void put_u16_be(uint8_t *output, uint16_t value)
{
    output[0] = (uint8_t)(value >> 8u);
    output[1] = (uint8_t)(value & UINT16_C(0x00FF));
}

static void put_u32_be(uint8_t *output, uint32_t value)
{
    output[0] = (uint8_t)(value >> 24u);
    output[1] = (uint8_t)(value >> 16u);
    output[2] = (uint8_t)(value >> 8u);
    output[3] = (uint8_t)(value & UINT32_C(0x000000FF));
}

static uint16_t get_u16_be(const uint8_t *input)
{
    return (uint16_t)(((uint16_t)input[0] << 8u) | (uint16_t)input[1]);
}

static uint32_t get_u32_be(const uint8_t *input)
{
    return ((uint32_t)input[0] << 24u) |
           ((uint32_t)input[1] << 16u) |
           ((uint32_t)input[2] << 8u) |
           (uint32_t)input[3];
}

static uint32_t crc32_bytes(const uint8_t *bytes, size_t byte_count)
{
    uint32_t crc = UINT32_C(0xFFFFFFFF);
    size_t byte_index;

    for (byte_index = 0u; byte_index < byte_count; ++byte_index) {
        uint8_t bit_index;
        crc ^= (uint32_t)bytes[byte_index];
        for (bit_index = 0u; bit_index < 8u; ++bit_index) {
            if ((crc & UINT32_C(1)) != 0u) {
                crc = (crc >> 1u) ^ UINT32_C(0xEDB88320);
            } else {
                crc >>= 1u;
            }
        }
    }

    return crc ^ UINT32_C(0xFFFFFFFF);
}

Tr2Result tr2_command_journal_record_encode(const CommandJournalRecord *record_value,
                                            uint8_t *record,
                                            size_t record_size)
{
    const CommandJournalEntry *entry;
    uint32_t crc;

    if (record_value == NULL || record == NULL ||
        record_size != TR2_COMMAND_JOURNAL_RECORD_SIZE ||
        record_value->generation == 0u ||
        !command_journal_entry_is_consistent(&record_value->entry)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    entry = &record_value->entry;
    memset(record, 0, record_size);
    put_u32_be(&record[0], TR2_COMMAND_JOURNAL_RECORD_MAGIC);
    put_u16_be(&record[4], TR2_COMMAND_JOURNAL_RECORD_FORMAT_VERSION);
    put_u16_be(&record[6], (uint16_t)TR2_COMMAND_JOURNAL_RECORD_SIZE);
    put_u32_be(&record[8], record_value->generation);
    put_u16_be(&record[12], entry->transaction_id);
    put_u16_be(&record[14], (uint16_t)entry->lifecycle);
    put_u16_be(&record[16], entry->request_identity.command_code);
    put_u16_be(&record[18], entry->request_identity.param1);
    put_u16_be(&record[20], entry->request_identity.param2);
    put_u32_be(&record[22], entry->request_identity.param3);
    put_u16_be(&record[26], entry->request_identity.confirm_key);
    put_u16_be(&record[28], entry->has_final_result ? 1u : 0u);
    put_u16_be(&record[30], entry->final_result.status);
    put_u16_be(&record[32], entry->final_result.result_code);
    put_u16_be(&record[34], entry->final_result.result_detail);
    put_u16_be(&record[36], entry->terminal_timestamp.available ? 1u : 0u);
    put_u32_be(&record[38], entry->terminal_timestamp.value);
    put_u32_be(&record[42], entry->completion_order);
    crc = crc32_bytes(record, TR2_COMMAND_JOURNAL_RECORD_CRC_OFFSET);
    put_u32_be(&record[TR2_COMMAND_JOURNAL_RECORD_CRC_OFFSET], crc);
    return TR2_OK;
}

Tr2Result tr2_command_journal_record_decode(const uint8_t *record,
                                            size_t record_size,
                                            CommandJournalRecord *record_value)
{
    uint32_t stored_crc;
    uint32_t computed_crc;

    if (record == NULL || record_value == NULL ||
        record_size != TR2_COMMAND_JOURNAL_RECORD_SIZE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (get_u32_be(&record[0]) != TR2_COMMAND_JOURNAL_RECORD_MAGIC ||
        get_u16_be(&record[6]) != (uint16_t)TR2_COMMAND_JOURNAL_RECORD_SIZE) {
        return TR2_ERROR_CORRUPTED;
    }

    stored_crc = get_u32_be(&record[TR2_COMMAND_JOURNAL_RECORD_CRC_OFFSET]);
    computed_crc = crc32_bytes(record, TR2_COMMAND_JOURNAL_RECORD_CRC_OFFSET);
    if (stored_crc != computed_crc) {
        return TR2_ERROR_CORRUPTED;
    }
    if (get_u16_be(&record[4]) != TR2_COMMAND_JOURNAL_RECORD_FORMAT_VERSION) {
        return TR2_ERROR_UNSUPPORTED;
    }

    memset(record_value, 0, sizeof(*record_value));
    record_value->generation = get_u32_be(&record[8]);
    record_value->entry.transaction_id = get_u16_be(&record[12]);
    record_value->entry.lifecycle = (CommandLifecycleState)get_u16_be(&record[14]);
    record_value->entry.request_identity.command_code = get_u16_be(&record[16]);
    record_value->entry.request_identity.param1 = get_u16_be(&record[18]);
    record_value->entry.request_identity.param2 = get_u16_be(&record[20]);
    record_value->entry.request_identity.param3 = get_u32_be(&record[22]);
    record_value->entry.request_identity.confirm_key = get_u16_be(&record[26]);
    record_value->entry.has_final_result = (get_u16_be(&record[28]) != 0u);
    record_value->entry.final_result.status = get_u16_be(&record[30]);
    record_value->entry.final_result.result_code = get_u16_be(&record[32]);
    record_value->entry.final_result.result_detail = get_u16_be(&record[34]);
    record_value->entry.terminal_timestamp.available = (get_u16_be(&record[36]) != 0u);
    record_value->entry.terminal_timestamp.value = get_u32_be(&record[38]);
    record_value->entry.completion_order = get_u32_be(&record[42]);

    if (record_value->generation == 0u ||
        !command_journal_entry_is_consistent(&record_value->entry)) {
        memset(record_value, 0, sizeof(*record_value));
        return TR2_ERROR_CORRUPTED;
    }
    return TR2_OK;
}
