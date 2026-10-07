#ifndef TR2_PERSISTENCE_CAMPAIGN_DATA_STORE_PERSISTENT_COMPOSITION_H
#define TR2_PERSISTENCE_CAMPAIGN_DATA_STORE_PERSISTENT_COMPOSITION_H

#include "tr2/persistence/system_persistent_layout.h"
#include "tr2/persistence/persistent_media_region.h"

/* Explicit historical composition. Caller owns this object and its backing media.
 * Initialization performs no format, migration or persistent write. */
typedef struct {
    PersistentMediaRegion region;
    PersistentStorageCore core;
    CampaignDataStorePersistent store;
} CampaignDataStorePersistentComposition;

static inline Tr2Result campaign_data_store_persistent_composition_init(
    CampaignDataStorePersistentComposition *composition,
    const PersistentMedia *media)
{
    Tr2Result result;
    if (composition == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    result = persistent_media_region_init(&composition->region, media,
        TR2_CAMPAIGN_DATA_STORAGE_OFFSET, (uint32_t)TR2_CAMPAIGN_DATA_STORAGE_SIZE);
    if (result != TR2_OK) {
        return result;
    }
    result = persistent_storage_core_init(&composition->core,
        persistent_media_region_interface(&composition->region));
    if (result != TR2_OK) {
        return result;
    }
    return campaign_data_store_persistent_init(&composition->store, &composition->core);
}

#endif
