#include "tr2/persistence/campaign_data_store_bulk.h"

#include <string.h>

static uint64_t descriptor_offset(size_t slot, size_t copy)
{
    return ((uint64_t)slot * UINT64_C(2) + (uint64_t)copy) *
           (uint64_t)TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE;
}

static Tr2Result init_slot_metadata(CampaignDataStoreBulk *store,
                                    size_t slot,
                                    CampaignBulkMetadata *metadata)
{
    return campaign_bulk_metadata_init(metadata,
                                       store->media,
                                       descriptor_offset(slot, 0u),
                                       descriptor_offset(slot, 1u));
}

static Tr2Result recover_slot(CampaignDataStoreBulk *store,
                              size_t slot,
                              CampaignBulkMetadata *metadata,
                              CampaignBulkMetadataRecoveryResult *recovery)
{
    Tr2Result result = init_slot_metadata(store, slot, metadata);
    if (result != TR2_OK) {
        return result;
    }
    return campaign_bulk_metadata_recover(metadata, recovery);
}

static Tr2Result scan_layout(CampaignDataStoreBulk *store,
                             CampaignId wanted_id,
                             bool find_free,
                             size_t *wanted_slot,
                             CampaignBulkMetadataRecoveryResult *wanted,
                             size_t *free_slot,
                             uint64_t *next_data_base)
{
    bool found = false;
    bool free_found = false;
    uint64_t data_end = TR2_CAMPAIGN_BULK_METADATA_BYTES;
    size_t slot;

    for (slot = 0u; slot < TR2_CAMPAIGN_BULK_SLOT_COUNT; ++slot) {
        CampaignBulkMetadata metadata;
        CampaignBulkMetadataRecoveryResult recovery;
        Tr2Result result = recover_slot(store, slot, &metadata, &recovery);

        if (result != TR2_OK) {
            return result;
        }
        if (recovery.status == CAMPAIGN_BULK_METADATA_RECOVERY_UNAVAILABLE) {
            return TR2_ERROR_UNAVAILABLE;
        }
        if (recovery.status == CAMPAIGN_BULK_METADATA_RECOVERY_UNSUPPORTED) {
            return TR2_ERROR_UNSUPPORTED;
        }
        if (recovery.status == CAMPAIGN_BULK_METADATA_RECOVERY_CORRUPTED) {
            return TR2_ERROR_CORRUPTED;
        }
        if (recovery.status == CAMPAIGN_BULK_METADATA_RECOVERY_EMPTY) {
            if (find_free && !free_found) {
                *free_slot = slot;
                free_found = true;
            }
            continue;
        }
        if (recovery.status != CAMPAIGN_BULK_METADATA_RECOVERY_VALID) {
            return TR2_ERROR_CORRUPTED;
        }

        if (recovery.descriptor.data_base < TR2_CAMPAIGN_BULK_METADATA_BYTES ||
            recovery.descriptor.durable_prefix_bytes >
                UINT64_MAX - recovery.descriptor.data_base) {
            return TR2_ERROR_CORRUPTED;
        }

        if (recovery.descriptor.data_base +
                recovery.descriptor.durable_prefix_bytes > data_end) {
            data_end = recovery.descriptor.data_base +
                       recovery.descriptor.durable_prefix_bytes;
        }

        if (recovery.descriptor.campaign_id == wanted_id) {
            if (found) {
                return TR2_ERROR_CORRUPTED;
            }
            found = true;
            if (wanted_slot != NULL) {
                *wanted_slot = slot;
            }
            if (wanted != NULL) {
                *wanted = recovery;
            }
        }
    }

    if (next_data_base != NULL) {
        *next_data_base = data_end;
    }
    if (wanted_id != TR2_CAMPAIGN_ID_INVALID && !found) {
        return TR2_ERROR_NOT_FOUND;
    }
    if (find_free && !free_found) {
        return TR2_ERROR_NOT_AVAILABLE;
    }
    return TR2_OK;
}

