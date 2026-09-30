#ifndef TR2_PERSISTENCE_CAMPAIGN_BULK_AGGREGATOR_H
#define TR2_PERSISTENCE_CAMPAIGN_BULK_AGGREGATOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/persistence/campaign_bulk_media.h"

typedef struct {
    CampaignBulkMedia *media;
    uint8_t *buffer;
    size_t buffer_capacity;
    size_t buffered_bytes;
    uint64_t write_offset;
    bool initialized;
    bool faulted;
} CampaignBulkAggregator;

Tr2Result campaign_bulk_aggregator_init(CampaignBulkAggregator *aggregator,
                                        CampaignBulkMedia *media,
                                        uint8_t *buffer,
                                        size_t buffer_capacity,
                                        uint64_t initial_offset);

Tr2Result campaign_bulk_aggregator_append(CampaignBulkAggregator *aggregator,
                                          const uint8_t *data,
                                          size_t size);

Tr2Result campaign_bulk_aggregator_flush(CampaignBulkAggregator *aggregator);

size_t campaign_bulk_aggregator_buffered_bytes(
    const CampaignBulkAggregator *aggregator);

uint64_t campaign_bulk_aggregator_write_offset(
    const CampaignBulkAggregator *aggregator);

bool campaign_bulk_aggregator_is_faulted(
    const CampaignBulkAggregator *aggregator);

#endif
