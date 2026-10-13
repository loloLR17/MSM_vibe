/* M016 prerequisite ONLY: no UART, FIFO capture, FRAM writes or auto START.
 * The allocation documented in ALLOCATION_STORAGE_M016.json is established
 * before a RAM trigger. Boot/recovery are read-only, no metadata formatting. */
#include <string.h>
#include "stm32_sdmmc_bulk_media.h"
#include "stm32_fram_storage.h"
#include "tr2/persistence/campaign_bulk_media_region.h"
#include "tr2/persistence/campaign_data_store_bulk.h"
#include "tr2/persistence/transactional_image_media.h"
#include "tr2/persistence/configuration_store.h"
#include "tr2/application/configuration_service.h"

#define BASE UINT32_C(16777216)
#define SIZE UINT32_C(8388608)
#define PAYLOAD 16384u
#define FIRST UINT32_C(0x01600003)
#define SECOND UINT32_C(0x01600004)
#define TEST_BYTES UINT32_C(4194304)
static Stm32SdmmcBulkMedia sd;
static CampaignBulkMedia raw;
static CampaignBulkMediaRegion region;
static CampaignDataStoreBulk store;
static uint8_t sector[512], payload[PAYLOAD], scratch[PAYLOAD+36];
static uint8_t inspect[8192];
static TransactionalImageMedia fram_media;
static uint8_t candidate[51914];
static PersistentStorageCore config_core;
static ConfigurationStore config_store;
static ConfigurationService config_service;
volatile uint32_t storage_qual_stage, storage_qual_result=UINT32_MAX;
volatile uint32_t storage_qual_trigger, storage_qual_scan_nonzero;
volatile uint32_t storage_qual_append_ms, storage_qual_max_call_ms;
volatile uint32_t storage_qual_samples, storage_qual_checkpoints;
volatile uint32_t storage_qual_configuration_available;
volatile uint32_t storage_qual_configuration_generation;
volatile uint32_t storage_qual_capacity_mb;
volatile uint32_t storage_qual_recovery_status[2];
volatile uint64_t storage_qual_recovery_bytes[2];
static bool fresh;

