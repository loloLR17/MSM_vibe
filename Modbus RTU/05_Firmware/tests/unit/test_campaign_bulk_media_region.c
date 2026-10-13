#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "tr2/persistence/campaign_bulk_media_region.h"
#include "tr2/persistence/campaign_data_store_bulk.h"
static unsigned char bytes[65536];
static unsigned calls;
static Tr2Result injected;
static Tr2Result cap(void *ctx, uint64_t *out) {(void)ctx; *out=sizeof(bytes); return injected;}
static Tr2Result read_bytes(void *ctx,uint64_t off,void *out,size_t n) {
 (void)ctx; ++calls; assert(off<=sizeof(bytes) && n<=sizeof(bytes)-off);
 if(injected!=TR2_OK)return injected;
 memcpy(out,bytes+off,n);return TR2_OK;
}
static Tr2Result write_bytes(void *ctx,uint64_t off,const void *in,size_t n) {
 (void)ctx; ++calls; assert(off<=sizeof(bytes) && n<=sizeof(bytes)-off);
 if(injected!=TR2_OK)return injected;
 memcpy(bytes+off,in,n);return TR2_OK;
}
static Tr2Result sync_bytes(void *ctx) {(void)ctx; ++calls; return injected;}
int main(void) {
 CampaignBulkMedia parent={NULL,cap,read_bytes,write_bytes,sync_bytes};
 CampaignBulkMediaRegion r={0}; CampaignDataStoreBulk store, recovered;
 uint8_t payload[64],scratch[128],sample[16]={1}; uint64_t capacity;
 CampaignDataRecoveryResult result; unsigned before;
 memset(bytes,0xA5,sizeof(bytes));memset(bytes+8192,0,32768);
 assert(campaign_bulk_media_region_init(&r,&parent,8192,32768)==TR2_OK);
 assert(campaign_bulk_media_capacity(&r.interface,&capacity)==TR2_OK && capacity==32768);
 assert(campaign_bulk_media_write(&r.interface,0,sample,16)==TR2_OK);
 memset(bytes+8192,0,16);
 before=calls;
 assert(campaign_bulk_media_write(&r.interface,32768,sample,1)==TR2_ERROR_NOT_AVAILABLE);
 assert(campaign_bulk_media_write(&r.interface,UINT64_MAX,sample,16)==TR2_ERROR_INVALID_ARGUMENT);
 assert(campaign_bulk_media_read(&r.interface,UINT64_C(1)<<32,sample,16)==TR2_ERROR_NOT_AVAILABLE);
 assert(campaign_bulk_media_read(&r.interface,32760,sample,16)==TR2_ERROR_NOT_AVAILABLE);
 assert(calls==before);
 assert(campaign_bulk_media_region_init(&r,&parent,8193,32768)==TR2_ERROR_INVALID_ARGUMENT);
 assert(campaign_bulk_media_region_init(&r,&parent,8192,UINT32_MAX)==TR2_ERROR_INVALID_ARGUMENT);
 assert(campaign_bulk_media_region_init(&r,&parent,65024,1024)==TR2_ERROR_NOT_AVAILABLE);
 assert(campaign_data_store_bulk_init(&store,&r.interface,payload,64,scratch,128)==TR2_OK);
 assert(store.interface.begin_campaign(&store,100)==TR2_OK);
 memset(sample,3,16);
 assert(store.interface.append(&store,100,sample,16)==TR2_OK);
 assert(store.interface.checkpoint(&store,100)==TR2_OK);
 assert(store.interface.append(&store,100,sample,16)==TR2_OK);
 assert(campaign_data_store_bulk_init(&recovered,&r.interface,payload,64,scratch,128)==TR2_OK);
 assert(recovered.interface.recover_campaign(&recovered,100,&result)==TR2_OK);
 assert(result.status==CAMPAIGN_DATA_RECOVERY_VALID && result.durable_prefix_bytes==16);
 injected=TR2_ERROR_STORAGE;
 assert(campaign_bulk_media_read(&r.interface,0,sample,16)==TR2_ERROR_STORAGE);
 assert(campaign_bulk_media_write(&r.interface,0,sample,16)==TR2_ERROR_STORAGE);
 assert(campaign_bulk_media_sync(&r.interface)==TR2_ERROR_STORAGE);
 for(unsigned i=0;i<8192;i++)assert(bytes[i]==0xA5);
 for(unsigned i=40960;i<65536;i++)assert(bytes[i]==0xA5);
 puts("region confinement, no overflow alias, error propagation, real bulk durable prefix, outside sentinels: PASS");
 return 0;
}
