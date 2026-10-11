#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tr2/application/production_commissioning.h"
#include "tr2/persistence/system_persistent_layout.h"
#include "tr2/persistence/campaign_data_store_persistent_composition.h"
#include "tr2/persistence/boot_intent_record.h"

static Tr2Result read_image(void *ctx, uint32_t offset, void *buffer, size_t size)
{
    if (offset > 51818u || size > 51818u-offset) return TR2_ERROR_STORAGE;
    memcpy(buffer, (uint8_t *)ctx+offset, size); return TR2_OK;
}
static Tr2Result write_image(void *ctx, uint32_t offset, const void *buffer, size_t size)
{
    if (offset > 51818u || size > 51818u-offset) return TR2_ERROR_STORAGE;
    memcpy((uint8_t *)ctx+offset, buffer, size); return TR2_OK;
}
static Tr2Result commit_image(void *ctx) { (void)ctx; return TR2_OK; }
static ActiveConfigurationSnapshot fixture(void)
{
    ActiveConfigurationSnapshot config = {0};
    config.generation=1; config.config_id=1; config.revision_counter=1;
    config.payload.sampling_frequency_hz=26667;
    config.payload.axes_enable_mask=7; config.payload.acquisition_mode=1;
    config.payload.window_size_samples=4096; config.payload.indicator_period_ms=2000;
    config.payload.campaign_duration_s=60; config.payload.storage_mode=1;
    config.payload.storage_limit_mb=1; config.payload.campaign_context_id=1;
    config.payload.mission_id=1; memcpy(config.payload.campaign_label,"HOST-ONLY",9);
    return config;
}
typedef struct { uint8_t bytes[262144]; unsigned writes, cut; } Physical;
static Tr2Result physical_read(void *ctx,uint32_t offset,void *buffer,size_t size)
{
    Physical *p=ctx; assert(offset<=sizeof(p->bytes)&&size<=sizeof(p->bytes)-offset);
    memcpy(buffer,p->bytes+offset,size); return TR2_OK;
}
static Tr2Result physical_write(void *ctx,uint32_t offset,const void *buffer,size_t size)
{
    Physical *p=ctx; assert(offset<=sizeof(p->bytes)&&size<=sizeof(p->bytes)-offset);
    ++p->writes;
    if(p->writes==p->cut){memcpy(p->bytes+offset,buffer,size/2);return TR2_ERROR_STORAGE;}
    memcpy(p->bytes+offset,buffer,size); return TR2_OK;
}
static void test_configuration_install(void)
{
    unsigned cut;
    for(cut=0;cut<=3;cut++){
        Physical *p=calloc(1,sizeof(*p)); uint8_t *candidate=malloc(51914), *reboot_candidate=malloc(51914);
        TransactionalImagePhysicalStorage physical={p,physical_read,physical_write};
        TransactionalImageGeometry geometry=transactional_image_geometry_qualification_profile();
        TransactionalImageMedia media,reboot; TransactionalImageRecoveryResult recovery;
        ProductionProvisioningResult result; ActiveConfigurationSnapshot config=fixture();
        PersistentIdentityAssignment assignment={{1,2,3},1};
        ProductionCommissioningAuthorities authority={{true,1},&assignment,1};
        ProductionCommissioningConfigurationRequest q={0}; uint8_t marker=0xAC;
        assert(p&&candidate&&reboot_candidate);
        memset(p->bytes,0xFF,sizeof(p->bytes));
        assert(transactional_image_media_init_v2(&media,&physical,&geometry,candidate,51914)==TR2_OK);
        assert(transactional_image_media_format_empty(&media)==TR2_OK);
        assert(media.interface.write(&media,0,&marker,1)==TR2_OK);
        assert(media.interface.commit(&media)==TR2_OK);
        q.authorities=&authority;memcpy(q.observed_uid,assignment.hardware_uid,12);
        memcpy(q.target_uid,assignment.hardware_uid,12);q.device_id=1;q.expected_generation=2;
        q.exclusive_ownership=true;q.configuration=&config;p->writes=0;
        assert(production_commissioning_install_configuration(&media,&q,false,&result)==TR2_ERROR_NOT_AVAILABLE);
        q.target_uid[2]^=1;
        assert(production_commissioning_install_configuration(&media,&q,true,&result)==TR2_ERROR_INVALID_STATE);
        q.target_uid[2]^=1;q.expected_generation=1;
        assert(production_commissioning_install_configuration(&media,&q,true,&result)==TR2_ERROR_INVALID_STATE);
        q.expected_generation=2;authority.environment.storage_capacity_known=false;
        assert(production_commissioning_install_configuration(&media,&q,true,&result)==TR2_ERROR_UNAVAILABLE);
        authority.environment.storage_capacity_known=true;assert(p->writes==0);p->cut=cut;
        if(cut==0){
            assert(production_commissioning_install_configuration(&media,&q,true,&result)==TR2_OK);
            assert(result.recovery.generation==3&&candidate[0]==0xAC&&p->writes==3);
            q.expected_generation=3;
            assert(production_commissioning_install_configuration(&media,&q,true,&result)==TR2_OK&&result.unchanged&&p->writes==3);
        }else assert(production_commissioning_install_configuration(&media,&q,true,&result)==TR2_ERROR_STORAGE);
        p->cut=0;
        assert(transactional_image_media_init_v2(&reboot,&physical,&geometry,reboot_candidate,51914)==TR2_OK);
        assert(transactional_image_media_recover(&reboot,&recovery)==TR2_OK);
        assert(recovery.status==TRANSACTIONAL_IMAGE_RECOVERY_VALID&&reboot_candidate[0]==0xAC);
        if(cut)assert(recovery.generation==2&&reboot_candidate[140]==0);
        else assert(recovery.generation==3&&memcmp(reboot_candidate+140,"TR2C",4)==0);
        free(reboot_candidate);free(candidate);free(p);
    }
}
int main(void)
{
    test_configuration_install();
    uint8_t *image=calloc(1,51818), *before=malloc(51818);
    ProductionCommissioningAuthorities authority={0};
    ActiveConfigurationSnapshot config=fixture();
    PersistentIdentityAssignment assignment={{1,2,3},1};
    const uint32_t offsets[]={280,298,4370,15922,51762,51782,51802};
    size_t i;
    assert(image && before);
    authority.environment.storage_capacity_known=true;
    authority.environment.usable_storage_capacity_mb=1;
    authority.assignments=&assignment; authority.assignment_count=1;
    assert(production_commissioning_check_attribution(&authority,assignment.hardware_uid,1)==TR2_OK);
    assert(production_commissioning_check_attribution(&authority,assignment.hardware_uid,2)==TR2_ERROR_INVALID_STATE);
    assert(production_commissioning_inspect_prefix(&authority,image,51818)==TR2_ERROR_NOT_AVAILABLE);
    /* Preserve an old qualification marker / invalid peer, install in empty slot1. */
    image[0]=0xAC;
    assert(tr2_configuration_record_encode(&config,image+140,140)==TR2_OK);
    memcpy(before,image,51818);
    assert(production_commissioning_inspect_prefix(&authority,image,51818)==TR2_OK);
    assert(memcmp(image,before,51818)==0);
    authority.environment.storage_capacity_known=false;
    assert(production_commissioning_inspect_prefix(&authority,image,51818)==TR2_ERROR_UNAVAILABLE);
    authority.environment.storage_capacity_known=true;
    config.payload.storage_limit_mb=2;
    assert(tr2_configuration_record_encode(&config,image+140,140)==TR2_OK);
    assert(production_commissioning_inspect_prefix(&authority,image,51818)==TR2_ERROR_CORRUPTED);
    memcpy(image,before,51818);
    for(i=0;i<sizeof(offsets)/sizeof(offsets[0]);++i){
        image[offsets[i]]=0xAC;
        { Tr2Result status=production_commissioning_inspect_prefix(&authority,image,51818); if(status==TR2_OK) fprintf(stderr,"Corrupt prefix region accepted at offset %u\n",offsets[i]); assert(status!=TR2_OK); }
        memcpy(image,before,51818);
    }
    { BootIntent intent=boot_intent_software_reset(1);
      assert(tr2_boot_intent_record_encode(&intent,image+51802,16)==TR2_OK);
      assert(production_commissioning_inspect_prefix(&authority,image,51818)==TR2_ERROR_INVALID_STATE); }
    memcpy(image,before,51818);
    { PersistentMedia media={image,read_image,write_image,commit_image};
      CampaignDataStorePersistentComposition historical={0};
      CampaignDataStore *port; uint8_t sample[16]={0};
      assert(campaign_data_store_persistent_composition_init(&historical,&media)==TR2_OK);
      port=campaign_data_store_persistent_interface(&historical.store);
      assert(port->begin_campaign(port->context,1)==TR2_OK);
      assert(port->append(port->context,1,sample,16)==TR2_OK);
      assert(port->checkpoint(port->context,1)==TR2_OK);
      assert(production_commissioning_inspect_prefix(&authority,image,51818)==TR2_OK);
      image[TR2_CAMPAIGN_DATA_STORAGE_OFFSET+72]^=1;
      assert(production_commissioning_inspect_prefix(&authority,image,51818)!=TR2_OK);
    }
    free(before); free(image); return 0;
}
