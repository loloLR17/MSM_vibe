#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tr2/persistence/transactional_image_media.h"

typedef struct { uint8_t *bytes; } RamPhysical;

static Tr2Result rd(void *ctx,uint32_t off,void *buf,size_t n)
{
    RamPhysical *p=(RamPhysical *)ctx;
    if((size_t)off+n>TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE) return TR2_ERROR_STORAGE;
    memcpy(buf,p->bytes+off,n); return TR2_OK;
}
static Tr2Result wr(void *ctx,uint32_t off,const void *buf,size_t n)
{
    RamPhysical *p=(RamPhysical *)ctx;
    if((size_t)off+n>TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE) return TR2_ERROR_STORAGE;
    memcpy(p->bytes+off,buf,n); return TR2_OK;
}
static void init_media(RamPhysical *p,TransactionalImageMedia *m,uint8_t *candidate)
{
    TransactionalImagePhysicalStorage s={p,rd,wr};
    TransactionalImageGeometry geometry =
        transactional_image_geometry_qualification_profile();
    assert(transactional_image_media_init(m,&s,&geometry,candidate,
        TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)==TR2_OK);
}
static void recover_valid(RamPhysical *p,TransactionalImageMedia *m,uint8_t *candidate,
                          uint64_t generation,uint8_t image)
{
    TransactionalImageRecoveryResult r;
    init_media(p,m,candidate);
    assert(transactional_image_media_recover(m,&r)==TR2_OK);
    assert(r.status==TRANSACTIONAL_IMAGE_RECOVERY_VALID);
    assert(r.generation==generation);
    assert(r.active_image==image);
}

static void test_reformat_must_not_resurrect_older_higher_generation(void)
{
    RamPhysical p; TransactionalImageMedia m,reboot; PersistentMedia *api;
    uint8_t *a,*b; uint8_t value=0x5Au, observed=0xFFu;
    p.bytes=malloc(TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);
    a=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    b=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    assert(p.bytes&&a&&b); memset(p.bytes,0xFF,TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);

    init_media(&p,&m,a);
    assert(transactional_image_media_format_empty(&m)==TR2_OK);
    api=transactional_image_media_interface(&m);

    /* Build a valid generation 2 in image B, then a valid generation 3 in A. */
    assert(api->write(api->context,1234u,&value,1u)==TR2_OK);
    assert(api->commit(api->context)==TR2_OK);
    value=0x6Bu;
    assert(api->write(api->context,1234u,&value,1u)==TR2_OK);
    assert(api->commit(api->context)==TR2_OK);

    /*
     * Explicit reformat must establish a fresh empty authority.  A stale
     * higher-generation publication must never win recovery afterwards.
     */
    assert(transactional_image_media_format_empty(&m)==TR2_OK);

    recover_valid(&p,&reboot,b,1u,0u);
    api=transactional_image_media_interface(&reboot);
    assert(api->read(api->context,1234u,&observed,1u)==TR2_OK);
    assert(observed==0u);

    free(b);free(a);free(p.bytes);
}

int main(void)
{
    test_reformat_must_not_resurrect_older_higher_generation();
    return 0;
}
