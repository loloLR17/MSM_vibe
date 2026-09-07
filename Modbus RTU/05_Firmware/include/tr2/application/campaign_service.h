#ifndef TR2_APPLICATION_CAMPAIGN_SERVICE_H
#define TR2_APPLICATION_CAMPAIGN_SERVICE_H

#include <stdbool.h>

#include "tr2/application/acquisition_service.h"
#include "tr2/application/configuration_service.h"
#include "tr2/common/result.h"
#include "tr2/domain/campaign/campaign.h"
#include "tr2/persistence/campaign_data_store.h"
#include "tr2/persistence/campaign_repository.h"

typedef struct {
    ConfigurationService *configuration_service;
    AcquisitionService *acquisition_service;
    CampaignRepository *repository;
    CampaignDataStore *data_store;
    bool initialized;
    bool campaign_open;
    bool data_store_started;
    bool data_store_recovery_pending;
    bool acquisition_window_started;
    CampaignMetadata active_metadata;
} CampaignService;

Tr2Result campaign_service_init(CampaignService *service,
                                ConfigurationService *configuration_service,
                                AcquisitionService *acquisition_service,
                                CampaignRepository *repository,
                                CampaignDataStore *data_store);

bool campaign_service_is_initialized(const CampaignService *service);
bool campaign_service_campaign_open(const CampaignService *service);
bool campaign_service_acquisition_running(const CampaignService *service);

Tr2Result campaign_service_start(CampaignService *service,
                                 CampaignId *out_campaign_id);

Tr2Result campaign_service_stop(CampaignService *service,
                                CampaignMetadata *out_closed_metadata);

#endif
