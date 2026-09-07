#ifndef TR2_PERSISTENCE_CAMPAIGN_REPOSITORY_H
#define TR2_PERSISTENCE_CAMPAIGN_REPOSITORY_H

#include <stdbool.h>
#include <stddef.h>

#include "tr2/common/result.h"
#include "tr2/domain/campaign/campaign.h"

typedef enum {
    CAMPAIGN_REPOSITORY_RECOVERY_VALID = 0,
    CAMPAIGN_REPOSITORY_RECOVERY_EMPTY,
    CAMPAIGN_REPOSITORY_RECOVERY_CORRUPTED,
    CAMPAIGN_REPOSITORY_RECOVERY_UNAVAILABLE,
    CAMPAIGN_REPOSITORY_RECOVERY_UNSUPPORTED
} CampaignRepositoryRecoveryStatus;

typedef struct {
    CampaignRepositoryRecoveryStatus status;
    CampaignInventorySummary inventory;
} CampaignRepositoryRecoveryResult;

typedef struct CampaignRepository CampaignRepository;

struct CampaignRepository {
    void *context;
    Tr2Result (*reserve_campaign_id)(void *context, CampaignIdReservation *reservation);
    Tr2Result (*open_campaign)(void *context, const CampaignMetadata *metadata);
    Tr2Result (*close_campaign)(void *context, const CampaignMetadata *metadata);
    Tr2Result (*get_inventory_summary)(void *context, CampaignInventorySummary *summary);
    Tr2Result (*get_campaign_by_index)(void *context, size_t index, CampaignMetadata *metadata);
    Tr2Result (*get_campaign_by_id)(void *context, CampaignId campaign_id, CampaignMetadata *metadata);
    Tr2Result (*recover)(void *context, CampaignRepositoryRecoveryResult *result);
};

#endif
