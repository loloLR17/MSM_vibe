#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tr2/persistence/transactional_image_media.h"

typedef struct {
    uint8_t bytes[TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE];
    uint32_t write_count;
    uint32_t fault_write_call;
    size_t fault_after;
    bool fail_after_full_write;
} FaultPhysical;

static Tr2Result physical_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    FaultPhysical *media = (FaultPhysical *)context;
    if ((size_t)offset + size > sizeof(media->bytes)) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(buffer, &media->bytes[offset], size);
    return TR2_OK;
}

static Tr2Result physical_write(void *context, uint32_t offset,
                                const void *buffer, size_t size)
{
    FaultPhysical *media = (FaultPhysical *)context;
    size_t count = size;

    media->write_count++;
    if ((size_t)offset + size > sizeof(media->bytes)) {
        return TR2_ERROR_STORAGE;
    }

    if (media->fault_write_call == media->write_count) {
        /* One-shot fault: model a single interrupted physical write. */
        media->fault_write_call = 0u;
        if (media->fault_after < count) {
            count = media->fault_after;
        }
        if (count != 0u) {
            memcpy(&media->bytes[offset], buffer, count);
        }
        if (count < size || media->fail_after_full_write) {
            return TR2_ERROR_STORAGE;
        }
        return TR2_OK;
    }

    memcpy(&media->bytes[offset], buffer, size);
    return TR2_OK;
}

static void physical_init(FaultPhysical *media)
{
    memset(media->bytes, 0xFF, sizeof(media->bytes));
    media->write_count = 0u;
    media->fault_write_call = 0u;
    media->fault_after = 0u;
    media->fail_after_full_write = false;
}

static void clear_fault(FaultPhysical *media)
{
    media->fault_write_call = 0u;
    media->fault_after = 0u;
    media->fail_after_full_write = false;
}

static void init_media(FaultPhysical *physical,
                       TransactionalImageMedia *media,
                       uint8_t *candidate)
{
    TransactionalImagePhysicalStorage storage = {
        physical,
        physical_read,
        physical_write
    };

    assert(transactional_image_media_init(
               media, &storage, candidate,
               TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE) == TR2_OK);
}

static void recover_and_expect(FaultPhysical *physical,
                               uint64_t generation,
                               uint8_t expected_value)
{
    TransactionalImageMedia reboot;
    TransactionalImageRecoveryResult recovery;
    TransactionalImagePhysicalStorage storage = {
        physical,
        physical_read,
        physical_write
    };
    uint8_t *candidate = malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    PersistentMedia *interface;
    uint8_t value = 0xFFu;

    assert(candidate != NULL);
    clear_fault(physical);
    assert(transactional_image_media_init(
               &reboot, &storage, candidate,
               TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE) == TR2_OK);
    assert(transactional_image_media_recover(&reboot, &recovery) == TR2_OK);
    assert(recovery.status == TRANSACTIONAL_IMAGE_RECOVERY_VALID);
    assert(recovery.generation == generation);

    interface = transactional_image_media_interface(&reboot);
    assert(interface != NULL);
    assert(interface->read(interface->context, 1234u, &value, 1u) == TR2_OK);
    assert(value == expected_value);
    free(candidate);
}

static void prepare_generation_one(FaultPhysical *physical,
                                   TransactionalImageMedia *media,
                                   uint8_t *candidate)
{
    physical_init(physical);
    init_media(physical, media, candidate);
    assert(transactional_image_media_format_empty(media) == TR2_OK);
}

static void run_torn_commit_case(uint32_t relative_write_call,
                                 size_t cut,
                                 uint8_t new_value)
{
    fprintf(stderr, "H3d2B case write=%u cut=%zu value=0x%02X\n",
            (unsigned)relative_write_call, cut, (unsigned)new_value);
    fflush(stderr);
    FaultPhysical physical;
    TransactionalImageMedia media;
    uint8_t *candidate = malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    PersistentMedia *interface;
    uint32_t baseline_writes;

    assert(candidate != NULL);
    prepare_generation_one(&physical, &media, candidate);
    interface = transactional_image_media_interface(&media);
    assert(interface != NULL);
    assert(interface->write(interface->context, 1234u, &new_value, 1u) == TR2_OK);

    baseline_writes = physical.write_count;
    physical.fault_write_call = baseline_writes + relative_write_call;
    physical.fault_after = cut;

    assert(interface->commit(interface->context) == TR2_ERROR_STORAGE);
    assert(media.recovery_required);
    recover_and_expect(&physical, 1u, 0u);
    free(candidate);
}

