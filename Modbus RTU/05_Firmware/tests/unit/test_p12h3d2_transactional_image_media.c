#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tr2/persistence/transactional_image_media.h"

typedef struct {
    uint8_t bytes[TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE];
    bool tear;
    size_t tear_after;
} RamPhysical;

static Tr2Result rd(void *c,uint32_t o,void *b,size_t s){RamPhysical*m=c;if((size_t)o+s>sizeof(m->bytes))return TR2_ERROR_STORAGE;memcpy(b,m->bytes+o,s);return TR2_OK;}
static Tr2Result wr(void *c,uint32_t o,const void *b,size_t s){RamPhysical*m=c;size_t n=s;if((size_t)o+s>sizeof(m->bytes))return TR2_ERROR_STORAGE;if(m->tear&&m->tear_after<s)n=m->tear_after;if(n)memcpy(m->bytes+o,b,n);if(n<s)return TR2_ERROR_STORAGE;return TR2_OK;}
static void init(RamPhysical *r,TransactionalImageMedia *m,uint8_t *candidate){TransactionalImagePhysicalStorage p;memset(r->bytes,0xFF,sizeof(r->bytes));r->tear=false;r->tear_after=0;p.context=r;p.read=rd;p.write=wr;assert(transactional_image_media_init(m,&p,candidate,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)==TR2_OK);}

static void test_format_write_commit_recover(void)
{
    RamPhysical r; TransactionalImageMedia m,reboot; uint8_t *c=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE),*c2=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE); PersistentMedia *pm; TransactionalImageRecoveryResult rr; uint8_t v[4]={1,2,3,4},out[4]={0}; TransactionalImagePhysicalStorage p={&r,rd,wr};
    assert(c&&c2);init(&r,&m,c);assert(transactional_image_media_format_empty(&m)==TR2_OK);pm=transactional_image_media_interface(&m);
    assert(pm->write(pm->context,123u,v,4)==TR2_OK);assert(pm->read(pm->context,123u,out,4)==TR2_OK);assert(memcmp(out,(uint8_t[4]){0,0,0,0},4)==0);
    assert(pm->commit(pm->context)==TR2_OK);memset(out,0,4);assert(pm->read(pm->context,123u,out,4)==TR2_OK);assert(memcmp(out,v,4)==0);
    assert(transactional_image_media_init(&reboot,&p,c2,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)==TR2_OK);assert(transactional_image_media_recover(&reboot,&rr)==TR2_OK);assert(rr.status==TRANSACTIONAL_IMAGE_RECOVERY_VALID);assert(rr.generation==2u);
    pm=transactional_image_media_interface(&reboot);memset(out,0,4);assert(pm->read(pm->context,123u,out,4)==TR2_OK);assert(memcmp(out,v,4)==0);free(c);free(c2);
}

static void test_uncommitted_candidate_is_lost_on_reboot(void)
{
    RamPhysical r; TransactionalImageMedia m,reboot; uint8_t *c=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE),*c2=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE); TransactionalImagePhysicalStorage p={&r,rd,wr}; TransactionalImageRecoveryResult rr; PersistentMedia *pm; uint8_t v=0x5A,out=1;
    assert(c&&c2);init(&r,&m,c);assert(transactional_image_media_format_empty(&m)==TR2_OK);pm=transactional_image_media_interface(&m);assert(pm->write(pm->context,10,&v,1)==TR2_OK);
    assert(transactional_image_media_init(&reboot,&p,c2,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)==TR2_OK);assert(transactional_image_media_recover(&reboot,&rr)==TR2_OK);assert(rr.status==TRANSACTIONAL_IMAGE_RECOVERY_VALID);pm=transactional_image_media_interface(&reboot);assert(pm->read(pm->context,10,&out,1)==TR2_OK);assert(out==0);free(c);free(c2);
}

static void test_torn_inactive_image_keeps_old_authority(void)
{
    RamPhysical r; TransactionalImageMedia m,reboot; uint8_t *c=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE),*c2=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE); TransactionalImagePhysicalStorage p={&r,rd,wr}; TransactionalImageRecoveryResult rr; PersistentMedia *pm; uint8_t v=0x77,out=1;
    assert(c&&c2);init(&r,&m,c);assert(transactional_image_media_format_empty(&m)==TR2_OK);pm=transactional_image_media_interface(&m);assert(pm->write(pm->context,20,&v,1)==TR2_OK);r.tear=true;r.tear_after=100;assert(pm->commit(pm->context)==TR2_ERROR_STORAGE);r.tear=false;
    assert(transactional_image_media_init(&reboot,&p,c2,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)==TR2_OK);assert(transactional_image_media_recover(&reboot,&rr)==TR2_OK);assert(rr.status==TRANSACTIONAL_IMAGE_RECOVERY_VALID);assert(rr.generation==1u);pm=transactional_image_media_interface(&reboot);assert(pm->read(pm->context,20,&out,1)==TR2_OK);assert(out==0);free(c);free(c2);
}

int main(void){test_format_write_commit_recover();test_uncommitted_candidate_is_lost_on_reboot();test_torn_inactive_image_keeps_old_authority();return 0;}
