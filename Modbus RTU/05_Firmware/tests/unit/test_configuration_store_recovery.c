#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/configuration_store.h"

#define RECORD_CRC_OFFSET (TR2_CONFIGURATION_RECORD_SIZE - 4u)

typedef struct {
    uint8_t durable[TR2_CONFIGURATION_STORE_STORAGE_SIZE];
    uint8_t staged[TR2_CONFIGURATION_STORE_STORAGE_SIZE];
    bool fail_read;
    bool fail_commit;
} RecoveryMediaContext;

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    RecoveryMediaContext *media = (RecoveryMediaContext *)context;

    if (media->fail_read) {
        return TR2_ERROR_STORAGE;
    }
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
    RecoveryMediaContext *media = (RecoveryMediaContext *)context;

    if ((size_t)offset + size > sizeof(media->staged)) {
        return TR2_ERROR_STORAGE;
    }

    memcpy(&media->staged[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    RecoveryMediaContext *media = (RecoveryMediaContext *)context;

    if (media->fail_commit) {
        return TR2_ERROR_STORAGE;
    }

    memcpy(media->durable, media->staged, sizeof(media->durable));
    return TR2_OK;
}

static void media_init(RecoveryMediaContext *media, uint8_t empty_value)
{
    memset(media, 0, sizeof(*media));
    memset(media->durable, empty_value, sizeof(media->durable));
    memcpy(media->staged, media->durable, sizeof(media->staged));
}

static void init_store(RecoveryMediaContext *media,
                       PersistentMedia *persistent_media,
                       PersistentStorageCore *core,
                       ConfigurationStore *store)
{
    persistent_media->context = media;
    persistent_media->read = media_read;
    persistent_media->write = media_write;
    persistent_media->commit = media_commit;

    assert(persistent_storage_core_init(core, persistent_media) == TR2_OK);
    assert(configuration_store_init(store, core) == TR2_OK);
}

static ConfigurationValidationEnvironment valid_environment(void)
{
    ConfigurationValidationEnvironment environment;

    environment.storage_capacity_known = true;
    environment.usable_storage_capacity_mb = 4096u;
    return environment;
}

static ActiveConfigurationSnapshot make_valid_snapshot(uint32_t generation,
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

static void persist_snapshot(RecoveryMediaContext *media,
                             size_t slot_index,
                             const ActiveConfigurationSnapshot *snapshot)
{
    uint8_t record[TR2_CONFIGURATION_RECORD_SIZE];
    const size_t offset = slot_index * TR2_CONFIGURATION_STORE_SLOT_SIZE;

    assert(tr2_configuration_record_encode(snapshot, record, sizeof(record)) == TR2_OK);
    memcpy(&media->durable[offset], record, sizeof(record));
    memcpy(&media->staged[offset], record, sizeof(record));
}

static uint32_t crc32_bytes(const uint8_t *data, size_t size)
{
    uint32_t crc = UINT32_C(0xFFFFFFFF);
    size_t byte_index;

    for (byte_index = 0u; byte_index < size; ++byte_index) {
        unsigned bit_index;

        crc ^= data[byte_index];
        for (bit_index = 0u; bit_index < 8u; ++bit_index) {
            if ((crc & 1u) != 0u) {
                crc = (crc >> 1u) ^ UINT32_C(0xEDB88320);
            } else {
                crc >>= 1u;
            }
        }
    }

    return crc ^ UINT32_C(0xFFFFFFFF);
}

static void make_slot_unsupported(RecoveryMediaContext *media, size_t slot_index)
{
    uint8_t *record = &media->durable[slot_index * TR2_CONFIGURATION_STORE_SLOT_SIZE];
    uint32_t crc;

    record[4] = 0u;
    record[5] = 2u;
    crc = crc32_bytes(record, RECORD_CRC_OFFSET);
    record[RECORD_CRC_OFFSET] = (uint8_t)(crc >> 24u);
    record[RECORD_CRC_OFFSET + 1u] = (uint8_t)(crc >> 16u);
    record[RECORD_CRC_OFFSET + 2u] = (uint8_t)(crc >> 8u);
    record[RECORD_CRC_OFFSET + 3u] = (uint8_t)crc;
    memcpy(&media->staged[slot_index * TR2_CONFIGURATION_STORE_SLOT_SIZE],
           record,
           TR2_CONFIGURATION_RECORD_SIZE);
}

static void assert_identity(const ActiveConfigurationSnapshot *expected,
                            const ActiveConfigurationSnapshot *actual)
{
    assert(actual->generation == expected->generation);
    assert(actual->config_id == expected->config_id);
    assert(actual->revision_counter == expected->revision_counter);
    assert(memcmp(&actual->payload, &expected->payload, sizeof(actual->payload)) == 0);
}

static void test_empty_erased_patterns(void)
{
    uint8_t empty_values[] = {0x00u, 0xFFu};
    size_t index;

    for (index = 0u; index < sizeof(empty_values); ++index) {
        RecoveryMediaContext media;
        PersistentMedia persistent_media;
        PersistentStorageCore core;
        ConfigurationStore store;
        ConfigurationRecoveryResult recovery;
        ConfigurationValidationEnvironment environment = valid_environment();

        media_init(&media, empty_values[index]);
        init_store(&media, &persistent_media, &core, &store);

        assert(configuration_store_recover(&store, &environment, &recovery) == TR2_OK);
        assert(recovery.status == CONFIGURATION_RECOVERY_EMPTY);
        assert(!recovery.has_snapshot);
    }
}

static void test_single_valid_snapshot_is_recovered(void)
{
    RecoveryMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationRecoveryResult recovery;
    ConfigurationValidationEnvironment environment = valid_environment();
    const ActiveConfigurationSnapshot expected = make_valid_snapshot(7u, 42u, 11u);

    media_init(&media, 0xFFu);
    persist_snapshot(&media, 1u, &expected);
    init_store(&media, &persistent_media, &core, &store);

    assert(configuration_store_recover(&store, &environment, &recovery) == TR2_OK);
    assert(recovery.status == CONFIGURATION_RECOVERY_VALID);
    assert(recovery.has_snapshot);
    assert_identity(&expected, &recovery.snapshot);
}

static void test_newest_business_valid_generation_wins(void)
{
    RecoveryMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationRecoveryResult recovery;
    ConfigurationValidationEnvironment environment = valid_environment();
    const ActiveConfigurationSnapshot older = make_valid_snapshot(7u, 42u, 11u);
    const ActiveConfigurationSnapshot newer = make_valid_snapshot(8u, 43u, 12u);

    media_init(&media, 0xFFu);
    persist_snapshot(&media, 0u, &older);
    persist_snapshot(&media, 1u, &newer);
    init_store(&media, &persistent_media, &core, &store);

    assert(configuration_store_recover(&store, &environment, &recovery) == TR2_OK);
    assert(recovery.status == CONFIGURATION_RECOVERY_VALID);
    assert(recovery.has_snapshot);
    assert_identity(&newer, &recovery.snapshot);
}

static void test_newer_corrupted_candidate_is_ignored(void)
{
    RecoveryMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationRecoveryResult recovery;
    ConfigurationValidationEnvironment environment = valid_environment();
    const ActiveConfigurationSnapshot older = make_valid_snapshot(7u, 42u, 11u);
    const ActiveConfigurationSnapshot newer = make_valid_snapshot(8u, 43u, 12u);

    media_init(&media, 0xFFu);
    persist_snapshot(&media, 0u, &older);
    persist_snapshot(&media, 1u, &newer);
    media.durable[TR2_CONFIGURATION_STORE_SLOT_SIZE + 20u] ^= 0x01u;
    init_store(&media, &persistent_media, &core, &store);

    assert(configuration_store_recover(&store, &environment, &recovery) == TR2_OK);
    assert(recovery.status == CONFIGURATION_RECOVERY_VALID);
    assert(recovery.has_snapshot);
    assert_identity(&older, &recovery.snapshot);
}

static void test_newer_business_invalid_candidate_is_ignored(void)
{
    RecoveryMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationRecoveryResult recovery;
    ConfigurationValidationEnvironment environment = valid_environment();
    const ActiveConfigurationSnapshot older = make_valid_snapshot(7u, 42u, 11u);
    ActiveConfigurationSnapshot newer = make_valid_snapshot(8u, 43u, 12u);

    newer.payload.sampling_frequency_hz = 1000u;
    media_init(&media, 0xFFu);
    persist_snapshot(&media, 0u, &older);
    persist_snapshot(&media, 1u, &newer);
    init_store(&media, &persistent_media, &core, &store);

    assert(configuration_store_recover(&store, &environment, &recovery) == TR2_OK);
    assert(recovery.status == CONFIGURATION_RECOVERY_VALID);
    assert(recovery.has_snapshot);
    assert_identity(&older, &recovery.snapshot);
}

static void test_corrupted_without_valid_candidate(void)
{
    RecoveryMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationRecoveryResult recovery;
    ConfigurationValidationEnvironment environment = valid_environment();

    media_init(&media, 0xFFu);
    media.durable[0] = 0x12u;
    init_store(&media, &persistent_media, &core, &store);

    assert(configuration_store_recover(&store, &environment, &recovery) == TR2_OK);
    assert(recovery.status == CONFIGURATION_RECOVERY_CORRUPTED);
    assert(!recovery.has_snapshot);
}

static void test_business_invalid_without_valid_candidate_is_corrupted(void)
{
    RecoveryMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationRecoveryResult recovery;
    ConfigurationValidationEnvironment environment = valid_environment();
    ActiveConfigurationSnapshot invalid = make_valid_snapshot(3u, 9u, 4u);

    invalid.config_id = 0u;
    media_init(&media, 0xFFu);
    persist_snapshot(&media, 0u, &invalid);
    init_store(&media, &persistent_media, &core, &store);

    assert(configuration_store_recover(&store, &environment, &recovery) == TR2_OK);
    assert(recovery.status == CONFIGURATION_RECOVERY_CORRUPTED);
    assert(!recovery.has_snapshot);
}

static void test_unsupported_without_valid_candidate(void)
{
    RecoveryMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationRecoveryResult recovery;
    ConfigurationValidationEnvironment environment = valid_environment();
    const ActiveConfigurationSnapshot snapshot = make_valid_snapshot(3u, 9u, 4u);

    media_init(&media, 0xFFu);
    persist_snapshot(&media, 0u, &snapshot);
    make_slot_unsupported(&media, 0u);
    init_store(&media, &persistent_media, &core, &store);

    assert(configuration_store_recover(&store, &environment, &recovery) == TR2_OK);
    assert(recovery.status == CONFIGURATION_RECOVERY_UNSUPPORTED);
    assert(!recovery.has_snapshot);
}

static void test_unavailable_read_has_priority_without_valid_candidate(void)
{
    RecoveryMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationRecoveryResult recovery;
    ConfigurationValidationEnvironment environment = valid_environment();

    media_init(&media, 0xFFu);
    init_store(&media, &persistent_media, &core, &store);
    media.fail_read = true;

    assert(configuration_store_recover(&store, &environment, &recovery) == TR2_OK);
    assert(recovery.status == CONFIGURATION_RECOVERY_UNAVAILABLE);
    assert(!recovery.has_snapshot);
}

static void test_uncharacterized_environment_is_unavailable(void)
{
    RecoveryMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationRecoveryResult recovery;
    ConfigurationValidationEnvironment environment = {false, 0u};
    const ActiveConfigurationSnapshot snapshot = make_valid_snapshot(3u, 9u, 4u);

    media_init(&media, 0xFFu);
    persist_snapshot(&media, 0u, &snapshot);
    init_store(&media, &persistent_media, &core, &store);

    assert(configuration_store_recover(&store, &environment, &recovery) == TR2_OK);
    assert(recovery.status == CONFIGURATION_RECOVERY_UNAVAILABLE);
    assert(!recovery.has_snapshot);
}

static void test_valid_candidate_wins_over_other_slot_failure_classes(void)
{
    RecoveryMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationRecoveryResult recovery;
    ConfigurationValidationEnvironment environment = valid_environment();
    const ActiveConfigurationSnapshot valid = make_valid_snapshot(3u, 9u, 4u);
    const ActiveConfigurationSnapshot other = make_valid_snapshot(4u, 10u, 5u);

    media_init(&media, 0xFFu);
    persist_snapshot(&media, 0u, &valid);
    persist_snapshot(&media, 1u, &other);
    make_slot_unsupported(&media, 1u);
    init_store(&media, &persistent_media, &core, &store);

    assert(configuration_store_recover(&store, &environment, &recovery) == TR2_OK);
    assert(recovery.status == CONFIGURATION_RECOVERY_VALID);
    assert(recovery.has_snapshot);
    assert_identity(&valid, &recovery.snapshot);
}

static void test_recovery_required_store_cannot_recover_staged_data(void)
{
    RecoveryMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationRecoveryResult recovery;
    ConfigurationValidationEnvironment environment = valid_environment();
    const ActiveConfigurationSnapshot snapshot = make_valid_snapshot(1u, 1u, 1u);

    media_init(&media, 0xFFu);
    init_store(&media, &persistent_media, &core, &store);
    media.fail_commit = true;

    assert(configuration_store_commit(&store, &snapshot) == TR2_ERROR_STORAGE);
    assert(configuration_store_recovery_required(&store));
    assert(configuration_store_recover(&store, &environment, &recovery) ==
           TR2_ERROR_INVALID_STATE);
}

static void test_invalid_arguments(void)
{
    RecoveryMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    ConfigurationStore store;
    ConfigurationRecoveryResult recovery;
    ConfigurationValidationEnvironment environment = valid_environment();

    media_init(&media, 0xFFu);
    init_store(&media, &persistent_media, &core, &store);

    assert(configuration_store_recover(NULL, &environment, &recovery) ==
           TR2_ERROR_INVALID_STATE);
    assert(configuration_store_recover(&store, NULL, &recovery) ==
           TR2_ERROR_INVALID_ARGUMENT);
    assert(configuration_store_recover(&store, &environment, NULL) ==
           TR2_ERROR_INVALID_ARGUMENT);
}

int main(void)
{
    test_empty_erased_patterns();
    test_single_valid_snapshot_is_recovered();
    test_newest_business_valid_generation_wins();
    test_newer_corrupted_candidate_is_ignored();
    test_newer_business_invalid_candidate_is_ignored();
    test_corrupted_without_valid_candidate();
    test_business_invalid_without_valid_candidate_is_corrupted();
    test_unsupported_without_valid_candidate();
    test_unavailable_read_has_priority_without_valid_candidate();
    test_uncharacterized_environment_is_unavailable();
    test_valid_candidate_wins_over_other_slot_failure_classes();
    test_recovery_required_store_cannot_recover_staged_data();
    test_invalid_arguments();
    return 0;
}
