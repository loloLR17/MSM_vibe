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
static void init(RamPhysical *r,TransactionalImageMedia *m,uint8_t *candidate){TransactionalImagePhysicalStorage p;memset(r->bytes,0xFF,sizeof(r->bytes));r->tear=false;r->tear_after=0;p.context=r;p.read=rd;p.write=wr;assert(transactional_image_media_init(m,&p,&(TransactionalImageGeometry){
TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE,TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE,
TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE,TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE,
TR2_TRANSACTIONAL_MEDIA_IMAGE_B_BASE,TR2_TRANSACTIONAL_MEDIA_IMAGE_AREA_SIZE},
candidate,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)==TR2_OK);}

static void test_format_write_commit_recover(void)
{
    RamPhysical r; TransactionalImageMedia m,reboot; uint8_t *c=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE),*c2=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE); PersistentMedia *pm; TransactionalImageRecoveryResult rr; uint8_t v[4]={1,2,3,4},out[4]={0}; TransactionalImagePhysicalStorage p={&r,rd,wr};
    assert(c&&c2);init(&r,&m,c);assert(transactional_image_media_format_empty(&m)==TR2_OK);pm=transactional_image_media_interface(&m);
    assert(pm->write(pm->context,123u,v,4)==TR2_OK);assert(pm->read(pm->context,123u,out,4)==TR2_OK);assert(memcmp(out,(uint8_t[4]){0,0,0,0},4)==0);
    assert(pm->commit(pm->context)==TR2_OK);memset(out,0,4);assert(pm->read(pm->context,123u,out,4)==TR2_OK);assert(memcmp(out,v,4)==0);
    assert(transactional_image_media_init(&reboot,&p,&(TransactionalImageGeometry){
TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE,TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE,
TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE,TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE,
TR2_TRANSACTIONAL_MEDIA_IMAGE_B_BASE,TR2_TRANSACTIONAL_MEDIA_IMAGE_AREA_SIZE},
c2,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)==TR2_OK);assert(transactional_image_media_recover(&reboot,&rr)==TR2_OK);assert(rr.status==TRANSACTIONAL_IMAGE_RECOVERY_VALID);assert(rr.generation==2u);
    pm=transactional_image_media_interface(&reboot);memset(out,0,4);assert(pm->read(pm->context,123u,out,4)==TR2_OK);assert(memcmp(out,v,4)==0);free(c);free(c2);
}

static void test_uncommitted_candidate_is_lost_on_reboot(void)
{
    RamPhysical r; TransactionalImageMedia m,reboot; uint8_t *c=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE),*c2=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE); TransactionalImagePhysicalStorage p={&r,rd,wr}; TransactionalImageRecoveryResult rr; PersistentMedia *pm; uint8_t v=0x5A,out=1;
    assert(c&&c2);init(&r,&m,c);assert(transactional_image_media_format_empty(&m)==TR2_OK);pm=transactional_image_media_interface(&m);assert(pm->write(pm->context,10,&v,1)==TR2_OK);
    assert(transactional_image_media_init(&reboot,&p,&(TransactionalImageGeometry){
TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE,TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE,
TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE,TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE,
TR2_TRANSACTIONAL_MEDIA_IMAGE_B_BASE,TR2_TRANSACTIONAL_MEDIA_IMAGE_AREA_SIZE},
c2,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)==TR2_OK);assert(transactional_image_media_recover(&reboot,&rr)==TR2_OK);assert(rr.status==TRANSACTIONAL_IMAGE_RECOVERY_VALID);pm=transactional_image_media_interface(&reboot);assert(pm->read(pm->context,10,&out,1)==TR2_OK);assert(out==0);free(c);free(c2);
}

static void test_torn_inactive_image_keeps_old_authority(void)
{
    RamPhysical r; TransactionalImageMedia m,reboot; uint8_t *c=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE),*c2=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE); TransactionalImagePhysicalStorage p={&r,rd,wr}; TransactionalImageRecoveryResult rr; PersistentMedia *pm; uint8_t v=0x77,out=1;
    assert(c&&c2);init(&r,&m,c);assert(transactional_image_media_format_empty(&m)==TR2_OK);pm=transactional_image_media_interface(&m);assert(pm->write(pm->context,20,&v,1)==TR2_OK);r.tear=true;r.tear_after=100;assert(pm->commit(pm->context)==TR2_ERROR_STORAGE);r.tear=false;
    assert(transactional_image_media_init(&reboot,&p,&(TransactionalImageGeometry){
TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE,TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE,
TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE,TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE,
TR2_TRANSACTIONAL_MEDIA_IMAGE_B_BASE,TR2_TRANSACTIONAL_MEDIA_IMAGE_AREA_SIZE},
c2,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)==TR2_OK);assert(transactional_image_media_recover(&reboot,&rr)==TR2_OK);assert(rr.status==TRANSACTIONAL_IMAGE_RECOVERY_VALID);assert(rr.generation==1u);pm=transactional_image_media_interface(&reboot);assert(pm->read(pm->context,20,&out,1)==TR2_OK);assert(out==0);free(c);free(c2);
}


