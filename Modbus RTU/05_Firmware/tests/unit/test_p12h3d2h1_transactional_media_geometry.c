#include <assert.h>
#include <stdint.h>

#include "tr2/persistence/transactional_image_media.h"

static TransactionalImageGeometry qualification_geometry(void)
{
    TransactionalImageGeometry g={
        TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE,
        TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE,
        TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE,
        TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE,
        TR2_TRANSACTIONAL_MEDIA_IMAGE_B_BASE,
        TR2_TRANSACTIONAL_MEDIA_IMAGE_AREA_SIZE
    };
    return g;
}

static void test_current_qualification_geometry_is_valid(void)
{
    TransactionalImageGeometry g=qualification_geometry();
    assert(transactional_image_geometry_validate(&g)==TR2_OK);
}

static void test_minimal_packed_geometry_is_valid(void)
{
    const uint32_t image_size=(uint32_t)(TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE+
                                         TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    TransactionalImageGeometry g={
        (size_t)(2u*TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE)+
        (size_t)(2u*image_size),
        0u,
        (uint32_t)TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE,
        (uint32_t)(2u*TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE),
        (uint32_t)(2u*TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE)+image_size,
        image_size
    };
    assert(g.physical_size==103892u);
    assert(transactional_image_geometry_validate(&g)==TR2_OK);
}

static void test_image_area_one_byte_too_small_is_rejected(void)
{
    TransactionalImageGeometry g=qualification_geometry();
    g.image_area_size=(uint32_t)(TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE+
                                 TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE-1u);
    assert(transactional_image_geometry_validate(&g)==TR2_ERROR_INVALID_ARGUMENT);
}

static void test_region_past_physical_end_is_rejected(void)
{
    TransactionalImageGeometry g=qualification_geometry();
    g.physical_size=(size_t)g.image_b_base+(size_t)g.image_area_size-1u;
    assert(transactional_image_geometry_validate(&g)==TR2_ERROR_INVALID_ARGUMENT);
}

static void test_superblocks_must_not_overlap(void)
{
    TransactionalImageGeometry g=qualification_geometry();
    g.superblock_b_base=g.superblock_a_base+32u;
    assert(transactional_image_geometry_validate(&g)==TR2_ERROR_INVALID_ARGUMENT);
}

static void test_image_must_not_overlap_superblock(void)
{
    TransactionalImageGeometry g=qualification_geometry();
    g.image_a_base=g.superblock_b_base;
    assert(transactional_image_geometry_validate(&g)==TR2_ERROR_INVALID_ARGUMENT);
}

static void test_images_must_not_overlap(void)
{
    TransactionalImageGeometry g=qualification_geometry();
    g.image_b_base=g.image_a_base+1u;
    assert(transactional_image_geometry_validate(&g)==TR2_ERROR_INVALID_ARGUMENT);
}

static void test_zero_physical_size_is_rejected(void)
{
    TransactionalImageGeometry g=qualification_geometry();
    g.physical_size=0u;
    assert(transactional_image_geometry_validate(&g)==TR2_ERROR_INVALID_ARGUMENT);
}

static void test_null_geometry_is_rejected(void)
{
    assert(transactional_image_geometry_validate(NULL)==TR2_ERROR_INVALID_ARGUMENT);
}

int main(void)
{
    test_current_qualification_geometry_is_valid();
    test_minimal_packed_geometry_is_valid();
    test_image_area_one_byte_too_small_is_rejected();
    test_region_past_physical_end_is_rejected();
    test_superblocks_must_not_overlap();
    test_image_must_not_overlap_superblock();
    test_images_must_not_overlap();
    test_zero_physical_size_is_rejected();
    test_null_geometry_is_rejected();
    return 0;
}
