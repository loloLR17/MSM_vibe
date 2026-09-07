#ifndef TR2_PERSISTENCE_CAMPAIGN_RECORD_H
#define TR2_PERSISTENCE_CAMPAIGN_RECORD_H

#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/campaign/campaign.h"

#define TR2_CAMPAIGN_RECORD_FORMAT_VERSION UINT16_C(1)
#define TR2_CAMPAIGN_RECORD_SIZE 252u

Tr2Result tr2_campaign_record_encode(const CampaignMetadata *metadata,
                                     uint8_t *record,
                                     size_t record_size);

Tr2Result tr2_campaign_record_decode(const uint8_t *record,
                                     size_t record_size,
                                     CampaignMetadata *metadata);

#endif
