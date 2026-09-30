#ifndef TR2_PERSISTENCE_CAMPAIGN_BULK_METADATA_H
#define TR2_PERSISTENCE_CAMPAIGN_BULK_METADATA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/campaign/campaign_types.h"
#include "tr2/persistence/campaign_bulk_media.h"

#define TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE ((size_t)512u)
#define TR2_CAMPAIGN_BULK_DESCRIPTOR_MAGIC UINT32_C(0x5452424D)
#define TR2_CAMPAIGN_BULK_DESCRIPTOR_VERSION UINT16_C(1)

typedef enum {
    CAMPAIGN_BULK_METADATA_STATE_OPEN = 1,
    CAMPAIGN_BULK_METADATA_STATE_FINISHED = 2
} CampaignBulkMetadataState;

typedef struct {
    uint64_t generation;
    CampaignId campaign_id;
    CampaignBulkMetadataState state;
    uint64_t data_base;
    uint64_t durable_prefix_bytes;
} CampaignBulkDescriptor;

typedef enum {
    CAMPAIGN_BULK_METADATA_RECOVERY_EMPTY = 0,
    CAMPAIGN_BULK_METADATA_RECOVERY_VALID,
    CAMPAIGN_BULK_METADATA_RECOVERY_UNSUPPORTED,
    CAMPAIGN_BULK_METADATA_RECOVERY_CORRUPTED,
    CAMPAIGN_BULK_METADATA_RECOVERY_UNAVAILABLE
} CampaignBulkMetadataRecoveryStatus;

typedef struct {
    CampaignBulkMetadataRecoveryStatus status;
    CampaignBulkDescriptor descriptor;
    uint8_t active_copy;
} CampaignBulkMetadataRecoveryResult;

typedef struct {
    CampaignBulkMedia *media;
    uint64_t descriptor_offsets[2];
    uint64_t generation;
    uint8_t active_copy;
    bool initialized;
    bool authority_loaded;
    bool recovery_required;
} CampaignBulkMetadata;

Tr2Result campaign_bulk_descriptor_encode(
    const CampaignBulkDescriptor *descriptor,
    uint8_t output[TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE]);

Tr2Result campaign_bulk_metadata_init(CampaignBulkMetadata *metadata,
                                      CampaignBulkMedia *media,
                                      uint64_t descriptor_a_offset,
                                      uint64_t descriptor_b_offset);

Tr2Result campaign_bulk_metadata_publish(
    CampaignBulkMetadata *metadata,
    const CampaignBulkDescriptor *descriptor);

Tr2Result campaign_bulk_metadata_recover(
    CampaignBulkMetadata *metadata,
    CampaignBulkMetadataRecoveryResult *result);

bool campaign_bulk_metadata_recovery_required(
    const CampaignBulkMetadata *metadata);

#endif
