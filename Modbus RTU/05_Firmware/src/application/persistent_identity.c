#include "tr2/application/persistent_identity.h"
#include <string.h>
#include "tr2/persistence/system_persistent_layout.h"
#include "tr2/persistence/boot_intent_record.h"
_Static_assert(TR2_PERSISTENT_IDENTITY_OFFSET == TR2_BOOT_INTENT_STORAGE_OFFSET + TR2_BOOT_INTENT_RECORD_SIZE, "identity must follow historical BootIntent");
_Static_assert(TR2_PERSISTENT_IDENTITY_OFFSET + TR2_PERSISTENT_IDENTITY_SIZE == TR2_TRANSACTIONAL_MEDIA_V2_LOGICAL_SIZE, "V2 image extent");
static void p16(uint8_t *p,uint16_t v){p[0]=(uint8_t)(v>>8);p[1]=(uint8_t)v;}
static void p32(uint8_t *p,uint32_t v){p16(p,(uint16_t)(v>>16));p16(p+2,(uint16_t)v);}
static uint16_t g16(const uint8_t *p){return (uint16_t)(((uint16_t)p[0]<<8)|p[1]);}
static uint32_t g32(const uint8_t *p){return ((uint32_t)g16(p)<<16)|g16(p+2);}
static uint32_t crc(const uint8_t *p,size_t n){uint32_t c=UINT32_MAX;size_t i;for(i=0;i<n;++i){unsigned b;c^=p[i];for(b=0;b<8;++b)c=(c&1u)?(c>>1)^UINT32_C(0xEDB88320):c>>1;}return c^UINT32_MAX;}
static bool ascii(const char *p,size_t n){size_t i;for(i=0;i<n;++i)if(p[i]!=0&&((unsigned char)p[i]<32u||(unsigned char)p[i]>126u))return false;return true;}
static Tr2Result valid(const PersistentIdentityRecord *r){
    if(r==NULL||r->generation==0u||r->generation==UINT64_MAX||
       r->identity.unit_id==0u||r->identity.unit_id>247u||
       (r->identity.functional.device_capabilities&0xFFF0u)!=0u||
       !ascii(r->identity.functional.serial_number,16u)||
       !ascii(r->identity.functional.manufacturer,8u))return TR2_ERROR_INVALID_ARGUMENT;
    return TR2_OK;
}
Tr2Result persistent_identity_encode(const PersistentIdentityRecord *r,uint8_t b[96]){
    const IdentitySnapshot *f;unsigned i;
    if(b==NULL||valid(r)!=TR2_OK)return TR2_ERROR_INVALID_ARGUMENT;
    f=&r->identity.functional;memset(b,0,96);memcpy(b,"TR2D",4);p16(b+4,1u);p16(b+6,96u);
    p32(b+8,(uint32_t)(r->generation>>32));p32(b+12,(uint32_t)r->generation);
    for(i=0;i<3;++i)p32(b+16+4*i,r->identity.hardware_uid[i]);
    p32(b+28,f->device_id);p32(b+32,f->generation);
    p16(b+36,f->hardware_version);p16(b+38,f->firmware_version_major);
    p16(b+40,f->firmware_version_minor);p16(b+42,f->firmware_version_patch);
    p16(b+44,f->protocol_version);p16(b+46,f->device_capabilities);
    memcpy(b+48,f->serial_number,16);memcpy(b+64,f->manufacturer,8);b[72]=r->identity.unit_id;
    p32(b+92,crc(b,92));return TR2_OK;
}
Tr2Result persistent_identity_decode(const uint8_t *b,size_t size,PersistentIdentityRecord *r){
    PersistentIdentityRecord parsed={0};IdentitySnapshot *f=&parsed.identity.functional;unsigned i;
    if(b==NULL||r==NULL||size!=96u)return TR2_ERROR_INVALID_ARGUMENT;
    memset(r,0,sizeof(*r));
    if(memcmp(b,"TR2D",4)!=0||g16(b+6)!=96u||g32(b+92)!=crc(b,92))return TR2_ERROR_CORRUPTED;
    if(g16(b+4)!=1u)return TR2_ERROR_UNSUPPORTED;
    for(i=73;i<92;++i)if(b[i]!=0u)return TR2_ERROR_CORRUPTED;
    parsed.generation=((uint64_t)g32(b+8)<<32)|g32(b+12);
    for(i=0;i<3;++i)parsed.identity.hardware_uid[i]=g32(b+16+4*i);
    f->device_id=g32(b+28);f->generation=g32(b+32);f->hardware_version=g16(b+36);
    f->firmware_version_major=g16(b+38);f->firmware_version_minor=g16(b+40);f->firmware_version_patch=g16(b+42);
    f->protocol_version=g16(b+44);f->device_capabilities=g16(b+46);
    memcpy(f->serial_number,b+48,16);memcpy(f->manufacturer,b+64,8);parsed.identity.unit_id=b[72];
    if(valid(&parsed)!=TR2_OK)return TR2_ERROR_CORRUPTED;
    *r=parsed;return TR2_OK;
}
Tr2Result persistent_identity_from_image(const uint8_t *image,size_t size,
    const uint32_t uid[3],uint16_t protocol,PersistentIdentityRecord *r,ModbusBlock0Image *b0){
    Tr2Result status;
    if(image==NULL||uid==NULL||r==NULL||b0==NULL||size!=TR2_TRANSACTIONAL_MEDIA_V2_LOGICAL_SIZE)return TR2_ERROR_INVALID_ARGUMENT;
    memset(r,0,sizeof(*r));memset(b0,0,sizeof(*b0));
    status=persistent_identity_decode(image+TR2_PERSISTENT_IDENTITY_OFFSET,96u,r);
    if(status!=TR2_OK)return status;
    if(memcmp(uid,r->identity.hardware_uid,12u)!=0){memset(r,0,sizeof(*r));return TR2_ERROR_INVALID_STATE;}
    if(r->identity.functional.protocol_version!=protocol){memset(r,0,sizeof(*r));return TR2_ERROR_UNSUPPORTED;}
    return modbus_project_b0(&r->identity.functional,b0);
}
Tr2Result persistent_identity_provision(TransactionalImageMedia *m,
    const PersistentIdentityProvisioningRequest *q,bool authorized,ProductionProvisioningResult *out){
    PersistentIdentityRecord r;ModbusBlock0Image b0;Tr2Result status;uintptr_t a,b;
    if(out==NULL)return TR2_ERROR_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->recovery.status=TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;
    if(m==NULL||!m->initialized||!m->accepts_v2||q==NULL||q->image==NULL||q->image_size!=TR2_TRANSACTIONAL_MEDIA_V2_LOGICAL_SIZE)return TR2_ERROR_INVALID_ARGUMENT;
    if(!authorized||!q->exclusive_ownership||q->attribution==NULL||q->validate_prefix==NULL)return TR2_ERROR_NOT_AVAILABLE;
    a=(uintptr_t)q->image;b=(uintptr_t)m->candidate;
    if((a>=b&&a-b<m->candidate_size)||(b>=a&&b-a<q->image_size))return TR2_ERROR_INVALID_ARGUMENT;
    status=persistent_identity_from_image(q->image,q->image_size,q->target_uid,q->supported_protocol_version,&r,&b0);
    if(status!=TR2_OK)return status;
    status=q->attribution(q->context,r.identity.hardware_uid,r.identity.functional.device_id);
    if(status!=TR2_OK)return status;
    status=q->validate_prefix(q->context,q->image,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    if(status!=TR2_OK)return status;
    status=transactional_image_media_recover(m,&out->recovery);if(status!=TR2_OK)return status;
    if(out->recovery.status!=TRANSACTIONAL_IMAGE_RECOVERY_VALID&&out->recovery.status!=TRANSACTIONAL_IMAGE_RECOVERY_EMPTY)return TR2_ERROR_INVALID_STATE;
    if(out->recovery.generation!=q->expected_generation)return TR2_ERROR_INVALID_STATE;
    if(out->recovery.status==TRANSACTIONAL_IMAGE_RECOVERY_VALID){
        if(m->logical_version==2u){
            if(memcmp(m->candidate,q->image,q->image_size)!=0)return TR2_ERROR_INVALID_STATE;
            out->unchanged=true;
        }else if(memcmp(m->candidate,q->image,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)!=0)return TR2_ERROR_INVALID_STATE;
    }
    if(!out->unchanged){
        out->write_attempted=true;
        status=transactional_image_media_publish_v2(m,q->image,q->image_size,q->expected_generation);
        if(status!=TR2_OK)return status;
        status=transactional_image_media_recover(m,&out->recovery);
        if(status!=TR2_OK||out->recovery.status!=TRANSACTIONAL_IMAGE_RECOVERY_VALID||m->logical_version!=2u||memcmp(m->candidate,q->image,q->image_size)!=0)return TR2_ERROR_STORAGE;
    }
    out->identity=b0;out->unit_id=r.identity.unit_id;out->identity_available=true;return TR2_OK;
}

Tr2Result persistent_identity_attribution_check(
    const PersistentIdentityAssignment *entries, size_t count,
    const uint32_t uid[3], uint32_t device_id)
{
    size_t i, j;
    bool found = false;
    if (uid == NULL || (entries == NULL && count != 0u))
        return TR2_ERROR_INVALID_ARGUMENT;
    for (i = 0u; i < count; ++i) {
        for (j = 0u; j < i; ++j) {
            if (entries[i].device_id == entries[j].device_id ||
                memcmp(entries[i].hardware_uid, entries[j].hardware_uid,
                       sizeof(entries[i].hardware_uid)) == 0)
                return TR2_ERROR_INVALID_STATE;
        }
        if (memcmp(uid, entries[i].hardware_uid, sizeof(entries[i].hardware_uid)) == 0) {
            if (device_id != entries[i].device_id) return TR2_ERROR_INVALID_STATE;
            found = true;
        }
    }
    return found ? TR2_OK : TR2_ERROR_NOT_AVAILABLE;
}
