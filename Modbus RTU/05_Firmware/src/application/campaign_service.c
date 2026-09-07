#include "tr2/application/campaign_service.h"

#include <string.h>

static bool repository_valid(const CampaignRepository *repository)
{
    return repository != NULL &&
           repository->reserve_campaign_id != NULL &&
           repository->open_campaign != NULL &&
           repository->close_campaign != NULL &&
           repository->recover != NULL;
}

static bool data_store_valid(const CampaignDataStore *data_store)
{
    return data_store != NULL &&
           data_store->begin_campaign != NULL &&
           data_store->finish_campaign != NULL &&
           data_store->recover_campaign != NULL;
}

static void metadata_from_active(const ActiveConfigurationSnapshot *active,
                                 CampaignId campaign_id,
                                 CampaignMetadata *metadata)
{
    memset(metadata, 0, sizeof(*metadata));
    metadata->campaign_id = campaign_id;
    metadata->mission_id = active->payload.mission_id;
    memcpy(metadata->campaign_label,
           active->payload.campaign_label,
           sizeof(metadata->campaign_label));
    memcpy(metadata->mission_label,
           active->payload.mission_label,
           sizeof(metadata->mission_label));
    metadata->lifecycle_state = CAMPAIGN_LIFECYCLE_OPEN;
    metadata->data_integrity = CAMPAIGN_DATA_INTEGRITY_UNKNOWN;
    metadata->historical_context.configuration_generation = active->generation;
    metadata->historical_context.configuration_id = active->config_id;
    metadata->historical_context.configuration_revision_counter =
        active->revision_counter;
    metadata->historical_context.configuration_payload = active->payload;
}

Tr2Result campaign_service_init(CampaignService *service,
                                ConfigurationService *configuration_service,
                                AcquisitionService *acquisition_service,
                                CampaignRepository *repository,
                                CampaignDataStore *data_store)
{
    if (service == NULL ||
        configuration_service == NULL ||
        !configuration_service_is_initialized(configuration_service) ||
        acquisition_service == NULL ||
        !acquisition_service_is_initialized(acquisition_service) ||
        !repository_valid(repository) ||
        !data_store_valid(data_store)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(service, 0, sizeof(*service));
    service->configuration_service = configuration_service;
    service->acquisition_service = acquisition_service;
    service->repository = repository;
    service->data_store = data_store;
    service->initialized = true;
    return TR2_OK;
}

bool campaign_service_is_initialized(const CampaignService *service)
{
    return service != NULL && service->initialized;
}

bool campaign_service_campaign_open(const CampaignService *service)
{
    return campaign_service_is_initialized(service) && service->campaign_open;
}

bool campaign_service_acquisition_running(const CampaignService *service)
{
    return campaign_service_is_initialized(service) &&
           service->acquisition_window_started;
}

Tr2Result campaign_service_start(CampaignService *service,
                                 CampaignId *out_campaign_id)
{
    ActiveConfigurationSnapshot active;
    CampaignIdReservation reservation;
    CampaignMetadata metadata;
    Tr2Result result;

    if (!campaign_service_is_initialized(service)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (out_campaign_id == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    *out_campaign_id = TR2_CAMPAIGN_ID_INVALID;

    if (service->campaign_open) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (!configuration_service_active_snapshot(service->configuration_service,
                                               &active)) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    memset(&reservation, 0, sizeof(reservation));
    result = service->repository->reserve_campaign_id(
        service->repository->context,
        &reservation);
    if (result != TR2_OK) {
        return result;
    }
    if (!reservation.valid ||
        reservation.campaign_id == TR2_CAMPAIGN_ID_INVALID) {
        return TR2_ERROR_INTERNAL;
    }

    metadata_from_active(&active, reservation.campaign_id, &metadata);
    result = service->repository->open_campaign(service->repository->context,
                                                &metadata);
    if (result != TR2_OK) {
        return result;
    }

    service->campaign_open = true;
    service->active_metadata = metadata;

    result = service->data_store->begin_campaign(service->data_store->context,
                                                  reservation.campaign_id);
    if (result != TR2_OK) {
        return result;
    }
    service->data_store_started = true;

    result = acquisition_service_begin_window(service->acquisition_service);
    if (result != TR2_OK) {
        return result;
    }
    service->acquisition_window_started = true;
    *out_campaign_id = reservation.campaign_id;
    return TR2_OK;
}

Tr2Result campaign_service_stop(CampaignService *service,
                                CampaignMetadata *out_closed_metadata)
{
    Tr2Result result;

    if (!campaign_service_is_initialized(service)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (out_closed_metadata == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memset(out_closed_metadata, 0, sizeof(*out_closed_metadata));

    if (!service->campaign_open) {
        return TR2_ERROR_INVALID_STATE;
    }

    if (service->acquisition_window_started) {
        AcquisitionWindow window;

        result = acquisition_service_end_window(service->acquisition_service,
                                                &window);
        service->acquisition_window_started = false;
        if (result != TR2_OK) {
            return result;
        }
    }

    if (service->data_store_started) {
        CampaignDataRecoveryResult recovery;

        result = service->data_store->finish_campaign(
            service->data_store->context,
            service->active_metadata.campaign_id);
        if (result != TR2_OK) {
            return result;
        }
        service->data_store_started = false;

        memset(&recovery, 0, sizeof(recovery));
        result = service->data_store->recover_campaign(
            service->data_store->context,
            service->active_metadata.campaign_id,
            &recovery);
        if (result != TR2_OK) {
            return result;
        }
        if (recovery.status != CAMPAIGN_DATA_RECOVERY_VALID) {
            return TR2_ERROR_CORRUPTED;
        }
        service->active_metadata.durable_data_size_bytes =
            recovery.durable_prefix_bytes;
    }

    service->active_metadata.lifecycle_state = CAMPAIGN_LIFECYCLE_CLOSED;
    result = service->repository->close_campaign(service->repository->context,
                                                 &service->active_metadata);
    if (result != TR2_OK) {
        return result;
    }

    *out_closed_metadata = service->active_metadata;
    service->campaign_open = false;
    memset(&service->active_metadata, 0, sizeof(service->active_metadata));
    return TR2_OK;
}
