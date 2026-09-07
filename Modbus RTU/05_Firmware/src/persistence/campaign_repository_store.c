#include "tr2/persistence/campaign_repository_store.h"

#include <string.h>

#define ALLOC_MAGIC UINT32_C(0x54523241)
#define RECORDS_BASE (TR2_CAMPAIGN_ALLOCATOR_SLOT_COUNT * TR2_CAMPAIGN_ALLOCATOR_RECORD_SIZE)

typedef struct {
    bool valid;
    bool unsupported;
    uint32_t generation;
    CampaignId next_id;
} AllocatorState;

typedef struct {
    bool valid;
    bool unsupported;
    CampaignMetadata metadata;
} CampaignCopy;

static void put_u16(uint8_t *p, uint16_t v) { p[0]=(uint8_t)(v>>8u); p[1]=(uint8_t)v; }
static void put_u32(uint8_t *p, uint32_t v) { p[0]=(uint8_t)(v>>24u); p[1]=(uint8_t)(v>>16u); p[2]=(uint8_t)(v>>8u); p[3]=(uint8_t)v; }
static uint16_t get_u16(const uint8_t *p) { return (uint16_t)(((uint16_t)p[0]<<8u)|p[1]); }
static uint32_t get_u32(const uint8_t *p) { return ((uint32_t)p[0]<<24u)|((uint32_t)p[1]<<16u)|((uint32_t)p[2]<<8u)|p[3]; }

static uint32_t crc32_bytes(const uint8_t *bytes, size_t count)
{
    uint32_t crc=UINT32_C(0xFFFFFFFF); size_t i;
    for (i=0u;i<count;++i) { uint8_t bit; crc^=bytes[i]; for(bit=0u;bit<8u;++bit) crc=(crc&1u)?((crc>>1u)^UINT32_C(0xEDB88320)):(crc>>1u); }
    return crc^UINT32_C(0xFFFFFFFF);
}

static bool empty_bytes(const uint8_t *bytes,size_t size)
{
    bool zero=true, ff=true; size_t i;
    for(i=0u;i<size;++i){ zero=zero&&bytes[i]==0u; ff=ff&&bytes[i]==UINT8_C(0xFF); }
    return zero||ff;
}

static uint32_t allocator_offset(size_t slot) { return (uint32_t)(slot*TR2_CAMPAIGN_ALLOCATOR_RECORD_SIZE); }
static uint32_t copy_offset(size_t slot,size_t copy)
{
    return (uint32_t)(RECORDS_BASE+(slot*TR2_CAMPAIGN_REPOSITORY_COPY_COUNT+copy)*TR2_CAMPAIGN_RECORD_SIZE);
}

static Tr2Result read_allocator(const CampaignRepositoryStore *store,size_t slot,AllocatorState *state)
{
    uint8_t r[TR2_CAMPAIGN_ALLOCATOR_RECORD_SIZE]; uint32_t crc; Tr2Result result;
    memset(state,0,sizeof(*state));
    result=persistent_storage_core_read(store->storage,allocator_offset(slot),r,sizeof(r));
    if(result!=TR2_OK) return result;
    if(empty_bytes(r,sizeof(r))) return TR2_OK;
    if(get_u32(&r[0])!=ALLOC_MAGIC||get_u16(&r[6])!=TR2_CAMPAIGN_ALLOCATOR_RECORD_SIZE) return TR2_OK;
    crc=crc32_bytes(r,16u); if(get_u32(&r[16])!=crc) return TR2_OK;
    if(get_u16(&r[4])!=UINT16_C(1)){ state->unsupported=true; return TR2_OK; }
    state->generation=get_u32(&r[8]); state->next_id=get_u32(&r[12]); state->valid=state->next_id!=0u; return TR2_OK;
}

static Tr2Result write_allocator(CampaignRepositoryStore *store,size_t slot,uint32_t generation,CampaignId next_id)
{
    uint8_t r[TR2_CAMPAIGN_ALLOCATOR_RECORD_SIZE]; Tr2Result result;
    memset(r,0,sizeof(r)); put_u32(&r[0],ALLOC_MAGIC); put_u16(&r[4],1u); put_u16(&r[6],TR2_CAMPAIGN_ALLOCATOR_RECORD_SIZE); put_u32(&r[8],generation); put_u32(&r[12],next_id); put_u32(&r[16],crc32_bytes(r,16u));
    result=persistent_storage_core_write(store->storage,allocator_offset(slot),r,sizeof(r));
    if(result==TR2_OK) result=persistent_storage_core_commit(store->storage);
    if(result!=TR2_OK) store->recovery_required=true;
    return result;
}

