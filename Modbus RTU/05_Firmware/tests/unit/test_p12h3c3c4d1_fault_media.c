#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/persistent_storage_core.h"

#define TEST_MEDIA_SIZE 256u

typedef enum {
    COMMIT_FAILURE_KEEP_WORKING = 0,
    COMMIT_FAILURE_ROLLBACK_WORKING
} CommitFailureMode;

typedef struct {
    uint8_t durable[TEST_MEDIA_SIZE];
    uint8_t working[TEST_MEDIA_SIZE];
    size_t tear_after;
    bool tear_enabled;
    bool fail_commit;
    CommitFailureMode commit_failure_mode;
    uint32_t write_count;
    uint32_t commit_count;
} FaultMedia;

static void fault_media_init(FaultMedia *media)
{
    memset(media->durable, 0xFF, sizeof(media->durable));
    memcpy(media->working, media->durable, sizeof(media->working));
    media->tear_after = 0u;
    media->tear_enabled = false;
    media->fail_commit = false;
    media->commit_failure_mode = COMMIT_FAILURE_KEEP_WORKING;
    media->write_count = 0u;
    media->commit_count = 0u;
}

static void fault_media_power_cycle(FaultMedia *media)
{
    memcpy(media->working, media->durable, sizeof(media->working));
    media->tear_enabled = false;
    media->fail_commit = false;
}

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    FaultMedia *media = (FaultMedia *)context;
    if ((size_t)offset + size > sizeof(media->working)) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(buffer, &media->working[offset], size);
    return TR2_OK;
}

static Tr2Result media_write(void *context, uint32_t offset,
                             const void *buffer, size_t size)
{
    FaultMedia *media = (FaultMedia *)context;
    size_t bytes_to_write = size;

    media->write_count++;
    if ((size_t)offset + size > sizeof(media->working)) {
        return TR2_ERROR_STORAGE;
    }

    if (media->tear_enabled && media->tear_after < size) {
        bytes_to_write = media->tear_after;
    }
    if (bytes_to_write != 0u) {
        memcpy(&media->working[offset], buffer, bytes_to_write);
    }
    if (media->tear_enabled && media->tear_after < size) {
        return TR2_ERROR_STORAGE;
    }
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    FaultMedia *media = (FaultMedia *)context;
    media->commit_count++;

    if (media->fail_commit) {
        if (media->commit_failure_mode == COMMIT_FAILURE_ROLLBACK_WORKING) {
            memcpy(media->working, media->durable, sizeof(media->working));
        } else {
            /* Ambiguous failure: bytes are readable now but are not durable. */
        }
        return TR2_ERROR_STORAGE;
    }

    memcpy(media->durable, media->working, sizeof(media->durable));
    return TR2_OK;
}

static void init_core(FaultMedia *media, PersistentMedia *pm,
                      PersistentStorageCore *core)
{
    pm->context = media;
    pm->read = media_read;
    pm->write = media_write;
    pm->commit = media_commit;
    assert(persistent_storage_core_init(core, pm) == TR2_OK);
}

static void test_partial_write_zero_one_middle_and_full(void)
{
    const uint8_t source[8] = {1u,2u,3u,4u,5u,6u,7u,8u};
    const size_t cuts[] = {0u, 1u, 4u, 8u};
    size_t index;

    for (index = 0u; index < sizeof(cuts) / sizeof(cuts[0]); ++index) {
        FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
        uint8_t readback[8];

        fault_media_init(&media);
        init_core(&media, &pm, &core);
        media.tear_enabled = cuts[index] < sizeof(source);
        media.tear_after = cuts[index];

        if (cuts[index] < sizeof(source)) {
            assert(persistent_storage_core_write(&core, 10u, source, sizeof(source)) ==
                   TR2_ERROR_STORAGE);
        } else {
            assert(persistent_storage_core_write(&core, 10u, source, sizeof(source)) ==
                   TR2_OK);
        }

        assert(persistent_storage_core_read(&core, 10u, readback, sizeof(readback)) == TR2_OK);
        if (cuts[index] != 0u) {
            assert(memcmp(readback, source, cuts[index]) == 0);
        }
        if (cuts[index] < sizeof(source)) {
            size_t tail;
            for (tail = cuts[index]; tail < sizeof(source); ++tail) {
                assert(readback[tail] == 0xFFu);
            }
        }
    }
}

