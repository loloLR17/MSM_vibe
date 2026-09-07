#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/configuration_store.h"

typedef struct {
    uint8_t bytes[TR2_CONFIGURATION_STORE_STORAGE_SIZE];
    size_t unavailable_slot;
} SelectiveReadMedia;

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    SelectiveReadMedia *media = (SelectiveReadMedia *)context;
    const size_t slot = (size_t)offset / TR2_CONFIGURATION_STORE_SLOT_SIZE;

    if (slot == media->unavailable_slot) {
        return TR2_ERROR_STORAGE;
    }
    if ((size_t)offset + size > sizeof(media->bytes)) {
        return TR2_ERROR_STORAGE;
    }

    memcpy(buffer, &media->bytes[offset], size);
    return TR2_OK;
}

static Tr2Result media_write(void *context,
                             uint32_t offset,
                             const void *buffer,
                             size_t size)
{
    SelectiveReadMedia *media = (SelectiveReadMedia *)context;

    if ((size_t)offset + size > sizeof(media->bytes)) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(&media->bytes[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    (void)context;
    return TR2_OK;
}

static ActiveConfigurationSnapshot valid_snapshot(void)
{
    ActiveConfigurationSnapshot snapshot;

    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.generation = 7u;
    snapshot.config_id = 42u;
    snapshot.revision_counter = 11u;
    snapshot.payload.sampling_frequency_hz = 26667u;
    snapshot.payload.axes_enable_mask = 0x0007u;
    snapshot.payload.full_scale_code = 1u;
    snapshot.payload.acquisition_mode = 1u;
    snapshot.payload.window_size_samples = 4096u;
    snapshot.payload.indicator_period_ms = 2000u;
    snapshot.payload.campaign_duration_s = 3600u;
    snapshot.payload.storage_mode = 1u;
    snapshot.payload.storage_limit_mb = 100u;
    snapshot.payload.campaign_context_id = 1u;
    snapshot.payload.mission_id = 1u;
    return snapshot;
}

int main(void)
{
    SelectiveReadMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationRecoveryResult recovery;
    ConfigurationValidationEnvironment environment = {true, 4096u};
    ActiveConfigurationSnapshot snapshot = valid_snapshot();
    uint8_t record[TR2_CONFIGURATION_RECORD_SIZE];

    memset(&media, 0xFF, sizeof(media));
    media.unavailable_slot = 1u;
    assert(tr2_configuration_record_encode(&snapshot, record, sizeof(record)) == TR2_OK);
    memcpy(&media.bytes[0], record, sizeof(record));

    persistent_media.context = &media;
    persistent_media.read = media_read;
    persistent_media.write = media_write;
    persistent_media.commit = media_commit;

    assert(persistent_storage_core_init(&core, &persistent_media) == TR2_OK);
    assert(configuration_store_init(&store, &core) == TR2_OK);
    assert(configuration_store_recover(&store, &environment, &recovery) == TR2_OK);
    assert(recovery.status == CONFIGURATION_RECOVERY_UNAVAILABLE);
    assert(!recovery.has_snapshot);
    return 0;
}