static Tr2Result flush_buffer(CampaignDataStoreBulk *store)
{
    Tr2Result result;

    if (store->buffered_bytes == 0u) {
        return TR2_OK;
    }
    if (store->buffered_bytes % TR2_CAMPAIGN_BULK_SAMPLE_RECORD_SIZE != 0u) {
        return TR2_ERROR_INVALID_STATE;
    }

    result = campaign_bulk_block_writer_append(&store->writer,
                                               store->payload_buffer,
                                               store->buffered_bytes,
                                               store->block_scratch,
                                               store->block_scratch_capacity);
    if (result != TR2_OK) {
        if (campaign_bulk_block_writer_is_faulted(&store->writer)) {
            store->recovery_required = true;
        }
        return result;
    }
    store->buffered_bytes = 0u;
    return TR2_OK;
}

static Tr2Result begin_campaign(void *context, CampaignId campaign_id)
{
    CampaignDataStoreBulk *store = context;
    CampaignBulkMetadata metadata;
    CampaignBulkDescriptor descriptor;
    size_t free_slot = 0u;
    uint64_t data_base = 0u;
    uint64_t capacity = 0u;
    Tr2Result result;

    if (!campaign_data_store_bulk_is_initialized(store) ||
        store->recovery_required || store->campaign_active) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (campaign_id == TR2_CAMPAIGN_ID_INVALID) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = scan_layout(store,
                         TR2_CAMPAIGN_ID_INVALID,
                         true,
                         NULL,
                         NULL,
                         &free_slot,
                         &data_base);
    if (result != TR2_OK) {
        return result;
    }

    {
        size_t existing_slot;
        CampaignBulkMetadataRecoveryResult existing;
        result = scan_layout(store,
                             campaign_id,
                             false,
                             &existing_slot,
                             &existing,
                             NULL,
                             NULL);
        if (result == TR2_OK) {
            return TR2_ERROR_INVALID_ARGUMENT;
        }
        if (result != TR2_ERROR_NOT_FOUND) {
            return result;
        }
    }

    result = campaign_bulk_media_capacity(store->media, &capacity);
    if (result != TR2_OK) {
        return result;
    }
    if (data_base >= capacity) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    result = init_slot_metadata(store, free_slot, &metadata);
    if (result != TR2_OK) {
        return result;
    }

    descriptor.generation = 1u;
    descriptor.campaign_id = campaign_id;
    descriptor.state = CAMPAIGN_BULK_METADATA_STATE_OPEN;
    descriptor.data_base = data_base;
    descriptor.durable_prefix_bytes = 0u;
    result = campaign_bulk_metadata_publish(&metadata, &descriptor);
    if (result != TR2_OK) {
        store->recovery_required =
            campaign_bulk_metadata_recovery_required(&metadata);
        return result;
    }

    result = campaign_bulk_block_writer_init(&store->writer,
                                             store->media,
                                             campaign_id,
                                             data_base,
                                             0u);
    if (result != TR2_OK) {
        store->recovery_required = true;
        return result;
    }

    store->campaign_active = true;
    store->active_slot = free_slot;
    store->active_campaign_id = campaign_id;
    store->active_generation = 1u;
    store->active_data_base = data_base;
    store->active_logical_bytes = 0u;
    store->active_durable_bytes = 0u;
    store->buffered_bytes = 0u;
    return TR2_OK;
}

static Tr2Result append_data(void *context,
                             CampaignId campaign_id,
                             const uint8_t *data,
                             size_t size)
{
    CampaignDataStoreBulk *store = context;
    size_t offset = 0u;

    if (!campaign_data_store_bulk_is_initialized(store) ||
        store->recovery_required || !store->campaign_active) {
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
        size_t available = store->payload_capacity - store->buffered_bytes;
        size_t take = size - offset;
        Tr2Result result;

        if (take > available) {
            take = available;
        }
        memcpy(&store->payload_buffer[store->buffered_bytes],
               &data[offset],
               take);
        store->buffered_bytes += take;
        store->active_logical_bytes += (uint64_t)take;
        offset += take;

        if (store->buffered_bytes == store->payload_capacity) {
            result = flush_buffer(store);
            if (result != TR2_OK) {
                return result;
            }
        }
    }

    return TR2_OK;
}

