#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tr2/persistence/campaign_data_store_persistent.h"
#include "tr2/persistence/persistent_storage_core.h"
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
    assert(transactional_image_media_init(m,&s,candidate,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)==TR2_OK);
}
static CampaignDataStore *init_campaign(TransactionalImageMedia *m,
                                        PersistentStorageCore *core,
                                        CampaignDataStorePersistent *store)
{
    assert(persistent_storage_core_init(core,transactional_image_media_interface(m))==TR2_OK);
    assert(campaign_data_store_persistent_init(store,core)==TR2_OK);
    return campaign_data_store_persistent_interface(store);
}
static void reboot(RamPhysical *p,TransactionalImageMedia *m,uint8_t *candidate)
{
    TransactionalImageRecoveryResult r;
    init_media(p,m,candidate);
    assert(transactional_image_media_recover(m,&r)==TR2_OK);
    assert(r.status==TRANSACTIONAL_IMAGE_RECOVERY_VALID);
}

static void test_uncheckpointed_tail_disappears_across_real_backend(void)
{
    RamPhysical p; TransactionalImageMedia m1,m2; PersistentStorageCore c1,c2;
    CampaignDataStorePersistent s1={0},s2={0}; CampaignDataStore *api;
    CampaignDataRecoveryResult r; uint8_t data[40]; uint8_t *a,*b; size_t i;
    p.bytes=malloc(TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);
    a=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE); b=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    assert(p.bytes&&a&&b); memset(p.bytes,0xFF,TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);
    for(i=0;i<sizeof(data);++i) data[i]=(uint8_t)(i+1u);
    init_media(&p,&m1,a); assert(transactional_image_media_format_empty(&m1)==TR2_OK);
    api=init_campaign(&m1,&c1,&s1);
    assert(api->begin_campaign(api->context,101u)==TR2_OK);
    assert(api->append(api->context,101u,data,sizeof(data))==TR2_OK);
    reboot(&p,&m2,b); api=init_campaign(&m2,&c2,&s2);
    assert(api->recover_campaign(api->context,101u,&r)==TR2_OK);
    assert(r.status==CAMPAIGN_DATA_RECOVERY_VALID);
    assert(r.durable_prefix_bytes==0u);
    free(b);free(a);free(p.bytes);
}

static void test_checkpoint_publishes_chunks_then_descriptor(void)
{
    RamPhysical p; TransactionalImageMedia m1,m2; PersistentStorageCore c1,c2;
    CampaignDataStorePersistent s1={0},s2={0}; CampaignDataStore *api;
    CampaignDataRecoveryResult r; uint8_t data[95]; uint8_t *a,*b; size_t i;
    p.bytes=malloc(TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);
    a=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE); b=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    assert(p.bytes&&a&&b); memset(p.bytes,0xFF,TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);
    for(i=0;i<sizeof(data);++i) data[i]=(uint8_t)(0x80u+i);
    init_media(&p,&m1,a); assert(transactional_image_media_format_empty(&m1)==TR2_OK);
    api=init_campaign(&m1,&c1,&s1);
    assert(api->begin_campaign(api->context,202u)==TR2_OK);
    assert(api->append(api->context,202u,data,sizeof(data))==TR2_OK);
    assert(api->checkpoint(api->context,202u)==TR2_OK);
    reboot(&p,&m2,b); api=init_campaign(&m2,&c2,&s2);
    assert(api->recover_campaign(api->context,202u,&r)==TR2_OK);
    assert(r.status==CAMPAIGN_DATA_RECOVERY_VALID);
    assert(r.durable_prefix_bytes==sizeof(data));
    free(b);free(a);free(p.bytes);
}

static void test_checkpoint_then_uncheckpointed_tail_recovers_checkpoint_only(void)
{
    RamPhysical p; TransactionalImageMedia m1,m2; PersistentStorageCore c1,c2;
    CampaignDataStorePersistent s1={0},s2={0}; CampaignDataStore *api;
    CampaignDataRecoveryResult r; uint8_t first[30],tail[20]; uint8_t *a,*b;
    p.bytes=malloc(TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);
    a=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE); b=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    assert(p.bytes&&a&&b); memset(p.bytes,0xFF,TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE);
    memset(first,0x31,sizeof(first)); memset(tail,0x72,sizeof(tail));
    init_media(&p,&m1,a); assert(transactional_image_media_format_empty(&m1)==TR2_OK);
    api=init_campaign(&m1,&c1,&s1);
    assert(api->begin_campaign(api->context,303u)==TR2_OK);
    assert(api->append(api->context,303u,first,sizeof(first))==TR2_OK);
    assert(api->checkpoint(api->context,303u)==TR2_OK);
    assert(api->append(api->context,303u,tail,sizeof(tail))==TR2_OK);
    reboot(&p,&m2,b); api=init_campaign(&m2,&c2,&s2);
    assert(api->recover_campaign(api->context,303u,&r)==TR2_OK);
    assert(r.status==CAMPAIGN_DATA_RECOVERY_VALID);
    assert(r.durable_prefix_bytes==sizeof(first));
    free(b);free(a);free(p.bytes);
}

int main(void)
{
    test_uncheckpointed_tail_disappears_across_real_backend();
    test_checkpoint_publishes_chunks_then_descriptor();
    test_checkpoint_then_uncheckpointed_tail_recovers_checkpoint_only();
    return 0;
}
