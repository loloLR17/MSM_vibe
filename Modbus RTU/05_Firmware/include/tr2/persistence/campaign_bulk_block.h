#ifndef TR2_PERSISTENCE_CAMPAIGN_BULK_BLOCK_H
#define TR2_PERSISTENCE_CAMPAIGN_BULK_BLOCK_H

#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/campaign/campaign.h"

#define TR2_CAMPAIGN_BULK_BLOCK_MAGIC UINT32_C(0x54524244)
#define TR2_CAMPAIGN_BULK_BLOCK_VERSION UINT16_C(1)
#define TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE ((size_t)32u)
#define TR2_CAMPAIGN_BULK_BLOCK_TRAILER_SIZE ((size_t)4u)
#define TR2_CAMPAIGN_BULK_SAMPLE_RECORD_SIZE ((size_t)16u)

typedef struct {
    CampaignId campaign_id;
    uint64_t block_index;
    uint32_t payload_size;
} CampaignBulkBlockInfo;

Tr2Result campaign_bulk_block_encoded_size(size_t payload_size,
                                           size_t *encoded_size);

Tr2Result campaign_bulk_block_encode(
    CampaignId campaign_id,
    uint64_t block_index,
    const uint8_t *payload,
    size_t payload_size,
    uint8_t *output,
    size_t output_capacity,
    size_t *encoded_size);

Tr2Result campaign_bulk_block_decode(
    const uint8_t *block,
    size_t block_size,
    CampaignBulkBlockInfo *info,
    const uint8_t **payload);

#endif
