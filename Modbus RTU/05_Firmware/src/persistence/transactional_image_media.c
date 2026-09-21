#include "tr2/persistence/transactional_image_media.h"

#include <string.h>

#define MEDIA_MAGIC UINT32_C(0x5452324D)
#define IMAGE_MAGIC UINT32_C(0x54523249)
#define PHYSICAL_VERSION UINT16_C(1)
#define LOGICAL_VERSION UINT16_C(1)

static void put_u16(uint8_t *p, uint16_t v) { p[0]=(uint8_t)(v>>8); p[1]=(uint8_t)v; }
static void put_u32(uint8_t *p, uint32_t v) { p[0]=(uint8_t)(v>>24); p[1]=(uint8_t)(v>>16); p[2]=(uint8_t)(v>>8); p[3]=(uint8_t)v; }
static void put_u64(uint8_t *p, uint64_t v) { put_u32(p,(uint32_t)(v>>32)); put_u32(p+4,(uint32_t)v); }
static uint16_t get_u16(const uint8_t *p) { return (uint16_t)(((uint16_t)p[0]<<8)|p[1]); }
static uint32_t get_u32(const uint8_t *p) { return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3]; }
static uint64_t get_u64(const uint8_t *p) { return ((uint64_t)get_u32(p)<<32)|get_u32(p+4); }

static uint32_t crc32_bytes(const uint8_t *p, size_t n)
{
    uint32_t crc=UINT32_C(0xFFFFFFFF); size_t i;
    for(i=0;i<n;++i){ uint8_t b; crc^=p[i]; for(b=0;b<8;++b) crc=(crc&1u)?(crc>>1)^UINT32_C(0xEDB88320):crc>>1; }
    return crc^UINT32_C(0xFFFFFFFF);
}

static bool range_ok(uint32_t offset, size_t size)
{
    return offset <= TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE &&
           size <= TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE - offset;
}

static uint32_t image_base(uint8_t image)
{
    return image == 0u ? TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE : TR2_TRANSACTIONAL_MEDIA_IMAGE_B_BASE;
}

static bool uniform(const uint8_t *p, size_t n, uint8_t value)
{
    size_t i; for(i=0;i<n;++i) if(p[i]!=value) return false; return true;
}

static Tr2Result physical_read(TransactionalImageMedia *m,uint32_t o,void *b,size_t s)
{ return m->physical.read(m->physical.context,o,b,s); }
static Tr2Result physical_write(TransactionalImageMedia *m,uint32_t o,const void *b,size_t s)
{ return m->physical.write(m->physical.context,o,b,s); }

