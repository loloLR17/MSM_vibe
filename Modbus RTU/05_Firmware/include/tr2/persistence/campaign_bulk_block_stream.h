#ifndef TR2_PERSISTENCE_CAMPAIGN_BULK_BLOCK_STREAM_H
#define TR2_PERSISTENCE_CAMPAIGN_BULK_BLOCK_STREAM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/campaign/campaign.h"
#include "tr2/persistence/campaign_bulk_block.h"
#include "tr2/persistence/campaign_bulk_media.h"

typedef struct {
    CampaignBulkMedia *media;
    CampaignId campaign_id;
    uint64_t next_offset;
    uint64_t next_block_index;
    bool initialized;
    bool faulted;
} CampaignBulkBlockWriter;

typedef struct {
    CampaignBulkMedia *media;
    CampaignId campaign_id;
    uint64_t next_offset;
    uint64_t next_block_index;
    bool initialized;
    bool faulted;
} CampaignBulkBlockReader;

Tr2Result campaign_bulk_block_writer_init(
    CampaignBulkBlockWriter *writer,
    CampaignBulkMedia *media,
    CampaignId campaign_id,
    uint64_t initial_offset,
    uint64_t initial_block_index);

Tr2Result campaign_bulk_block_writer_append(
    CampaignBulkBlockWriter *writer,
    const uint8_t *payload,
    size_t payload_size,
    uint8_t *scratch,
    size_t scratch_capacity);

uint64_t campaign_bulk_block_writer_next_offset(
    const CampaignBulkBlockWriter *writer);

uint64_t campaign_bulk_block_writer_next_index(
    const CampaignBulkBlockWriter *writer);

bool campaign_bulk_block_writer_is_faulted(
    const CampaignBulkBlockWriter *writer);

Tr2Result campaign_bulk_block_reader_init(
    CampaignBulkBlockReader *reader,
    CampaignBulkMedia *media,
    CampaignId campaign_id,
    uint64_t initial_offset,
    uint64_t initial_block_index);

Tr2Result campaign_bulk_block_reader_next(
    CampaignBulkBlockReader *reader,
    uint8_t *scratch,
    size_t scratch_capacity,
    CampaignBulkBlockInfo *info,
    const uint8_t **payload);

uint64_t campaign_bulk_block_reader_next_offset(
    const CampaignBulkBlockReader *reader);

uint64_t campaign_bulk_block_reader_next_index(
    const CampaignBulkBlockReader *reader);

bool campaign_bulk_block_reader_is_faulted(
    const CampaignBulkBlockReader *reader);

#endif
