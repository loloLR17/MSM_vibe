#ifndef TR2_DOMAIN_CAMPAIGN_H
#define TR2_DOMAIN_CAMPAIGN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/domain/configuration/configuration.h"

#define TR2_CAMPAIGN_LABEL_LENGTH TR2_CONFIGURATION_LABEL_LENGTH

typedef uint32_t CampaignId;

#define TR2_CAMPAIGN_ID_INVALID ((CampaignId)0u)

typedef struct {
    bool available;
    uint32_t value;
} CampaignCivilTimestamp;

typedef struct {
    bool available;
    uint32_t seconds;
} CampaignDuration;

typedef struct {
    uint32_t configuration_generation;
    uint32_t configuration_id;
    uint32_t configuration_revision_counter;
    ConfigurationPayload configuration_payload;
} CampaignHistoricalContext;

typedef enum {
    CAMPAIGN_LIFECYCLE_OPEN = 0,
    CAMPAIGN_LIFECYCLE_CLOSED
} CampaignLifecycleState;

typedef enum {
    CAMPAIGN_DATA_INTEGRITY_UNKNOWN = 0,
    CAMPAIGN_DATA_INTEGRITY_COMPLETE,
    CAMPAIGN_DATA_INTEGRITY_PARTIAL,
    CAMPAIGN_DATA_INTEGRITY_CORRUPTED
} CampaignDataIntegrity;

typedef struct {
    CampaignId campaign_id;
    uint32_t mission_id;
    char campaign_label[TR2_CAMPAIGN_LABEL_LENGTH];
    char mission_label[TR2_CAMPAIGN_LABEL_LENGTH];
    CampaignCivilTimestamp start_timestamp;
    CampaignCivilTimestamp end_timestamp;
    CampaignDuration duration;
    uint64_t durable_data_size_bytes;
    CampaignLifecycleState lifecycle_state;
    CampaignDataIntegrity data_integrity;
    CampaignHistoricalContext historical_context;
} CampaignMetadata;

typedef struct {
    CampaignId campaign_id;
    bool valid;
} CampaignIdReservation;

typedef struct {
    size_t total_campaign_count;
    size_t valid_campaign_count;
} CampaignInventorySummary;

typedef struct {
    uint32_t generation;
    CampaignInventorySummary inventory;
} CampaignInventorySnapshot;

#endif
