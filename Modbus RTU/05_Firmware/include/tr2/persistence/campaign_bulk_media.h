#ifndef TR2_PERSISTENCE_CAMPAIGN_BULK_MEDIA_H
#define TR2_PERSISTENCE_CAMPAIGN_BULK_MEDIA_H

#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"

/*
 * Raw byte-addressed bulk medium used by the future CampaignDataStore backend.
 *
 * This interface deliberately defines no campaign layout, buffering policy or
 * checkpoint format.  Those belong to later H3h-D slices.  A successful sync()
 * is the only media-level durability barrier exposed to the core.
 */
typedef struct {
    void *context;
    Tr2Result (*capacity_bytes)(void *context, uint64_t *capacity_bytes);
    Tr2Result (*read)(void *context,
                      uint64_t offset,
                      void *buffer,
                      size_t size);
    Tr2Result (*write)(void *context,
                       uint64_t offset,
                       const void *buffer,
                       size_t size);
    Tr2Result (*sync)(void *context);
} CampaignBulkMedia;

Tr2Result campaign_bulk_media_capacity(const CampaignBulkMedia *media,
                                       uint64_t *capacity_bytes);
Tr2Result campaign_bulk_media_read(const CampaignBulkMedia *media,
                                   uint64_t offset,
                                   void *buffer,
                                   size_t size);
Tr2Result campaign_bulk_media_write(const CampaignBulkMedia *media,
                                    uint64_t offset,
                                    const void *buffer,
                                    size_t size);
Tr2Result campaign_bulk_media_sync(const CampaignBulkMedia *media);

#endif
