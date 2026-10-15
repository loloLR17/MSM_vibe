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

static bool physical_region_end(uint32_t base, size_t size, size_t *end)
{
    size_t b=(size_t)base;
    if(end==NULL||size>SIZE_MAX-b) return false;
    *end=b+size;
    return true;
}

static bool regions_overlap(uint32_t a_base,size_t a_size,
                            uint32_t b_base,size_t b_size)
{
    size_t a_end,b_end;
    if(!physical_region_end(a_base,a_size,&a_end)||
       !physical_region_end(b_base,b_size,&b_end)) return true;
    return (size_t)a_base<b_end && (size_t)b_base<a_end;
}

Tr2Result transactional_image_geometry_validate(
    const TransactionalImageGeometry *g)
{
    uint32_t bases[4];
    size_t sizes[4];
    size_t i,j,end;
    const size_t image_min=TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE+
                           TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE;

    if(g==NULL||g->physical_size==0u||
       (size_t)g->image_area_size<image_min) return TR2_ERROR_INVALID_ARGUMENT;
#if SIZE_MAX > UINT32_MAX
    /*
     * A 32-bit physical offset can address at most 2^32 bytes.  On 32-bit
     * targets size_t cannot represent 2^32, so applying this upper-bound
     * expression there would wrap to zero and reject every non-empty medium.
     */
    if(g->physical_size>(size_t)UINT32_MAX+1u)
        return TR2_ERROR_INVALID_ARGUMENT;
#endif

    bases[0]=g->superblock_a_base; sizes[0]=TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE;
    bases[1]=g->superblock_b_base; sizes[1]=TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE;
    bases[2]=g->image_a_base; sizes[2]=(size_t)g->image_area_size;
    bases[3]=g->image_b_base; sizes[3]=(size_t)g->image_area_size;

    for(i=0u;i<4u;++i){
        if(!physical_region_end(bases[i],sizes[i],&end)||
           end>g->physical_size) return TR2_ERROR_INVALID_ARGUMENT;
    }
    for(i=0u;i<4u;++i){
        for(j=i+1u;j<4u;++j){
            if(regions_overlap(bases[i],sizes[i],bases[j],sizes[j]))
                return TR2_ERROR_INVALID_ARGUMENT;
        }
    }
    return TR2_OK;
}

TransactionalImageGeometry transactional_image_geometry_qualification_profile(void)
{
    TransactionalImageGeometry g={
        TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE,
        TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE,
        TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE,
        TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE,
        TR2_TRANSACTIONAL_MEDIA_IMAGE_B_BASE,
        TR2_TRANSACTIONAL_MEDIA_IMAGE_AREA_SIZE
    };
    return g;
}

static bool range_ok(const TransactionalImageMedia *m, uint32_t offset, size_t size)
{
    return offset <= m->logical_size &&
           size <= m->logical_size - offset;
}

static uint32_t image_base(const TransactionalImageMedia *m,uint8_t image)
{
    return image == 0u ? m->geometry.image_a_base : m->geometry.image_b_base;
}

static uint32_t superblock_base(const TransactionalImageMedia *m,uint8_t copy)
{
    return copy == 0u ? m->geometry.superblock_a_base : m->geometry.superblock_b_base;
}

static bool uniform(const uint8_t *p, size_t n, uint8_t value)
{
    size_t i; for(i=0;i<n;++i) if(p[i]!=value) return false; return true;
}

static Tr2Result physical_read(TransactionalImageMedia *m,uint32_t o,void *b,size_t s)
{ return m->physical.read(m->physical.context,o,b,s); }
static Tr2Result physical_write(TransactionalImageMedia *m,uint32_t o,const void *b,size_t s)
{
    if (m->accepts_live) {
        Tr2Result result;
        if (o < TR2_LIVE_PHYSICAL_START || o > TR2_LIVE_PHYSICAL_END ||
            s > TR2_LIVE_PHYSICAL_END - o || m->check_write_ownership == NULL)
            return TR2_ERROR_NOT_AVAILABLE;
        result = m->check_write_ownership(m->ownership_context, o, s);
        if (result != TR2_OK) return result;
    }
    return m->physical.write(m->physical.context,o,b,s);
}

