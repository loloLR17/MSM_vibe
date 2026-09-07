#include "tr2/application/campaign_inventory_service.h"

#include <string.h>

static bool repository_valid(const CampaignRepository *repository)
{
    return repository != NULL &&
           repository->get_inventory_summary != NULL &&
           repository->get_campaign_by_index != NULL;
}

Tr2Result campaign_inventory_service_init(CampaignInventoryService *service,
                                          CampaignRepository *repository)
{
    if (service == NULL || !repository_valid(repository)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(service, 0, sizeof(*service));
    service->repository = repository;
    service->initialized = true;
    return TR2_OK;
}

bool campaign_inventory_service_is_initialized(
    const CampaignInventoryService *service)
{
    return service != NULL && service->initialized &&
           repository_valid(service->repository);
}

Tr2Result campaign_inventory_service_select(CampaignInventoryService *service,
                                            uint16_t selected_campaign_index)
{
    if (!campaign_inventory_service_is_initialized(service)) {
        return TR2_ERROR_INVALID_STATE;
    }

    service->selected_campaign_index = selected_campaign_index;
    service->generation++;
    return TR2_OK;
}

Tr2Result campaign_inventory_service_snapshot(
    CampaignInventoryService *service,
    CampaignInventoryViewSnapshot *snapshot)
{
    CampaignInventoryViewSnapshot candidate;
    Tr2Result result;

    if (!campaign_inventory_service_is_initialized(service)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (snapshot == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(&candidate, 0, sizeof(candidate));
    result = service->repository->get_inventory_summary(
        service->repository->context,
        &candidate.inventory);
    if (result != TR2_OK) {
        return result;
    }

    candidate.selected_campaign_index = service->selected_campaign_index;
    if ((size_t)candidate.selected_campaign_index <
        candidate.inventory.valid_campaign_count) {
        result = service->repository->get_campaign_by_index(
            service->repository->context,
            (size_t)candidate.selected_campaign_index,
            &candidate.selected_campaign);
        if (result != TR2_OK) {
            return result;
        }
        candidate.selected_campaign_valid = true;
    }

    service->generation++;
    candidate.generation = service->generation;
    *snapshot = candidate;
    return TR2_OK;
}