static Tr2Result read_copy(const CampaignRepositoryStore *store,size_t slot,size_t copy,CampaignCopy *out)
{
    uint8_t r[TR2_CAMPAIGN_RECORD_SIZE]; Tr2Result result;
    memset(out,0,sizeof(*out)); result=persistent_storage_core_read(store->storage,copy_offset(slot,copy),r,sizeof(r));
    if(result!=TR2_OK) return result; if(empty_bytes(r,sizeof(r))) return TR2_OK;
    result=tr2_campaign_record_decode(r,sizeof(r),&out->metadata);
    if(result==TR2_OK) out->valid=true; else if(result==TR2_ERROR_UNSUPPORTED) out->unsupported=true;
    return TR2_OK;
}

static bool select_copy(const CampaignCopy copies[2],CampaignMetadata *metadata,bool *unsupported)
{
    *unsupported=copies[0].unsupported||copies[1].unsupported;
    if(copies[1].valid){*metadata=copies[1].metadata;return true;}
    if(copies[0].valid){*metadata=copies[0].metadata;return true;}
    memset(metadata,0,sizeof(*metadata)); return false;
}

static Tr2Result load_slot(const CampaignRepositoryStore *store,size_t slot,CampaignMetadata *metadata,bool *valid,bool *unsupported)
{
    CampaignCopy copies[2]; Tr2Result result; size_t copy;
    for(copy=0u;copy<2u;++copy){ result=read_copy(store,slot,copy,&copies[copy]); if(result!=TR2_OK) return result; }
    *valid=select_copy(copies,metadata,unsupported); return TR2_OK;
}

static Tr2Result write_copy(CampaignRepositoryStore *store,size_t slot,size_t copy,const CampaignMetadata *metadata)
{
    uint8_t r[TR2_CAMPAIGN_RECORD_SIZE]; Tr2Result result=tr2_campaign_record_encode(metadata,r,sizeof(r));
    if(result!=TR2_OK) return result; result=persistent_storage_core_write(store->storage,copy_offset(slot,copy),r,sizeof(r));
    if(result==TR2_OK) result=persistent_storage_core_commit(store->storage); if(result!=TR2_OK) store->recovery_required=true; return result;
}

static Tr2Result reserve_id(void *context,CampaignIdReservation *reservation)
{
    CampaignRepositoryStore *store=context; AllocatorState a[2]; CampaignId candidate=1u,max_id=0u; uint32_t generation=0u; size_t target=0u,i; Tr2Result result;
    if(!campaign_repository_store_is_initialized(store)||store->recovery_required) return TR2_ERROR_INVALID_STATE;
    if(reservation==NULL) return TR2_ERROR_INVALID_ARGUMENT; memset(reservation,0,sizeof(*reservation));
    for(i=0u;i<2u;++i){ result=read_allocator(store,i,&a[i]); if(result!=TR2_OK){store->recovery_required=true;return result;} if(a[i].unsupported)return TR2_ERROR_UNSUPPORTED; if(a[i].valid&&a[i].generation>=generation){generation=a[i].generation;candidate=a[i].next_id;target=(i+1u)%2u;} }
    for(i=0u;i<TR2_CAMPAIGN_REPOSITORY_CAPACITY;++i){ CampaignMetadata m; bool valid,unsupported; result=load_slot(store,i,&m,&valid,&unsupported); if(result!=TR2_OK){store->recovery_required=true;return result;} if(unsupported)return TR2_ERROR_UNSUPPORTED; if(valid&&m.campaign_id>max_id)max_id=m.campaign_id; }
    if(candidate<=max_id){ if(max_id==UINT32_MAX)return TR2_ERROR_NOT_AVAILABLE; candidate=max_id+1u; }
    if(candidate==UINT32_MAX)return TR2_ERROR_NOT_AVAILABLE;
    result=write_allocator(store,target,generation+1u,candidate+1u); if(result!=TR2_OK)return result;
    reservation->campaign_id=candidate; reservation->valid=true; return TR2_OK;
}

