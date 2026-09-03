#ifndef TR2_PLATFORM_PERSISTENT_MEDIA_H
#define TR2_PLATFORM_PERSISTENT_MEDIA_H

#include <stddef.h>
#include <stdint.h>
#include "tr2/common/result.h"

typedef struct {
    void *context;
    Tr2Result (*read)(void *context, uint32_t offset, void *buffer, size_t size);
    Tr2Result (*write)(void *context, uint32_t offset, const void *buffer, size_t size);
    Tr2Result (*commit)(void *context);
} PersistentMedia;

#endif
