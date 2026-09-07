#ifndef TR2_PERSISTENCE_CAMPAIGN_DATA_STORE_PERSISTENT_H
#define TR2_PERSISTENCE_CAMPAIGN_DATA_STORE_PERSISTENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/persistence/campaign_data_store.h"
#include "tr2/persistence/persistent_storage_core.h"

#define TR2_CAMPAIGN_DATA_SLOT_COUNT 4u
#define TR2_CAMPAIGN_DATA_DESCRIPTOR_COPY_COUNT 2u
#define TR2_CAMPAIGN_DATA_DESCRIPTOR_SIZE 36u
#define TR2_CAMPAIGN_DATA_CHUNK_PAYLOAD_SIZE 64u
#define TR2_CAMPAIGN_DATA_CHUNK_SIZE 88u
#define TR2_CAMPAIGN_DATA_CHUNKS_PER_SLOT 32u
#define TR2_CAMPAIGN_DATA_SLOT_SIZE \
    (TR2_CAMPAIGN_DATA_DESCRIPTOR_COPY_COUNT * TR2_CAMPAIGN_DATA_DESCRIPTOR_SIZE + \
     TR2_CAMPAIGN_DATA_CHUNKS_PER_SLOT * TR2_CAMPAIGN_DATA_CHUNK_SIZE)
#define TR2_CAMPAIGN_DATA_STORAGE_SIZE \
    (TR2_CAMPAIGN_DATA_SLOT_COUNT * TR2_CAMPAIGN_DATA_SLOT_SIZE)

typedef struct {
    PersistentStorageCore *storage;
    bool initialized;
    bool recovery_required;
    bool campaign_active;
    size_t active_slot;
    CampaignId active_campaign_id;
    uint32_t active_generation;
    uint32_t active_chunk_count;
    uint64_t active_total_bytes;
    CampaignDataStore interface;
} CampaignDataStorePersistent;

Tr2Result campaign_data_store_persistent_init(
    CampaignDataStorePersistent *store,
    PersistentStorageCore *storage);

bool campaign_data_store_persistent_is_initialized(
    const CampaignDataStorePersistent *store);

bool campaign_data_store_persistent_recovery_required(
    const CampaignDataStorePersistent *store);

CampaignDataStore *campaign_data_store_persistent_interface(
    CampaignDataStorePersistent *store);

#endif
