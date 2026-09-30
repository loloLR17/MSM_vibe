#ifndef TR2_PERSISTENCE_CAMPAIGN_BULK_CHECKPOINT_H
#define TR2_PERSISTENCE_CAMPAIGN_BULK_CHECKPOINT_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/persistence/campaign_bulk_aggregator.h"

typedef struct {
    CampaignBulkAggregator *aggregator;
    uint64_t initial_offset;
    uint64_t durable_prefix_bytes;
    bool initialized;
    bool faulted;
} CampaignBulkCheckpoint;

Tr2Result campaign_bulk_checkpoint_init(CampaignBulkCheckpoint *checkpoint,
                                        CampaignBulkAggregator *aggregator);

Tr2Result campaign_bulk_checkpoint_publish(CampaignBulkCheckpoint *checkpoint);

uint64_t campaign_bulk_checkpoint_written_prefix_bytes(
    const CampaignBulkCheckpoint *checkpoint);

uint64_t campaign_bulk_checkpoint_durable_prefix_bytes(
    const CampaignBulkCheckpoint *checkpoint);

bool campaign_bulk_checkpoint_is_faulted(
    const CampaignBulkCheckpoint *checkpoint);

#endif
