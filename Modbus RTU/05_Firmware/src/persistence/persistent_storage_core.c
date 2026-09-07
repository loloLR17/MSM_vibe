#include "tr2/persistence/persistent_storage_core.h"
#include "tr2/persistence/boot_intent_store.h"

#include <string.h>

#define TR2_BOOT_INTENT_RECORD_MAGIC UINT32_C(0x54523242)
#define TR2_BOOT_INTENT_RECORD_CRC_OFFSET 12u

static bool media_contract_valid(const PersistentMedia *media)
{
    return media != NULL &&
           media->read != NULL &&
           media->write != NULL &&
           media->commit != NULL;
}

Tr2Result persistent_storage_core_init(
    PersistentStorageCore *core,
    const PersistentMedia *media)
{
    if (core == NULL || !media_contract_valid(media)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    core->media = media;
    core->initialized = true;
    return TR2_OK;
}

bool persistent_storage_core_is_initialized(const PersistentStorageCore *core)
{
    return core != NULL && core->initialized && media_contract_valid(core->media);
}

Tr2Result persistent_storage_core_read(
    const PersistentStorageCore *core,
    uint32_t offset,
    void *buffer,
    size_t size)
{
    if (!persistent_storage_core_is_initialized(core)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (buffer == NULL && size != 0u) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (size == 0u) {
        return TR2_OK;
    }

    return core->media->read(core->media->context, offset, buffer, size);
}

Tr2Result persistent_storage_core_write(
    PersistentStorageCore *core,
    uint32_t offset,
    const void *buffer,
    size_t size)
{
    if (!persistent_storage_core_is_initialized(core)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (buffer == NULL && size != 0u) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (size == 0u) {
        return TR2_OK;
    }

    return core->media->write(core->media->context, offset, buffer, size);
}

Tr2Result persistent_storage_core_commit(PersistentStorageCore *core)
{
    if (!persistent_storage_core_is_initialized(core)) {
        return TR2_ERROR_INVALID_STATE;
    }

    return core->media->commit(core->media->context);
}

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
            crc = (crc & UINT32_C(1)) != 0u
                      ? (crc >> 1u) ^ UINT32_C(0xEDB88320)
                      : crc >> 1u;
        }
    }
    return crc ^ UINT32_C(0xFFFFFFFF);
}