static Tr2Result publish_checkpoint(CampaignDataStoreBulk *store,
                                    CampaignBulkMetadataState state)
{
    CampaignBulkMetadata metadata;
    CampaignBulkMetadataRecoveryResult recovery;
    CampaignBulkDescriptor descriptor;
    Tr2Result result;

    result = flush_buffer(store);
    if (result != TR2_OK) {
        return result;
    }
    result = campaign_bulk_media_sync(store->media);
    if (result != TR2_OK) {
        store->recovery_required = true;
        return result;
    }

    result = recover_slot(store, store->active_slot, &metadata, &recovery);
    if (result != TR2_OK) {
        store->recovery_required = true;
        return result;
    }
    if (recovery.status != CAMPAIGN_BULK_METADATA_RECOVERY_VALID ||
        recovery.descriptor.campaign_id != store->active_campaign_id ||
        recovery.descriptor.generation != store->active_generation ||
        recovery.descriptor.data_base != store->active_data_base) {
        store->recovery_required = true;
        return TR2_ERROR_CORRUPTED;
    }

    descriptor = recovery.descriptor;
    if (descriptor.generation >= UINT64_MAX - 1u) {
        return TR2_ERROR_NOT_AVAILABLE;
    }
    descriptor.generation += 1u;
    descriptor.state = state;
    descriptor.durable_prefix_bytes = store->active_logical_bytes;

    result = campaign_bulk_metadata_publish(&metadata, &descriptor);
    if (result != TR2_OK) {
        store->recovery_required = true;
        return result;
    }

    store->active_generation = descriptor.generation;
    store->active_durable_bytes = descriptor.durable_prefix_bytes;
    return TR2_OK;
}

static Tr2Result checkpoint_data(void *context, CampaignId campaign_id)
{
    CampaignDataStoreBulk *store = context;

    if (!campaign_data_store_bulk_is_initialized(store) ||
        store->recovery_required || !store->campaign_active) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (campaign_id != store->active_campaign_id) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    return publish_checkpoint(store, CAMPAIGN_BULK_METADATA_STATE_OPEN);
}

static Tr2Result finish_campaign(void *context, CampaignId campaign_id)
{
    CampaignDataStoreBulk *store = context;
    Tr2Result result;

    if (!campaign_data_store_bulk_is_initialized(store) ||
        store->recovery_required || !store->campaign_active) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (campaign_id != store->active_campaign_id) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = publish_checkpoint(store, CAMPAIGN_BULK_METADATA_STATE_FINISHED);
    if (result != TR2_OK) {
        return result;
    }

    store->campaign_active = false;
    store->active_campaign_id = TR2_CAMPAIGN_ID_INVALID;
    store->buffered_bytes = 0u;
    return TR2_OK;
}

static Tr2Result validate_prefix(CampaignDataStoreBulk *store,
                                 const CampaignBulkDescriptor *descriptor)
{
    CampaignBulkBlockReader reader;
    uint64_t recovered = 0u;
    Tr2Result result;

    result = campaign_bulk_block_reader_init(&reader,
                                             store->media,
                                             descriptor->campaign_id,
                                             descriptor->data_base,
                                             0u);
    if (result != TR2_OK) {
        return result;
    }

    while (recovered < descriptor->durable_prefix_bytes) {
        CampaignBulkBlockInfo info;
        const uint8_t *payload;

        result = campaign_bulk_block_reader_next(&reader,
                                                 store->block_scratch,
                                                 store->block_scratch_capacity,
                                                 &info,
                                                 &payload);
        (void)payload;
        if (result != TR2_OK) {
            return result;
        }
        if ((uint64_t)info.payload_size >
            descriptor->durable_prefix_bytes - recovered) {
            return TR2_ERROR_CORRUPTED;
        }
        recovered += (uint64_t)info.payload_size;
    }

    return recovered == descriptor->durable_prefix_bytes
               ? TR2_OK
               : TR2_ERROR_CORRUPTED;
}