static Tr2Result open_campaign(void *context,const CampaignMetadata *metadata)
{
    CampaignRepositoryStore *store=context; size_t i,free_slot=TR2_CAMPAIGN_REPOSITORY_CAPACITY; Tr2Result result;
    if(!campaign_repository_store_is_initialized(store)||store->recovery_required)return TR2_ERROR_INVALID_STATE;
    if(metadata==NULL||metadata->campaign_id==0u||metadata->lifecycle_state!=CAMPAIGN_LIFECYCLE_OPEN)return TR2_ERROR_INVALID_ARGUMENT;
    for(i=0u;i<TR2_CAMPAIGN_REPOSITORY_CAPACITY;++i){ CampaignMetadata m; bool valid,unsupported; result=load_slot(store,i,&m,&valid,&unsupported); if(result!=TR2_OK){store->recovery_required=true;return result;} if(unsupported)return TR2_ERROR_UNSUPPORTED; if(valid&&m.campaign_id==metadata->campaign_id)return TR2_ERROR_INVALID_ARGUMENT; if(!valid&&free_slot==TR2_CAMPAIGN_REPOSITORY_CAPACITY)free_slot=i; }
    if(free_slot==TR2_CAMPAIGN_REPOSITORY_CAPACITY)return TR2_ERROR_NOT_AVAILABLE; return write_copy(store,free_slot,0u,metadata);
}

static Tr2Result close_campaign(void *context,const CampaignMetadata *metadata)
{
    CampaignRepositoryStore *store=context; size_t i; Tr2Result result;
    if(!campaign_repository_store_is_initialized(store)||store->recovery_required)return TR2_ERROR_INVALID_STATE;
    if(metadata==NULL||metadata->campaign_id==0u||metadata->lifecycle_state!=CAMPAIGN_LIFECYCLE_CLOSED)return TR2_ERROR_INVALID_ARGUMENT;
    for(i=0u;i<TR2_CAMPAIGN_REPOSITORY_CAPACITY;++i){ CampaignMetadata m; bool valid,unsupported; result=load_slot(store,i,&m,&valid,&unsupported); if(result!=TR2_OK){store->recovery_required=true;return result;} if(unsupported)return TR2_ERROR_UNSUPPORTED; if(valid&&m.campaign_id==metadata->campaign_id)return write_copy(store,i,1u,metadata); }
    return TR2_ERROR_NOT_FOUND;
}

static Tr2Result inventory(void *context,CampaignInventorySummary *summary)
{
    CampaignRepositoryStore *store=context; size_t i; Tr2Result result;
    if(!campaign_repository_store_is_initialized(store)||store->recovery_required)return TR2_ERROR_INVALID_STATE;
    if(summary==NULL)return TR2_ERROR_INVALID_ARGUMENT; memset(summary,0,sizeof(*summary));
    for(i=0u;i<TR2_CAMPAIGN_REPOSITORY_CAPACITY;++i){ CampaignMetadata m; bool valid,unsupported; result=load_slot(store,i,&m,&valid,&unsupported); if(result!=TR2_OK){store->recovery_required=true;return result;} if(unsupported)return TR2_ERROR_UNSUPPORTED; if(valid){summary->total_campaign_count++;summary->valid_campaign_count++;} }
    return TR2_OK;
}

static Tr2Result by_index(void *context,size_t wanted,CampaignMetadata *metadata)
{
    CampaignRepositoryStore *store=context; size_t i,logical=0u; Tr2Result result;
    if(!campaign_repository_store_is_initialized(store)||store->recovery_required)return TR2_ERROR_INVALID_STATE;
    if(metadata==NULL)return TR2_ERROR_INVALID_ARGUMENT;
    for(i=0u;i<TR2_CAMPAIGN_REPOSITORY_CAPACITY;++i){ bool valid,unsupported; result=load_slot(store,i,metadata,&valid,&unsupported); if(result!=TR2_OK){store->recovery_required=true;return result;} if(unsupported)return TR2_ERROR_UNSUPPORTED; if(valid){if(logical==wanted)return TR2_OK;logical++;} }
    memset(metadata,0,sizeof(*metadata));return TR2_ERROR_NOT_FOUND;
}