static void test_power_cycle_discards_uncommitted_partial_write(void)
{
    FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
    const uint8_t source[8] = {10u,11u,12u,13u,14u,15u,16u,17u};
    uint8_t readback[8];
    size_t index;

    fault_media_init(&media);
    init_core(&media, &pm, &core);
    media.tear_enabled = true;
    media.tear_after = 4u;
    assert(persistent_storage_core_write(&core, 20u, source, sizeof(source)) ==
           TR2_ERROR_STORAGE);

    fault_media_power_cycle(&media);
    assert(persistent_storage_core_read(&core, 20u, readback, sizeof(readback)) == TR2_OK);
    for (index = 0u; index < sizeof(readback); ++index) {
        assert(readback[index] == 0xFFu);
    }
}

static void test_successful_commit_survives_power_cycle(void)
{
    FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
    const uint8_t source[8] = {21u,22u,23u,24u,25u,26u,27u,28u};
    uint8_t readback[8];

    fault_media_init(&media);
    init_core(&media, &pm, &core);
    assert(persistent_storage_core_write(&core, 30u, source, sizeof(source)) == TR2_OK);
    assert(persistent_storage_core_commit(&core) == TR2_OK);

    fault_media_power_cycle(&media);
    assert(persistent_storage_core_read(&core, 30u, readback, sizeof(readback)) == TR2_OK);
    assert(memcmp(readback, source, sizeof(source)) == 0);
}

static void test_commit_failure_can_rollback_working_image(void)
{
    FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
    const uint8_t source[4] = {31u,32u,33u,34u};
    uint8_t readback[4];
    size_t index;

    fault_media_init(&media);
    init_core(&media, &pm, &core);
    assert(persistent_storage_core_write(&core, 40u, source, sizeof(source)) == TR2_OK);
    media.fail_commit = true;
    media.commit_failure_mode = COMMIT_FAILURE_ROLLBACK_WORKING;
    assert(persistent_storage_core_commit(&core) == TR2_ERROR_STORAGE);
    assert(persistent_storage_core_read(&core, 40u, readback, sizeof(readback)) == TR2_OK);
    for (index = 0u; index < sizeof(readback); ++index) {
        assert(readback[index] == 0xFFu);
    }
}

static void test_commit_failure_can_leave_bytes_readable_but_not_durable(void)
{
    FaultMedia media; PersistentMedia pm; PersistentStorageCore core;
    const uint8_t source[4] = {41u,42u,43u,44u};
    uint8_t readback[4];
    size_t index;

    fault_media_init(&media);
    init_core(&media, &pm, &core);
    assert(persistent_storage_core_write(&core, 50u, source, sizeof(source)) == TR2_OK);
    media.fail_commit = true;
    media.commit_failure_mode = COMMIT_FAILURE_KEEP_WORKING;
    assert(persistent_storage_core_commit(&core) == TR2_ERROR_STORAGE);

    assert(persistent_storage_core_read(&core, 50u, readback, sizeof(readback)) == TR2_OK);
    assert(memcmp(readback, source, sizeof(source)) == 0);

    fault_media_power_cycle(&media);
    assert(persistent_storage_core_read(&core, 50u, readback, sizeof(readback)) == TR2_OK);
    for (index = 0u; index < sizeof(readback); ++index) {
        assert(readback[index] == 0xFFu);
    }
}

int main(void)
{
    test_partial_write_zero_one_middle_and_full();
    test_power_cycle_discards_uncommitted_partial_write();
    test_successful_commit_survives_power_cycle();
    test_commit_failure_can_rollback_working_image();
    test_commit_failure_can_leave_bytes_readable_but_not_durable();
    return 0;
}
