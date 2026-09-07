#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/configuration_service.h"

#define NO_PARTIAL_FAILURE ((size_t)-1)

typedef struct {
    uint8_t durable[TR2_CONFIGURATION_STORE_STORAGE_SIZE];
    uint8_t staged[TR2_CONFIGURATION_STORE_STORAGE_SIZE];
    size_t partial_write_count;
    bool fail_commit;
} TestMediaContext;

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestMediaContext *media = (TestMediaContext *)context;

    if ((size_t)offset + size > sizeof(media->durable)) {
        return TR2_ERROR_STORAGE;
    }

    memcpy(buffer, &media->durable[offset], size);
    return TR2_OK;
}

static Tr2Result media_write(void *context,
                             uint32_t offset,
                             const void *buffer,
                             size_t size)
{
    TestMediaContext *media = (TestMediaContext *)context;
    size_t copy_size = size;

    if ((size_t)offset + size > sizeof(media->staged)) {
        return TR2_ERROR_STORAGE;
    }

    if (media->partial_write_count != NO_PARTIAL_FAILURE &&
        media->partial_write_count < size) {
        copy_size = media->partial_write_count;
        memcpy(&media->staged[offset], buffer, copy_size);
        return TR2_ERROR_STORAGE;
    }

    memcpy(&media->staged[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    TestMediaContext *media = (TestMediaContext *)context;

    if (media->fail_commit) {
        return TR2_ERROR_STORAGE;
    }

    memcpy(media->durable, media->staged, sizeof(media->durable));
    return TR2_OK;
}

static void media_init(TestMediaContext *media)
{
    memset(media, 0, sizeof(*media));
    memset(media->durable, 0xFF, sizeof(media->durable));
    memcpy(media->staged, media->durable, sizeof(media->staged));
    media->partial_write_count = NO_PARTIAL_FAILURE;
}

static void simulate_reboot(TestMediaContext *media)
{
    memcpy(media->staged, media->durable, sizeof(media->staged));
    media->partial_write_count = NO_PARTIAL_FAILURE;
    media->fail_commit = false;
}

static void init_service(TestMediaContext *media,
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

static ConfigurationValidationEnvironment valid_environment(void)
{
    ConfigurationValidationEnvironment environment;

    environment.storage_capacity_known = true;
    environment.usable_storage_capacity_mb = 4096u;
    return environment;
}

static ActiveConfigurationSnapshot make_snapshot(uint32_t generation,
                                                 uint32_t config_id,
                                                 uint32_t revision_counter)
{
    ActiveConfigurationSnapshot snapshot;

    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.generation = generation;
    snapshot.config_id = config_id;
    snapshot.revision_counter = revision_counter;
    snapshot.payload.sampling_frequency_hz = 26667u;
    snapshot.payload.axes_enable_mask = 0x0007u;
    snapshot.payload.full_scale_code = 1u;
    snapshot.payload.acquisition_mode = 1u;
    snapshot.payload.window_size_samples = 4096u;
    snapshot.payload.indicator_period_ms = 2000u;
    snapshot.payload.campaign_duration_s = 3600u;
    snapshot.payload.storage_mode = 1u;
    snapshot.payload.storage_limit_mb = 100u;
    snapshot.payload.campaign_context_id = UINT32_C(0x12345678);
    snapshot.payload.mission_id = UINT32_C(0x87654321);
    snapshot.payload.operating_mode_code = 1u;
    return snapshot;
}

static void assert_identity(const ActiveConfigurationSnapshot *expected,
                            const ActiveConfigurationSnapshot *actual)
{
    assert(expected->generation == actual->generation);
    assert(expected->config_id == actual->config_id);
    assert(expected->revision_counter == actual->revision_counter);
    assert(memcmp(&expected->payload, &actual->payload, sizeof(expected->payload)) == 0);
}

static void test_empty_recovery_has_no_runtime_active(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationService service;
    ActiveConfigurationSnapshot active;
    ConfigurationValidationEnvironment environment = valid_environment();

    media_init(&media);
    init_service(&media, &persistent_media, &core, &store, &service);

    assert(configuration_service_recover(&service, &environment) == TR2_OK);
    assert(configuration_service_recovery_status(&service) == CONFIGURATION_RECOVERY_EMPTY);
    assert(!configuration_service_active_snapshot(&service, &active));
}

static void test_commit_publishes_only_after_durable_success(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationService service;
    ActiveConfigurationSnapshot active;
    const ActiveConfigurationSnapshot a = make_snapshot(1u, 10u, 100u);

    media_init(&media);
    init_service(&media, &persistent_media, &core, &store, &service);

    media.fail_commit = true;
    assert(configuration_service_commit_candidate(&service, &a) == TR2_ERROR_STORAGE);
    assert(!configuration_service_active_snapshot(&service, &active));

    simulate_reboot(&media);
}

static void test_commit_success_publishes_coherent_snapshot(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationService service;
    ActiveConfigurationSnapshot active;
    const ActiveConfigurationSnapshot a = make_snapshot(1u, 10u, 100u);

    media_init(&media);
    init_service(&media, &persistent_media, &core, &store, &service);

    assert(configuration_service_commit_candidate(&service, &a) == TR2_OK);
    assert(configuration_service_recovery_status(&service) == CONFIGURATION_RECOVERY_VALID);
    assert(configuration_service_active_snapshot(&service, &active));
    assert_identity(&a, &active);
}

static void test_failed_replacement_keeps_old_runtime_active(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationService service;
    ActiveConfigurationSnapshot active;
    const ActiveConfigurationSnapshot a = make_snapshot(1u, 10u, 100u);
    const ActiveConfigurationSnapshot b = make_snapshot(2u, 20u, 200u);

    media_init(&media);
    init_service(&media, &persistent_media, &core, &store, &service);

    assert(configuration_service_commit_candidate(&service, &a) == TR2_OK);
    media.fail_commit = true;
    assert(configuration_service_commit_candidate(&service, &b) == TR2_ERROR_STORAGE);

    assert(configuration_service_active_snapshot(&service, &active));
    assert_identity(&a, &active);
}

static void test_reboot_recovers_committed_runtime_authority(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media_a;
    PersistentStorageCore core_a;
    ConfigurationStore store_a;
    ConfigurationService service_a;
    PersistentMedia persistent_media_b;
    PersistentStorageCore core_b;
    ConfigurationStore store_b;
    ConfigurationService service_b;
    ActiveConfigurationSnapshot active;
    ConfigurationValidationEnvironment environment = valid_environment();
    const ActiveConfigurationSnapshot a = make_snapshot(7u, 42u, 11u);

    media_init(&media);
    init_service(&media, &persistent_media_a, &core_a, &store_a, &service_a);
    assert(configuration_service_commit_candidate(&service_a, &a) == TR2_OK);

    simulate_reboot(&media);
    init_service(&media, &persistent_media_b, &core_b, &store_b, &service_b);
    assert(configuration_service_recover(&service_b, &environment) == TR2_OK);

    assert(configuration_service_recovery_status(&service_b) == CONFIGURATION_RECOVERY_VALID);
    assert(configuration_service_active_snapshot(&service_b, &active));
    assert_identity(&a, &active);
}

static void test_corrupted_recovery_publishes_no_active(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationService service;
    ActiveConfigurationSnapshot active;
    ConfigurationValidationEnvironment environment = valid_environment();

    media_init(&media);
    media.durable[0] = 0x12u;
    media.staged[0] = 0x12u;
    init_service(&media, &persistent_media, &core, &store, &service);

    assert(configuration_service_recover(&service, &environment) == TR2_OK);
    assert(configuration_service_recovery_status(&service) == CONFIGURATION_RECOVERY_CORRUPTED);
    assert(!configuration_service_active_snapshot(&service, &active));
}

static void test_invalid_arguments(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationService service;
    ConfigurationValidationEnvironment environment = valid_environment();
    ActiveConfigurationSnapshot snapshot = make_snapshot(1u, 1u, 1u);

    memset(&service, 0, sizeof(service));
    assert(configuration_service_commit_candidate(&service, &snapshot) == TR2_ERROR_INVALID_STATE);
    assert(configuration_service_recover(&service, &environment) == TR2_ERROR_INVALID_STATE);
    assert(configuration_service_init(NULL, &store) == TR2_ERROR_INVALID_ARGUMENT);

    media_init(&media);
    persistent_media.context = &media;
    persistent_media.read = media_read;
    persistent_media.write = media_write;
    persistent_media.commit = media_commit;
    assert(persistent_storage_core_init(&core, &persistent_media) == TR2_OK);
    assert(configuration_store_init(&store, &core) == TR2_OK);
    assert(configuration_service_init(&service, &store) == TR2_OK);
    assert(configuration_service_commit_candidate(&service, NULL) == TR2_ERROR_INVALID_ARGUMENT);
    assert(configuration_service_recover(&service, NULL) == TR2_ERROR_INVALID_ARGUMENT);
}

int main(void)
{
    test_empty_recovery_has_no_runtime_active();
    test_commit_publishes_only_after_durable_success();
    test_commit_success_publishes_coherent_snapshot();
    test_failed_replacement_keeps_old_runtime_active();
    test_reboot_recovers_committed_runtime_authority();
    test_corrupted_recovery_publishes_no_active();
    test_invalid_arguments();
    return 0;
}