static void encode_image_header(uint8_t h[64],uint64_t generation,const uint8_t *payload)
{
    memset(h,0,64); put_u32(h,IMAGE_MAGIC); put_u16(h+4,PHYSICAL_VERSION); put_u16(h+6,LOGICAL_VERSION);
    put_u64(h+8,generation); put_u32(h+16,(uint32_t)TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    put_u32(h+20,(uint32_t)TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE);
    put_u32(h+24,crc32_bytes(payload,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE));
    put_u32(h+60,crc32_bytes(h,60));
}

static void encode_superblock(uint8_t s[64],uint64_t generation,uint8_t image)
{
    memset(s,0,64); put_u32(s,MEDIA_MAGIC); put_u16(s+4,PHYSICAL_VERSION); put_u16(s+6,LOGICAL_VERSION);
    put_u64(s+8,generation); s[16]=image; put_u32(s+20,(uint32_t)TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    put_u32(s+24,(uint32_t)TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE);
    put_u32(s+60,crc32_bytes(s,60));
}

typedef enum { REC_EMPTY=0, REC_VALID, REC_UNSUPPORTED, REC_BAD, REC_IO } RecordState;

static RecordState validate_image(TransactionalImageMedia *m,uint8_t image,uint64_t *generation)
{
    uint8_t h[64]; uint8_t block[256]; uint32_t crc=UINT32_C(0xFFFFFFFF); size_t done=0; uint32_t base=image_base(image);
    if(physical_read(m,base,h,sizeof(h))!=TR2_OK) return REC_IO;
    if(uniform(h,sizeof(h),0x00)||uniform(h,sizeof(h),0xFF)) return REC_EMPTY;
    if(get_u32(h)!=IMAGE_MAGIC) return REC_BAD;
    if(get_u16(h+4)!=PHYSICAL_VERSION||get_u16(h+6)!=LOGICAL_VERSION) return REC_UNSUPPORTED;
    if(get_u64(h+8)==0u||get_u64(h+8)==UINT64_MAX) return REC_BAD;
    if(get_u32(h+16)!=(uint32_t)TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE||get_u32(h+20)!=64u) return REC_BAD;
    if(get_u32(h+28)!=0u) return REC_BAD;
    { size_t i; for(i=32;i<60;++i) if(h[i]!=0u) return REC_BAD; }
    if(get_u32(h+60)!=crc32_bytes(h,60)) return REC_BAD;
    while(done<TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE){
        size_t n=TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE-done; size_t i; uint8_t bit;
        if(n>sizeof(block)) n=sizeof(block);
        if(physical_read(m,base+64u+(uint32_t)done,block,n)!=TR2_OK) return REC_IO;
        for(i=0;i<n;++i){ crc^=block[i]; for(bit=0;bit<8;++bit) crc=(crc&1u)?(crc>>1)^UINT32_C(0xEDB88320):crc>>1; }
        done+=n;
    }
    crc^=UINT32_C(0xFFFFFFFF);
    if(crc!=get_u32(h+24)) return REC_BAD;
    *generation=get_u64(h+8); return REC_VALID;
}

static RecordState validate_superblock(TransactionalImageMedia *m,uint8_t copy,uint64_t *generation,uint8_t *image)
{
    uint8_t s[64]; uint64_t image_generation; uint32_t base=copy==0u?TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE:TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE;
    if(physical_read(m,base,s,sizeof(s))!=TR2_OK) return REC_IO;
    if(uniform(s,sizeof(s),0x00)||uniform(s,sizeof(s),0xFF)) return REC_EMPTY;
    if(get_u32(s)!=MEDIA_MAGIC) return REC_BAD;
    if(get_u16(s+4)!=PHYSICAL_VERSION||get_u16(s+6)!=LOGICAL_VERSION) return REC_UNSUPPORTED;
    if(get_u64(s+8)==0u||get_u64(s+8)==UINT64_MAX||s[16]>1u) return REC_BAD;
    if(s[17]!=0u||s[18]!=0u||s[19]!=0u||get_u32(s+20)!=(uint32_t)TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE||get_u32(s+24)!=64u||get_u32(s+28)!=0u) return REC_BAD;
    { size_t i; for(i=32;i<60;++i) if(s[i]!=0u) return REC_BAD; }
    if(get_u32(s+60)!=crc32_bytes(s,60)) return REC_BAD;
    if(validate_image(m,s[16],&image_generation)!=REC_VALID||image_generation!=get_u64(s+8)) return REC_BAD;
    *generation=get_u64(s+8); *image=s[16]; return REC_VALID;
}

static Tr2Result media_read_cb(void *context,uint32_t offset,void *buffer,size_t size)
{
    TransactionalImageMedia *m=context;
    if(m==NULL||!m->recovered||m->recovery_required||buffer==NULL||!range_ok(offset,size)) return TR2_ERROR_INVALID_STATE;
    return physical_read(m,image_base(m->active_image)+64u+offset,buffer,size);
}

static Tr2Result media_write_cb(void *context,uint32_t offset,const void *buffer,size_t size)
{
    TransactionalImageMedia *m=context;
    if(m==NULL||!m->recovered||m->recovery_required||(buffer==NULL&&size!=0u)||!range_ok(offset,size)) return TR2_ERROR_INVALID_STATE;
    if(size!=0u) memcpy(m->candidate+offset,buffer,size);
    return TR2_OK;
}

static Tr2Result media_commit_cb(void *context)
{
    TransactionalImageMedia *m=context; uint8_t header[64],super[64],inactive,target; uint64_t next,verified; RecordState st; uint32_t base;
    if(m==NULL||!m->recovered||m->recovery_required) return TR2_ERROR_INVALID_STATE;
    if(m->generation>=UINT64_MAX-1u){ m->recovery_required=true; return TR2_ERROR_UNSUPPORTED; }
    next=m->generation+1u; inactive=(uint8_t)(1u-m->active_image); base=image_base(inactive);
    encode_image_header(header,next,m->candidate);
    if(physical_write(m,base,header,sizeof(header))!=TR2_OK||
       physical_write(m,base+64u,m->candidate,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)!=TR2_OK){
        m->recovery_required=true; return TR2_ERROR_STORAGE;
    }
    st=validate_image(m,inactive,&verified);
    if(st!=REC_VALID||verified!=next){ m->recovery_required=true; return TR2_ERROR_STORAGE; }
    target=(uint8_t)((m->generation&1u)==0u?0u:1u);
    encode_superblock(super,next,inactive);
    if(physical_write(m,target==0u?TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE:TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE,super,sizeof(super))!=TR2_OK){
        m->recovery_required=true; return TR2_ERROR_STORAGE;
    }
    { uint8_t img; st=validate_superblock(m,target,&verified,&img); if(st!=REC_VALID||verified!=next||img!=inactive){ m->recovery_required=true; return TR2_ERROR_STORAGE; } }
    m->generation=next; m->active_image=inactive; return TR2_OK;
}

Tr2Result transactional_image_media_init(TransactionalImageMedia *m,const TransactionalImagePhysicalStorage *p,uint8_t *candidate,size_t candidate_size)
{
    if(m==NULL||p==NULL||p->read==NULL||p->write==NULL||candidate==NULL||candidate_size<TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE) return TR2_ERROR_INVALID_ARGUMENT;
    memset(m,0,sizeof(*m)); m->physical=*p; m->candidate=candidate; m->initialized=true;
    m->interface.context=m; m->interface.read=media_read_cb; m->interface.write=media_write_cb; m->interface.commit=media_commit_cb; return TR2_OK;
}

Tr2Result transactional_image_media_format_empty(TransactionalImageMedia *m)
{
    uint8_t h[64],s[64]; uint64_t verified; uint8_t img;
    if(m==NULL||!m->initialized) return TR2_ERROR_INVALID_STATE;
    memset(m->candidate,0,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE); encode_image_header(h,1u,m->candidate);
    if(physical_write(m,TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE,h,64)!=TR2_OK||
       physical_write(m,TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE+64u,m->candidate,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)!=TR2_OK||
       validate_image(m,0u,&verified)!=REC_VALID||verified!=1u){ m->recovery_required=true; return TR2_ERROR_STORAGE; }
    encode_superblock(s,1u,0u);
    if(physical_write(m,TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE,s,64)!=TR2_OK||
       validate_superblock(m,0u,&verified,&img)!=REC_VALID||verified!=1u||img!=0u){ m->recovery_required=true; return TR2_ERROR_STORAGE; }
    m->generation=1u; m->active_image=0u; m->recovered=true; m->recovery_required=false; return TR2_OK;
}

Tr2Result transactional_image_media_recover(TransactionalImageMedia *m,TransactionalImageRecoveryResult *out)
{
    RecordState a,b; uint64_t ga=0,gb=0; uint8_t ia=0,ib=0; uint8_t chosen; uint64_t generation;
    if(m==NULL||!m->initialized||out==NULL) return TR2_ERROR_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out)); a=validate_superblock(m,0u,&ga,&ia); b=validate_superblock(m,1u,&gb,&ib);
    if(a==REC_IO||b==REC_IO){out->status=TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;return TR2_OK;}
    /*
     * A valid publication remains authoritative even if the peer copy is a
     * torn record that happens to contain recognizable magic plus incomplete
     * version bytes.  UNSUPPORTED is authoritative only when no valid
     * superblock-image pair survives.
     */
    if(a!=REC_VALID&&b!=REC_VALID){
        if(a==REC_UNSUPPORTED||b==REC_UNSUPPORTED){out->status=TRANSACTIONAL_IMAGE_RECOVERY_UNSUPPORTED;return TR2_OK;}
        uint8_t sa[64],sb[64],ha[64],hb[64];
        if(physical_read(m,TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE,sa,64)!=TR2_OK||physical_read(m,TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE,sb,64)!=TR2_OK||
           physical_read(m,TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE,ha,64)!=TR2_OK||physical_read(m,TR2_TRANSACTIONAL_MEDIA_IMAGE_B_BASE,hb,64)!=TR2_OK){out->status=TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;return TR2_OK;}
        if((uniform(sa,64,0)||uniform(sa,64,0xFF))&&(uniform(sb,64,0)||uniform(sb,64,0xFF))&&(uniform(ha,64,0)||uniform(ha,64,0xFF))&&(uniform(hb,64,0)||uniform(hb,64,0xFF))) out->status=TRANSACTIONAL_IMAGE_RECOVERY_EMPTY;
        else out->status=TRANSACTIONAL_IMAGE_RECOVERY_CORRUPTED;
        return TR2_OK;
    }
    if(a==REC_VALID&&b==REC_VALID&&ga==gb&&(ia!=ib)){out->status=TRANSACTIONAL_IMAGE_RECOVERY_CORRUPTED;return TR2_OK;}
    if(b==REC_VALID&&(a!=REC_VALID||gb>ga)){chosen=ib;generation=gb;}else{chosen=ia;generation=ga;}
    if(physical_read(m,image_base(chosen)+64u,m->candidate,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)!=TR2_OK){out->status=TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;return TR2_OK;}
    m->generation=generation;m->active_image=chosen;m->recovered=true;m->recovery_required=false;
    out->status=TRANSACTIONAL_IMAGE_RECOVERY_VALID;out->generation=generation;out->active_image=chosen;return TR2_OK;
}

PersistentMedia *transactional_image_media_interface(TransactionalImageMedia *m)
{ return m!=NULL&&m->initialized?&m->interface:NULL; }
