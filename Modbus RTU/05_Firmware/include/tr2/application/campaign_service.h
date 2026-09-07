#ifndef TR2_APPLICATION_CAMPAIGN_SERVICE_H
#define TR2_APPLICATION_CAMPAIGN_SERVICE_H

#include <stdbool.h>

#include "tr2/application/acquisition_service.h"
#include "tr2/application/configuration_service.h"
#include "tr2/common/result.h"
#include "tr2/domain/campaign/campaign.h"
#include "tr2/persistence/campaign_data_store.h"
#include "tr2/persistence/campaign_repository.h"

typedef enum {
    CAMPAIGN_ACQUISITION_STEP_NONE = 0,
    CAMPAIGN_ACQUISITION_STEP_SAMPLE_READ = 1,
    CAMPAIGN_ACQUISITION_STEP_WINDOW_COMPLETED = 2
} CampaignAcquisitionStepKind;

typedef struct {
    CampaignAcquisitionStepKind kind;
    VibrationSample sample;
    AcquisitionWindow window;
    Tr2Result source_result;
    Tr2Result stop_result;
} CampaignAcquisitionStep;

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

Tr2Result campaign_service_reserve_start_id(CampaignService *service,
                                            CampaignId *out_campaign_id);

Tr2Result campaign_service_start_reserved(CampaignService *service,
                                          CampaignId campaign_id);

Tr2Result campaign_service_start(CampaignService *service,
                                 CampaignId *out_campaign_id);

Tr2Result campaign_service_drive_acquisition_step(
    CampaignService *service,
    CampaignAcquisitionStep *out_step);

Tr2Result campaign_service_stop(CampaignService *service,
                                CampaignMetadata *out_closed_metadata);

#endif
