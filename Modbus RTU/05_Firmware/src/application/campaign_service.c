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
           data_store->append != NULL &&
           data_store->checkpoint != NULL &&
           data_store->finish_campaign != NULL &&
           data_store->recover_campaign != NULL;
}

static void encode_u16_le(uint8_t *destination, uint16_t value)
{
    destination[0] = (uint8_t)(value & UINT16_C(0x00FF));
    destination[1] = (uint8_t)(value >> 8u);
}

static void encode_u32_le(uint8_t *destination, uint32_t value)
{
    destination[0] = (uint8_t)(value & UINT32_C(0x000000FF));
    destination[1] = (uint8_t)((value >> 8u) & UINT32_C(0x000000FF));
    destination[2] = (uint8_t)((value >> 16u) & UINT32_C(0x000000FF));
    destination[3] = (uint8_t)((value >> 24u) & UINT32_C(0x000000FF));
}

static void encode_campaign_sample(const VibrationSample *sample,
                                   uint8_t record[TR2_CAMPAIGN_SAMPLE_RECORD_SIZE])
{
    uint16_t flags = 0u;

    memset(record, 0, TR2_CAMPAIGN_SAMPLE_RECORD_SIZE);
    encode_u32_le(&record[0], (uint32_t)sample->x_mg);
    encode_u32_le(&record[4], (uint32_t)sample->y_mg);
    encode_u32_le(&record[8], (uint32_t)sample->z_mg);
    if (sample->valid) {
        flags |= UINT16_C(0x0001);
    }
    if (sample->saturated) {
        flags |= UINT16_C(0x0002);
    }
    encode_u16_le(&record[12], flags);
    encode_u16_le(&record[14], UINT16_C(0));
}

