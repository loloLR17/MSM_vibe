#include "tr2/persistence/campaign_repository_store.h"

#include <string.h>

#define TR2_CAMPAIGN_ALLOCATOR_MAGIC UINT32_C(0x54523241)
#define TR2_CAMPAIGN_RECORDS_BASE \
    (TR2_CAMPAIGN_ALLOCATOR_SLOT_COUNT * TR2_CAMPAIGN_ALLOCATOR_RECORD_SIZE)

typedef struct {
    bool valid;
    bool unsupported;
    uint32_t generation;
    CampaignId next_id;
} AllocatorState;

typedef struct {
    bool valid;
    bool unsupported;
    CampaignMetadata metadata;
} CampaignCopy;

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
            crc = ((crc & UINT32_C(1)) != 0u)
                      ? ((crc >> 1u) ^ UINT32_C(0xEDB88320))
                      : (crc >> 1u);
        }
    }
    return crc ^ UINT32_C(0xFFFFFFFF);
}

static bool bytes_are_empty(const uint8_t *bytes, size_t size)
{
    size_t index;
    bool all_zero = true;
    bool all_ff = true;
    for (index = 0u; index < size; ++index) {
        all_zero = all_zero && bytes[index] == UINT8_C(0x00);
        all_ff = all_ff && bytes[index] == UINT8_C(0xFF);
    }
    return all_zero || all_ff;
}

static uint32_t allocator_offset(size_t slot)
{
    return (uint32_t)(slot * TR2_CAMPAIGN_ALLOCATOR_RECORD_SIZE);
}

static uint32_t campaign_copy_offset(size_t slot, size_t copy)
{
    return (uint32_t)(TR2_CAMPAIGN_RECORDS_BASE +
                      (slot * TR2_CAMPAIGN_REPOSITORY_COPY_COUNT + copy) *
                          TR2_CAMPAIGN_RECORD_SIZE);
}

static Tr2Result read_allocator(const CampaignRepositoryStore *store,
                                size_t slot,
                                AllocatorState *state)
{
    uint8_t record[TR2_CAMPAIGN_ALLOCATOR_RECORD_SIZE];
    uint32_t stored_crc;
    uint32_t computed_crc;
    Tr2Result result;

    memset(state, 0, sizeof(*state));
    result = persistent_storage_core_read(store->storage,
                                          allocator_offset(slot),
                                          record,
                                          sizeof(record));
    if (result != TR2_OK) {
        return result;
    }
    if (bytes_are_empty(record, sizeof(record))) {
        return TR2_OK;
    }
    if (get_u32_be(&record[0]) != TR2_CAMPAIGN_ALLOCATOR_MAGIC ||
        get_u16_be(&record[6]) != TR2_CAMPAIGN_ALLOCATOR_RECORD_SIZE) {
        return TR2_OK;
    }
    stored_crc = get_u32_be(&record[16]);
    computed_crc = crc32_bytes(record, 16u);
    if (stored_crc != computed_crc) {
        return TR2_OK;
    }
    if (get_u16_be(&record[4]) != UINT16_C(1)) {
        state->unsupported = true;
        return TR2_OK;
    }
    state->generation = get_u32_be(&record[8]);
    state->next_id = get_u32_be(&record[12]);
    state->valid = state->next_id != TR2_CAMPAIGN_ID_INVALID;
    return TR2_OK;
}

static Tr2Result write_allocator(CampaignRepositoryStore *store,
                                 size_t slot,
                                 uint32_t generation,
                                 CampaignId next_id)
{
    uint8_t record[TR2_CAMPAIGN_ALLOCATOR_RECORD_SIZE];
    uint32_t crc;
    Tr2Result result;

    memset(record, 0, sizeof(record));
    put_u32_be(&record[0], TR2_CAMPAIGN_ALLOCATOR_MAGIC);
    put_u16_be(&record[4], UINT16_C(1));
    put_u16_be(&record[6], TR2_CAMPAIGN_ALLOCATOR_RECORD_SIZE);
    put_u32_be(&record[8], generation);
    put_u32_be(&record[12], next_id);
    crc = crc32_bytes(record, 16u);
    put_u32_be(&record[16], crc);

    result = persistent_storage_core_write(store->storage,
                                           allocator_offset(slot),
                                           record,
                                           sizeof(record));
    if (result != TR2_OK) {
        store->recovery_required = true;
        return result;
    }
    result = persistent_storage_core_commit(store->storage);
    if (result != TR2_OK) {
        store->recovery_required = true;
    }
    return result;
}

