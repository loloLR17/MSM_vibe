#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/configuration_store.h"

#define NO_PARTIAL_FAILURE ((size_t)-1)

typedef struct {
    uint8_t durable[TR2_CONFIGURATION_STORE_STORAGE_SIZE];
    uint8_t staged[TR2_CONFIGURATION_STORE_STORAGE_SIZE];
    size_t partial_write_count;
    bool fail_commit;
    unsigned write_calls;
    unsigned commit_calls;
} TestMediaContext;

static Tr2Result test_media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestMediaContext *media = (TestMediaContext *)context;

    if ((size_t)offset + size > sizeof(media->staged)) {
        return TR2_ERROR_STORAGE;
    }

    memcpy(buffer, &media->staged[offset], size);
    return TR2_OK;
}

static Tr2Result test_media_write(void *context,
                                  uint32_t offset,
                                  const void *buffer,
                                  size_t size)
{
    TestMediaContext *media = (TestMediaContext *)context;
    size_t copy_size = size;

    ++media->write_calls;

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

static Tr2Result test_media_commit(void *context)
{
    TestMediaContext *media = (TestMediaContext *)context;

    ++media->commit_calls;
    if (media->fail_commit) {
        return TR2_ERROR_STORAGE;
    }

    memcpy(media->durable, media->staged, sizeof(media->durable));
    return TR2_OK;
}

static void test_media_init(TestMediaContext *media)
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

static ActiveConfigurationSnapshot make_snapshot(uint32_t generation,
                                                 uint32_t config_id,
                                                 uint32_t revision_counter,
                                                 uint16_t marker)
{
    ActiveConfigurationSnapshot snapshot;

    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.generation = generation;
    snapshot.config_id = config_id;
    snapshot.revision_counter = revision_counter;
    snapshot.payload.sampling_frequency_hz = marker;
    snapshot.payload.axes_enable_mask = (uint16_t)(marker + 1u);
    snapshot.payload.window_size_samples = (uint16_t)(marker + 2u);
    snapshot.payload.campaign_context_id = UINT32_C(0x10000000) + marker;
    snapshot.payload.mission_id = UINT32_C(0x20000000) + marker;
    snapshot.payload.operating_mode_code = (uint16_t)(marker + 3u);
    return snapshot;
}

static bool recover_newest(const TestMediaContext *media,
                           ActiveConfigurationSnapshot *snapshot)
{
    ActiveConfigurationSnapshot candidate;
    bool found = false;
    size_t slot_index;

    for (slot_index = 0u; slot_index < TR2_CONFIGURATION_STORE_SLOT_COUNT; ++slot_index) {
        const uint8_t *record = &media->durable[slot_index * TR2_CONFIGURATION_STORE_SLOT_SIZE];
        if (tr2_configuration_record_decode(record,
                                            TR2_CONFIGURATION_RECORD_SIZE,
                                            &candidate) == TR2_OK) {
            if (!found || candidate.generation > snapshot->generation) {
                *snapshot = candidate;
                found = true;
            }
        }
    }

    return found;
}

static void init_store(TestMediaContext *media,
                       PersistentMedia *persistent_media,
                       PersistentStorageCore *core,
                       ConfigurationStore *store)
{
    persistent_media->context = media;
    persistent_media->read = test_media_read;
    persistent_media->write = test_media_write;
    persistent_media->commit = test_media_commit;

    assert(persistent_storage_core_init(core, persistent_media) == TR2_OK);
    assert(configuration_store_init(store, core) == TR2_OK);
}

static void assert_snapshot_identity(const ActiveConfigurationSnapshot *expected,
                                     const ActiveConfigurationSnapshot *actual)
{
    assert(expected->generation == actual->generation);
    assert(expected->config_id == actual->config_id);
    assert(expected->revision_counter == actual->revision_counter);
    assert(expected->payload.sampling_frequency_hz == actual->payload.sampling_frequency_hz);
    assert(expected->payload.axes_enable_mask == actual->payload.axes_enable_mask);
    assert(expected->payload.window_size_samples == actual->payload.window_size_samples);
    assert(expected->payload.campaign_context_id == actual->payload.campaign_context_id);
    assert(expected->payload.mission_id == actual->payload.mission_id);
    assert(expected->payload.operating_mode_code == actual->payload.operating_mode_code);
}

static void test_successive_commits_preserve_previous_slot(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ActiveConfigurationSnapshot recovered;
    const ActiveConfigurationSnapshot a = make_snapshot(1u, 10u, 100u, 1000u);
    const ActiveConfigurationSnapshot b = make_snapshot(2u, 20u, 200u, 2000u);

    test_media_init(&media);
    init_store(&media, &persistent_media, &core, &store);

    assert(configuration_store_commit(&store, &a) == TR2_OK);
    simulate_reboot(&media);
    assert(recover_newest(&media, &recovered));
    assert_snapshot_identity(&a, &recovered);

    assert(configuration_store_commit(&store, &b) == TR2_OK);
    simulate_reboot(&media);
    assert(recover_newest(&media, &recovered));
    assert_snapshot_identity(&b, &recovered);

    assert(tr2_configuration_record_decode(&media.durable[0],
                                           TR2_CONFIGURATION_RECORD_SIZE,
                                           &recovered) == TR2_OK);
    assert(recovered.generation == a.generation);
    assert(tr2_configuration_record_decode(&media.durable[TR2_CONFIGURATION_STORE_SLOT_SIZE],
                                           TR2_CONFIGURATION_RECORD_SIZE,
                                           &recovered) == TR2_OK);
    assert(recovered.generation == b.generation);
}

static void test_partial_write_failures_recover_previous_commit(void)
{
    size_t failed_bytes;

    for (failed_bytes = 0u; failed_bytes < TR2_CONFIGURATION_RECORD_SIZE; ++failed_bytes) {
        TestMediaContext media;
        PersistentMedia persistent_media;
        PersistentStorageCore core;
        ConfigurationStore store;
        ActiveConfigurationSnapshot recovered;
        const ActiveConfigurationSnapshot a = make_snapshot(1u, 10u, 100u, 1000u);
        const ActiveConfigurationSnapshot b = make_snapshot(2u, 20u, 200u, 2000u);

        test_media_init(&media);
        init_store(&media, &persistent_media, &core, &store);
        assert(configuration_store_commit(&store, &a) == TR2_OK);

        media.partial_write_count = failed_bytes;
        assert(configuration_store_commit(&store, &b) == TR2_ERROR_STORAGE);
        assert(configuration_store_recovery_required(&store));
        simulate_reboot(&media);

        assert(recover_newest(&media, &recovered));
        assert_snapshot_identity(&a, &recovered);
    }
}

static void test_commit_failure_recovers_previous_commit(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ActiveConfigurationSnapshot recovered;
    const ActiveConfigurationSnapshot a = make_snapshot(1u, 10u, 100u, 1000u);
    const ActiveConfigurationSnapshot b = make_snapshot(2u, 20u, 200u, 2000u);

    test_media_init(&media);
    init_store(&media, &persistent_media, &core, &store);
    assert(configuration_store_commit(&store, &a) == TR2_OK);

    media.fail_commit = true;
    assert(configuration_store_commit(&store, &b) == TR2_ERROR_STORAGE);
    assert(configuration_store_recovery_required(&store));
    simulate_reboot(&media);

    assert(recover_newest(&media, &recovered));
    assert_snapshot_identity(&a, &recovered);
}

static void test_failed_commit_blocks_retry_until_recovery(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ActiveConfigurationSnapshot recovered;
    const ActiveConfigurationSnapshot a = make_snapshot(1u, 10u, 100u, 1000u);
    const ActiveConfigurationSnapshot b = make_snapshot(2u, 20u, 200u, 2000u);
    const ActiveConfigurationSnapshot c = make_snapshot(3u, 30u, 300u, 3000u);
    unsigned writes_before_retry;
    unsigned commits_before_retry;

    test_media_init(&media);
    init_store(&media, &persistent_media, &core, &store);
    assert(configuration_store_commit(&store, &a) == TR2_OK);

    media.fail_commit = true;
    assert(configuration_store_commit(&store, &b) == TR2_ERROR_STORAGE);
    assert(configuration_store_recovery_required(&store));
    media.fail_commit = false;
    writes_before_retry = media.write_calls;
    commits_before_retry = media.commit_calls;

    assert(configuration_store_commit(&store, &c) == TR2_ERROR_INVALID_STATE);
    assert(media.write_calls == writes_before_retry);
    assert(media.commit_calls == commits_before_retry);

    simulate_reboot(&media);
    assert(recover_newest(&media, &recovered));
    assert_snapshot_identity(&a, &recovered);

    assert(configuration_store_init(&store, &core) == TR2_OK);
    assert(!configuration_store_recovery_required(&store));
    assert(configuration_store_commit(&store, &c) == TR2_OK);
    simulate_reboot(&media);
    assert(recover_newest(&media, &recovered));
    assert_snapshot_identity(&c, &recovered);
}

static void test_stale_generation_is_rejected_before_write(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ActiveConfigurationSnapshot recovered;
    const ActiveConfigurationSnapshot a = make_snapshot(5u, 10u, 100u, 1000u);
    const ActiveConfigurationSnapshot stale = make_snapshot(5u, 20u, 200u, 2000u);
    unsigned writes_before;
    unsigned commits_before;

    test_media_init(&media);
    init_store(&media, &persistent_media, &core, &store);
    assert(configuration_store_commit(&store, &a) == TR2_OK);
    writes_before = media.write_calls;
    commits_before = media.commit_calls;

    assert(configuration_store_commit(&store, &stale) == TR2_ERROR_INVALID_ARGUMENT);
    assert(media.write_calls == writes_before);
    assert(media.commit_calls == commits_before);

    simulate_reboot(&media);
    assert(recover_newest(&media, &recovered));
    assert_snapshot_identity(&a, &recovered);
}

static void test_invalid_initialization_and_arguments(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ActiveConfigurationSnapshot snapshot = make_snapshot(1u, 1u, 1u, 1u);

    memset(&store, 0, sizeof(store));
    assert(configuration_store_commit(&store, &snapshot) == TR2_ERROR_INVALID_STATE);
    assert(!configuration_store_recovery_required(&store));
    assert(configuration_store_init(NULL, &core) == TR2_ERROR_INVALID_ARGUMENT);
    assert(configuration_store_init(&store, NULL) == TR2_ERROR_INVALID_ARGUMENT);

    test_media_init(&media);
    persistent_media.context = &media;
    persistent_media.read = test_media_read;
    persistent_media.write = test_media_write;
    persistent_media.commit = test_media_commit;
    assert(persistent_storage_core_init(&core, &persistent_media) == TR2_OK);
    assert(configuration_store_init(&store, &core) == TR2_OK);
    assert(configuration_store_is_initialized(&store));
    assert(!configuration_store_recovery_required(&store));
    assert(configuration_store_commit(&store, NULL) == TR2_ERROR_INVALID_ARGUMENT);
}

int main(void)
{
    test_successive_commits_preserve_previous_slot();
    test_partial_write_failures_recover_previous_commit();
    test_commit_failure_recovers_previous_commit();
    test_failed_commit_blocks_retry_until_recovery();
    test_stale_generation_is_rejected_before_write();
    test_invalid_initialization_and_arguments();
    return 0;
}