static void test_every_image_header_cut_keeps_old_generation(void)
{
    size_t cut;
    for (cut = 0u; cut < TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE; ++cut) {
        run_torn_commit_case(1u, cut, 0x31u);
    }
}

static void test_representative_payload_cuts_keep_old_generation(void)
{
    const size_t cuts[] = {
        0u,
        1u,
        63u,
        64u,
        255u,
        256u,
        TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE / 2u,
        TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE - 2u,
        TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE - 1u
    };
    size_t index;

    for (index = 0u; index < sizeof(cuts) / sizeof(cuts[0]); ++index) {
        run_torn_commit_case(2u, cuts[index], 0x42u);
    }
}

static void test_every_superblock_cut_keeps_old_generation(void)
{
    size_t cut;
    for (cut = 0u; cut < TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE; ++cut) {
        run_torn_commit_case(3u, cut, 0x53u);
    }
}

static void test_full_publication_then_reported_failure_recovers_new_generation(void)
{
    FaultPhysical physical;
    TransactionalImageMedia media;
    uint8_t *candidate = malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    PersistentMedia *interface;
    uint8_t value = 0x64u;
    uint32_t baseline_writes;

    assert(candidate != NULL);
    prepare_generation_one(&physical, &media, candidate);
    interface = transactional_image_media_interface(&media);
    assert(interface->write(interface->context, 1234u, &value, 1u) == TR2_OK);

    baseline_writes = physical.write_count;
    physical.fault_write_call = baseline_writes + 3u;
    physical.fault_after = TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE;
    physical.fail_after_full_write = true;

    assert(interface->commit(interface->context) == TR2_ERROR_STORAGE);
    assert(media.recovery_required);

    /* The caller sees failure, but publication may already be durable.
       Recovery, never automatic retry, resolves the ambiguity. */
    recover_and_expect(&physical, 2u, value);
    free(candidate);
}

static void test_successive_commits_alternate_and_recover_latest(void)
{
    FaultPhysical physical;
    TransactionalImageMedia media;
    TransactionalImageRecoveryResult recovery;
    TransactionalImagePhysicalStorage storage = {
        &physical,
        physical_read,
        physical_write
    };
    uint8_t *candidate = malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    uint8_t *reboot_candidate = malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    PersistentMedia *interface;
    uint8_t value;

    assert(candidate != NULL && reboot_candidate != NULL);
    prepare_generation_one(&physical, &media, candidate);
    interface = transactional_image_media_interface(&media);

    value = 0x71u;
    assert(interface->write(interface->context, 1234u, &value, 1u) == TR2_OK);
    assert(interface->commit(interface->context) == TR2_OK);
    assert(media.generation == 2u);
    assert(media.active_image == 1u);

    value = 0x72u;
    assert(interface->write(interface->context, 1234u, &value, 1u) == TR2_OK);
    assert(interface->commit(interface->context) == TR2_OK);
    assert(media.generation == 3u);
    assert(media.active_image == 0u);

    assert(transactional_image_media_init(
               &media, &storage, reboot_candidate,
               TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE) == TR2_OK);
    assert(transactional_image_media_recover(&media, &recovery) == TR2_OK);
    assert(recovery.status == TRANSACTIONAL_IMAGE_RECOVERY_VALID);
    assert(recovery.generation == 3u);
    interface = transactional_image_media_interface(&media);
    value = 0u;
    assert(interface->read(interface->context, 1234u, &value, 1u) == TR2_OK);
    assert(value == 0x72u);

    free(candidate);
    free(reboot_candidate);
}

int main(void)
{
    test_every_image_header_cut_keeps_old_generation();
    test_representative_payload_cuts_keep_old_generation();
    test_every_superblock_cut_keeps_old_generation();
    test_full_publication_then_reported_failure_recovers_new_generation();
    test_successive_commits_alternate_and_recover_latest();
    return 0;
}
