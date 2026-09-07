#include "tr2/persistence/persistent_media_region.h"

#include <stddef.h>

static bool range_valid(const PersistentMediaRegion *region,
                        uint32_t offset,
                        size_t size)
{
    return offset <= region->size && size <= (size_t)(region->size - offset);
}

static Tr2Result region_read(void *context,
                             uint32_t offset,
                             void *buffer,
                             size_t size)
{
    PersistentMediaRegion *region = context;

    if (!persistent_media_region_is_initialized(region) ||
        (buffer == NULL && size != 0u) ||
        !range_valid(region, offset, size)) {
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

static Tr2Result region_write(void *context,
                              uint32_t offset,
                              const void *buffer,
                              size_t size)
{
    PersistentMediaRegion *region = context;

    if (!persistent_media_region_is_initialized(region) ||
        (buffer == NULL && size != 0u) ||
        !range_valid(region, offset, size)) {
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

static Tr2Result region_commit(void *context)
{
    PersistentMediaRegion *region = context;

    if (!persistent_media_region_is_initialized(region)) {
        return TR2_ERROR_INVALID_STATE;
    }

    return region->parent->commit(region->parent->context);
}

Tr2Result persistent_media_region_init(PersistentMediaRegion *region,
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
    region->interface.read = region_read;
    region->interface.write = region_write;
    region->interface.commit = region_commit;
    return TR2_OK;
}

bool persistent_media_region_is_initialized(const PersistentMediaRegion *region)
{
    return region != NULL && region->initialized && region->parent != NULL &&
           region->parent->read != NULL && region->parent->write != NULL &&
           region->parent->commit != NULL && region->size != 0u;
}

const PersistentMedia *persistent_media_region_interface(
    const PersistentMediaRegion *region)
{
    return persistent_media_region_is_initialized(region)
               ? &region->interface
               : NULL;
}
