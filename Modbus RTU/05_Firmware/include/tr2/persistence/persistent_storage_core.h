#ifndef TR2_PERSISTENCE_PERSISTENT_STORAGE_CORE_H
#define TR2_PERSISTENCE_PERSISTENT_STORAGE_CORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/platform/persistent_media.h"

typedef struct {
    const PersistentMedia *media;
    bool initialized;
} PersistentStorageCore;

Tr2Result persistent_storage_core_init(
    PersistentStorageCore *core,
    const PersistentMedia *media);

bool persistent_storage_core_is_initialized(const PersistentStorageCore *core);

Tr2Result persistent_storage_core_read(
    const PersistentStorageCore *core,
    uint32_t offset,
    void *buffer,
    size_t size);

Tr2Result persistent_storage_core_write(
    PersistentStorageCore *core,
    uint32_t offset,
    const void *buffer,
    size_t size);

Tr2Result persistent_storage_core_commit(PersistentStorageCore *core);

#endif
