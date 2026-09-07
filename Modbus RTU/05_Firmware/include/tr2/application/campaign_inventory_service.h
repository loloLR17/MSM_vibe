#ifndef TR2_APPLICATION_CAMPAIGN_INVENTORY_SERVICE_H
#define TR2_APPLICATION_CAMPAIGN_INVENTORY_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/campaign/campaign.h"
#include "tr2/persistence/campaign_repository.h"

typedef struct {
    uint32_t generation;
    CampaignInventorySummary inventory;
    uint16_t selected_campaign_index;
    bool selected_campaign_valid;
    CampaignMetadata selected_campaign;
} CampaignInventoryViewSnapshot;

typedef struct {
    CampaignRepository *repository;
    uint16_t selected_campaign_index;
    uint32_t generation;
    bool initialized;
} CampaignInventoryService;

Tr2Result campaign_inventory_service_init(CampaignInventoryService *service,
                                          CampaignRepository *repository);

bool campaign_inventory_service_is_initialized(
    const CampaignInventoryService *service);

Tr2Result campaign_inventory_service_select(CampaignInventoryService *service,
                                            uint16_t selected_campaign_index);

Tr2Result campaign_inventory_service_snapshot(
    CampaignInventoryService *service,
    CampaignInventoryViewSnapshot *snapshot);

#endif
