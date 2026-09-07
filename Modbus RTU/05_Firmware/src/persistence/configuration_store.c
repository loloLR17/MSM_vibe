#include "tr2/persistence/configuration_store.h"

#include <string.h>

typedef struct {
    Tr2Result decode_status;
    bool valid;
    ActiveConfigurationSnapshot snapshot;
} ConfigurationStoreSlotInfo;

static uint32_t slot_offset(size_t slot_index)
{
    return (uint32_t)(slot_index * TR2_CONFIGURATION_STORE_SLOT_SIZE);
}

static Tr2Result read_slot(const ConfigurationStore *store,
                           size_t slot_index,
                           ConfigurationStoreSlotInfo *info)
{
    uint8_t record[TR2_CONFIGURATION_RECORD_SIZE];
    Tr2Result result;

    memset(info, 0, sizeof(*info));

    result = persistent_storage_core_read(store->storage,
                                          slot_offset(slot_index),
                                          record,
                                          sizeof(record));
    if (result != TR2_OK) {
        return result;
    }

    info->decode_status = tr2_configuration_record_decode(record,
                                                           sizeof(record),
                                                           &info->snapshot);
    info->valid = (info->decode_status == TR2_OK);
    return TR2_OK;
}

static Tr2Result select_target_slot(const ConfigurationStoreSlotInfo slots[TR2_CONFIGURATION_STORE_SLOT_COUNT],
                                    size_t *target_slot,
                                    bool *has_current_generation,
                                    uint32_t *current_generation)
{
    const bool valid0 = slots[0].valid;
    const bool valid1 = slots[1].valid;

    if (valid0 && valid1) {
        *has_current_generation = true;
        if (slots[0].snapshot.generation >= slots[1].snapshot.generation) {
            *current_generation = slots[0].snapshot.generation;
            *target_slot = 1u;
        } else {
            *current_generation = slots[1].snapshot.generation;
            *target_slot = 0u;
        }
        return TR2_OK;
    }

    if (valid0) {
        *has_current_generation = true;
        *current_generation = slots[0].snapshot.generation;
        *target_slot = 1u;
        return TR2_OK;
    }

    if (valid1) {
        *has_current_generation = true;
        *current_generation = slots[1].snapshot.generation;
        *target_slot = 0u;
        return TR2_OK;
    }

    if (slots[0].decode_status == TR2_ERROR_UNSUPPORTED ||
        slots[1].decode_status == TR2_ERROR_UNSUPPORTED) {
        return TR2_ERROR_UNSUPPORTED;
    }

    *has_current_generation = false;
    *current_generation = 0u;
    *target_slot = 0u;
    return TR2_OK;
}

Tr2Result configuration_store_init(ConfigurationStore *store,
                                   PersistentStorageCore *storage)
{
    if (store == NULL || storage == NULL ||
        !persistent_storage_core_is_initialized(storage)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    store->storage = storage;
    store->initialized = true;
    return TR2_OK;
}

bool configuration_store_is_initialized(const ConfigurationStore *store)
{
    return store != NULL && store->initialized && store->storage != NULL &&
           persistent_storage_core_is_initialized(store->storage);
}

Tr2Result configuration_store_commit(ConfigurationStore *store,
                                     const ActiveConfigurationSnapshot *snapshot)
{
    ConfigurationStoreSlotInfo slots[TR2_CONFIGURATION_STORE_SLOT_COUNT];
    uint8_t record[TR2_CONFIGURATION_RECORD_SIZE];
    size_t target_slot;
    bool has_current_generation;
    uint32_t current_generation;
    size_t slot_index;
    Tr2Result result;

    if (!configuration_store_is_initialized(store)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (snapshot == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    for (slot_index = 0u; slot_index < TR2_CONFIGURATION_STORE_SLOT_COUNT; ++slot_index) {
        result = read_slot(store, slot_index, &slots[slot_index]);
        if (result != TR2_OK) {
            return result;
        }
    }

    result = select_target_slot(slots,
                                &target_slot,
                                &has_current_generation,
                                &current_generation);
    if (result != TR2_OK) {
        return result;
    }

    if (has_current_generation && snapshot->generation <= current_generation) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = tr2_configuration_record_encode(snapshot, record, sizeof(record));
    if (result != TR2_OK) {
        return result;
    }

    result = persistent_storage_core_write(store->storage,
                                           slot_offset(target_slot),
                                           record,
                                           sizeof(record));
    if (result != TR2_OK) {
        return result;
    }

    return persistent_storage_core_commit(store->storage);
}