static Tr2Result recover_campaign(void *context,
                                  CampaignId campaign_id,
                                  CampaignDataRecoveryResult *out)
{
    CampaignDataStoreBulk *store = context;
    CampaignBulkMetadataRecoveryResult recovery;
    size_t slot;
    Tr2Result result;

    if (!campaign_data_store_bulk_is_initialized(store)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (campaign_id == TR2_CAMPAIGN_ID_INVALID || out == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memset(out, 0, sizeof(*out));

    result = scan_layout(store,
                         campaign_id,
                         false,
                         &slot,
                         &recovery,
                         NULL,
                         NULL);
    (void)slot;
    if (result == TR2_ERROR_NOT_FOUND) {
        out->status = CAMPAIGN_DATA_RECOVERY_EMPTY;
        store->recovery_required = false;
        return TR2_OK;
    }
    if (result == TR2_ERROR_UNAVAILABLE) {
        out->status = CAMPAIGN_DATA_RECOVERY_UNAVAILABLE;
        return TR2_OK;
    }
    if (result == TR2_ERROR_UNSUPPORTED) {
        out->status = CAMPAIGN_DATA_RECOVERY_UNSUPPORTED;
        return TR2_OK;
    }
    if (result != TR2_OK) {
        out->status = CAMPAIGN_DATA_RECOVERY_CORRUPTED;
        return TR2_OK;
    }

    result = validate_prefix(store, &recovery.descriptor);
    if (result == TR2_ERROR_UNSUPPORTED) {
        out->status = CAMPAIGN_DATA_RECOVERY_UNSUPPORTED;
        return TR2_OK;
    }
    if (result == TR2_ERROR_UNAVAILABLE || result == TR2_ERROR_STORAGE) {
        out->status = CAMPAIGN_DATA_RECOVERY_UNAVAILABLE;
        return TR2_OK;
    }
    if (result != TR2_OK) {
        out->status = CAMPAIGN_DATA_RECOVERY_CORRUPTED;
        return TR2_OK;
    }

    out->status = CAMPAIGN_DATA_RECOVERY_VALID;
    out->durable_prefix_bytes = recovery.descriptor.durable_prefix_bytes;
    store->recovery_required = false;
    return TR2_OK;
}

Tr2Result campaign_data_store_bulk_init(
    CampaignDataStoreBulk *store,
    CampaignBulkMedia *media,
    uint8_t *payload_buffer,
    size_t payload_capacity,
    uint8_t *block_scratch,
    size_t block_scratch_capacity)
{
    uint64_t capacity;
    size_t minimum_block_size;
    Tr2Result result;

    if (store == NULL || media == NULL || payload_buffer == NULL ||
        block_scratch == NULL ||
        payload_capacity == 0u ||
        payload_capacity % TR2_CAMPAIGN_BULK_SAMPLE_RECORD_SIZE != 0u) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    result = campaign_bulk_block_encoded_size(payload_capacity,
                                              &minimum_block_size);
    if (result != TR2_OK || block_scratch_capacity < minimum_block_size) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    result = campaign_bulk_media_capacity(media, &capacity);
    if (result != TR2_OK) {
        return result;
    }
    if (capacity <= TR2_CAMPAIGN_BULK_METADATA_BYTES) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    memset(store, 0, sizeof(*store));
    store->media = media;
    store->payload_buffer = payload_buffer;
    store->payload_capacity = payload_capacity;
    store->block_scratch = block_scratch;
    store->block_scratch_capacity = block_scratch_capacity;
    store->initialized = true;
    store->interface.context = store;
    store->interface.begin_campaign = begin_campaign;
    store->interface.append = append_data;
    store->interface.checkpoint = checkpoint_data;
    store->interface.finish_campaign = finish_campaign;
    store->interface.recover_campaign = recover_campaign;
    return TR2_OK;
}

bool campaign_data_store_bulk_is_initialized(
    const CampaignDataStoreBulk *store)
{
    return store != NULL && store->initialized && store->media != NULL &&
           store->payload_buffer != NULL && store->block_scratch != NULL;
}

bool campaign_data_store_bulk_recovery_required(
    const CampaignDataStoreBulk *store)
{
    return campaign_data_store_bulk_is_initialized(store) &&
           store->recovery_required;
}

CampaignDataStore *campaign_data_store_bulk_interface(
    CampaignDataStoreBulk *store)
{
    return campaign_data_store_bulk_is_initialized(store)
               ? &store->interface
               : NULL;
}