static void encode_image_header(const TransactionalImageMedia *m,uint8_t h[64],uint64_t generation,const uint8_t *payload)
{
    memset(h,0,64); put_u32(h,IMAGE_MAGIC); put_u16(h+4,PHYSICAL_VERSION); put_u16(h+6,m->logical_version);
    put_u64(h+8,generation); put_u32(h+16,(uint32_t)m->logical_size);
    put_u32(h+20,(uint32_t)TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE);
    put_u32(h+24,crc32_bytes(payload,m->logical_size));
    put_u32(h+60,crc32_bytes(h,60));
}

static void encode_superblock(const TransactionalImageMedia *m,uint8_t s[64],uint64_t generation,uint8_t image)
{
    memset(s,0,64); put_u32(s,MEDIA_MAGIC); put_u16(s+4,PHYSICAL_VERSION); put_u16(s+6,m->logical_version);
    put_u64(s+8,generation); s[16]=image; put_u32(s+20,(uint32_t)m->logical_size);
    put_u32(s+24,(uint32_t)TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE);
    put_u32(s+60,crc32_bytes(s,60));
}

typedef enum { REC_EMPTY=0, REC_VALID, REC_UNSUPPORTED, REC_BAD, REC_IO } RecordState;

static bool supported_shape(const TransactionalImageMedia *m, uint16_t version, uint32_t size)
{
    if (m->accepts_live) return version == 3u && size == TR2_LIVE_LOGICAL_SIZE;
    return (version==1u && size==TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE) ||
        (m->accepts_v2 && version==2u && size==TR2_TRANSACTIONAL_MEDIA_V2_LOGICAL_SIZE);
}

