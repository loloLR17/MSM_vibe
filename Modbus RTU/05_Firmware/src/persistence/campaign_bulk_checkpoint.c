#include "tr2/persistence/campaign_bulk_checkpoint.h"

#include <stddef.h>
#include <string.h>

static uint64_t written_prefix_bytes(const CampaignBulkCheckpoint *checkpoint)
{
    uint64_t write_offset =
        campaign_bulk_aggregator_write_offset(checkpoint->aggregator);

    return write_offset - checkpoint->initial_offset;
}

Tr2Result campaign_bulk_checkpoint_init(CampaignBulkCheckpoint *checkpoint,
                                        CampaignBulkAggregator *aggregator)
{
    if (checkpoint == NULL || aggregator == NULL ||
        aggregator->media == NULL || !aggregator->initialized ||
        campaign_bulk_aggregator_is_faulted(aggregator)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(checkpoint, 0, sizeof(*checkpoint));
    checkpoint->aggregator = aggregator;
    checkpoint->initial_offset =
        campaign_bulk_aggregator_write_offset(aggregator);
    checkpoint->initialized = true;
    return TR2_OK;
}

Tr2Result campaign_bulk_checkpoint_publish(CampaignBulkCheckpoint *checkpoint)
{
    Tr2Result result;
    uint64_t candidate_prefix;

    if (checkpoint == NULL || !checkpoint->initialized ||
        checkpoint->faulted) {
        return TR2_ERROR_INVALID_STATE;
    }

    result = campaign_bulk_aggregator_flush(checkpoint->aggregator);
    if (result != TR2_OK) {
        checkpoint->faulted = true;
        return result;
    }

    candidate_prefix = written_prefix_bytes(checkpoint);

    result = campaign_bulk_media_sync(checkpoint->aggregator->media);
    if (result != TR2_OK) {
        checkpoint->faulted = true;
        return result;
    }

    checkpoint->durable_prefix_bytes = candidate_prefix;
    return TR2_OK;
}

uint64_t campaign_bulk_checkpoint_written_prefix_bytes(
    const CampaignBulkCheckpoint *checkpoint)
{
    if (checkpoint == NULL || !checkpoint->initialized) {
        return 0u;
    }
    return written_prefix_bytes(checkpoint);
}

uint64_t campaign_bulk_checkpoint_durable_prefix_bytes(
    const CampaignBulkCheckpoint *checkpoint)
{
    return checkpoint != NULL && checkpoint->initialized
               ? checkpoint->durable_prefix_bytes
               : 0u;
}

bool campaign_bulk_checkpoint_is_faulted(
    const CampaignBulkCheckpoint *checkpoint)
{
    return checkpoint != NULL && checkpoint->initialized &&
           checkpoint->faulted;
}
