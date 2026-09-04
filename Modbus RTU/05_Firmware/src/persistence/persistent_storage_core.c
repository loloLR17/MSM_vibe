#include "tr2/persistence/persistent_storage_core.h"

static bool media_contract_valid(const PersistentMedia *media)
{
    return media != NULL &&
           media->read != NULL &&
           media->write != NULL &&
           media->commit != NULL;
}

Tr2Result persistent_storage_core_init(
    PersistentStorageCore *core,
    const PersistentMedia *media)
{
    if (core == NULL || !media_contract_valid(media)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    core->media = media;
    core->initialized = true;
    return TR2_OK;
}

bool persistent_storage_core_is_initialized(const PersistentStorageCore *core)
{
    return core != NULL && core->initialized && media_contract_valid(core->media);
}

Tr2Result persistent_storage_core_read(
    const PersistentStorageCore *core,
    uint32_t offset,
    void *buffer,
    size_t size)
{
    if (!persistent_storage_core_is_initialized(core)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (buffer == NULL && size != 0u) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (size == 0u) {
        return TR2_OK;
    }

    return core->media->read(core->media->context, offset, buffer, size);
}

Tr2Result persistent_storage_core_write(
    PersistentStorageCore *core,
    uint32_t offset,
    const void *buffer,
    size_t size)
{
    if (!persistent_storage_core_is_initialized(core)) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (buffer == NULL && size != 0u) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (size == 0u) {
        return TR2_OK;
    }

    return core->media->write(core->media->context, offset, buffer, size);
}

Tr2Result persistent_storage_core_commit(PersistentStorageCore *core)
{
    if (!persistent_storage_core_is_initialized(core)) {
        return TR2_ERROR_INVALID_STATE;
    }

    return core->media->commit(core->media->context);
}