Tr2Result tr2_boot_intent_record_encode(const BootIntent *intent,
                                        uint8_t *record,
                                        size_t record_size)
{
    uint32_t crc;

    if (!boot_intent_is_valid(intent) || record == NULL ||
        record_size != TR2_BOOT_INTENT_RECORD_SIZE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (intent->kind == BOOT_INTENT_NONE) {
        return TR2_ERROR_INVALID_STATE;
    }

    memset(record, 0, record_size);
    put_u32_be(&record[0], TR2_BOOT_INTENT_RECORD_MAGIC);
    put_u16_be(&record[4], TR2_BOOT_INTENT_RECORD_FORMAT_VERSION);
    put_u16_be(&record[6], (uint16_t)TR2_BOOT_INTENT_RECORD_SIZE);
    put_u16_be(&record[8], (uint16_t)intent->kind);
    put_u16_be(&record[10], intent->transaction_id);
    crc = crc32_bytes(record, TR2_BOOT_INTENT_RECORD_CRC_OFFSET);
    put_u32_be(&record[TR2_BOOT_INTENT_RECORD_CRC_OFFSET], crc);
    return TR2_OK;
}

Tr2Result tr2_boot_intent_record_decode(const uint8_t *record,
                                        size_t record_size,
                                        BootIntent *intent)
{
    BootIntent decoded;
    uint32_t stored_crc;
    uint32_t computed_crc;

    if (record == NULL || intent == NULL ||
        record_size != TR2_BOOT_INTENT_RECORD_SIZE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (get_u32_be(&record[0]) != TR2_BOOT_INTENT_RECORD_MAGIC ||
        get_u16_be(&record[6]) != (uint16_t)TR2_BOOT_INTENT_RECORD_SIZE) {
        return TR2_ERROR_CORRUPTED;
    }
    stored_crc = get_u32_be(&record[TR2_BOOT_INTENT_RECORD_CRC_OFFSET]);
    computed_crc = crc32_bytes(record, TR2_BOOT_INTENT_RECORD_CRC_OFFSET);
    if (stored_crc != computed_crc) {
        return TR2_ERROR_CORRUPTED;
    }
    if (get_u16_be(&record[4]) != TR2_BOOT_INTENT_RECORD_FORMAT_VERSION) {
        return TR2_ERROR_UNSUPPORTED;
    }

    decoded.kind = (BootIntentKind)get_u16_be(&record[8]);
    decoded.transaction_id = get_u16_be(&record[10]);
    if (!boot_intent_is_valid(&decoded) || decoded.kind == BOOT_INTENT_NONE) {
        return TR2_ERROR_CORRUPTED;
    }
    *intent = decoded;
    return TR2_OK;
}

static bool boot_intent_record_is_uniform(const uint8_t *record, uint8_t value)
{
    size_t index;
    for (index = 0u; index < TR2_BOOT_INTENT_RECORD_SIZE; ++index) {
        if (record[index] != value) {
            return false;
        }
    }
    return true;
}

Tr2Result boot_intent_store_init(BootIntentStore *store,
                                 PersistentStorageCore *storage,
                                 uint32_t storage_offset)
{
    if (store == NULL || storage == NULL ||
        !persistent_storage_core_is_initialized(storage)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    store->storage = storage;
    store->storage_offset = storage_offset;
    store->initialized = true;
    store->recovery_required = false;
    return TR2_OK;
}

bool boot_intent_store_is_initialized(const BootIntentStore *store)
{
    return store != NULL && store->initialized && store->storage != NULL &&
           persistent_storage_core_is_initialized(store->storage);
}

bool boot_intent_store_recovery_required(const BootIntentStore *store)
{
    return boot_intent_store_is_initialized(store) && store->recovery_required;
}

Tr2Result boot_intent_store_commit(BootIntentStore *store,
                                   const BootIntent *intent)
{
    uint8_t record[TR2_BOOT_INTENT_RECORD_SIZE];
    Tr2Result result;

    if (!boot_intent_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (!boot_intent_is_valid(intent) || intent->kind == BOOT_INTENT_NONE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    result = tr2_boot_intent_record_encode(intent, record, sizeof(record));
    if (result != TR2_OK) {
        return result;
    }
    result = persistent_storage_core_write(store->storage, store->storage_offset,
                                           record, sizeof(record));
    if (result == TR2_OK) {
        result = persistent_storage_core_commit(store->storage);
    }
    if (result != TR2_OK) {
        store->recovery_required = true;
    }
    return result;
}

Tr2Result boot_intent_store_clear(BootIntentStore *store)
{
    uint8_t record[TR2_BOOT_INTENT_RECORD_SIZE] = {0};
    Tr2Result result;

    if (!boot_intent_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    result = persistent_storage_core_write(store->storage, store->storage_offset,
                                           record, sizeof(record));
    if (result == TR2_OK) {
        result = persistent_storage_core_commit(store->storage);
    }
    if (result != TR2_OK) {
        store->recovery_required = true;
    }
    return result;
}

Tr2Result boot_intent_store_recover(const BootIntentStore *store,
                                    BootIntentRecoveryResult *result)
{
    uint8_t record[TR2_BOOT_INTENT_RECORD_SIZE];
    Tr2Result status;

    if (!boot_intent_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (result == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(result, 0, sizeof(*result));
    result->intent = boot_intent_none();
    status = persistent_storage_core_read(store->storage, store->storage_offset,
                                          record, sizeof(record));
    if (status != TR2_OK) {
        result->status = BOOT_INTENT_RECOVERY_UNAVAILABLE;
        return TR2_OK;
    }
    if (boot_intent_record_is_uniform(record, UINT8_C(0x00)) ||
        boot_intent_record_is_uniform(record, UINT8_C(0xFF))) {
        result->status = BOOT_INTENT_RECOVERY_EMPTY;
        return TR2_OK;
    }

    status = tr2_boot_intent_record_decode(record, sizeof(record), &result->intent);
    if (status == TR2_OK) {
        result->status = BOOT_INTENT_RECOVERY_VALID;
    } else if (status == TR2_ERROR_UNSUPPORTED) {
        result->status = BOOT_INTENT_RECOVERY_UNSUPPORTED;
        result->intent = boot_intent_none();
    } else {
        result->status = BOOT_INTENT_RECOVERY_CORRUPTED;
        result->intent = boot_intent_none();
    }
    return TR2_OK;
}
