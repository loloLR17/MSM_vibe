#include "tr2/persistence/campaign_bulk_media.h"

#include <stdbool.h>
#include <stdint.h>

static bool range_is_valid(uint64_t offset, size_t size)
{
    return (uint64_t)size <= UINT64_MAX - offset;
}

Tr2Result campaign_bulk_media_capacity(const CampaignBulkMedia *media,
                                       uint64_t *capacity_bytes)
{
    if (media == NULL || media->capacity_bytes == NULL || capacity_bytes == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    return media->capacity_bytes(media->context, capacity_bytes);
}

Tr2Result campaign_bulk_media_read(const CampaignBulkMedia *media,
                                   uint64_t offset,
                                   void *buffer,
                                   size_t size)
{
    if (media == NULL || media->read == NULL ||
        (buffer == NULL && size != 0u) ||
        !range_is_valid(offset, size)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (size == 0u) {
        return TR2_OK;
    }
    return media->read(media->context, offset, buffer, size);
}

Tr2Result campaign_bulk_media_write(const CampaignBulkMedia *media,
                                    uint64_t offset,
                                    const void *buffer,
                                    size_t size)
{
    if (media == NULL || media->write == NULL ||
        (buffer == NULL && size != 0u) ||
        !range_is_valid(offset, size)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (size == 0u) {
        return TR2_OK;
    }
    return media->write(media->context, offset, buffer, size);
}

Tr2Result campaign_bulk_media_sync(const CampaignBulkMedia *media)
{
    if (media == NULL || media->sync == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    return media->sync(media->context);
}
