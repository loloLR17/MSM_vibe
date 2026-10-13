#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "stm32_sdmmc_bulk_media.h"
static unsigned statuses,reads,writes,tick;
static uint32_t state=HAL_SD_CARD_TRANSFER, cmd_error, readiness_delay;
uint32_t SDMMC_CmdSendStatus(SDMMC_TypeDef *d,uint32_t arg){
 (void)arg;++statuses;d->response=(state<<9u);
 if(statuses>readiness_delay)d->response|=0x100u;
 return cmd_error;
}
uint32_t SDMMC_GetResponse(const SDMMC_TypeDef *d,uint32_t i){(void)i;return d->response;}
uint32_t HAL_GetTick(void){return tick++;}
HAL_SD_CardStateTypeDef HAL_SD_GetCardState(SD_HandleTypeDef *h){(void)h;++statuses;return state;}
HAL_StatusTypeDef HAL_SD_GetCardInfo(SD_HandleTypeDef *h,HAL_SD_CardInfoTypeDef *i){(void)h;i->LogBlockNbr=1024;i->LogBlockSize=512;return HAL_OK;}
HAL_StatusTypeDef HAL_SD_ReadBlocks(SD_HandleTypeDef *h,uint8_t *b,uint32_t l,uint32_t n,uint32_t t){(void)h;(void)t;assert(l+n<=1024);++reads;memset(b,3,n*512);return HAL_OK;}
HAL_StatusTypeDef HAL_SD_WriteBlocks(SD_HandleTypeDef *h,const uint8_t *b,uint32_t l,uint32_t n,uint32_t t){(void)h;(void)b;(void)t;assert(l+n<=1024);++writes;return HAL_OK;}
int main(void){
 SDMMC_TypeDef instance={0};SD_HandleTypeDef h={0};h.Instance=&instance;Stm32SdmmcBulkMedia a;uint8_t scratch[512],bytes[16];
 assert(stm32_sdmmc_bulk_media_init(&a,&h,scratch,512,20,20)==TR2_OK);
 CampaignBulkMedia rw=stm32_sdmmc_bulk_media_interface(&a),ro=stm32_sdmmc_bulk_media_readonly_interface(&a);
 assert(campaign_bulk_media_read(&rw,1,bytes,16)==TR2_OK && reads==1 && bytes[0]==3);
 unsigned old=statuses;state=0;h.ErrorCode=5;
 assert(campaign_bulk_media_read(&rw,1,bytes,16)==TR2_ERROR_STORAGE);
 assert(statuses==old+1 && reads==1 && h.ErrorCode==5);
 old=statuses;
 assert(campaign_bulk_media_write(&rw,1,bytes,16)==TR2_ERROR_STORAGE);
 assert(statuses==old+1 && writes==0);
 state=HAL_SD_CARD_PROGRAMMING;old=statuses;
 assert(campaign_bulk_media_sync(&rw)==TR2_ERROR_UNAVAILABLE && statuses>old+1);
 state=HAL_SD_CARD_TRANSFER;
 assert(campaign_bulk_media_write(&ro,1,bytes,16)==TR2_ERROR_NOT_AVAILABLE && writes==0);
 assert(campaign_bulk_media_write(&rw,1,bytes,16)==TR2_OK && writes==1);
 cmd_error=1;old=statuses;reads=0;
 assert(campaign_bulk_media_read(&rw,0,bytes,16)==TR2_ERROR_STORAGE);
 assert(statuses==old+1 && reads==0 && a.last_status_error==1);
 cmd_error=0;readiness_delay=statuses+2;old=statuses;
 assert(campaign_bulk_media_read(&rw,0,bytes,16)==TR2_OK);
 assert(statuses>=old+3 && a.transfer_not_ready_polls==2 && reads==1);
 puts("undefined card state fails once; programming stays bounded; error preserved; readonly refuses writes: PASS");
 return 0;
}
