#include "tr2/persistence/configuration_store.h"

#include <string.h>

typedef enum {
    SLOT_RECOVERY_EMPTY = 0,
    SLOT_RECOVERY_VALID,
    SLOT_RECOVERY_CORRUPTED,
    SLOT_RECOVERY_UNSUPPORTED,
    SLOT_RECOVERY_UNAVAILABLE
} ConfigurationStoreSlotRecoveryStatus;

typedef struct {
    Tr2Result decode_status;
    bool valid;
    ActiveConfigurationSnapshot snapshot;
} ConfigurationStoreSlotInfo;

typedef struct {
    ConfigurationStoreSlotRecoveryStatus status;
    ActiveConfigurationSnapshot snapshot;
} ConfigurationStoreRecoveredSlot;

static uint32_t slot_offset(size_t slot_index)
{
    return (uint32_t)(slot_index * TR2_CONFIGURATION_STORE_SLOT_SIZE);
}

static bool record_is_uniform(const uint8_t *record, uint8_t value)
{
    size_t index;

    for (index = 0u; index < TR2_CONFIGURATION_RECORD_SIZE; ++index) {
        if (record[index] != value) {
            return false;
        }
    }

    return true;
}

static bool record_is_empty(const uint8_t *record)
{
    return record_is_uniform(record, UINT8_C(0x00)) ||
           record_is_uniform(record, UINT8_C(0xFF));
}

static Tr2Result read_slot_record(const ConfigurationStore *store,
                                  size_t slot_index,
                                  uint8_t record[TR2_CONFIGURATION_RECORD_SIZE])
{
    return persistent_storage_core_read(store->storage,
                                        slot_offset(slot_index),
                                        record,
                                        TR2_CONFIGURATION_RECORD_SIZE);
}

static Tr2Result read_slot(const ConfigurationStore *store,
                           size_t slot_index,
                           ConfigurationStoreSlotInfo *info)
{
    uint8_t record[TR2_CONFIGURATION_RECORD_SIZE];
    Tr2Result result;

    memset(info, 0, sizeof(*info));

    result = read_slot_record(store, slot_index, record);
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

static ConfigurationStoreSlotRecoveryStatus validate_recovered_snapshot(
    const ActiveConfigurationSnapshot *snapshot,
    const ConfigurationValidationEnvironment *environment)
{
    PreparedConfiguration prepared;
    ConfigurationValidationResult validation;

    memset(&prepared, 0, sizeof(prepared));
    prepared.generation = snapshot->generation;
    prepared.config_id = snapshot->config_id;
    prepared.payload = snapshot->payload;

    validation = configuration_validate(&prepared, environment, NULL);
    if (validation.status == CONFIGURATION_VALIDATION_VALID) {
        return SLOT_RECOVERY_VALID;
    }
    if (validation.status == CONFIGURATION_VALIDATION_ENVIRONMENT_NOT_CHARACTERIZED) {
        return SLOT_RECOVERY_UNAVAILABLE;
    }

    return SLOT_RECOVERY_CORRUPTED;
}

static ConfigurationStoreRecoveredSlot recover_slot(
    const ConfigurationStore *store,
    size_t slot_index,
    const ConfigurationValidationEnvironment *environment)
{
    ConfigurationStoreRecoveredSlot recovered;
    uint8_t record[TR2_CONFIGURATION_RECORD_SIZE];
    Tr2Result result;

    memset(&recovered, 0, sizeof(recovered));

    result = read_slot_record(store, slot_index, record);
    if (result != TR2_OK) {
        recovered.status = SLOT_RECOVERY_UNAVAILABLE;
        return recovered;
    }

    if (record_is_empty(record)) {
        recovered.status = SLOT_RECOVERY_EMPTY;
        return recovered;
    }

    result = tr2_configuration_record_decode(record,
                                              sizeof(record),
                                              &recovered.snapshot);
    if (result == TR2_ERROR_UNSUPPORTED) {
        recovered.status = SLOT_RECOVERY_UNSUPPORTED;
        return recovered;
    }
    if (result != TR2_OK) {
        recovered.status = SLOT_RECOVERY_CORRUPTED;
        return recovered;
    }

    recovered.status = validate_recovered_snapshot(&recovered.snapshot, environment);
    return recovered;
}

static ConfigurationRecoveryStatus select_recovery_status(
    const ConfigurationStoreRecoveredSlot slots[TR2_CONFIGURATION_STORE_SLOT_COUNT],
    bool *has_snapshot,
    ActiveConfigurationSnapshot *snapshot)
{
    size_t index;
    bool saw_unavailable = false;
    bool saw_unsupported = false;
    bool saw_corrupted = false;

    *has_snapshot = false;
    memset(snapshot, 0, sizeof(*snapshot));

    for (index = 0u; index < TR2_CONFIGURATION_STORE_SLOT_COUNT; ++index) {
        if (slots[index].status == SLOT_RECOVERY_VALID) {
            if (!*has_snapshot || slots[index].snapshot.generation > snapshot->generation) {
                *snapshot = slots[index].snapshot;
                *has_snapshot = true;
            }
        } else if (slots[index].status == SLOT_RECOVERY_UNAVAILABLE) {
            saw_unavailable = true;
        } else if (slots[index].status == SLOT_RECOVERY_UNSUPPORTED) {
            saw_unsupported = true;
        } else if (slots[index].status == SLOT_RECOVERY_CORRUPTED) {
            saw_corrupted = true;
        }
    }

    if (*has_snapshot) {
        return CONFIGURATION_RECOVERY_VALID;
    }
    if (saw_unavailable) {
        return CONFIGURATION_RECOVERY_UNAVAILABLE;
    }
    if (saw_unsupported) {
        return CONFIGURATION_RECOVERY_UNSUPPORTED;
    }
    if (saw_corrupted) {
        return CONFIGURATION_RECOVERY_CORRUPTED;
    }

    return CONFIGURATION_RECOVERY_EMPTY;
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
    store->recovery_required = false;
    return TR2_OK;
}

bool configuration_store_is_initialized(const ConfigurationStore *store)
{
    return store != NULL && store->initialized && store->storage != NULL &&
           persistent_storage_core_is_initialized(store->storage);
}

bool configuration_store_recovery_required(const ConfigurationStore *store)
{
    return configuration_store_is_initialized(store) && store->recovery_required;
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

    if (!configuration_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (snapshot == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    for (slot_index = 0u; slot_index < TR2_CONFIGURATION_STORE_SLOT_COUNT; ++slot_index) {
        result = read_slot(store, slot_index, &slots[slot_index]);
        if (result != TR2_OK) {
            store->recovery_required = true;
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
        store->recovery_required = true;
        return result;
    }

    result = persistent_storage_core_commit(store->storage);
    if (result != TR2_OK) {
        store->recovery_required = true;
        return result;
    }

    return TR2_OK;
}

Tr2Result configuration_store_recover(
    const ConfigurationStore *store,
    const ConfigurationValidationEnvironment *environment,
    ConfigurationRecoveryResult *result)
{
    ConfigurationStoreRecoveredSlot slots[TR2_CONFIGURATION_STORE_SLOT_COUNT];
    size_t slot_index;

    if (!configuration_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (environment == NULL || result == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(result, 0, sizeof(*result));

    for (slot_index = 0u; slot_index < TR2_CONFIGURATION_STORE_SLOT_COUNT; ++slot_index) {
        slots[slot_index] = recover_slot(store, slot_index, environment);
    }

    result->status = select_recovery_status(slots,
                                            &result->has_snapshot,
                                            &result->snapshot);
    return TR2_OK;
}