static Tr2Result by_id(void *context,CampaignId id,CampaignMetadata *metadata)
{
    CampaignRepositoryStore *store=context; size_t i; Tr2Result result;
    if(!campaign_repository_store_is_initialized(store)||store->recovery_required)return TR2_ERROR_INVALID_STATE;
    if(metadata==NULL||id==0u)return TR2_ERROR_INVALID_ARGUMENT;
    for(i=0u;i<TR2_CAMPAIGN_REPOSITORY_CAPACITY;++i){ bool valid,unsupported; result=load_slot(store,i,metadata,&valid,&unsupported); if(result!=TR2_OK){store->recovery_required=true;return result;} if(unsupported)return TR2_ERROR_UNSUPPORTED; if(valid&&metadata->campaign_id==id)return TR2_OK; }
    memset(metadata,0,sizeof(*metadata));return TR2_ERROR_NOT_FOUND;
}

static Tr2Result recover_repo(void *context,CampaignRepositoryRecoveryResult *out)
{
    CampaignRepositoryStore *store=context; bool corrupted=false,unsupported=false; size_t i,copy;
    if(!campaign_repository_store_is_initialized(store))return TR2_ERROR_INVALID_STATE;
    if(out==NULL)return TR2_ERROR_INVALID_ARGUMENT; memset(out,0,sizeof(*out));
    for(i=0u;i<TR2_CAMPAIGN_REPOSITORY_CAPACITY;++i){ CampaignCopy copies[2]; CampaignMetadata m; bool valid,u; memset(copies,0,sizeof(copies));
        for(copy=0u;copy<2u;++copy){ uint8_t r[TR2_CAMPAIGN_RECORD_SIZE]; Tr2Result rr=persistent_storage_core_read(store->storage,copy_offset(i,copy),r,sizeof(r)); Tr2Result dr; if(rr!=TR2_OK){out->status=CAMPAIGN_REPOSITORY_RECOVERY_UNAVAILABLE;return TR2_OK;} if(empty_bytes(r,sizeof(r)))continue; dr=tr2_campaign_record_decode(r,sizeof(r),&copies[copy].metadata); if(dr==TR2_OK)copies[copy].valid=true; else if(dr==TR2_ERROR_UNSUPPORTED){copies[copy].unsupported=true;unsupported=true;} else corrupted=true; }
        valid=select_copy(copies,&m,&u); (void)u; if(valid){out->inventory.total_campaign_count++;out->inventory.valid_campaign_count++;}
    }
    if(out->inventory.valid_campaign_count>0u)out->status=CAMPAIGN_REPOSITORY_RECOVERY_VALID; else if(unsupported)out->status=CAMPAIGN_REPOSITORY_RECOVERY_UNSUPPORTED; else if(corrupted)out->status=CAMPAIGN_REPOSITORY_RECOVERY_CORRUPTED; else out->status=CAMPAIGN_REPOSITORY_RECOVERY_EMPTY;
    store->recovery_required=false; return TR2_OK;
}

Tr2Result campaign_repository_store_init(CampaignRepositoryStore *store,PersistentStorageCore *storage)
{
    if(store==NULL||storage==NULL||!persistent_storage_core_is_initialized(storage))return TR2_ERROR_INVALID_ARGUMENT;
    memset(store,0,sizeof(*store)); store->storage=storage; store->initialized=true; store->interface.context=store; store->interface.reserve_campaign_id=reserve_id; store->interface.open_campaign=open_campaign; store->interface.close_campaign=close_campaign; store->interface.get_inventory_summary=inventory; store->interface.get_campaign_by_index=by_index; store->interface.get_campaign_by_id=by_id; store->interface.recover=recover_repo; return TR2_OK;
}

bool campaign_repository_store_is_initialized(const CampaignRepositoryStore *store)
{
    return store!=NULL&&store->initialized&&store->storage!=NULL&&persistent_storage_core_is_initialized(store->storage);
}

bool campaign_repository_store_recovery_required(const CampaignRepositoryStore *store)
{
    return campaign_repository_store_is_initialized(store)&&store->recovery_required;
}

CampaignRepository *campaign_repository_store_interface(CampaignRepositoryStore *store)
{
    return campaign_repository_store_is_initialized(store)?&store->interface:NULL;
}
