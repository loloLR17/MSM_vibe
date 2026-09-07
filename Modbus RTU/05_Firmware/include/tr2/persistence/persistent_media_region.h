#ifndef TR2_PERSISTENCE_PERSISTENT_MEDIA_REGION_H
#define TR2_PERSISTENCE_PERSISTENT_MEDIA_REGION_H

#include <stdbool.h>
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

Tr2Result persistent_media_region_init(PersistentMediaRegion *region,
                                       const PersistentMedia *parent,
                                       uint32_t base_offset,
                                       uint32_t size);

bool persistent_media_region_is_initialized(const PersistentMediaRegion *region);

const PersistentMedia *persistent_media_region_interface(
    const PersistentMediaRegion *region);

#endif