static Tr2Result read_campaign_copy(const CampaignRepositoryStore *store,
                                    size_t slot,
                                    size_t copy,
                                    CampaignCopy *result_copy)
{
    uint8_t record[TR2_CAMPAIGN_RECORD_SIZE];
    Tr2Result result;

    memset(result_copy, 0, sizeof(*result_copy));
    result = persistent_storage_core_read(store->storage,
                                          campaign_copy_offset(slot, copy),
                                          record,
                                          sizeof(record));
    if (result != TR2_OK) {
        return result;
    }
    if (bytes_are_empty(record, sizeof(record))) {
        return TR2_OK;
    }
    result = tr2_campaign_record_decode(record,
                                        sizeof(record),
                                        &result_copy->metadata);
    if (result == TR2_ERROR_UNSUPPORTED) {
        result_copy->unsupported = true;
        return TR2_OK;
    }
    if (result == TR2_OK) {
        result_copy->valid = true;
        return TR2_OK;
    }
    return TR2_OK;
}

static bool select_campaign(const CampaignCopy copies[TR2_CAMPAIGN_REPOSITORY_COPY_COUNT],
                            CampaignMetadata *metadata,
                            bool *unsupported)
{
    *unsupported = copies[0].unsupported || copies[1].unsupported;
    if (copies[1].valid) {
        *metadata = copies[1].metadata;
        return true;
    }
    if (copies[0].valid) {
        *metadata = copies[0].metadata;
        return true;
    }
    memset(metadata, 0, sizeof(*metadata));
    return false;
}

static Tr2Result load_slot(const CampaignRepositoryStore *store,
                           size_t slot,
                           CampaignMetadata *metadata,
                           bool *valid,
                           bool *unsupported)
{
    CampaignCopy copies[TR2_CAMPAIGN_REPOSITORY_COPY_COUNT];
    size_t copy;
    Tr2Result result;

    for (copy = 0u; copy < TR2_CAMPAIGN_REPOSITORY_COPY_COUNT; ++copy) {
        result = read_campaign_copy(store, slot, copy, &copies[copy]);
        if (result != TR2_OK) {
            return result;
        }
    }
    *valid = select_campaign(copies, metadata, unsupported);
    return TR2_OK;
}

static Tr2Result write_campaign_copy(CampaignRepositoryStore *store,
                                     size_t slot,
                                     size_t copy,
                                     const CampaignMetadata *metadata)
{
    uint8_t record[TR2_CAMPAIGN_RECORD_SIZE];
    Tr2Result result;

    result = tr2_campaign_record_encode(metadata, record, sizeof(record));
    if (result != TR2_OK) {
        return result;
    }
    result = persistent_storage_core_write(store->storage,
                                           campaign_copy_offset(slot, copy),
                                           record,
                                           sizeof(record));
    if (result != TR2_OK) {
        store->recovery_required = true;
        return result;
    }
    result = persistent_storage_core_commit(store->storage);
    if (result != TR2_OK) {
        store->recovery_required = true;
    }
    return result;
}