static RecordState validate_image(TransactionalImageMedia *m,uint8_t image,uint64_t *generation)
{
    uint8_t h[64]; uint8_t block[256]; uint32_t crc=UINT32_C(0xFFFFFFFF); size_t done=0; uint32_t base=image_base(m,image);
    if(physical_read(m,base,h,sizeof(h))!=TR2_OK) return REC_IO;
    if(uniform(h,sizeof(h),0x00)||uniform(h,sizeof(h),0xFF)) return REC_EMPTY;
    if(get_u32(h)!=IMAGE_MAGIC) return REC_BAD;
    if(get_u16(h+4)!=PHYSICAL_VERSION||!supported_shape(m,get_u16(h+6),get_u32(h+16))) return REC_UNSUPPORTED;
    if(get_u64(h+8)==0u||get_u64(h+8)==UINT64_MAX) return REC_BAD;
    if(get_u32(h+20)!=64u) return REC_BAD;
    if(get_u32(h+28)!=0u) return REC_BAD;
    { size_t i; for(i=32;i<60;++i) if(h[i]!=0u) return REC_BAD; }
    if(get_u32(h+60)!=crc32_bytes(h,60)) return REC_BAD;
    while(done<get_u32(h+16)){
        size_t n=get_u32(h+16)-done; size_t i; uint8_t bit;
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
    uint8_t s[64]; uint64_t image_generation; uint32_t base=superblock_base(m,copy);
    if(physical_read(m,base,s,sizeof(s))!=TR2_OK) return REC_IO;
    if(uniform(s,sizeof(s),0x00)||uniform(s,sizeof(s),0xFF)) return REC_EMPTY;
    if(get_u32(s)!=MEDIA_MAGIC) return REC_BAD;
    if(get_u16(s+4)!=PHYSICAL_VERSION||!supported_shape(m,get_u16(s+6),get_u32(s+20))) return REC_UNSUPPORTED;
    if(get_u64(s+8)==0u||get_u64(s+8)==UINT64_MAX||s[16]>1u) return REC_BAD;
    if(s[17]!=0u||s[18]!=0u||s[19]!=0u||get_u32(s+24)!=64u||get_u32(s+28)!=0u) return REC_BAD;
    { size_t i; for(i=32;i<60;++i) if(s[i]!=0u) return REC_BAD; }
    if(get_u32(s+60)!=crc32_bytes(s,60)) return REC_BAD;
    {
        RecordState image_state=validate_image(m,s[16],&image_generation);
        if(image_state==REC_IO) return REC_IO;
        if(image_state==REC_UNSUPPORTED) return REC_UNSUPPORTED;
        if(image_state!=REC_VALID||image_generation!=get_u64(s+8)) return REC_BAD;
    }
    { uint8_t h[64];
      if(physical_read(m,image_base(m,s[16]),h,64)!=TR2_OK) return REC_IO;
      if(get_u16(h+6)!=get_u16(s+6)||get_u32(h+16)!=get_u32(s+20)) return REC_BAD;
    }
    *generation=get_u64(s+8); *image=s[16]; return REC_VALID;
}

static Tr2Result media_read_cb(void *context,uint32_t offset,void *buffer,size_t size)
{
    TransactionalImageMedia *m=context;
    if(m==NULL||!m->recovered||m->recovery_required||(buffer==NULL&&size!=0u)||!range_ok(m,offset,size)) return TR2_ERROR_INVALID_STATE;
    if(size==0u) return TR2_OK;
    return physical_read(m,image_base(m,m->active_image)+64u+offset,buffer,size);
}

static Tr2Result media_write_cb(void *context,uint32_t offset,const void *buffer,size_t size)
{
    TransactionalImageMedia *m=context;
    if(m==NULL||!m->recovered||m->recovery_required||(buffer==NULL&&size!=0u)||!range_ok(m,offset,size)) return TR2_ERROR_INVALID_STATE;
    if (m->accepts_live) {
        Tr2Result result;
        if (m->check_write_ownership == NULL) return TR2_ERROR_NOT_AVAILABLE;
        result = m->check_write_ownership(m->ownership_context,
            TR2_LIVE_PHYSICAL_START, TR2_LIVE_PHYSICAL_END - TR2_LIVE_PHYSICAL_START);
        if (result != TR2_OK) return result;
    }
    if(size!=0u) memcpy(m->candidate+offset,buffer,size);
    return TR2_OK;
}

static Tr2Result media_commit_cb(void *context)
{
    TransactionalImageMedia *m=context; uint8_t header[64],super[64],inactive,target; uint64_t next,verified; RecordState st; uint32_t base;
    if(m==NULL||!m->recovered||m->recovery_required) return TR2_ERROR_INVALID_STATE;
    if(m->generation>=UINT64_MAX-1u){ m->recovery_required=true; return TR2_ERROR_UNSUPPORTED; }
    next=m->generation+1u; inactive=(uint8_t)(1u-m->active_image); base=image_base(m,inactive);
    encode_image_header(m,header,next,m->candidate);
    /*
     * Finalize an image only after its payload is fully written.  The header
     * carries generation and payload CRC and therefore acts as the image
     * finalization record; the superblock remains the publication record.
     */
    if(physical_write(m,base+64u,m->candidate,m->logical_size)!=TR2_OK||
       physical_write(m,base,header,sizeof(header))!=TR2_OK){
        m->recovery_required=true; return TR2_ERROR_STORAGE;
    }
    st=validate_image(m,inactive,&verified);
    if(st!=REC_VALID||verified!=next){ m->recovery_required=true; return TR2_ERROR_STORAGE; }
    target=(uint8_t)(1u-m->active_superblock);
    encode_superblock(m,super,next,inactive);
    if(physical_write(m,superblock_base(m,target),super,sizeof(super))!=TR2_OK){
        m->recovery_required=true; return TR2_ERROR_STORAGE;
    }
    { uint8_t img; st=validate_superblock(m,target,&verified,&img); if(st!=REC_VALID||verified!=next||img!=inactive){ m->recovery_required=true; return TR2_ERROR_STORAGE; } }
    m->generation=next; m->active_image=inactive; m->active_superblock=target; return TR2_OK;
}

Tr2Result transactional_image_media_init(TransactionalImageMedia *m,const TransactionalImagePhysicalStorage *p,const TransactionalImageGeometry *g,uint8_t *candidate,size_t candidate_size)
{
    if(m==NULL||p==NULL||p->read==NULL||p->write==NULL||g==NULL||candidate==NULL||candidate_size<TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE) return TR2_ERROR_INVALID_ARGUMENT;
    if(transactional_image_geometry_validate(g)!=TR2_OK) return TR2_ERROR_INVALID_ARGUMENT;
    memset(m,0,sizeof(*m)); m->physical=*p; m->geometry=*g;
    m->candidate=candidate; m->candidate_size=candidate_size; m->logical_size=TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE; m->logical_version=LOGICAL_VERSION; m->initialized=true;
    m->interface.context=m; m->interface.read=media_read_cb; m->interface.write=media_write_cb; m->interface.commit=media_commit_cb; return TR2_OK;
}

Tr2Result transactional_image_media_format_empty(TransactionalImageMedia *m)
{
    uint8_t h[64],s[64],invalid[64]={0}; uint64_t verified; uint8_t img;
    if(m==NULL||!m->initialized||m->accepts_live) return TR2_ERROR_INVALID_STATE;

    /*
     * Formatting is explicitly destructive.  Invalidate both publication
     * records first so that no stale higher-generation authority can survive
     * a successful format and later win recovery.
     */
    if(physical_write(m,superblock_base(m,0u),invalid,sizeof(invalid))!=TR2_OK||
       physical_write(m,superblock_base(m,1u),invalid,sizeof(invalid))!=TR2_OK){
        m->recovery_required=true; return TR2_ERROR_STORAGE;
    }

    memset(m->candidate,0,m->logical_size); encode_image_header(m,h,1u,m->candidate);
    if(physical_write(m,image_base(m,0u)+64u,m->candidate,m->logical_size)!=TR2_OK||
       physical_write(m,image_base(m,0u),h,64)!=TR2_OK||
       validate_image(m,0u,&verified)!=REC_VALID||verified!=1u){ m->recovery_required=true; return TR2_ERROR_STORAGE; }
    encode_superblock(m,s,1u,0u);
    if(physical_write(m,superblock_base(m,0u),s,64)!=TR2_OK||
       validate_superblock(m,0u,&verified,&img)!=REC_VALID||verified!=1u||img!=0u){ m->recovery_required=true; return TR2_ERROR_STORAGE; }
    m->generation=1u; m->active_image=0u; m->active_superblock=0u; m->recovered=true; m->recovery_required=false; return TR2_OK;
}

static Tr2Result recover_live(TransactionalImageMedia *m, TransactionalImageRecoveryResult *out)
{
    uint8_t roots[2][64];
    int chosen = -1;
    uint64_t generation = 0u, observed;
    unsigned i;
    bool all_empty = true;
    m->recovered = false;
    m->recovery_required = true;
    memset(out, 0, sizeof(*out));
    out->status = TRANSACTIONAL_IMAGE_RECOVERY_CORRUPTED;
    for (i = 0u; i < 2u; ++i) {
        if (physical_read(m, superblock_base(m, (uint8_t)i), roots[i], 64u) != TR2_OK) {
            out->status = TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;
            return TR2_OK;
        }
        if (uniform(roots[i], 64u, 0u) || uniform(roots[i], 64u, 0xFFu)) continue;
        all_empty = false;
        /* Torn publication records have no authority. Check CRC BEFORE shape. */
        if (get_u32(roots[i] + 60) != crc32_bytes(roots[i], 60u)) continue;
        if (get_u32(roots[i]) != MEDIA_MAGIC ||
            get_u16(roots[i] + 4) != PHYSICAL_VERSION) {
            out->status = TRANSACTIONAL_IMAGE_RECOVERY_UNSUPPORTED;
            return TR2_OK;
        }
        observed = get_u64(roots[i] + 8);
        if (observed == 0u || observed == UINT64_MAX) return TR2_OK;
        if (chosen >= 0 && generation == observed &&
            memcmp(roots[chosen], roots[i], 64u) != 0) return TR2_OK;
        if (chosen < 0 || observed > generation) {
            chosen = (int)i;
            generation = observed;
        }
    }
    if (chosen < 0) {
        if (all_empty) {
            uint8_t header[64];
            for (i = 0u; i < 2u; ++i) {
                if (physical_read(m, image_base(m, (uint8_t)i), header, 64u) != TR2_OK) {
                    out->status = TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;
                    return TR2_OK;
                }
                if (!uniform(header,64u,0u) && !uniform(header,64u,0xFFu)) return TR2_OK;
            }
            out->status = TRANSACTIONAL_IMAGE_RECOVERY_EMPTY;
        }
        return TR2_OK;
    }
    {
        RecordState state;
        uint64_t verified;
        uint8_t image;
        const uint8_t *root = roots[chosen];
        if (!supported_shape(m,get_u16(root+6),get_u32(root+20))) {
            out->status = TRANSACTIONAL_IMAGE_RECOVERY_UNSUPPORTED;
            return TR2_OK;
        }
        /* Validate only the selected publication against its bank. A lower
         * root may legitimately reference an inactive bank already rewritten. */
        state = validate_superblock(m, (uint8_t)chosen, &verified, &image);
        if (state != REC_VALID) {
            if (state == REC_IO) out->status = TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;
            if (state == REC_UNSUPPORTED) out->status = TRANSACTIONAL_IMAGE_RECOVERY_UNSUPPORTED;
            return TR2_OK;
        }
        if (physical_read(m, image_base(m,image)+64u,m->candidate,TR2_LIVE_LOGICAL_SIZE) != TR2_OK) {
            out->status = TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;
            return TR2_OK;
        }
        m->logical_size = TR2_LIVE_LOGICAL_SIZE;
        m->logical_version = 3u;
        m->active_image = image;
        m->active_superblock = (uint8_t)chosen;
        m->generation = verified;
        m->recovered = true;
        m->recovery_required = false;
        out->status = TRANSACTIONAL_IMAGE_RECOVERY_VALID;
        out->generation = verified;
        out->active_image = image;
    }
    return TR2_OK;
}

Tr2Result transactional_image_media_recover(TransactionalImageMedia *m,TransactionalImageRecoveryResult *out)
{
    RecordState a,b; uint64_t ga=0,gb=0; uint8_t ia=0,ib=0; uint8_t chosen; uint64_t generation;
    if(m==NULL||!m->initialized||out==NULL) return TR2_ERROR_INVALID_ARGUMENT;
    if (m->accepts_live) return recover_live(m,out);
    /* Revoke previous authority before every recovery attempt, including failures. */
    m->recovered=false; m->recovery_required=true;
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
        if(physical_read(m,superblock_base(m,0u),sa,64)!=TR2_OK||physical_read(m,superblock_base(m,1u),sb,64)!=TR2_OK||
           physical_read(m,image_base(m,0u),ha,64)!=TR2_OK||physical_read(m,image_base(m,1u),hb,64)!=TR2_OK){out->status=TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;return TR2_OK;}
        if((uniform(sa,64,0)||uniform(sa,64,0xFF))&&(uniform(sb,64,0)||uniform(sb,64,0xFF))&&(uniform(ha,64,0)||uniform(ha,64,0xFF))&&(uniform(hb,64,0)||uniform(hb,64,0xFF))) out->status=TRANSACTIONAL_IMAGE_RECOVERY_EMPTY;
        else out->status=TRANSACTIONAL_IMAGE_RECOVERY_CORRUPTED;
        return TR2_OK;
    }
    { uint8_t record[64]; uint8_t k;
      uint64_t best=a==REC_VALID?ga:0u; if(b==REC_VALID&&gb>best) best=gb;
      for(k=0u;k<2u;++k) if((k==0u?a:b)==REC_UNSUPPORTED){
        if(physical_read(m,superblock_base(m,k),record,64)!=TR2_OK){out->status=TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;return TR2_OK;}
        if(get_u32(record+60)==crc32_bytes(record,60)&&get_u64(record+8)>=best){out->status=TRANSACTIONAL_IMAGE_RECOVERY_UNSUPPORTED;return TR2_OK;}
      }
    }
    if(a==REC_VALID&&b==REC_VALID&&ga==gb&&(ia!=ib)){out->status=TRANSACTIONAL_IMAGE_RECOVERY_CORRUPTED;return TR2_OK;}
    if(b==REC_VALID&&(a!=REC_VALID||gb>ga)){chosen=ib;generation=gb;m->active_superblock=1u;}else{chosen=ia;generation=ga;m->active_superblock=0u;}
    { uint8_t header[64];
      if(physical_read(m,image_base(m,chosen),header,64)!=TR2_OK){out->status=TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;return TR2_OK;}
      m->logical_size=get_u32(header+16);m->logical_version=get_u16(header+6);
    }
    if(physical_read(m,image_base(m,chosen)+64u,m->candidate,m->logical_size)!=TR2_OK){out->status=TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;return TR2_OK;}
    m->generation=generation;m->active_image=chosen;m->recovered=true;m->recovery_required=false;
    out->status=TRANSACTIONAL_IMAGE_RECOVERY_VALID;out->generation=generation;out->active_image=chosen;return TR2_OK;
}

