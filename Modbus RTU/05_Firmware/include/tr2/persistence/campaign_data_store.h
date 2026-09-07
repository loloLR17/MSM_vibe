#ifndef TR2_PERSISTENCE_CAMPAIGN_DATA_STORE_H
#define TR2_PERSISTENCE_CAMPAIGN_DATA_STORE_H

#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/campaign/campaign.h"

typedef enum {
    CAMPAIGN_DATA_RECOVERY_VALID = 0,
    CAMPAIGN_DATA_RECOVERY_EMPTY,
    CAMPAIGN_DATA_RECOVERY_CORRUPTED,
    CAMPAIGN_DATA_RECOVERY_UNAVAILABLE,
    CAMPAIGN_DATA_RECOVERY_UNSUPPORTED
} CampaignDataRecoveryStatus;

typedef struct {
    CampaignDataRecoveryStatus status;
    uint64_t durable_prefix_bytes;
} CampaignDataRecoveryResult;

typedef struct CampaignDataStore CampaignDataStore;

struct CampaignDataStore {
    void *context;
    Tr2Result (*begin_campaign)(void *context, CampaignId campaign_id);
    Tr2Result (*append)(void *context,
                        CampaignId campaign_id,
                        const uint8_t *data,
                        size_t size);
    Tr2Result (*checkpoint)(void *context, CampaignId campaign_id);
    Tr2Result (*finish_campaign)(void *context, CampaignId campaign_id);
    Tr2Result (*recover_campaign)(void *context,
                                  CampaignId campaign_id,
                                  CampaignDataRecoveryResult *result);
};

#endif
