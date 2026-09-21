#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tr2/persistence/transactional_image_media.h"

typedef struct {
    uint8_t *bytes;
    size_t write_call;
    size_t fail_call;
    size_t cut_after;
} FaultPhysical;

static Tr2Result rd(void *ctx,uint32_t off,void *buf,size_t n)
{
    FaultPhysical *p=(FaultPhysical *)ctx;
    if((size_t)off+n>TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE) return TR2_ERROR_STORAGE;
    memcpy(buf,p->bytes+off,n); return TR2_OK;
}

static Tr2Result wr(void *ctx,uint32_t off,const void *buf,size_t n)
{
    FaultPhysical *p=(FaultPhysical *)ctx;
    size_t writable=n;
    p->write_call++;
    if((size_t)off+n>TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE) return TR2_ERROR_STORAGE;
    if(p->fail_call==p->write_call){
        if(p->cut_after<writable) writable=p->cut_after;
        if(writable!=0u) memcpy(p->bytes+off,buf,writable);
        return TR2_ERROR_STORAGE;
    }
    memcpy(p->bytes+off,buf,n); return TR2_OK;
}

static void init_media(FaultPhysical *p,TransactionalImageMedia *m,uint8_t *candidate)
{
    TransactionalImagePhysicalStorage s={p,rd,wr};
    assert(transactional_image_media_init(m,&s,candidate,
        TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)==TR2_OK);
}

static void build_old_generation(FaultPhysical *p,uint8_t *candidate)
{
    TransactionalImageMedia m;
    PersistentMedia *api;
    uint8_t value=0x5Au;

    p->write_call=0u; p->fail_call=0u; p->cut_after=0u;
    init_media(p,&m,candidate);
    assert(transactional_image_media_format_empty(&m)==TR2_OK);
    api=transactional_image_media_interface(&m);
    assert(api->write(api->context,1234u,&value,1u)==TR2_OK);
    assert(api->commit(api->context)==TR2_OK);
}

static void test_format_power_loss_never_resurrects_old_authority(void)
{
    FaultPhysical p;
    uint8_t *seed,*work,*recovery;
    size_t fail_call;
    const size_t cuts[]={0u,1u,4u,32u,63u};

    p.bytes=malloc(TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);
    seed=malloc(TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);
    work=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    recovery=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    assert(p.bytes&&seed&&work&&recovery);

    memset(p.bytes,0xFF,TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);
    build_old_generation(&p,work);
    memcpy(seed,p.bytes,TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);

    /*
     * format_empty physical writes:
     * 1 invalidate superblock A
     * 2 invalidate superblock B
     * 3 zero payload A
     * 4 finalize image A header
     * 5 publish superblock A
     *
     * For every cut in either destructive invalidation record, recovery must
     * not report the old generation as VALID.  Later failures may yield EMPTY
     * or CORRUPTED until the final publication succeeds.
     */
    for(fail_call=1u;fail_call<=5u;++fail_call){
        size_t i;
        for(i=0u;i<sizeof(cuts)/sizeof(cuts[0]);++i){
            TransactionalImageMedia m;
            TransactionalImageRecoveryResult r;
            memcpy(p.bytes,seed,TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);
            p.write_call=0u; p.fail_call=fail_call; p.cut_after=cuts[i];
            init_media(&p,&m,work);
            (void)transactional_image_media_format_empty(&m);

            p.fail_call=0u; p.write_call=0u;
            init_media(&p,&m,recovery);
            assert(transactional_image_media_recover(&m,&r)==TR2_OK);
            if(r.status==TRANSACTIONAL_IMAGE_RECOVERY_VALID){
                assert(r.generation==1u);
                assert(r.active_image==0u);
            }
        }
    }
}

int main(void)
{
    test_format_power_loss_never_resurrects_old_authority();
    return 0;
}
