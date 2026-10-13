#ifndef TR2_CAMPAIGN_BULK_MEDIA_REGION_H
#define TR2_CAMPAIGN_BULK_MEDIA_REGION_H

#include "tr2/persistence/campaign_bulk_media.h"
#include "tr2/persistence/persistent_media_region.h"

/* Caller must establish ownership first. This adapter only confines accesses;
 * it does not allocate, format, infer capacity, or create resource authority.
 * Reuse the existing region for 32-bit bounded allocations on a 64-bit medium. */
typedef struct {
    CampaignBulkMedia *parent;
    PersistentMedia bridge;
    PersistentMediaRegion region;
    CampaignBulkMedia interface;
} CampaignBulkMediaRegion;

static inline Tr2Result bulk_region_parent_read(void *ctx, uint32_t off, void *out, size_t n)
{
    CampaignBulkMediaRegion *r = ctx;
    return campaign_bulk_media_read(r->parent, off, out, n);
}
static inline Tr2Result bulk_region_parent_write(void *ctx, uint32_t off, const void *in, size_t n)
{
    CampaignBulkMediaRegion *r = ctx;
    return campaign_bulk_media_write(r->parent, off, in, n);
}
static inline Tr2Result bulk_region_parent_sync(void *ctx)
{
    CampaignBulkMediaRegion *r = ctx;
    return campaign_bulk_media_sync(r->parent);
}
static inline Tr2Result bulk_region_capacity(void *ctx, uint64_t *out)
{
    CampaignBulkMediaRegion *r = ctx;
    if (!persistent_media_region_is_initialized(&r->region) || out == NULL)
        return TR2_ERROR_INVALID_STATE;
    *out = r->region.size;
    return TR2_OK;
}
static inline bool bulk_region_range(CampaignBulkMediaRegion *r, uint64_t off, size_t n)
{
    return persistent_media_region_is_initialized(&r->region) &&
        off <= r->region.size && (uint64_t)n <= (uint64_t)r->region.size - off;
}
static inline Tr2Result bulk_region_read(void *ctx, uint64_t off, void *out, size_t n)
{
    CampaignBulkMediaRegion *r = ctx;
    if (!bulk_region_range(r, off, n)) return TR2_ERROR_NOT_AVAILABLE;
    return persistent_media_region_read(&r->region, (uint32_t)off, out, n);
}
static inline Tr2Result bulk_region_write(void *ctx, uint64_t off, const void *in, size_t n)
{
    CampaignBulkMediaRegion *r = ctx;
    if (!bulk_region_range(r, off, n)) return TR2_ERROR_NOT_AVAILABLE;
    return persistent_media_region_write(&r->region, (uint32_t)off, in, n);
}
static inline Tr2Result bulk_region_sync(void *ctx)
{
    CampaignBulkMediaRegion *r = ctx;
    return persistent_media_region_commit(&r->region);
}
static inline Tr2Result campaign_bulk_media_region_init(CampaignBulkMediaRegion *r,
    CampaignBulkMedia *parent, uint32_t base, uint32_t size)
{
    uint64_t capacity;
    Tr2Result result;
    if (r == NULL || parent == NULL || size == 0u || base > UINT32_MAX - size ||
        base % 512u != 0u || size % 512u != 0u)
        return TR2_ERROR_INVALID_ARGUMENT;
    result = campaign_bulk_media_capacity(parent, &capacity);
    if (result != TR2_OK) return result;
    if ((uint64_t)base + size > capacity) return TR2_ERROR_NOT_AVAILABLE;
    if (parent->read == NULL || parent->write == NULL || parent->sync == NULL)
        return TR2_ERROR_INVALID_ARGUMENT;
    r->parent = parent;
    r->bridge = (PersistentMedia){r, bulk_region_parent_read,
        bulk_region_parent_write, bulk_region_parent_sync};
    result = persistent_media_region_init(&r->region, &r->bridge, base, size);
    if (result != TR2_OK) return result;
    r->interface = (CampaignBulkMedia){r, bulk_region_capacity,
        bulk_region_read, bulk_region_write, bulk_region_sync};
    return TR2_OK;
}
#endif
