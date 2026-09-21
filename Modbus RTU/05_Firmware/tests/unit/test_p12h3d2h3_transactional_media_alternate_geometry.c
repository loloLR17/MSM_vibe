#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tr2/persistence/transactional_image_media.h"

#define ALT_PHYSICAL_SIZE ((size_t)131072u)

typedef struct {
    uint8_t bytes[ALT_PHYSICAL_SIZE];
} RamPhysical;

static Tr2Result rd(void *context, uint32_t offset, void *buffer, size_t size)
{
    RamPhysical *ram=context;
    if ((size_t)offset+size>sizeof(ram->bytes)) return TR2_ERROR_STORAGE;
    memcpy(buffer,ram->bytes+offset,size);
    return TR2_OK;
}

static Tr2Result wr(void *context, uint32_t offset, const void *buffer, size_t size)
{
    RamPhysical *ram=context;
    if ((size_t)offset+size>sizeof(ram->bytes)) return TR2_ERROR_STORAGE;
    memcpy(ram->bytes+offset,buffer,size);
    return TR2_OK;
}

static TransactionalImageGeometry alternate_geometry(void)
{
    const uint32_t image_area=UINT32_C(0x0D000);
    TransactionalImageGeometry geometry={
        ALT_PHYSICAL_SIZE,
        UINT32_C(0x00100),
        UINT32_C(0x00200),
        UINT32_C(0x01000),
        UINT32_C(0x0E000),
        image_area
    };
    return geometry;
}

static void test_alternate_geometry_format_commit_recover(void)
{
    RamPhysical ram;
    TransactionalImageMedia media,reboot;
    TransactionalImagePhysicalStorage storage={&ram,rd,wr};
    TransactionalImageGeometry geometry=alternate_geometry();
    TransactionalImageRecoveryResult recovery;
    PersistentMedia *persistent;
    uint8_t *candidate=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    uint8_t *reboot_candidate=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    uint8_t value[4]={0x12,0x34,0x56,0x78};
    uint8_t out[4]={0};

    assert(candidate&&reboot_candidate);
    memset(ram.bytes,0xFF,sizeof(ram.bytes));
    assert(transactional_image_geometry_validate(&geometry)==TR2_OK);
    assert(geometry.physical_size!=TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);
    assert(geometry.superblock_a_base!=TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE);
    assert(geometry.image_a_base!=TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE);
    assert(geometry.image_area_size!=TR2_TRANSACTIONAL_MEDIA_IMAGE_AREA_SIZE);

    assert(transactional_image_media_init(
        &media,&storage,&geometry,candidate,
        TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)==TR2_OK);
    assert(transactional_image_media_format_empty(&media)==TR2_OK);

    persistent=transactional_image_media_interface(&media);
    assert(persistent!=NULL);
    assert(persistent->write(persistent->context,321u,value,sizeof(value))==TR2_OK);
    assert(persistent->commit(persistent->context)==TR2_OK);

    assert(transactional_image_media_init(
        &reboot,&storage,&geometry,reboot_candidate,
        TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)==TR2_OK);
    assert(transactional_image_media_recover(&reboot,&recovery)==TR2_OK);
    assert(recovery.status==TRANSACTIONAL_IMAGE_RECOVERY_VALID);
    assert(recovery.generation==2u);

    persistent=transactional_image_media_interface(&reboot);
    assert(persistent->read(persistent->context,321u,out,sizeof(out))==TR2_OK);
    assert(memcmp(out,value,sizeof(value))==0);

    free(candidate);
    free(reboot_candidate);
}

int main(void)
{
    test_alternate_geometry_format_commit_recover();
    return 0;
}
