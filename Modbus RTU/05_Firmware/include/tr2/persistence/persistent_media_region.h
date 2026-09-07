#ifndef TR2_PERSISTENCE_PERSISTENT_MEDIA_REGION_H
#define TR2_PERSISTENCE_PERSISTENT_MEDIA_REGION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/platform/persistent_media.h"

typedef struct {
    const PersistentMedia *parent;
    uint32_t base_offset;
    uint32_t size;
    bool initialized;
    PersistentMedia interface;
} PersistentMediaRegion;

static inline bool persistent_media_region_is_initialized(
    const PersistentMediaRegion *region)
{
    return region != NULL && region->initialized && region->parent != NULL &&
           region->parent->read != NULL && region->parent->write != NULL &&
           region->parent->commit != NULL && region->size != 0u;
}

static inline bool persistent_media_region_range_valid(
    const PersistentMediaRegion *region,
    uint32_t offset,
    size_t size)
{
    return offset <= region->size && size <= (size_t)(region->size - offset);
}

static inline Tr2Result persistent_media_region_read(void *context,
                                                     uint32_t offset,
                                                     void *buffer,
                                                     size_t size)
{
    PersistentMediaRegion *region = context;

    if (!persistent_media_region_is_initialized(region) ||
        (buffer == NULL && size != 0u) ||
        !persistent_media_region_range_valid(region, offset, size)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (size == 0u) {
        return TR2_OK;
    }

    return region->parent->read(region->parent->context,
                                region->base_offset + offset,
                                buffer,
                                size);
}

static inline Tr2Result persistent_media_region_write(void *context,
                                                      uint32_t offset,
                                                      const void *buffer,
                                                      size_t size)
{
    PersistentMediaRegion *region = context;

    if (!persistent_media_region_is_initialized(region) ||
        (buffer == NULL && size != 0u) ||
        !persistent_media_region_range_valid(region, offset, size)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (size == 0u) {
        return TR2_OK;
    }

    return region->parent->write(region->parent->context,
                                 region->base_offset + offset,
                                 buffer,
                                 size);
}

static inline Tr2Result persistent_media_region_commit(void *context)
{
    PersistentMediaRegion *region = context;

    if (!persistent_media_region_is_initialized(region)) {
        return TR2_ERROR_INVALID_STATE;
    }
    return region->parent->commit(region->parent->context);
}

static inline Tr2Result persistent_media_region_init(PersistentMediaRegion *region,
                                                     const PersistentMedia *parent,
                                                     uint32_t base_offset,
                                                     uint32_t size)
{
    if (region == NULL || parent == NULL ||
        parent->read == NULL || parent->write == NULL || parent->commit == NULL ||
        size == 0u || base_offset > UINT32_MAX - size) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    region->parent = parent;
    region->base_offset = base_offset;
    region->size = size;
    region->initialized = true;
    region->interface.context = region;
    region->interface.read = persistent_media_region_read;
    region->interface.write = persistent_media_region_write;
    region->interface.commit = persistent_media_region_commit;
    return TR2_OK;
}

static inline const PersistentMedia *persistent_media_region_interface(
    const PersistentMediaRegion *region)
{
    return persistent_media_region_is_initialized(region)
               ? &region->interface
               : NULL;
}

#endif
