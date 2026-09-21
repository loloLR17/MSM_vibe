#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tr2/persistence/transactional_image_media.h"

typedef struct {
    uint8_t bytes[TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE];
    bool fail_read;
    uint32_t fail_read_base;
    uint32_t fail_read_end;
} TestPhysical;

static Tr2Result rd(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestPhysical *m = (TestPhysical *)context;
    uint32_t end = offset + (uint32_t)size;
    if ((size_t)offset + size > sizeof(m->bytes)) return TR2_ERROR_STORAGE;
    if (m->fail_read && offset < m->fail_read_end && end > m->fail_read_base) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(buffer, &m->bytes[offset], size);
    return TR2_OK;
}

static Tr2Result wr(void *context, uint32_t offset, const void *buffer, size_t size)
{
    TestPhysical *m = (TestPhysical *)context;
    if ((size_t)offset + size > sizeof(m->bytes)) return TR2_ERROR_STORAGE;
    memcpy(&m->bytes[offset], buffer, size);
    return TR2_OK;
}

static void init_physical(TestPhysical *m)
{
    memset(m->bytes, 0xFF, sizeof(m->bytes));
    m->fail_read = false;
    m->fail_read_base = 0u;
    m->fail_read_end = 0u;
}

static void init_media(TestPhysical *p, TransactionalImageMedia *m, uint8_t *candidate)
{
    TransactionalImagePhysicalStorage storage = { p, rd, wr };
    TransactionalImageGeometry geometry =
        transactional_image_geometry_qualification_profile();
    assert(transactional_image_media_init(
               m, &storage, &geometry, candidate,
               TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE) == TR2_OK);
}

static void format(TestPhysical *p)
{
    TransactionalImageMedia m;
    uint8_t *candidate = malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    assert(candidate != NULL);
    init_media(p, &m, candidate);
    assert(transactional_image_media_format_empty(&m) == TR2_OK);
    free(candidate);
}

static TransactionalImageRecoveryStatus recover(TestPhysical *p,
                                                uint64_t *generation)
{
    TransactionalImageMedia m;
    TransactionalImageRecoveryResult result;
    uint8_t *candidate = malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    assert(candidate != NULL);
    init_media(p, &m, candidate);
    assert(transactional_image_media_recover(&m, &result) == TR2_OK);
    if (generation != NULL) *generation = result.generation;
    free(candidate);
    return result.status;
}

static void test_factory_erased_media_is_empty(void)
{
    TestPhysical p;
    init_physical(&p);
    assert(recover(&p, NULL) == TRANSACTIONAL_IMAGE_RECOVERY_EMPTY);
}

static void test_random_unformatted_media_is_corrupted_not_empty(void)
{
    TestPhysical p;
    init_physical(&p);
    p.bytes[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE] = 0x12u;
    assert(recover(&p, NULL) == TRANSACTIONAL_IMAGE_RECOVERY_CORRUPTED);
}

static void test_valid_authority_masks_corrupt_peer(void)
{
    TestPhysical p;
    uint64_t generation = 0u;
    init_physical(&p);
    format(&p);
    p.bytes[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE] = 0x12u;
    assert(recover(&p, &generation) == TRANSACTIONAL_IMAGE_RECOVERY_VALID);
    assert(generation == 1u);
}

static void test_valid_authority_masks_unsupported_peer(void)
{
    TestPhysical p;
    uint64_t generation = 0u;
    init_physical(&p);
    format(&p);
    memcpy(&p.bytes[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE], "TR2M", 4u);
    p.bytes[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE + 4u] = 0xFFu;
    p.bytes[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE + 5u] = 0xFFu;
    assert(recover(&p, &generation) == TRANSACTIONAL_IMAGE_RECOVERY_VALID);
    assert(generation == 1u);
}

static void test_no_valid_authority_with_unsupported_record_is_unsupported(void)
{
    TestPhysical p;
    init_physical(&p);
    memcpy(&p.bytes[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE], "TR2M", 4u);
    assert(recover(&p, NULL) == TRANSACTIONAL_IMAGE_RECOVERY_UNSUPPORTED);
}