PersistentMedia *transactional_image_media_interface(TransactionalImageMedia *m)
{ return m!=NULL&&m->initialized?&m->interface:NULL; }

Tr2Result transactional_image_media_init_v2(TransactionalImageMedia *m,
    const TransactionalImagePhysicalStorage *p,const TransactionalImageGeometry *g,
    uint8_t *candidate,size_t size)
{
    Tr2Result r;
    if(size<TR2_TRANSACTIONAL_MEDIA_V2_LOGICAL_SIZE || g==NULL ||
       g->image_area_size<64u+TR2_TRANSACTIONAL_MEDIA_V2_LOGICAL_SIZE)
        return TR2_ERROR_INVALID_ARGUMENT;
    r=transactional_image_media_init(m,p,g,candidate,size);
    if(r==TR2_OK) m->accepts_v2=true;
    return r;
}

Tr2Result transactional_image_media_publish_v2(TransactionalImageMedia *m,
    const uint8_t *image,size_t size,uint64_t expected)
{
    TransactionalImageRecoveryResult recovery;
    Tr2Result r;uintptr_t a,b;
    if(m==NULL||!m->initialized||!m->accepts_v2||image==NULL||
       size!=TR2_TRANSACTIONAL_MEDIA_V2_LOGICAL_SIZE) return TR2_ERROR_INVALID_ARGUMENT;
    a=(uintptr_t)image;b=(uintptr_t)m->candidate;
    if((a>=b&&a-b<m->candidate_size)||(b>=a&&b-a<size)) return TR2_ERROR_INVALID_ARGUMENT;
    r=transactional_image_media_recover(m,&recovery);if(r!=TR2_OK) return r;
    if(recovery.status==TRANSACTIONAL_IMAGE_RECOVERY_VALID){
        if(m->generation!=expected) return TR2_ERROR_INVALID_STATE;
        if(m->logical_version==2u){
            return memcmp(m->candidate,image,size)==0?TR2_OK:TR2_ERROR_INVALID_STATE;
        }
        if(memcmp(m->candidate,image,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)!=0)
            return TR2_ERROR_INVALID_STATE;
        memcpy(m->candidate,image,size);m->logical_version=2u;m->logical_size=size;
        return media_commit_cb(m);
    }
    if(recovery.status!=TRANSACTIONAL_IMAGE_RECOVERY_EMPTY || expected!=0u)
        return TR2_ERROR_INVALID_STATE;
    /* EMPTY has no publication to invalidate. Reuse the normal transaction:
     * generation0 -> 1, inactive imageA / superblockA. Never format first. */
    memcpy(m->candidate,image,size);m->logical_version=2u;m->logical_size=size;
    m->generation=0u;m->active_image=1u;m->active_superblock=1u;
    m->recovered=true;m->recovery_required=false;
    return media_commit_cb(m);
}

