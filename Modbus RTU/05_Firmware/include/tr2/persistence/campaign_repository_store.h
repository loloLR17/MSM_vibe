#ifndef TR2_PERSISTENCE_CAMPAIGN_REPOSITORY_STORE_H
#define TR2_PERSISTENCE_CAMPAIGN_REPOSITORY_STORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/persistence/campaign_record.h"
#include "tr2/persistence/campaign_repository.h"
#include "tr2/persistence/persistent_storage_core.h"

#define TR2_CAMPAIGN_REPOSITORY_CAPACITY 8u
#define TR2_CAMPAIGN_REPOSITORY_COPY_COUNT 2u
#define TR2_CAMPAIGN_ALLOCATOR_SLOT_COUNT 2u
#define TR2_CAMPAIGN_ALLOCATOR_RECORD_SIZE 20u
#define TR2_CAMPAIGN_REPOSITORY_STORAGE_SIZE \
    (TR2_CAMPAIGN_ALLOCATOR_SLOT_COUNT * TR2_CAMPAIGN_ALLOCATOR_RECORD_SIZE + \
     TR2_CAMPAIGN_REPOSITORY_CAPACITY * TR2_CAMPAIGN_REPOSITORY_COPY_COUNT * \
         TR2_CAMPAIGN_RECORD_SIZE)

typedef struct {
    PersistentStorageCore *storage;
    bool initialized;
    bool recovery_required;
    CampaignRepository interface;
} CampaignRepositoryStore;

Tr2Result campaign_repository_store_init(CampaignRepositoryStore *store,
                                         PersistentStorageCore *storage);

bool campaign_repository_store_is_initialized(const CampaignRepositoryStore *store);

bool campaign_repository_store_recovery_required(const CampaignRepositoryStore *store);

CampaignRepository *campaign_repository_store_interface(CampaignRepositoryStore *store);

#endif