static bool identity_matches(SD_HandleTypeDef *hsd)
{
    return HAL_GetUIDw0()==0x004A003D && HAL_GetUIDw1()==0x38425018 &&
        HAL_GetUIDw2()==0x20333157 &&
        hsd->CID[0]==0x03534453 && hsd->CID[1]==0x44353132 &&
        hsd->CID[2]==0x8562E43C && hsd->CID[3]==0xF80195EA;
}
static uint32_t le32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1]<<8 | (uint32_t)p[2]<<16 | (uint32_t)p[3]<<24;
}
static Tr2Result prepare(SD_HandleTypeDef *hsd)
{
    uint64_t capacity; Tr2Result result;
    if(!identity_matches(hsd))return TR2_ERROR_NOT_AVAILABLE;
    result=stm32_sdmmc_bulk_media_init(&sd,hsd,sector,sizeof(sector),2000,2000);
    if(result!=TR2_OK)return result;
    raw=stm32_sdmmc_bulk_media_interface(&sd);
    result=campaign_bulk_media_capacity(&raw,&capacity);
    if(result!=TR2_OK)return result;
    if(capacity!=UINT64_C(511868665856))return TR2_ERROR_NOT_AVAILABLE;
    result=campaign_bulk_media_read(&raw,0,inspect,512);
    if(result!=TR2_OK)return result;
    if(inspect[510]!=0x55 || inspect[511]!=0xAA || inspect[450]!=7 ||
        le32(inspect+454)!=65536 || le32(inspect+458)!=999677952)
        return TR2_ERROR_NOT_AVAILABLE;
    for(size_t i=462;i<510;i++)if(inspect[i])return TR2_ERROR_NOT_AVAILABLE;
    result=campaign_bulk_media_region_init(&region,&raw,BASE,SIZE);
    if(result!=TR2_OK)return result;
    /* Full read gate before first write; a nonblank region is never erased.
     * It permits recovery only, not a new qualification run. */
    storage_qual_scan_nonzero=0;
    for(uint32_t off=0;off<SIZE;off+=sizeof(inspect)) {
        result=campaign_bulk_media_read(&region.interface,off,inspect,sizeof(inspect));
        if(result!=TR2_OK)return result;
        for(size_t i=0;i<sizeof(inspect);i++)if(inspect[i])++storage_qual_scan_nonzero;
    }
    fresh=storage_qual_scan_nonzero==0;
    return campaign_data_store_bulk_init(&store,&region.interface,payload,sizeof(payload),scratch,sizeof(scratch));
}
static Tr2Result recover_saved(void)
{
    CampaignDataRecoveryResult r; Tr2Result result;
    const CampaignId ids[2]={FIRST,SECOND};
    for(unsigned i=0;i<2;i++) {
        result=store.interface.recover_campaign(&store,ids[i],&r);
        if(result!=TR2_OK)return result;
        storage_qual_recovery_status[i]=r.status;
        storage_qual_recovery_bytes[i]=r.durable_prefix_bytes;
    }
    return TR2_OK;
}
static Tr2Result deny_fram_write(void *ctx,uint32_t off,const void *in,size_t n)
{
    (void)ctx;(void)off;(void)in;(void)n;
    return TR2_ERROR_NOT_AVAILABLE;
}
static Tr2Result configuration_recovery(Stm32FramStorage *fram)
{
    TransactionalImagePhysicalStorage physical=stm32_fram_storage_physical(fram);
    const TransactionalImageGeometry g={262144,0,4096,8192,65536,57344};
    TransactionalImageRecoveryResult recovered; ActiveConfigurationSnapshot active;
    /* Fixed payload blocks and minimum V1 windows avoid an optimistic raw
     * sector capacity. Retain eight partial-block reserves and metadata. */
    const uint64_t extent=(PAYLOAD+36u+511u)/512u*512u;
    const uint64_t reserved=TR2_CAMPAIGN_BULK_METADATA_BYTES+8u*extent;
    const uint64_t logical=((SIZE-reserved)/extent)*PAYLOAD;
    ConfigurationValidationEnvironment env={true,(uint32_t)(logical/1000000u)};
    physical.write=deny_fram_write;
    Tr2Result result=transactional_image_media_init_v2(&fram_media,&physical,&g,candidate,sizeof(candidate));
    if(result==TR2_OK)result=transactional_image_media_recover(&fram_media,&recovered);
    if(result!=TR2_OK)return result;
    if(recovered.status!=TRANSACTIONAL_IMAGE_RECOVERY_VALID || recovered.generation!=12)
        return TR2_ERROR_NOT_AVAILABLE;
    result=persistent_storage_core_init(&config_core,transactional_image_media_interface(&fram_media));
    if(result==TR2_OK)result=configuration_store_init(&config_store,&config_core);
    if(result==TR2_OK)result=configuration_service_init(&config_service,&config_store);
    if(result==TR2_OK)result=configuration_service_recover(&config_service,&env);
    if(result!=TR2_OK)return result;
    storage_qual_capacity_mb=env.usable_storage_capacity_mb;
    storage_qual_configuration_available=configuration_service_active_snapshot(&config_service,&active);
    if(!storage_qual_configuration_available)return TR2_ERROR_NOT_AVAILABLE;
    storage_qual_configuration_generation=active.generation;
    return TR2_OK;
}
static Tr2Result timed_append(CampaignId id,uint32_t index)
{
    uint8_t sample[16]; uint32_t before=HAL_GetTick(); Tr2Result result;
    for(unsigned i=0;i<16;i++)sample[i]=(uint8_t)(index+i);
    result=store.interface.append(&store,id,sample,sizeof(sample));
    uint32_t elapsed=HAL_GetTick()-before;
    if(elapsed>storage_qual_max_call_ms)storage_qual_max_call_ms=elapsed;
    if(result==TR2_OK)++storage_qual_samples;
    return result;
}
static Tr2Result run_once(void)
{
    if(!fresh)return TR2_ERROR_NOT_AVAILABLE;
    uint32_t before=HAL_GetTick();
    Tr2Result result=store.interface.begin_campaign(&store,FIRST);
    if(result!=TR2_OK)return result;
    for(uint32_t i=0;i<TEST_BYTES/16u;i++) {
        result=timed_append(FIRST,i);if(result!=TR2_OK)return result;
        if((i+1u)%4096u==0) {
            result=store.interface.checkpoint(&store,FIRST);
            if(result!=TR2_OK)return result;
            ++storage_qual_checkpoints;
        }
    }
    storage_qual_append_ms=HAL_GetTick()-before;
    /* Write one complete physical block AFTER the last checkpoint, then lose
     * only the volatile writer. Recovery must ignore this physical tail. */
    for(uint32_t i=0;i<PAYLOAD/16u;i++) {
        result=timed_append(FIRST,TEST_BYTES/16u+i);if(result!=TR2_OK)return result;
    }
    result=campaign_data_store_bulk_init(&store,&region.interface,payload,sizeof(payload),scratch,sizeof(scratch));
    if(result!=TR2_OK)return result;
    result=store.interface.begin_campaign(&store,SECOND);if(result!=TR2_OK)return result;
    for(uint32_t i=0;i<4096u;i++) {result=timed_append(SECOND,i);if(result!=TR2_OK)return result;}
    result=store.interface.finish_campaign(&store,SECOND);if(result!=TR2_OK)return result;
    fresh=false;
    return recover_saved();
}
__attribute__((noreturn)) void stm32_storage_qualification_run(SD_HandleTypeDef *hsd,Stm32FramStorage *fram)
{
    storage_qual_stage=1;
    storage_qual_result=prepare(hsd);
    if(storage_qual_result==TR2_OK)storage_qual_result=recover_saved();
    storage_qual_stage=storage_qual_result==TR2_OK?2:99;
    for(;;) {
        const uint32_t trigger=storage_qual_trigger; storage_qual_trigger=0;
        if(storage_qual_stage==2 && trigger==UINT32_C(0x4D313657)) {
            storage_qual_stage=3;storage_qual_result=run_once();
            if(storage_qual_result==TR2_OK && storage_qual_recovery_status[0]==CAMPAIGN_DATA_RECOVERY_VALID &&
                storage_qual_recovery_bytes[0]==TEST_BYTES && storage_qual_recovery_status[1]==CAMPAIGN_DATA_RECOVERY_VALID &&
                storage_qual_recovery_bytes[1]==65536 && storage_qual_append_ms>0 &&
                (uint64_t)TEST_BYTES*1000u/storage_qual_append_ms>=426672u)
                storage_qual_result=configuration_recovery(fram);
            else if(storage_qual_result==TR2_OK)storage_qual_result=TR2_ERROR_NOT_AVAILABLE;
            storage_qual_stage=storage_qual_result==TR2_OK?4:99;
        } else if(storage_qual_stage==2 && trigger==UINT32_C(0x4D313652)) {
            /* Explicit recovery after software reset; no write or auto-resume. */
            storage_qual_result=recover_saved();storage_qual_stage=storage_qual_result==TR2_OK?5:99;
        }
        HAL_Delay(1);
    }
}
