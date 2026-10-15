#include "tr2/application/live_configuration.h"
#include "tr2/common/sha256.h"
#include "tr2/modbus/b4_configuration_codec.h"
#include "tr2/persistence/configuration_record.h"
#include <string.h>

static uint16_t get16(const uint8_t *p) {return (uint16_t)(((uint16_t)p[0]<<8)|p[1]);}
static uint32_t get32(const uint8_t *p) {return ((uint32_t)get16(p)<<16)|get16(p+2);}
static uint64_t get64(const uint8_t *p) {return ((uint64_t)get32(p)<<32)|get32(p+4);}
static void put16(uint8_t *p,uint16_t v) {p[0]=(uint8_t)(v>>8);p[1]=(uint8_t)v;}
static void put32(uint8_t *p,uint32_t v) {put16(p,(uint16_t)(v>>16));put16(p+2,(uint16_t)v);}
static void put64(uint8_t *p,uint64_t v) {put32(p,(uint32_t)(v>>32));put32(p+4,(uint32_t)v);}
static uint32_t crc(const uint8_t *p,size_t n)
{
    uint32_t c=UINT32_MAX;
    size_t i;
    unsigned b;
    for(i=0u;i<n;++i) {
        c^=p[i];
        for(b=0u;b<8u;++b) c=(c&1u)?(c>>1)^UINT32_C(0xEDB88320):c>>1;
    }
    return c^UINT32_MAX;
}
static bool uniform(const uint8_t *p,size_t n,uint8_t v)
{size_t i;for(i=0u;i<n;++i) if(p[i]!=v) return false;return true;}
static bool blank(const uint8_t *p,size_t n)
{return uniform(p,n,0u)||uniform(p,n,0xFFu);}
static void header(uint8_t *p,const char magic[4],uint64_t generation)
{memset(p,0,64u);memcpy(p,magic,4u);put16(p+4,1u);put16(p+6,64u);put64(p+8,generation);}
static void seal(uint8_t *p) {put32(p+60,crc(p,60u));}
static bool record_ok(const uint8_t *p,const char magic[4])
{return memcmp(p,magic,4u)==0&&get16(p+4)==1u&&get16(p+6)==64u&&get32(p+60)==crc(p,60u);}
static bool initialized(const LiveConfiguration *s)
{return s!=NULL&&s->initialized&&s->historical!=NULL&&s->live!=NULL;}
static Tr2Result revoke(LiveConfiguration *s,Tr2Result error)
{
    s->mounted=false;s->has_active=false;s->state=CONFIGURATION_STATE_APPLICATION_ERROR;
    s->error=error==TR2_ERROR_UNSUPPORTED?4u:5u;
    return error;
}
static Tr2Result recovery_error(TransactionalImageRecoveryStatus status)
{
    switch(status) {
    case TRANSACTIONAL_IMAGE_RECOVERY_UNSUPPORTED: return TR2_ERROR_UNSUPPORTED;
    case TRANSACTIONAL_IMAGE_RECOVERY_CORRUPTED: return TR2_ERROR_CORRUPTED;
    case TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE: return TR2_ERROR_UNAVAILABLE;
    default: return TR2_ERROR_NOT_AVAILABLE;
    }
}
static Tr2Result read_history(LiveConfiguration *s,PersistentIdentityRecord *identity)
{
    TransactionalImageRecoveryResult recovery;
    ModbusBlock0Image ignored;
    Tr2Result result=transactional_image_media_recover(s->historical,&recovery);
    if(result!=TR2_OK) return result;
    if(recovery.status!=TRANSACTIONAL_IMAGE_RECOVERY_VALID) return recovery_error(recovery.status);
    if(s->historical->logical_version!=2u) return TR2_ERROR_UNSUPPORTED;
    result=persistent_identity_from_image(s->historical->candidate,
        TR2_TRANSACTIONAL_MEDIA_V2_LOGICAL_SIZE,s->uid,1u,identity,&ignored);
    if(result!=TR2_OK) return result;
    result=persistent_identity_attribution_check(s->assignments,s->assignment_count,
        s->uid,identity->identity.functional.device_id);
    if(result!=TR2_OK) return result;
    s->historical_generation=recovery.generation;
    memcpy(s->historical_slots,s->historical->candidate,sizeof(s->historical_slots));
    return tr2_sha256_bytes(s->historical->candidate,
        TR2_TRANSACTIONAL_MEDIA_V2_LOGICAL_SIZE,s->historical_sha);
}
static Tr2Result check_provenance(const LiveConfiguration *s)
{
    const uint8_t *base=s->live->candidate;
    const uint8_t *chosen=NULL;
    unsigned i;
    for(i=0u;i<2u;++i) {
        const uint8_t *p=base+TR2_LIVE_PROVENANCE_OFFSET+64u*i;
        if(blank(p,64u)) continue;
        if(!record_ok(p,"TR2M")) return TR2_ERROR_CORRUPTED;
        if(chosen!=NULL&&memcmp(chosen,p,64u)!=0) return TR2_ERROR_CORRUPTED;
        chosen=p;
    }
    if(chosen==NULL||get64(chosen+8)!=s->historical_generation||
        memcmp(chosen+16,s->historical_sha,32u)!=0) return TR2_ERROR_INVALID_STATE;
    for(i=0u;i<3u;++i) if(get32(chosen+48+4u*i)!=s->uid[i]) return TR2_ERROR_INVALID_STATE;
    return TR2_OK;
}
static Tr2Result check_r1_control_boundary(const uint8_t *image)
{
    const uint8_t *epoch=image+TR2_LIVE_EPOCH_OFFSET;
    /* R1 creates an anchor, never a pretend transaction engine. Anything R1
     * cannot interpret (including effects/journal admitted by future R2) is
     * refused, never replayed or silently discarded by recovery. */
    if(!record_ok(epoch,"TR2Q")||get64(epoch+8)!=1u||get32(epoch+16)!=1u||
        get16(epoch+20)!=1u||!uniform(epoch+22,38u,0u)||
        !blank(epoch+64u,64u)||
        !uniform(image+TR2_LIVE_EFFECT_OFFSET,128u,0u)||
        !uniform(image+15922u,35840u,0u)||
        !uniform(image+51802u,16u,0u)) return TR2_ERROR_UNSUPPORTED;
    return TR2_OK;
}
static Tr2Result recover_configuration(LiveConfiguration *s)
{
    ActiveConfigurationSnapshot slots[2];
    unsigned i;
    int chosen=-1;
    uint16_t versions[2]={0u,0u};
    bool known[2]={false,false};
    s->last_generation=0u;s->last_revision=0u;s->target_slot=0u;
    for(i=0u;i<2u;++i) {
        const uint8_t *p=s->live->candidate+140u*i;
        Tr2Result result;
        if(blank(p,140u)) continue;
        versions[i]=get16(p+4);
        if(versions[i]==2u) result=tr2_configuration_record_decode_live(p,140u,&slots[i]);
        else result=tr2_configuration_record_decode(p,140u,&slots[i]);
        if(result!=TR2_OK) {
            /* Historical commissioning intentionally retained a non-record
             * marker in slot0. Only byte-identical inherited damage may be
             * ignored; a new broken/unknown record never triggers fallback. */
            if(result==TR2_ERROR_CORRUPTED&&memcmp(p,s->historical_slots+140u*i,140u)==0)
                continue;
            return result;
        }
        known[i]=true;
        if(slots[i].generation==0u||slots[i].config_id==0u) return TR2_ERROR_CORRUPTED;
        if(slots[i].revision_counter>s->last_revision) s->last_revision=slots[i].revision_counter;
        if(chosen>=0&&slots[chosen].generation==slots[i].generation&&
            memcmp(s->live->candidate+140u*(unsigned)chosen,p,140u)!=0)
            return TR2_ERROR_CORRUPTED;
        if(chosen<0||slots[i].generation>s->last_generation) {
            chosen=(int)i;s->last_generation=slots[i].generation;
        }
    }
    s->target_slot=chosen<0?0u:(unsigned)(1-chosen);
    if(!known[0]) s->target_slot=0u;
    else if(!known[1]) s->target_slot=1u;
    s->state=CONFIGURATION_STATE_EMPTY;s->error=0u;
    if(chosen>=0&&versions[chosen]==2u) {
        PreparedConfiguration prepared={0};
        prepared.config_id=slots[chosen].config_id;prepared.payload=slots[chosen].payload;
        if(configuration_validate_live(&prepared,NULL).status!=CONFIGURATION_VALIDATION_VALID)
            return TR2_ERROR_CORRUPTED;
        s->active=slots[chosen];s->has_active=true;s->state=CONFIGURATION_STATE_ACTIVE;
    }
    return TR2_OK;
}
Tr2Result live_configuration_init(LiveConfiguration *s,
    TransactionalImageMedia *historical,TransactionalImageMedia *live,
    const uint32_t uid[3],const PersistentIdentityAssignment *assignments,size_t count)
{
    if(s==NULL||historical==NULL||live==NULL||historical==live||uid==NULL||
        assignments==NULL||count==0u||!historical->initialized||!live->initialized||
        historical->accepts_live||!historical->accepts_v2||!live->accepts_live||
        historical->geometry.superblock_a_base!=0u||historical->geometry.superblock_b_base!=4096u||
        historical->geometry.image_a_base!=8192u||historical->geometry.image_b_base!=65536u||
        historical->geometry.image_area_size!=57344u)
        return TR2_ERROR_INVALID_ARGUMENT;
    memset(s,0,sizeof(*s));s->historical=historical;s->live=live;
    s->assignments=assignments;s->assignment_count=count;memcpy(s->uid,uid,sizeof(s->uid));
    s->initialized=true;return TR2_OK;
}
Tr2Result live_configuration_recover(LiveConfiguration *s)
{
    PersistentIdentityRecord old;
    TransactionalImageRecoveryResult recovery;
    Tr2Result result;
    if(!initialized(s)) return TR2_ERROR_INVALID_STATE;
    s->has_active=false;s->mounted=false;memset(&s->identity,0,sizeof(s->identity));
    result=read_history(s,&old);
    if(result!=TR2_OK) return revoke(s,result);
    result=transactional_image_media_recover(s->live,&recovery);
    if(result!=TR2_OK) return revoke(s,result);
    if(recovery.status!=TRANSACTIONAL_IMAGE_RECOVERY_VALID)
        return revoke(s,recovery_error(recovery.status));
    result=check_provenance(s);
    if(result==TR2_OK) result=check_r1_control_boundary(s->live->candidate);
    if(result==TR2_OK) result=persistent_identity_decode_live(
        s->live->candidate+TR2_PERSISTENT_IDENTITY_OFFSET,96u,&s->identity);
    if(result!=TR2_OK) return revoke(s,result);
    if((s->identity.identity.functional.device_capabilities & UINT16_C(0xFFF3))!=0u)
        return revoke(s,TR2_ERROR_UNSUPPORTED); /* R2/R3 capabilities require their engines. */
    if(memcmp(s->identity.identity.hardware_uid,s->uid,sizeof(s->uid))!=0||
        s->identity.identity.functional.device_id!=old.identity.functional.device_id||
        s->identity.identity.unit_id!=old.identity.unit_id||
        s->identity.identity.functional.hardware_version!=old.identity.functional.hardware_version||
        memcmp(s->identity.identity.functional.serial_number,old.identity.functional.serial_number,16u)!=0||
        memcmp(s->identity.identity.functional.manufacturer,old.identity.functional.manufacturer,8u)!=0)
        return revoke(s,TR2_ERROR_INVALID_STATE);
    result=recover_configuration(s);
    if(result!=TR2_OK) return revoke(s,result);
    s->mounted=true;return TR2_OK;
}
Tr2Result live_configuration_migrate(LiveConfiguration *s)
{
    PersistentIdentityRecord old,successor;
    TransactionalImageRecoveryResult recovery;
    Tr2Result result;
    uint8_t *image,*record;
    unsigned i;
    if(!initialized(s)) return TR2_ERROR_INVALID_STATE;
    s->has_active=false;s->mounted=false;
    /* Check EMPTY before constructing candidate, allowing one shared work
     * buffer for historical and LIVE media without an extra 52 KiB copy. */
    result=transactional_image_media_recover(s->live,&recovery);
    if(result!=TR2_OK) return result;
    if(recovery.status!=TRANSACTIONAL_IMAGE_RECOVERY_EMPTY) return TR2_ERROR_INVALID_STATE;
    result=read_history(s,&old);
    if(result!=TR2_OK) return result;
    if(old.generation>=UINT64_MAX-1u||old.identity.functional.generation==UINT32_MAX)
        return TR2_ERROR_INVALID_STATE;
    successor=old;successor.generation++;
    successor.identity.functional.generation++;
    successor.identity.functional.protocol_version=2u;
    /* Staged R1 descriptor: no acquisition/SD/LIVE capability is advertised.
     * It is NOT bound to the production server. R2/R3 must explicitly upgrade
     * actual capabilities with their real implementations and guards. */
    successor.identity.functional.device_capabilities&=UINT16_C(0x000C);
    image=s->live->candidate;
    memmove(image,s->historical->candidate,TR2_TRANSACTIONAL_MEDIA_V2_LOGICAL_SIZE);
    memset(image+TR2_TRANSACTIONAL_MEDIA_V2_LOGICAL_SIZE,0,
        TR2_LIVE_LOGICAL_SIZE-TR2_TRANSACTIONAL_MEDIA_V2_LOGICAL_SIZE);
    memset(image+15922u,0,35840u); /* no reinterpretation of historical commands */
    memset(image+51802u,0,16u);   /* no inherited BootIntent execution */
    result=persistent_identity_encode_live(&successor,image+TR2_PERSISTENT_IDENTITY_OFFSET);
    if(result!=TR2_OK) return result;
    record=image+TR2_LIVE_EPOCH_OFFSET;
    header(record,"TR2Q",1u);put32(record+16,1u);put16(record+20,1u);seal(record);
    record=image+TR2_LIVE_PROVENANCE_OFFSET;
    header(record,"TR2M",s->historical_generation);memcpy(record+16,s->historical_sha,32u);
    for(i=0u;i<3u;++i) put32(record+48+4u*i,s->uid[i]);
    seal(record);
    result=transactional_image_media_create_live(s->live,image,TR2_LIVE_LOGICAL_SIZE);
    if(result!=TR2_OK) return revoke(s,result);
    return live_configuration_recover(s);
}
Tr2Result live_configuration_apply(LiveConfiguration *s,
    const PreparedConfiguration *prepared,bool running)
{
    ValidatedConfiguration validated;
    ActiveConfigurationSnapshot candidate;
    uint8_t record[140];
    PersistentStorageCore storage;
    Tr2Result result;
    if(!initialized(s)) return TR2_ERROR_INVALID_STATE;
    if(running) return TR2_ERROR_INVALID_STATE; /* no I/O while acquisition runs */
    if(prepared==NULL||prepared->config_id==0u) {
        s->state=CONFIGURATION_STATE_VALIDATION_ERROR;s->error=1u;
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if(tr2_b4_prepared_payload_crc(&prepared->payload)!=prepared->supplied_crc) {
        s->state=CONFIGURATION_STATE_VALIDATION_ERROR;s->error=3u;
        return TR2_ERROR_CORRUPTED;
    }
    if(configuration_validate_live(prepared,&validated).status!=CONFIGURATION_VALIDATION_VALID) {
        s->state=CONFIGURATION_STATE_VALIDATION_ERROR;s->error=2u;
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    result=live_configuration_recover(s);
    if(result!=TR2_OK) return result;
    if(s->last_generation==UINT32_MAX||s->last_revision==UINT32_MAX)
        return TR2_ERROR_INVALID_STATE;
    memset(&candidate,0,sizeof(candidate));candidate.generation=s->last_generation+1u;
    candidate.config_id=validated.config_id;candidate.revision_counter=s->last_revision+1u;
    candidate.payload=validated.payload;
    result=tr2_configuration_record_encode_live(&candidate,record,sizeof(record));
    if(result==TR2_OK) result=persistent_storage_core_init(&storage,
        transactional_image_media_interface(s->live));
    if(result==TR2_OK) result=persistent_storage_core_write(&storage,140u*s->target_slot,record,sizeof(record));
    if(result==TR2_OK) result=persistent_storage_core_commit(&storage);
    if(result!=TR2_OK) {
        s->state=CONFIGURATION_STATE_APPLICATION_ERROR;s->error=5u;
        return result;
    }
    s->active=candidate;s->has_active=true;s->state=CONFIGURATION_STATE_ACTIVE;s->error=0u;
    s->last_generation=candidate.generation;s->last_revision=candidate.revision_counter;
    s->target_slot=1u-s->target_slot;
    return TR2_OK;
}
bool live_configuration_active(const LiveConfiguration *s,ActiveConfigurationSnapshot *snapshot)
{
    if(!initialized(s)||!s->mounted||!s->has_active||!s->live->recovered||
        s->live->recovery_required||snapshot==NULL) return false;
    *snapshot=s->active;return true;
}
Tr2Result live_configuration_project_b4(const LiveConfiguration *s,
    const PreparedConfiguration *prepared,ModbusBlock4Image *image)
{
    ActiveConfigurationSnapshot active;
    ModbusBlock4ProjectionSource source;
    if(!initialized(s)||image==NULL) return TR2_ERROR_INVALID_ARGUMENT;
    source.config_structure_version=2u;source.config_capabilities_mask=1u;
    source.config_state=s->state;source.config_error_code=s->error;source.prepared=prepared;
    source.active=live_configuration_active(s,&active)?&active:NULL;
    return modbus_project_b4(&source,image);
}
Tr2Result live_configuration_identity(const LiveConfiguration *s,PersistentIdentityRecord *identity)
{
    if(!initialized(s)||!s->mounted||s->live->recovery_required||identity==NULL)
        return TR2_ERROR_NOT_AVAILABLE;
    *identity=s->identity;return TR2_OK;
}
