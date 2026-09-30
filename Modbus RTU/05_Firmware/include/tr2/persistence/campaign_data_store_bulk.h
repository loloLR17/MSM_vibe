#ifndef TR2_PERSISTENCE_CAMPAIGN_DATA_STORE_BULK_H
#define TR2_PERSISTENCE_CAMPAIGN_DATA_STORE_BULK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/persistence/campaign_bulk_block_stream.h"
#include "tr2/persistence/campaign_bulk_metadata.h"
#include "tr2/persistence/campaign_data_store.h"
#include "tr2/persistence/campaign_repository_store.h"

#define TR2_CAMPAIGN_BULK_SLOT_COUNT TR2_CAMPAIGN_REPOSITORY_CAPACITY
#define TR2_CAMPAIGN_BULK_METADATA_BYTES     ((uint64_t)TR2_CAMPAIGN_BULK_SLOT_COUNT * UINT64_C(2) *      (uint64_t)TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE)

typedef struct {
    CampaignBulkMedia *media;
    uint8_t *payload_buffer;
    size_t payload_capacity;
    uint8_t *block_scratch;
    size_t block_scratch_capacity;
    bool initialized;
    bool recovery_required;
    bool campaign_active;
    size_t active_slot;
    CampaignId active_campaign_id;
    uint64_t active_generation;
    uint64_t active_data_base;
    uint64_t active_logical_bytes;
    uint64_t active_durable_bytes;
    size_t buffered_bytes;
    CampaignBulkBlockWriter writer;
    CampaignDataStore interface;
} CampaignDataStoreBulk;

Tr2Result campaign_data_store_bulk_init(
    CampaignDataStoreBulk *store,
    CampaignBulkMedia *media,
    uint8_t *payload_buffer,
    size_t payload_capacity,
    uint8_t *block_scratch,
    size_t block_scratch_capacity);

bool campaign_data_store_bulk_is_initialized(
    const CampaignDataStoreBulk *store);

bool campaign_data_store_bulk_recovery_required(
    const CampaignDataStoreBulk *store);

CampaignDataStore *campaign_data_store_bulk_interface(
    CampaignDataStoreBulk *store);

#endif
