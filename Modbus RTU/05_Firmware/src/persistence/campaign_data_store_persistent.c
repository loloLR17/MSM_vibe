#include "tr2/persistence/campaign_data_store_persistent.h"

#include <string.h>

#define TR2_CAMPAIGN_DATA_DESCRIPTOR_MAGIC UINT32_C(0x54523244)
#define TR2_CAMPAIGN_DATA_CHUNK_MAGIC UINT32_C(0x5452324B)
#define TR2_CAMPAIGN_DATA_FORMAT_VERSION UINT16_C(1)
#define TR2_CAMPAIGN_DATA_STATE_OPEN UINT16_C(1)
#define TR2_CAMPAIGN_DATA_STATE_FINISHED UINT16_C(2)

#define DESCRIPTOR_CRC_OFFSET 32u
#define CHUNK_HEADER_SIZE 20u
#define CHUNK_CRC_OFFSET 84u

typedef struct {
    bool present;
    bool valid;
    bool unsupported;
    uint32_t generation;
    CampaignId campaign_id;
    uint64_t durable_prefix_bytes;
    uint32_t chunk_count;
    uint16_t state;
} DescriptorInfo;

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

static void put_u64_be(uint8_t *output, uint64_t value)
{
    put_u32_be(output, (uint32_t)(value >> 32u));
    put_u32_be(&output[4], (uint32_t)(value & UINT64_C(0xFFFFFFFF)));
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

static uint64_t get_u64_be(const uint8_t *input)
{
    return ((uint64_t)get_u32_be(input) << 32u) |
           (uint64_t)get_u32_be(&input[4]);
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

static bool bytes_are_empty(const uint8_t *bytes, size_t size)
{
    bool all_zero = true;
    bool all_ff = true;
    size_t index;

    for (index = 0u; index < size; ++index) {
        all_zero = all_zero && bytes[index] == UINT8_C(0x00);
        all_ff = all_ff && bytes[index] == UINT8_C(0xFF);
    }

    return all_zero || all_ff;
}

static uint32_t slot_base(size_t slot)
{
    return (uint32_t)(slot * TR2_CAMPAIGN_DATA_SLOT_SIZE);
}

static uint32_t descriptor_offset(size_t slot, size_t copy)
{
    return slot_base(slot) +
           (uint32_t)(copy * TR2_CAMPAIGN_DATA_DESCRIPTOR_SIZE);
}

static uint32_t chunk_offset(size_t slot, size_t chunk_index)
{
    return slot_base(slot) +
           (uint32_t)(TR2_CAMPAIGN_DATA_DESCRIPTOR_COPY_COUNT *
                      TR2_CAMPAIGN_DATA_DESCRIPTOR_SIZE) +
           (uint32_t)(chunk_index * TR2_CAMPAIGN_DATA_CHUNK_SIZE);
}

static Tr2Result read_descriptor(const CampaignDataStorePersistent *store,
                                 size_t slot,
                                 size_t copy,
                                 DescriptorInfo *info)
{
    uint8_t record[TR2_CAMPAIGN_DATA_DESCRIPTOR_SIZE];
    uint32_t stored_crc;
    uint32_t computed_crc;
    Tr2Result result;

    memset(info, 0, sizeof(*info));
    result = persistent_storage_core_read(store->storage,
                                          descriptor_offset(slot, copy),
                                          record,
                                          sizeof(record));
    if (result != TR2_OK) {
        return result;
    }

    if (bytes_are_empty(record, sizeof(record))) {
        return TR2_OK;
    }

    info->present = true;
    if (get_u32_be(&record[0]) != TR2_CAMPAIGN_DATA_DESCRIPTOR_MAGIC ||
        get_u16_be(&record[6]) != TR2_CAMPAIGN_DATA_DESCRIPTOR_SIZE) {
        return TR2_OK;
    }

    stored_crc = get_u32_be(&record[DESCRIPTOR_CRC_OFFSET]);
    computed_crc = crc32_bytes(record, DESCRIPTOR_CRC_OFFSET);
    if (stored_crc != computed_crc) {
        return TR2_OK;
    }

    if (get_u16_be(&record[4]) != TR2_CAMPAIGN_DATA_FORMAT_VERSION) {
        info->unsupported = true;
        return TR2_OK;
    }

    info->generation = get_u32_be(&record[8]);
    info->campaign_id = get_u32_be(&record[12]);
    info->durable_prefix_bytes = get_u64_be(&record[16]);
    info->chunk_count = get_u32_be(&record[24]);
    info->state = get_u16_be(&record[28]);

    if (info->campaign_id == TR2_CAMPAIGN_ID_INVALID ||
        info->chunk_count > TR2_CAMPAIGN_DATA_CHUNKS_PER_SLOT ||
        (info->state != TR2_CAMPAIGN_DATA_STATE_OPEN &&
         info->state != TR2_CAMPAIGN_DATA_STATE_FINISHED)) {
        return TR2_OK;
    }

    info->valid = true;
    return TR2_OK;
}

static bool select_descriptor(const DescriptorInfo descriptors[2],
                              DescriptorInfo *selected,
                              bool *saw_present,
                              bool *saw_unsupported)
{
    size_t copy;

    memset(selected, 0, sizeof(*selected));
    *saw_present = false;
    *saw_unsupported = false;

    for (copy = 0u; copy < 2u; ++copy) {
        *saw_present = *saw_present || descriptors[copy].present;
        *saw_unsupported = *saw_unsupported || descriptors[copy].unsupported;

        if (descriptors[copy].valid &&
            (!selected->valid ||
             descriptors[copy].generation > selected->generation)) {
            *selected = descriptors[copy];
        }
    }

    return selected->valid;
}

static Tr2Result load_slot_descriptor(
    const CampaignDataStorePersistent *store,
    size_t slot,
    DescriptorInfo *selected,
    bool *saw_present,
    bool *saw_unsupported)
{
    DescriptorInfo descriptors[2];
    size_t copy;
    Tr2Result result;

    for (copy = 0u; copy < 2u; ++copy) {
        result = read_descriptor(store, slot, copy, &descriptors[copy]);
        if (result != TR2_OK) {
            return result;
        }
    }

    (void)select_descriptor(descriptors,
                            selected,
                            saw_present,
                            saw_unsupported);
    return TR2_OK;
}

static Tr2Result write_descriptor(CampaignDataStorePersistent *store,
                                  size_t slot,
                                  size_t copy,
                                  uint32_t generation,
                                  CampaignId campaign_id,
                                  uint64_t durable_prefix_bytes,
                                  uint32_t chunk_count,
                                  uint16_t state)
{
    uint8_t record[TR2_CAMPAIGN_DATA_DESCRIPTOR_SIZE];
    Tr2Result result;

    memset(record, 0, sizeof(record));
    put_u32_be(&record[0], TR2_CAMPAIGN_DATA_DESCRIPTOR_MAGIC);
    put_u16_be(&record[4], TR2_CAMPAIGN_DATA_FORMAT_VERSION);
    put_u16_be(&record[6], TR2_CAMPAIGN_DATA_DESCRIPTOR_SIZE);
    put_u32_be(&record[8], generation);
    put_u32_be(&record[12], campaign_id);
    put_u64_be(&record[16], durable_prefix_bytes);
    put_u32_be(&record[24], chunk_count);
    put_u16_be(&record[28], state);
    put_u32_be(&record[DESCRIPTOR_CRC_OFFSET],
               crc32_bytes(record, DESCRIPTOR_CRC_OFFSET));

    result = persistent_storage_core_write(store->storage,
                                           descriptor_offset(slot, copy),
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

static Tr2Result write_chunk(CampaignDataStorePersistent *store,
                             size_t slot,
                             CampaignId campaign_id,
                             uint32_t chunk_index,
                             const uint8_t *data,
                             size_t size)
{
    uint8_t record[TR2_CAMPAIGN_DATA_CHUNK_SIZE];
    uint32_t crc;
    Tr2Result result;

    if (size == 0u || size > TR2_CAMPAIGN_DATA_CHUNK_PAYLOAD_SIZE) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(record, 0, sizeof(record));
    put_u32_be(&record[0], TR2_CAMPAIGN_DATA_CHUNK_MAGIC);
    put_u16_be(&record[4], TR2_CAMPAIGN_DATA_FORMAT_VERSION);
    put_u16_be(&record[6], TR2_CAMPAIGN_DATA_CHUNK_SIZE);
    put_u32_be(&record[8], campaign_id);
    put_u32_be(&record[12], chunk_index);
    put_u16_be(&record[16], (uint16_t)size);
    memcpy(&record[CHUNK_HEADER_SIZE], data, size);
    crc = crc32_bytes(record, CHUNK_CRC_OFFSET);
    put_u32_be(&record[CHUNK_CRC_OFFSET], crc);

    result = persistent_storage_core_write(store->storage,
                                           chunk_offset(slot, chunk_index),
                                           record,
                                           sizeof(record));
    if (result != TR2_OK) {
        store->recovery_required = true;
    }
    return result;
}

static Tr2Result validate_chunk(const CampaignDataStorePersistent *store,
                                size_t slot,
                                CampaignId campaign_id,
                                uint32_t chunk_index,
                                size_t *payload_size)
{
    uint8_t record[TR2_CAMPAIGN_DATA_CHUNK_SIZE];
    uint16_t size;
    Tr2Result result;

    result = persistent_storage_core_read(store->storage,
                                          chunk_offset(slot, chunk_index),
                                          record,
                                          sizeof(record));
    if (result != TR2_OK) {
        return result;
    }

    if (get_u32_be(&record[0]) != TR2_CAMPAIGN_DATA_CHUNK_MAGIC ||
        get_u16_be(&record[6]) != TR2_CAMPAIGN_DATA_CHUNK_SIZE ||
        get_u32_be(&record[8]) != campaign_id ||
        get_u32_be(&record[12]) != chunk_index) {
        return TR2_ERROR_CORRUPTED;
    }

    if (get_u16_be(&record[4]) != TR2_CAMPAIGN_DATA_FORMAT_VERSION) {
        return TR2_ERROR_UNSUPPORTED;
    }

    size = get_u16_be(&record[16]);
    if (size == 0u || size > TR2_CAMPAIGN_DATA_CHUNK_PAYLOAD_SIZE) {
        return TR2_ERROR_CORRUPTED;
    }

    if (get_u32_be(&record[CHUNK_CRC_OFFSET]) !=
        crc32_bytes(record, CHUNK_CRC_OFFSET)) {
        return TR2_ERROR_CORRUPTED;
    }

    *payload_size = (size_t)size;
    return TR2_OK;
}

static Tr2Result publish_checkpoint(CampaignDataStorePersistent *store,
                                    uint16_t state)
{
    size_t descriptor_copy;
    Tr2Result result;

    result = persistent_storage_core_commit(store->storage);
    if (result != TR2_OK) {
        store->recovery_required = true;
        return result;
    }

    descriptor_copy = (size_t)((store->active_generation + 1u) % 2u);
    result = write_descriptor(store,
                              store->active_slot,
                              descriptor_copy,
                              store->active_generation + 1u,
                              store->active_campaign_id,
                              store->active_total_bytes,
                              store->active_chunk_count,
                              state);
    if (result == TR2_OK) {
        store->active_generation += 1u;
    }
    return result;
}

static Tr2Result begin_campaign(void *context, CampaignId campaign_id)
{
    CampaignDataStorePersistent *store = context;
    size_t slot;
    size_t free_slot = TR2_CAMPAIGN_DATA_SLOT_COUNT;
    Tr2Result result;

    if (!campaign_data_store_persistent_is_initialized(store) ||
        store->recovery_required) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (campaign_id == TR2_CAMPAIGN_ID_INVALID) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (store->campaign_active) {
        return TR2_ERROR_INVALID_STATE;
    }

    for (slot = 0u; slot < TR2_CAMPAIGN_DATA_SLOT_COUNT; ++slot) {
        DescriptorInfo descriptor;
        bool present;
        bool unsupported;

        result = load_slot_descriptor(store,
                                      slot,
                                      &descriptor,
                                      &present,
                                      &unsupported);
        if (result != TR2_OK) {
            store->recovery_required = true;
            return result;
        }
        if (unsupported) {
            return TR2_ERROR_UNSUPPORTED;
        }
        if (descriptor.valid && descriptor.campaign_id == campaign_id) {
            return TR2_ERROR_INVALID_ARGUMENT;
        }
        if (!present && free_slot == TR2_CAMPAIGN_DATA_SLOT_COUNT) {
            free_slot = slot;
        }
    }

    if (free_slot == TR2_CAMPAIGN_DATA_SLOT_COUNT) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    result = write_descriptor(store,
                              free_slot,
                              0u,
                              1u,
                              campaign_id,
                              0u,
                              0u,
                              TR2_CAMPAIGN_DATA_STATE_OPEN);
    if (result != TR2_OK) {
        return result;
    }

    store->campaign_active = true;
    store->active_slot = free_slot;
    store->active_campaign_id = campaign_id;
    store->active_generation = 1u;
    store->active_chunk_count = 0u;
    store->active_total_bytes = 0u;
    return TR2_OK;
}

static Tr2Result append_data(void *context,
                             CampaignId campaign_id,
                             const uint8_t *data,
                             size_t size)
{
    CampaignDataStorePersistent *store = context;
    size_t offset = 0u;

    if (!campaign_data_store_persistent_is_initialized(store) ||
        store->recovery_required ||
        !store->campaign_active) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (campaign_id != store->active_campaign_id ||
        (data == NULL && size != 0u)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (size == 0u) {
        return TR2_OK;
    }

    while (offset < size) {
        size_t remaining = size - offset;
        size_t chunk_size = remaining;
        Tr2Result result;

        if (store->active_chunk_count >= TR2_CAMPAIGN_DATA_CHUNKS_PER_SLOT) {
            return TR2_ERROR_NOT_AVAILABLE;
        }
        if (chunk_size > TR2_CAMPAIGN_DATA_CHUNK_PAYLOAD_SIZE) {
            chunk_size = TR2_CAMPAIGN_DATA_CHUNK_PAYLOAD_SIZE;
        }

        result = write_chunk(store,
                             store->active_slot,
                             campaign_id,
                             store->active_chunk_count,
                             &data[offset],
                             chunk_size);
        if (result != TR2_OK) {
            return result;
        }

        store->active_chunk_count += 1u;
        store->active_total_bytes += (uint64_t)chunk_size;
        offset += chunk_size;
    }

    return TR2_OK;
}

static Tr2Result checkpoint_data(void *context, CampaignId campaign_id)
{
    CampaignDataStorePersistent *store = context;

    if (!campaign_data_store_persistent_is_initialized(store) ||
        store->recovery_required ||
        !store->campaign_active) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (campaign_id != store->active_campaign_id) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    return publish_checkpoint(store, TR2_CAMPAIGN_DATA_STATE_OPEN);
}

static Tr2Result finish_campaign(void *context, CampaignId campaign_id)
{
    CampaignDataStorePersistent *store = context;
    Tr2Result result;

    if (!campaign_data_store_persistent_is_initialized(store) ||
        store->recovery_required ||
        !store->campaign_active) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (campaign_id != store->active_campaign_id) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = publish_checkpoint(store, TR2_CAMPAIGN_DATA_STATE_FINISHED);
    if (result != TR2_OK) {
        return result;
    }

    store->campaign_active = false;
    store->active_campaign_id = TR2_CAMPAIGN_ID_INVALID;
    store->active_generation = 0u;
    store->active_chunk_count = 0u;
    store->active_total_bytes = 0u;
    return TR2_OK;
}

static Tr2Result recover_campaign(void *context,
                                  CampaignId campaign_id,
                                  CampaignDataRecoveryResult *out)
{
    CampaignDataStorePersistent *store = context;
    size_t slot;

    if (!campaign_data_store_persistent_is_initialized(store)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (campaign_id == TR2_CAMPAIGN_ID_INVALID || out == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(out, 0, sizeof(*out));

    for (slot = 0u; slot < TR2_CAMPAIGN_DATA_SLOT_COUNT; ++slot) {
        DescriptorInfo descriptor;
        bool present;
        bool unsupported;
        Tr2Result result;

        result = load_slot_descriptor(store,
                                      slot,
                                      &descriptor,
                                      &present,
                                      &unsupported);
        if (result != TR2_OK) {
            out->status = CAMPAIGN_DATA_RECOVERY_UNAVAILABLE;
            return TR2_OK;
        }
        if (unsupported) {
            out->status = CAMPAIGN_DATA_RECOVERY_UNSUPPORTED;
            return TR2_OK;
        }
        if (!descriptor.valid || descriptor.campaign_id != campaign_id) {
            continue;
        }

        {
            uint64_t recovered_bytes = 0u;
            uint32_t chunk_index;

            for (chunk_index = 0u;
                 chunk_index < descriptor.chunk_count;
                 ++chunk_index) {
                size_t payload_size = 0u;

                result = validate_chunk(store,
                                        slot,
                                        campaign_id,
                                        chunk_index,
                                        &payload_size);
                if (result == TR2_ERROR_UNSUPPORTED) {
                    out->status = CAMPAIGN_DATA_RECOVERY_UNSUPPORTED;
                    return TR2_OK;
                }
                if (result != TR2_OK) {
                    out->status = CAMPAIGN_DATA_RECOVERY_CORRUPTED;
                    return TR2_OK;
                }
                recovered_bytes += (uint64_t)payload_size;
            }

            if (recovered_bytes != descriptor.durable_prefix_bytes) {
                out->status = CAMPAIGN_DATA_RECOVERY_CORRUPTED;
                return TR2_OK;
            }

            out->status = CAMPAIGN_DATA_RECOVERY_VALID;
            out->durable_prefix_bytes = recovered_bytes;
            store->recovery_required = false;
            return TR2_OK;
        }
    }

    out->status = CAMPAIGN_DATA_RECOVERY_EMPTY;
    store->recovery_required = false;
    return TR2_OK;
}

Tr2Result campaign_data_store_persistent_init(
    CampaignDataStorePersistent *store,
    PersistentStorageCore *storage)
{
    if (store == NULL ||
        storage == NULL ||
        !persistent_storage_core_is_initialized(storage)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(store, 0, sizeof(*store));
    store->storage = storage;
    store->initialized = true;
    store->interface.context = store;
    store->interface.begin_campaign = begin_campaign;
    store->interface.append = append_data;
    store->interface.checkpoint = checkpoint_data;
    store->interface.finish_campaign = finish_campaign;
    store->interface.recover_campaign = recover_campaign;
    return TR2_OK;
}

bool campaign_data_store_persistent_is_initialized(
    const CampaignDataStorePersistent *store)
{
    return store != NULL &&
           store->initialized &&
           store->storage != NULL &&
           persistent_storage_core_is_initialized(store->storage);
}

bool campaign_data_store_persistent_recovery_required(
    const CampaignDataStorePersistent *store)
{
    return campaign_data_store_persistent_is_initialized(store) &&
           store->recovery_required;
}

CampaignDataStore *campaign_data_store_persistent_interface(
    CampaignDataStorePersistent *store)
{
    return campaign_data_store_persistent_is_initialized(store)
               ? &store->interface
               : NULL;
}