Tr2Result transactional_image_media_init_live(TransactionalImageMedia *m,
    const TransactionalImagePhysicalStorage *p,uint8_t *candidate,size_t size,
    void *owner_context,Tr2Result (*check_owner)(void *,uint32_t,size_t))
{
    const TransactionalImageGeometry geometry = {262144u,122880u,126976u,131072u,188416u,57344u};
    Tr2Result result;
    if (size < TR2_LIVE_LOGICAL_SIZE) return TR2_ERROR_INVALID_ARGUMENT;
    result = transactional_image_media_init(m,p,&geometry,candidate,size);
    if (result != TR2_OK) return result;
    m->accepts_live = true;
    m->logical_version = 3u;
    m->logical_size = TR2_LIVE_LOGICAL_SIZE;
    m->ownership_context = owner_context;
    m->check_write_ownership = check_owner;
    return TR2_OK;
}

Tr2Result transactional_image_media_create_live(TransactionalImageMedia *m,
    const uint8_t *image,size_t size)
{
    TransactionalImageRecoveryResult recovery;
    Tr2Result result;
    if (m == NULL || !m->initialized || !m->accepts_live || image == NULL ||
        size != TR2_LIVE_LOGICAL_SIZE) return TR2_ERROR_INVALID_ARGUMENT;
    if (m->check_write_ownership == NULL) return TR2_ERROR_NOT_AVAILABLE;
    result = m->check_write_ownership(m->ownership_context,
        TR2_LIVE_PHYSICAL_START,TR2_LIVE_PHYSICAL_END-TR2_LIVE_PHYSICAL_START);
    if (result != TR2_OK) return result;
    result = transactional_image_media_recover(m,&recovery);
    if (result != TR2_OK) return result;
    if (recovery.status != TRANSACTIONAL_IMAGE_RECOVERY_EMPTY) return TR2_ERROR_INVALID_STATE;
    memmove(m->candidate,image,size);
    m->generation = 0u;
    m->active_image = 1u;
    m->active_superblock = 1u;
    m->recovered = true;
    m->recovery_required = false;
    return media_commit_cb(m);
}