static Tr2Result repo_reserve_campaign_id(void *context, CampaignIdReservation *reservation)
{
    CampaignRepositoryStore *store = (CampaignRepositoryStore *)context;
    AllocatorState allocators[TR2_CAMPAIGN_ALLOCATOR_SLOT_COUNT];
    CampaignId max_present = 0u;
    CampaignId candidate;
    uint32_t generation = 0u;
    size_t target_allocator = 0u;
    size_t index;
    Tr2Result result;

    if (!campaign_repository_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (reservation == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memset(reservation, 0, sizeof(*reservation));

    for (index = 0u; index < TR2_CAMPAIGN_ALLOCATOR_SLOT_COUNT; ++index) {
        result = read_allocator(store, index, &allocators[index]);
        if (result != TR2_OK) {
            store->recovery_required = true;
            return result;
        }
        if (allocators[index].unsupported) {
            return TR2_ERROR_UNSUPPORTED;
        }
        if (allocators[index].valid && allocators[index].generation >= generation) {
            generation = allocators[index].generation;
            target_allocator = (index + 1u) % TR2_CAMPAIGN_ALLOCATOR_SLOT_COUNT;
        }
    }

    candidate = UINT32_C(1);
    for (index = 0u; index < TR2_CAMPAIGN_REPOSITORY_CAPACITY; ++index) {
        CampaignMetadata metadata;
        bool valid;
        bool unsupported;
        result = load_slot(store, index, &metadata, &valid, &unsupported);
        if (result != TR2_OK) {
            store->recovery_required = true;
            return result;
        }
        if (unsupported) {
            return TR2_ERROR_UNSUPPORTED;
        }
        if (valid && metadata.campaign_id > max_present) {
            max_present = metadata.campaign_id;
        }
    }

    for (index = 0u; index < TR2_CAMPAIGN_ALLOCATOR_SLOT_COUNT; ++index) {
        if (allocators[index].valid && allocators[index].generation == generation) {
            candidate = allocators[index].next_id;
        }
    }
    if (candidate <= max_present) {
        if (max_present == UINT32_MAX) {
            return TR2_ERROR_NOT_AVAILABLE;
        }
        candidate = max_present + UINT32_C(1);
    }
    if (candidate == UINT32_MAX) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    result = write_allocator(store,
                             target_allocator,
                             generation + UINT32_C(1),
                             candidate + UINT32_C(1));
    if (result != TR2_OK) {
        return result;
    }
    reservation->campaign_id = candidate;
    reservation->valid = true;
    return TR2_OK;
}

static Tr2Result repo_open_campaign(void *context, const CampaignMetadata *metadata)
{
    CampaignRepositoryStore *store = (CampaignRepositoryStore *)context;
    size_t index;
    size_t free_slot = TR2_CAMPAIGN_REPOSITORY_CAPACITY;
    Tr2Result result;

    if (!campaign_repository_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (metadata == NULL || metadata->campaign_id == TR2_CAMPAIGN_ID_INVALID ||
        metadata->lifecycle_state != CAMPAIGN_LIFECYCLE_OPEN) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    for (index = 0u; index < TR2_CAMPAIGN_REPOSITORY_CAPACITY; ++index) {
        CampaignMetadata existing;
        bool valid;
        bool unsupported;
        result = load_slot(store, index, &existing, &valid, &unsupported);
        if (result != TR2_OK) {
            store->recovery_required = true;
            return result;
        }
        if (unsupported) {
            return TR2_ERROR_UNSUPPORTED;
        }
        if (valid && existing.campaign_id == metadata->campaign_id) {
            return TR2_ERROR_INVALID_ARGUMENT;
        }
        if (!valid && free_slot == TR2_CAMPAIGN_REPOSITORY_CAPACITY) {
            free_slot = index;
        }
    }
    if (free_slot == TR2_CAMPAIGN_REPOSITORY_CAPACITY) {
        return TR2_ERROR_NOT_AVAILABLE;
    }
    return write_campaign_copy(store, free_slot, 0u, metadata);
}

static Tr2Result repo_close_campaign(void *context, const CampaignMetadata *metadata)
{
    CampaignRepositoryStore *store = (CampaignRepositoryStore *)context;
    size_t index;
    Tr2Result result;

    if (!campaign_repository_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (metadata == NULL || metadata->campaign_id == TR2_CAMPAIGN_ID_INVALID ||
        metadata->lifecycle_state != CAMPAIGN_LIFECYCLE_CLOSED) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    for (index = 0u; index < TR2_CAMPAIGN_REPOSITORY_CAPACITY; ++index) {
        CampaignMetadata existing;
        bool valid;
        bool unsupported;
        result = load_slot(store, index, &existing, &valid, &unsupported);
        if (result != TR2_OK) {
            store->recovery_required = true;
            return result;
        }
        if (unsupported) {
            return TR2_ERROR_UNSUPPORTED;
        }
        if (valid && existing.campaign_id == metadata->campaign_id) {
            return write_campaign_copy(store, index, 1u, metadata);
        }
    }
    return TR2_ERROR_NOT_FOUND;
}

static Tr2Result repo_get_inventory_summary(void *context, CampaignInventorySummary *summary)
{
    CampaignRepositoryStore *store = (CampaignRepositoryStore *)context;
    size_t index;
    Tr2Result result;

    if (!campaign_repository_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (summary == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memset(summary, 0, sizeof(*summary));
    for (index = 0u; index < TR2_CAMPAIGN_REPOSITORY_CAPACITY; ++index) {
        CampaignMetadata metadata;
        bool valid;
        bool unsupported;
        result = load_slot(store, index, &metadata, &valid, &unsupported);
        if (result != TR2_OK) {
            store->recovery_required = true;
            return result;
        }
        if (unsupported) {
            return TR2_ERROR_UNSUPPORTED;
        }
        summary->total_campaign_count += valid ? 1u : 0u;
        summary->valid_campaign_count += valid ? 1u : 0u;
    }
    return TR2_OK;
}

static Tr2Result repo_get_campaign_by_index(void *context,
                                            size_t wanted,
                                            CampaignMetadata *metadata)
{
    CampaignRepositoryStore *store = (CampaignRepositoryStore *)context;
    size_t index;
    size_t logical_index = 0u;
    Tr2Result result;

    if (!campaign_repository_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (metadata == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    for (index = 0u; index < TR2_CAMPAIGN_REPOSITORY_CAPACITY; ++index) {
        bool valid;
        bool unsupported;
        result = load_slot(store, index, metadata, &valid, &unsupported);
        if (result != TR2_OK) {
            store->recovery_required = true;
            return result;
        }
        if (unsupported) {
            return TR2_ERROR_UNSUPPORTED;
        }
        if (valid) {
            if (logical_index == wanted) {
                return TR2_OK;
            }
            logical_index++;
        }
    }
    memset(metadata, 0, sizeof(*metadata));
    return TR2_ERROR_NOT_FOUND;
}

static Tr2Result repo_get_campaign_by_id(void *context,
                                         CampaignId id,
                                         CampaignMetadata *metadata)
{
    CampaignRepositoryStore *store = (CampaignRepositoryStore *)context;
    size_t index;
    Tr2Result result;

    if (!campaign_repository_store_is_initialized(store) || store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (metadata == NULL || id == TR2_CAMPAIGN_ID_INVALID) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    for (index = 0u; index < TR2_CAMPAIGN_REPOSITORY_CAPACITY; ++index) {
        bool valid;
        bool unsupported;
        result = load_slot(store, index, metadata, &valid, &unsupported);
        if (result != TR2_OK) {
            store->recovery_required = true;
            return result;
        }
        if (unsupported) {
            return TR2_ERROR_UNSUPPORTED;
        }
        if (valid && metadata->campaign_id == id) {
            return TR2_OK;
        }
    }
    memset(metadata, 0, sizeof(*metadata));
    return TR2_ERROR_NOT_FOUND;
}

static Tr2Result repo_recover(void *context, CampaignRepositoryRecoveryResult *result_out)
{
    CampaignRepositoryStore *store = (CampaignRepositoryStore *)context;
    size_t index;
    bool saw_corrupted = false;
    bool saw_unsupported = false;

    if (!campaign_repository_store_is_initialized(store)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (result_out == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memset(result_out, 0, sizeof(*result_out));

    for (index = 0u; index < TR2_CAMPAIGN_REPOSITORY_CAPACITY; ++index) {
        CampaignCopy copies[TR2_CAMPAIGN_REPOSITORY_COPY_COUNT];
        CampaignMetadata metadata;
        bool valid;
        bool unsupported;
        size_t copy;
        Tr2Result result;
        for (copy = 0u; copy < TR2_CAMPAIGN_REPOSITORY_COPY_COUNT; ++copy) {
            uint8_t raw[TR2_CAMPAIGN_RECORD_SIZE];
            result = persistent_storage_core_read(store->storage,
                                                  campaign_copy_offset(index, copy),
                                                  raw,
                                                  sizeof(raw));
            if (result != TR2_OK) {
                result_out->status = CAMPAIGN_REPOSITORY_RECOVERY_UNAVAILABLE;
                return TR2_OK;
            }
            if (!bytes_are_empty(raw, sizeof(raw)) &&
                tr2_campaign_record_decode(raw, sizeof(raw), &copies[copy].metadata) != TR2_OK) {
                Tr2Result decoded = tr2_campaign_record_decode(raw, sizeof(raw), &copies[copy].metadata);
                if (decoded == TR2_ERROR_UNSUPPORTED) {
                    copies[copy].unsupported = true;
                    saw_unsupported = true;
                } else {
                    saw_corrupted = true;
                }
            } else {
                copies[copy].valid = !bytes_are_empty(raw, sizeof(raw));
                copies[copy].unsupported = false;
            }
        }
        valid = select_campaign(copies, &metadata, &unsupported);
        (void)unsupported;
        if (valid) {
            result_out->inventory.total_campaign_count += 1u;
            result_out->inventory.valid_campaign_count += 1u;
        }
    }

    if (result_out->inventory.valid_campaign_count > 0u) {
        result_out->status = CAMPAIGN_REPOSITORY_RECOVERY_VALID;
    } else if (saw_unsupported) {
        result_out->status = CAMPAIGN_REPOSITORY_RECOVERY_UNSUPPORTED;
    } else if (saw_corrupted) {
        result_out->status = CAMPAIGN_REPOSITORY_RECOVERY_CORRUPTED;
    } else {
        result_out->status = CAMPAIGN_REPOSITORY_RECOVERY_EMPTY;
    }
    store->recovery_required = false;
    return TR2_OK;
}

Tr2Result campaign_repository_store_init(CampaignRepositoryStore *store,
                                         PersistentStorageCore *storage)
{
    if (store == NULL || storage == NULL ||
        !persistent_storage_core_is_initialized(storage)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memset(store, 0, sizeof(*store));
    store->storage = storage;
    store->initialized = true;
    store->interface.context = store;
    store->interface.reserve_campaign_id = repo_reserve_campaign_id;
    store->interface.open_campaign = repo_open_campaign;
    store->interface.close_campaign = repo_close_campaign;
    store->interface.get_inventory_summary = repo_get_inventory_summary;
    store->interface.get_campaign_by_index = repo_get_campaign_by_index;
    store->interface.get_campaign_by_id = repo_get_campaign_by_id;
    store->interface.recover = repo_recover;
    return TR2_OK;
}

bool campaign_repository_store_is_initialized(const CampaignRepositoryStore *store)
{
    return store != NULL && store->initialized && store->storage != NULL &&
           persistent_storage_core_is_initialized(store->storage);
}

bool campaign_repository_store_recovery_required(const CampaignRepositoryStore *store)
{
    return campaign_repository_store_is_initialized(store) && store->recovery_required;
}

CampaignRepository *campaign_repository_store_interface(CampaignRepositoryStore *store)
{
    return campaign_repository_store_is_initialized(store) ? &store->interface : NULL;
}