static Tr2Result checkpoint_completed_window(CampaignService *service,
                                             CampaignAcquisitionStep *step)
{
    Tr2Result result;

    if (!service->data_store_started) {
        step->storage_result = TR2_ERROR_INVALID_STATE;
        return step->storage_result;
    }

    result = service->data_store->checkpoint(service->data_store->context,
                                             service->active_metadata.campaign_id);
    step->storage_result = result;
    return result;
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

Tr2Result campaign_service_reserve_start_id(CampaignService *service,
                                            CampaignId *out_campaign_id)
{
    ActiveConfigurationSnapshot active;
    CampaignIdReservation reservation;
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
    result = service->repository->reserve_campaign_id(service->repository->context,
                                                      &reservation);
    if (result != TR2_OK) {
        return result;
    }
    if (!reservation.valid || reservation.campaign_id == TR2_CAMPAIGN_ID_INVALID) {
        return TR2_ERROR_INTERNAL;
    }

    *out_campaign_id = reservation.campaign_id;
    return TR2_OK;
}

Tr2Result campaign_service_start_reserved(CampaignService *service,
                                          CampaignId campaign_id)
{
    ActiveConfigurationSnapshot active;
    CampaignMetadata metadata;
    Tr2Result result;

    if (!campaign_service_is_initialized(service)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (campaign_id == TR2_CAMPAIGN_ID_INVALID) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (service->campaign_open) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (!configuration_service_active_snapshot(service->configuration_service,
                                               &active)) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    metadata_from_active(&active, campaign_id, &metadata);
    result = service->repository->open_campaign(service->repository->context,
                                                &metadata);
    if (result != TR2_OK) {
        return result;
    }

    service->campaign_open = true;
    service->active_metadata = metadata;

    result = service->data_store->begin_campaign(service->data_store->context,
                                                  campaign_id);
    if (result != TR2_OK) {
        return result;
    }
    service->data_store_started = true;
    service->data_store_recovery_pending = false;

    result = acquisition_service_begin_window(service->acquisition_service);
    if (result != TR2_OK) {
        return result;
    }
    service->acquisition_window_started = true;
    return TR2_OK;
}

Tr2Result campaign_service_start(CampaignService *service,
                                 CampaignId *out_campaign_id)
{
    CampaignId campaign_id;
    Tr2Result result;

    if (out_campaign_id == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    *out_campaign_id = TR2_CAMPAIGN_ID_INVALID;

    result = campaign_service_reserve_start_id(service, &campaign_id);
    if (result != TR2_OK) {
        return result;
    }
    result = campaign_service_start_reserved(service, campaign_id);
    if (result != TR2_OK) {
        return result;
    }

    *out_campaign_id = campaign_id;
    return TR2_OK;
}

Tr2Result campaign_service_drive_acquisition_step(
    CampaignService *service,
    CampaignAcquisitionStep *out_step)
{
    uint8_t record[TR2_CAMPAIGN_SAMPLE_RECORD_SIZE];
    VibrationSample sample;
    AcquisitionWindow window;
    Tr2Result read_result;
    Tr2Result stop_result;
    Tr2Result storage_result;

    if (!campaign_service_is_initialized(service)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (out_step == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(out_step, 0, sizeof(*out_step));
    out_step->source_result = TR2_OK;
    out_step->stop_result = TR2_OK;
    out_step->storage_result = TR2_OK;

    if (!service->campaign_open || !service->data_store_started ||
        !service->acquisition_window_started ||
        !acquisition_service_window_active(service->acquisition_service)) {
        return TR2_ERROR_INVALID_STATE;
    }

    if (service->acquisition_service->current_window.acquired_sample_count >=
        service->acquisition_service->current_window.configuration.payload.window_size_samples) {
        stop_result = acquisition_service_end_window(service->acquisition_service, &window);
        service->acquisition_window_started = false;
        out_step->kind = CAMPAIGN_ACQUISITION_STEP_WINDOW_COMPLETED;
        out_step->window = window;
        out_step->stop_result = stop_result;
        storage_result = checkpoint_completed_window(service, out_step);
        if (storage_result != TR2_OK) {
            return storage_result;
        }
        return stop_result;
    }

    memset(&sample, 0, sizeof(sample));
    read_result = acquisition_service_read_sample(service->acquisition_service, &sample);
    if (read_result == TR2_OK) {
        out_step->kind = CAMPAIGN_ACQUISITION_STEP_SAMPLE_READ;
        out_step->sample = sample;
        encode_campaign_sample(&sample, record);
        storage_result = service->data_store->append(service->data_store->context,
                                                     service->active_metadata.campaign_id,
                                                     record,
                                                     sizeof(record));
        out_step->storage_result = storage_result;
        return storage_result;
    }

    out_step->source_result = read_result;
    stop_result = acquisition_service_end_window(service->acquisition_service, &window);
    service->acquisition_window_started = false;
    out_step->kind = CAMPAIGN_ACQUISITION_STEP_WINDOW_COMPLETED;
    out_step->window = window;
    out_step->stop_result = stop_result;
    storage_result = checkpoint_completed_window(service, out_step);
    if (storage_result != TR2_OK) {
        return storage_result;
    }

    return read_result;
}

Tr2Result campaign_service_publish_supervision_step(
    SupervisionService *supervision_service,
    const CampaignAcquisitionStep *step)
{
    if (supervision_service == NULL || step == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!supervision_service_is_initialized(supervision_service)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (step->kind != CAMPAIGN_ACQUISITION_STEP_WINDOW_COMPLETED ||
        step->storage_result != TR2_OK) {
        return TR2_ERROR_INVALID_STATE;
    }

    return supervision_service_publish_window(supervision_service, &step->window);
}

static Tr2Result campaign_service_stop_internal(
    CampaignService *service,
    SupervisionService *supervision_service,
    CampaignMetadata *out_closed_metadata)
{
    Tr2Result result;
    Tr2Result supervision_result = TR2_OK;

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
        CampaignAcquisitionStep step;

        memset(&step, 0, sizeof(step));
        step.kind = CAMPAIGN_ACQUISITION_STEP_WINDOW_COMPLETED;
        step.source_result = TR2_OK;
        step.stop_result = acquisition_service_end_window(service->acquisition_service,
                                                          &step.window);
        service->acquisition_window_started = false;
        if (step.stop_result != TR2_OK) {
            return step.stop_result;
        }

        result = checkpoint_completed_window(service, &step);
        if (result != TR2_OK) {
            return result;
        }

        if (supervision_service != NULL) {
            supervision_result = campaign_service_publish_supervision_step(supervision_service,
                                                                           &step);
            if (supervision_result == TR2_ERROR_NOT_AVAILABLE) {
                supervision_result = TR2_OK;
            }
        }
    }

    if (service->data_store_started) {
        result = service->data_store->finish_campaign(
            service->data_store->context,
            service->active_metadata.campaign_id);
        if (result != TR2_OK) {
            return result;
        }
        service->data_store_started = false;
        service->data_store_recovery_pending = true;
    }

    if (service->data_store_recovery_pending) {
        CampaignDataRecoveryResult recovery;

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
        service->data_store_recovery_pending = false;
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

    return supervision_result;
}

Tr2Result campaign_service_stop(CampaignService *service,
                                CampaignMetadata *out_closed_metadata)
{
    return campaign_service_stop_internal(service, NULL, out_closed_metadata);
}

Tr2Result campaign_service_stop_with_supervision(
    CampaignService *service,
    SupervisionService *supervision_service,
    CampaignMetadata *out_closed_metadata)
{
    if (supervision_service == NULL ||
        !supervision_service_is_initialized(supervision_service)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    return campaign_service_stop_internal(service,
                                          supervision_service,
                                          out_closed_metadata);
}
