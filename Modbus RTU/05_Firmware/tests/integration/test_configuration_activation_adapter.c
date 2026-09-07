#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/configuration_activation_adapter.h"
#include "tr2/application/configuration_workflow.h"
#include "tr2/modbus/b4_configuration_codec.h"
#include "tr2/persistence/configuration_store.h"
#include "tr2/persistence/persistent_storage_core.h"

#define TEST_STORAGE_SIZE TR2_CONFIGURATION_STORE_STORAGE_SIZE

typedef struct {
    uint8_t durable[TEST_STORAGE_SIZE];
    uint8_t staged[TEST_STORAGE_SIZE];
} TestMedia;

typedef struct {
    uint32_t generation;
    uint32_t revision;
    unsigned calls;
} MetadataSource;

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    if ((size_t)offset + size > TEST_STORAGE_SIZE) return TR2_ERROR_STORAGE;
    memcpy(buffer, &media->staged[offset], size);
    return TR2_OK;
}

static Tr2Result media_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;
    if ((size_t)offset + size > TEST_STORAGE_SIZE) return TR2_ERROR_STORAGE;
    memcpy(&media->staged[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    TestMedia *media = (TestMedia *)context;
    memcpy(media->durable, media->staged, TEST_STORAGE_SIZE);
    return TR2_OK;
}

static void media_init(TestMedia *media)
{
    memset(media->durable, 0xFF, TEST_STORAGE_SIZE);
    memcpy(media->staged, media->durable, TEST_STORAGE_SIZE);
}

static void media_reboot(TestMedia *media)
{
    memcpy(media->staged, media->durable, TEST_STORAGE_SIZE);
}

static Tr2Result acquire_metadata(void *context,
                                  const ValidatedConfiguration *validated,
                                  ConfigurationActivationMetadata *out_metadata)
{
    MetadataSource *source = (MetadataSource *)context;
    assert(validated != NULL);
    assert(out_metadata != NULL);
    ++source->calls;
    out_metadata->persistent_generation = source->generation;
    out_metadata->revision_counter = source->revision;
    return TR2_OK;
}

static ConfigurationPayload valid_payload(uint16_t window_size)
{
    ConfigurationPayload payload;
    memset(&payload, 0, sizeof(payload));
    payload.sampling_frequency_hz = UINT16_C(26667);
    payload.axes_enable_mask = UINT16_C(7);
    payload.full_scale_code = UINT16_C(2);
    payload.acquisition_mode = UINT16_C(1);
    payload.window_size_samples = window_size;
    payload.indicator_period_ms = UINT16_C(2000);
    payload.campaign_duration_s = UINT32_C(3600);
    payload.storage_mode = UINT16_C(1);
    payload.storage_limit_mb = UINT32_C(512);
    payload.campaign_context_id = UINT32_C(1);
    payload.mission_id = UINT32_C(2);
    payload.operating_mode_code = UINT16_C(1);
    return payload;
}

static void init_persistence(TestMedia *media,
                             PersistentMedia *persistent_media,
                             PersistentStorageCore *core,
                             ConfigurationStore *store,
                             ConfigurationService *service)
{
    persistent_media->context = media;
    persistent_media->read = media_read;
    persistent_media->write = media_write;
    persistent_media->commit = media_commit;
    assert(persistent_storage_core_init(core, persistent_media) == TR2_OK);
    assert(configuration_store_init(store, core) == TR2_OK);
    assert(configuration_service_init(service, store) == TR2_OK);
}

static void apply_validated(ConfigurationWorkflow *workflow,
                            ConfigurationStagingService *staging,
                            const ConfigurationPayload *payload,
                            uint32_t config_id,
                            const ConfigurationValidationEnvironment *environment)
{
    configuration_staging_set_config_id(staging, config_id);
    configuration_staging_replace_payload(staging, payload);
    configuration_staging_set_supplied_crc(staging, tr2_b4_prepared_payload_crc(payload));
    configuration_workflow_note_prepared_payload_modified(workflow);
    assert(configuration_workflow_validate(workflow, environment).status == CONFIGURATION_VALIDATION_VALID);
    assert(configuration_workflow_apply(workflow) == TR2_OK);
}

int main(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore core_a, core_b;
    ConfigurationStore store_a, store_b;
    ConfigurationService service_a, service_b;
    ConfigurationActivationAdapter adapter_a, adapter_b;
    ConfigurationStagingService staging_a, staging_b;
    ConfigurationWorkflow workflow_a, workflow_b;
    ConfigurationIntegrityPort integrity = { tr2_b4_prepared_payload_crc };
    ConfigurationValidationEnvironment environment = { true, UINT32_C(4096) };
    MetadataSource metadata = { UINT32_C(100), UINT32_C(10), 0u };
    ActiveConfigurationSnapshot active;
    ConfigurationRecoveryStatus status;
    PersistentMedia persistent_media_b;
    const ConfigurationPayload payload_a = valid_payload(UINT16_C(4096));
    const ConfigurationPayload payload_b = valid_payload(UINT16_C(8192));

    media_init(&media);
    init_persistence(&media, &persistent_media, &core_a, &store_a, &service_a);
    assert(configuration_activation_adapter_init(&adapter_a, &service_a, &metadata, acquire_metadata) == TR2_OK);
    configuration_staging_init(&staging_a);
    assert(configuration_workflow_init(&workflow_a, &staging_a, integrity,
                                       configuration_activation_adapter_port(&adapter_a)) == TR2_OK);

    apply_validated(&workflow_a, &staging_a, &payload_a, UINT32_C(42), &environment);
    assert(metadata.calls == 1u);
    assert(configuration_service_active_snapshot(&service_a, &active));
    assert(active.generation == UINT32_C(100));
    assert(active.revision_counter == UINT32_C(10));
    assert(active.config_id == UINT32_C(42));

    media_reboot(&media);
    init_persistence(&media, &persistent_media_b, &core_b, &store_b, &service_b);
    assert(configuration_service_recover(&service_b, &environment) == TR2_OK);
    assert(configuration_service_recovery_status(&service_b, &status));
    assert(status == CONFIGURATION_RECOVERY_VALID);
    assert(configuration_service_active_snapshot(&service_b, &active));
    assert(active.generation == UINT32_C(100));

    metadata.generation = UINT32_C(101);
    metadata.revision = UINT32_C(11);
    assert(configuration_activation_adapter_init(&adapter_b, &service_b, &metadata, acquire_metadata) == TR2_OK);
    configuration_staging_init(&staging_b);
    assert(configuration_workflow_init(&workflow_b, &staging_b, integrity,
                                       configuration_activation_adapter_port(&adapter_b)) == TR2_OK);

    apply_validated(&workflow_b, &staging_b, &payload_b, UINT32_C(43), &environment);
    assert(metadata.calls == 2u);
    assert(configuration_service_active_snapshot(&service_b, &active));
    assert(active.generation == UINT32_C(101));
    assert(active.revision_counter == UINT32_C(11));
    assert(active.config_id == UINT32_C(43));
    assert(active.payload.window_size_samples == UINT16_C(8192));

    return 0;
}