static void test_recovered_authority_publishes_to_opposite_superblock(void)
{
    RamPhysical r;
    TransactionalImageMedia m,reboot;
    uint8_t *c=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    uint8_t *c2=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    TransactionalImagePhysicalStorage p={&r,rd,wr};
    TransactionalImageGeometry g=transactional_image_geometry_qualification_profile();
    TransactionalImageRecoveryResult rr;
    PersistentMedia *pm;
    uint8_t v=0xA5u;
    uint8_t super_a_before[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE];

    assert(c&&c2);
    init(&r,&m,c);
    assert(transactional_image_media_format_empty(&m)==TR2_OK);
    memcpy(super_a_before,
           r.bytes+TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE,
           sizeof(super_a_before));

    assert(transactional_image_media_init(
        &reboot,&p,&g,c2,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)==TR2_OK);
    assert(transactional_image_media_recover(&reboot,&rr)==TR2_OK);
    assert(rr.status==TRANSACTIONAL_IMAGE_RECOVERY_VALID);
    assert(rr.generation==1u);
    assert(reboot.active_superblock==0u);

    pm=transactional_image_media_interface(&reboot);
    assert(pm->write(pm->context,42u,&v,1u)==TR2_OK);
    assert(pm->commit(pm->context)==TR2_OK);

    assert(reboot.generation==2u);
    assert(reboot.active_superblock==1u);
    assert(memcmp(super_a_before,
                  r.bytes+TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE,
                  sizeof(super_a_before))==0);
    assert(memcmp(r.bytes+TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE,
                  (uint8_t[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE]){0},
                  TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE)!=0);

    free(c);
    free(c2);
}


static void test_direct_zero_length_media_operations_are_noops(void)
{
    RamPhysical r;
    TransactionalImageMedia m;
    uint8_t *candidate=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    PersistentMedia *pm;

    assert(candidate);
    init(&r,&m,candidate);
    assert(transactional_image_media_format_empty(&m)==TR2_OK);
    pm=transactional_image_media_interface(&m);

    assert(pm->read(pm->context,0u,NULL,0u)==TR2_OK);
    assert(pm->read(pm->context,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE,NULL,0u)==TR2_OK);
    assert(pm->write(pm->context,0u,NULL,0u)==TR2_OK);
    assert(pm->write(pm->context,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE,NULL,0u)==TR2_OK);

    free(candidate);
}


static void test_recovered_b_authority_publishes_to_superblock_a(void)
{
    RamPhysical r;
    TransactionalImageMedia first,reboot;
    uint8_t *c=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    uint8_t *c2=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    TransactionalImagePhysicalStorage p={&r,rd,wr};
    TransactionalImageGeometry g=transactional_image_geometry_qualification_profile();
    TransactionalImageRecoveryResult rr;
    PersistentMedia *pm;
    uint8_t v=0x5Au;
    uint8_t super_b_before[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE];
    uint8_t invalid[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE]={0};

    assert(c&&c2);
    init(&r,&first,c);
    assert(transactional_image_media_format_empty(&first)==TR2_OK);

    memcpy(r.bytes+TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE,
           r.bytes+TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE,
           TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE);
    memcpy(super_b_before,
           r.bytes+TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE,
           sizeof(super_b_before));
    memcpy(r.bytes+TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE,
           invalid,sizeof(invalid));

    assert(transactional_image_media_init(
        &reboot,&p,&g,c2,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)==TR2_OK);
    assert(transactional_image_media_recover(&reboot,&rr)==TR2_OK);
    assert(rr.status==TRANSACTIONAL_IMAGE_RECOVERY_VALID);
    assert(rr.generation==1u);
    assert(reboot.active_superblock==1u);

    pm=transactional_image_media_interface(&reboot);
    assert(pm->write(pm->context,84u,&v,1u)==TR2_OK);
    assert(pm->commit(pm->context)==TR2_OK);

    assert(reboot.generation==2u);
    assert(reboot.active_superblock==0u);
    assert(memcmp(super_b_before,
                  r.bytes+TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE,
                  sizeof(super_b_before))==0);
    assert(memcmp(r.bytes+TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE,
                  invalid,sizeof(invalid))!=0);

    free(c);
    free(c2);
}

int main(void){test_format_write_commit_recover();test_uncommitted_candidate_is_lost_on_reboot();test_torn_inactive_image_keeps_old_authority();test_recovered_authority_publishes_to_opposite_superblock();test_direct_zero_length_media_operations_are_noops();test_recovered_b_authority_publishes_to_superblock_a();return 0;}