static void test_read_unavailable_is_not_masked_by_valid_peer(void)
{
    TestPhysical p;
    init_physical(&p);
    format(&p);
    p.fail_read = true;
    p.fail_read_base = TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE;
    p.fail_read_end = TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE +
                      TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE;
    assert(recover(&p, NULL) == TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE);
}

static void test_referenced_image_unavailable_is_not_collapsed_to_corrupted(void)
{
    TestPhysical p;
    init_physical(&p);
    format(&p);
    p.fail_read = true;
    p.fail_read_base = TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE;
    p.fail_read_end = TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE +
                      TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE;
    assert(recover(&p, NULL) == TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE);
}

static void test_referenced_image_unsupported_is_reported_unsupported(void)
{
    TestPhysical p;
    init_physical(&p);
    format(&p);

    /*
     * Preserve a structurally recognizable image header while making its
     * physical-format version unsupported.  The superblock still references
     * this image; recovery must preserve UNSUPPORTED rather than collapse it
     * into generic corruption.
     */
    p.bytes[TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE + 4u] = 0xFFu;
    p.bytes[TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE + 5u] = 0xFFu;
    assert(recover(&p, NULL) == TRANSACTIONAL_IMAGE_RECOVERY_UNSUPPORTED);
}

static void test_corrupt_only_authority_is_corrupted(void)
{
    TestPhysical p;
    init_physical(&p);
    format(&p);
    p.bytes[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE + 60u] ^= 0x01u;
    assert(recover(&p, NULL) == TRANSACTIONAL_IMAGE_RECOVERY_CORRUPTED);
}


static uint32_t test_crc32(const uint8_t *data, size_t size)
{
    uint32_t crc=UINT32_C(0xFFFFFFFF);
    size_t i;
    for(i=0u;i<size;++i){
        uint8_t bit;
        crc^=data[i];
        for(bit=0u;bit<8u;++bit)
            crc=(crc&1u)?(crc>>1)^UINT32_C(0xEDB88320):crc>>1;
    }
    return crc^UINT32_C(0xFFFFFFFF);
}

static void put_test_u32(uint8_t *p, uint32_t v)
{
    p[0]=(uint8_t)(v>>24);
    p[1]=(uint8_t)(v>>16);
    p[2]=(uint8_t)(v>>8);
    p[3]=(uint8_t)v;
}

static void test_same_generation_identical_publications_are_accepted(void)
{
    TestPhysical p;
    uint64_t generation=0u;
    init_physical(&p);
    format(&p);

    memcpy(&p.bytes[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE],
           &p.bytes[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE],
           TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE);

    assert(recover(&p,&generation)==TRANSACTIONAL_IMAGE_RECOVERY_VALID);
    assert(generation==1u);
}

static void test_same_generation_conflicting_publications_are_corrupted(void)
{
    TestPhysical p;
    uint8_t *super_b;
    init_physical(&p);
    format(&p);

    memcpy(&p.bytes[TR2_TRANSACTIONAL_MEDIA_IMAGE_B_BASE],
           &p.bytes[TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE],
           TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE+
           TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);

    super_b=&p.bytes[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE];
    memcpy(super_b,
           &p.bytes[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE],
           TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE);
    super_b[16]=1u;
    put_test_u32(super_b+60u,test_crc32(super_b,60u));

    assert(recover(&p,NULL)==TRANSACTIONAL_IMAGE_RECOVERY_CORRUPTED);
}

int main(void)
{
    test_factory_erased_media_is_empty();
    test_random_unformatted_media_is_corrupted_not_empty();
    test_valid_authority_masks_corrupt_peer();
    test_valid_authority_masks_unsupported_peer();
    test_no_valid_authority_with_unsupported_record_is_unsupported();
    test_read_unavailable_is_not_masked_by_valid_peer();
    test_referenced_image_unavailable_is_not_collapsed_to_corrupted();
    test_referenced_image_unsupported_is_reported_unsupported();
    test_corrupt_only_authority_is_corrupted();
    test_same_generation_identical_publications_are_accepted();
    test_same_generation_conflicting_publications_are_corrupted();
    return 0;
}
