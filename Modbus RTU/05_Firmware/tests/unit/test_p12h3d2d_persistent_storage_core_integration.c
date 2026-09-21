#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tr2/persistence/persistent_storage_core.h"
#include "tr2/persistence/transactional_image_media.h"

typedef struct {
    uint8_t *bytes;
} RamPhysical;

static Tr2Result physical_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    RamPhysical *p = (RamPhysical *)context;
    if ((size_t)offset + size > TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(buffer, p->bytes + offset, size);
    return TR2_OK;
}

static Tr2Result physical_write(void *context, uint32_t offset,
                                const void *buffer, size_t size)
{
    RamPhysical *p = (RamPhysical *)context;
    if ((size_t)offset + size > TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(p->bytes + offset, buffer, size);
    return TR2_OK;
}

static void init_backend(RamPhysical *physical,
                         TransactionalImageMedia *media,
                         uint8_t *candidate)
{
    TransactionalImagePhysicalStorage storage = {
        physical, physical_read, physical_write
    };
    assert(transactional_image_media_init(
               media, &storage, candidate,
               TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE) == TR2_OK);
}

static void test_core_preserves_candidate_commit_boundary(void)
{
    RamPhysical physical;
    TransactionalImageMedia media;
    PersistentStorageCore core;
    uint8_t *candidate;
    uint8_t before = 0u;
    uint8_t value = 0xA5u;

    physical.bytes = malloc(TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);
    candidate = malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    assert(physical.bytes != NULL && candidate != NULL);
    memset(physical.bytes, 0xFF, TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);

    init_backend(&physical, &media, candidate);
    assert(transactional_image_media_format_empty(&media) == TR2_OK);
    assert(persistent_storage_core_init(
               &core, transactional_image_media_interface(&media)) == TR2_OK);

    assert(persistent_storage_core_read(&core, 4096u, &before, 1u) == TR2_OK);
    assert(before == 0u);

    assert(persistent_storage_core_write(&core, 4096u, &value, 1u) == TR2_OK);
    before = 0xFFu;
    assert(persistent_storage_core_read(&core, 4096u, &before, 1u) == TR2_OK);
    assert(before == 0u);

    assert(persistent_storage_core_commit(&core) == TR2_OK);
    before = 0u;
    assert(persistent_storage_core_read(&core, 4096u, &before, 1u) == TR2_OK);
    assert(before == value);

    free(candidate);
    free(physical.bytes);
}

static void test_core_reboot_discards_uncommitted_candidate(void)
{
    RamPhysical physical;
    TransactionalImageMedia first;
    TransactionalImageMedia reboot;
    TransactionalImageRecoveryResult recovery;
    PersistentStorageCore core;
    PersistentStorageCore reboot_core;
    uint8_t *candidate;
    uint8_t *reboot_candidate;
    uint8_t committed = 0x11u;
    uint8_t uncommitted = 0x22u;
    uint8_t observed = 0u;

    physical.bytes = malloc(TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);
    candidate = malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    reboot_candidate = malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    assert(physical.bytes != NULL && candidate != NULL && reboot_candidate != NULL);
    memset(physical.bytes, 0xFF, TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);

    init_backend(&physical, &first, candidate);
    assert(transactional_image_media_format_empty(&first) == TR2_OK);
    assert(persistent_storage_core_init(
               &core, transactional_image_media_interface(&first)) == TR2_OK);

    assert(persistent_storage_core_write(&core, 7000u, &committed, 1u) == TR2_OK);
    assert(persistent_storage_core_commit(&core) == TR2_OK);
    assert(persistent_storage_core_write(&core, 7000u, &uncommitted, 1u) == TR2_OK);

    init_backend(&physical, &reboot, reboot_candidate);
    assert(transactional_image_media_recover(&reboot, &recovery) == TR2_OK);
    assert(recovery.status == TRANSACTIONAL_IMAGE_RECOVERY_VALID);
    assert(recovery.generation == 2u);
    assert(persistent_storage_core_init(
               &reboot_core, transactional_image_media_interface(&reboot)) == TR2_OK);
    assert(persistent_storage_core_read(
               &reboot_core, 7000u, &observed, 1u) == TR2_OK);
    assert(observed == committed);

    free(reboot_candidate);
    free(candidate);
    free(physical.bytes);
}

static void test_core_zero_length_operations_remain_noops(void)
{
    RamPhysical physical;
    TransactionalImageMedia media;
    PersistentStorageCore core;
    uint8_t *candidate;

    physical.bytes = malloc(TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);
    candidate = malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    assert(physical.bytes != NULL && candidate != NULL);
    memset(physical.bytes, 0xFF, TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);

    init_backend(&physical, &media, candidate);
    assert(transactional_image_media_format_empty(&media) == TR2_OK);
    assert(persistent_storage_core_init(
               &core, transactional_image_media_interface(&media)) == TR2_OK);

    assert(persistent_storage_core_read(&core, 0u, NULL, 0u) == TR2_OK);
    assert(persistent_storage_core_write(&core, 0u, NULL, 0u) == TR2_OK);

    free(candidate);
    free(physical.bytes);
}

int main(void)
{
    test_core_preserves_candidate_commit_boundary();
    test_core_reboot_discards_uncommitted_candidate();
    test_core_zero_length_operations_remain_noops();
    return 0;
}
