#include "tr2/persistence/live_command_store.h"
#include <string.h>

static uint16_t g16(const uint8_t *p){return (uint16_t)(((uint16_t)p[0]<<8)|p[1]);}
static uint32_t g32(const uint8_t *p){return ((uint32_t)g16(p)<<16)|g16(p+2);}
static uint64_t g64(const uint8_t *p){return ((uint64_t)g32(p)<<32)|g32(p+4);}
static void p16(uint8_t *p,uint16_t v){p[0]=(uint8_t)(v>>8);p[1]=(uint8_t)v;}
static void p32(uint8_t *p,uint32_t v){p16(p,(uint16_t)(v>>16));p16(p+2,(uint16_t)v);}
static void p64(uint8_t *p,uint64_t v){p32(p,(uint32_t)(v>>32));p32(p+4,(uint32_t)v);}
static uint32_t crc(const uint8_t *p,size_t n)
{
    uint32_t c=UINT32_MAX;size_t i;unsigned b;
    for(i=0;i<n;i++){c^=p[i];for(b=0;b<8;b++)c=(c&1u)?(c>>1)^0xEDB88320u:c>>1;}
    return c^UINT32_MAX;
}
static bool uniform(const uint8_t *p,size_t n,uint8_t v)
{size_t i;for(i=0;i<n;i++)if(p[i]!=v)return false;return true;}
static bool blank(const uint8_t *p,size_t n){return uniform(p,n,0)||uniform(p,n,255);}
static void header(uint8_t *b,const char *magic,uint16_t version,size_t size,uint64_t generation)
{memset(b,0,size);memcpy(b,magic,4);p16(b+4,version);p16(b+6,(uint16_t)size);p64(b+8,generation);}
static Tr2Result check(const uint8_t *b,const char *magic,uint16_t version,size_t size)
{
    if(memcmp(b,magic,4)||g16(b+6)!=size||g32(b+size-4)!=crc(b,size-4))return TR2_ERROR_CORRUPTED;
    if(g16(b+4)!=version)return TR2_ERROR_UNSUPPORTED;
    if(g64(b+8)==0||g64(b+8)==UINT64_MAX)return TR2_ERROR_CORRUPTED;
    return TR2_OK;
}
static bool valid_record(const LiveCommandRecord *r)
{
    uint16_t code=r->request.command.identity.command_code,tx=r->request.command.transaction_id;
    if(r->generation==0||r->generation==UINT64_MAX||r->request.epoch==0||tx==0||
       ((tx==65535u)!=(code==12u))||code==0||code>14u||r->phase<1||r->phase>3||
       r->effect_state>3u||(!r->timestamp.available&&r->timestamp.value!=0u))return false;
    if((code==1u||code==13u||code==14u)&&(!r->config_id||!r->config_generation))return false;
    if((code==13u||code==14u)&&!r->run_id)return false;
    if(r->phase<3u)return r->status==0&&r->result==0&&r->detail==0&&!r->timestamp.available;
    return r->status>=4u&&r->status<=8u&&r->result<=29u;
}
Tr2Result live_command_record_encode(const LiveCommandRecord *r,uint8_t b[112])
{
    const CommandRequestIdentity *q;
    if(r==NULL||b==NULL||!valid_record(r))return TR2_ERROR_INVALID_ARGUMENT;
    q=&r->request.command.identity;header(b,"TR2J",4,112,r->generation);
    p32(b+16,r->request.epoch);p16(b+20,r->request.command.transaction_id);p16(b+22,q->command_code);
    p16(b+24,q->param1);p16(b+26,q->param2);p32(b+28,q->param3);p16(b+32,q->confirm_key);
    p16(b+34,r->phase);p16(b+36,r->status);p16(b+38,r->result);p16(b+40,r->detail);
    p16(b+42,r->timestamp.available?1u:0u);p32(b+44,r->timestamp.value);p64(b+48,r->run_id);
    p32(b+56,r->config_id);p32(b+60,r->config_generation);p16(b+64,r->effect_state);
    p32(b+108,crc(b,108));return TR2_OK;
}
Tr2Result live_command_record_decode(const uint8_t b[112],LiveCommandRecord *r)
{
    Tr2Result result;CommandRequestIdentity *q;
    if(b==NULL||r==NULL)return TR2_ERROR_INVALID_ARGUMENT;
    result=check(b,"TR2J",4,112);if(result!=TR2_OK)return result;
    if(!uniform(b+66,42,0)||g16(b+42)>1u)return TR2_ERROR_CORRUPTED;
    memset(r,0,sizeof(*r));q=&r->request.command.identity;r->generation=g64(b+8);
    r->request.epoch=g32(b+16);r->request.command.transaction_id=g16(b+20);q->command_code=g16(b+22);
    q->param1=g16(b+24);q->param2=g16(b+26);q->param3=g32(b+28);q->confirm_key=g16(b+32);
    r->phase=g16(b+34);r->status=g16(b+36);r->result=g16(b+38);r->detail=g16(b+40);
    r->timestamp.available=g16(b+42)!=0;r->timestamp.value=g32(b+44);r->run_id=g64(b+48);
    r->config_id=g32(b+56);r->config_generation=g32(b+60);r->effect_state=g16(b+64);
    return valid_record(r)?TR2_OK:TR2_ERROR_CORRUPTED;
}
static void encode_epoch(const LiveEpochRecord *r,uint8_t b[64])
{
    header(b,"TR2Q",1,64,r->generation);p32(b+16,r->current);p16(b+20,1);
    p32(b+24,r->previous);p16(b+28,r->renew_txid);p16(b+30,r->flags);
    p64(b+32,r->transition);p32(b+60,crc(b,60));
}
static Tr2Result decode_epoch(const uint8_t b[64],LiveEpochRecord *r)
{
    Tr2Result result=check(b,"TR2Q",1,64);if(result!=TR2_OK)return result;
    if(g16(b+20)!=1||g16(b+22)!=0||!uniform(b+40,20,0))return TR2_ERROR_CORRUPTED;
    memset(r,0,sizeof(*r));r->generation=g64(b+8);r->current=g32(b+16);r->previous=g32(b+24);
    r->renew_txid=g16(b+28);r->flags=g16(b+30);r->transition=g64(b+32);
    if(r->current==0||r->flags>1)return TR2_ERROR_CORRUPTED;
    if(r->current==1)return r->previous==0&&r->renew_txid==0&&r->flags==0&&r->transition==0?TR2_OK:TR2_ERROR_CORRUPTED;
    return r->previous==r->current-1u&&r->renew_txid==65535u&&r->transition!=0?TR2_OK:TR2_ERROR_CORRUPTED;
}
static void encode_effect(const LiveEffectRecord *r,uint8_t b[64])
{
    header(b,"TR2E",1,64,r->generation);p32(b+16,r->epoch);p16(b+20,r->txid);p16(b+22,r->code);
    p64(b+24,r->run_id);p32(b+32,r->config_id);p32(b+36,r->config_generation);p16(b+40,r->phase);
    p32(b+60,crc(b,60));
}
static Tr2Result decode_effect(const uint8_t b[64],LiveEffectRecord *r)
{
    Tr2Result result=check(b,"TR2E",1,64);if(result!=TR2_OK)return result;
    if(!uniform(b+42,18,0))return TR2_ERROR_CORRUPTED;
    memset(r,0,sizeof(*r));r->generation=g64(b+8);r->epoch=g32(b+16);r->txid=g16(b+20);
    r->code=g16(b+22);r->run_id=g64(b+24);r->config_id=g32(b+32);r->config_generation=g32(b+36);r->phase=g16(b+40);
    return r->epoch&&r->txid&&r->txid<65535u&&(r->code==13||r->code==14)&&r->run_id&&
        r->config_id&&r->config_generation&&r->phase>=1&&r->phase<=4?TR2_OK:TR2_ERROR_CORRUPTED;
}
static Tr2Result select_pair(const uint8_t *a,size_t size,int *copy)
{
    unsigned i;*copy=-1;
    for(i=0;i<2;i++) {
        const uint8_t *p=a+size*i;
        if(blank(p,size))continue;
        if(g32(p+size-4)!=crc(p,size-4))return TR2_ERROR_CORRUPTED;
        if(*copy>=0&&g64(a+size*(unsigned)*copy+8)==g64(p+8)&&memcmp(a+size*(unsigned)*copy,p,size))return TR2_ERROR_CORRUPTED;
        if(*copy<0||g64(p+8)>g64(a+size*(unsigned)*copy+8))*copy=(int)i;
    }
    return TR2_OK;
}
static Tr2Result validate_control(void *context,const uint8_t *image,size_t size)
{
    LiveCommandStore *s=context;int copy;Tr2Result result;unsigned i,j,nonterminal=0;
    s->mounted=false;s->has_effect=false;s->has_terminal=false;s->generation=0;memset(s->present,0,sizeof(s->present));
    if(size!=52298u||!uniform(image+44594u,7168u,0)||!uniform(image+51802u,16u,0))return TR2_ERROR_CORRUPTED;
    result=select_pair(image+51914u,64,&copy);if(result!=TR2_OK)return result;
    if(copy<0)return TR2_ERROR_NOT_AVAILABLE;
    s->epoch_copy=(uint8_t)copy;result=decode_epoch(image+51914u+64u*(unsigned)copy,&s->epoch);if(result!=TR2_OK)return result;
    {
        const uint8_t *old=image+51914u+64u*(1u-s->epoch_copy);
        if(blank(old,64u)) {
            if(s->epoch.generation!=1u||s->epoch.current!=1u)return TR2_ERROR_CORRUPTED;
        } else {
            LiveEpochRecord previous;
            result=decode_epoch(old,&previous);if(result!=TR2_OK)return result;
            if(s->epoch.generation==previous.generation) {
                if(memcmp(old,image+51914u+64u*s->epoch_copy,64u))return TR2_ERROR_CORRUPTED;
            } else if(s->epoch.generation!=previous.generation+1u)return TR2_ERROR_CORRUPTED;
            else if(s->epoch.current==previous.current) {
                if(previous.flags!=1u||s->epoch.flags!=0u||s->epoch.transition!=previous.transition)return TR2_ERROR_CORRUPTED;
            } else if(previous.current==UINT32_MAX||s->epoch.current!=previous.current+1u||previous.flags!=0u||s->epoch.flags!=1u)
                return TR2_ERROR_CORRUPTED;
        }
    }
    result=select_pair(image+52042u,64,&copy);if(result!=TR2_OK)return result;
    s->effect_copy=copy<0?1u:(uint8_t)copy;
    if(copy>=0){result=decode_effect(image+52042u+64u*(unsigned)copy,&s->effect);if(result!=TR2_OK)return result;s->has_effect=true;}
    for(i=0;i<128;i++) {
        result=select_pair(image+15922u+224u*i,112,&copy);if(result!=TR2_OK)return result;
        s->copy[i]=copy<0?1u:(uint8_t)copy;
        if(copy<0)continue;
        result=live_command_record_decode(image+15922u+224u*i+112u*(unsigned)copy,&s->entries[i]);if(result!=TR2_OK)return result;
        {
            const uint8_t *old=image+15922u+224u*i+112u*(1u-(unsigned)copy);
            if(!blank(old,112u)) {
                LiveCommandRecord previous;
                result=live_command_record_decode(old,&previous);if(result!=TR2_OK)return result;
                if(previous.request.epoch==s->entries[i].request.epoch&&
                   (previous.request.command.transaction_id!=s->entries[i].request.command.transaction_id||
                    !command_request_identity_equal(&previous.request.command.identity,&s->entries[i].request.command.identity)||
                    previous.phase>s->entries[i].phase))return TR2_ERROR_CORRUPTED;
                if(previous.request.epoch>s->entries[i].request.epoch)return TR2_ERROR_CORRUPTED;
                if(previous.phase==3u&&(!s->has_terminal||previous.generation>s->terminal.generation)) {
                    s->terminal=previous;s->has_terminal=true;
                }
            }
        }
        if(s->entries[i].phase==3u&&(!s->has_terminal||s->entries[i].generation>s->terminal.generation)) {
            s->terminal=s->entries[i];s->has_terminal=true;
        }
        s->present[i]=true;
        if(s->entries[i].generation>s->generation)s->generation=s->entries[i].generation;
        if(s->entries[i].request.epoch>s->epoch.current)return TR2_ERROR_CORRUPTED;
        if(s->entries[i].phase<3u) {
            nonterminal++;
            if(s->entries[i].request.epoch!=s->epoch.current&&
               !(s->epoch.flags&&i==127u&&s->entries[i].request.epoch==s->epoch.previous))return TR2_ERROR_CORRUPTED;
        }
        if(i==127u&&s->entries[i].request.command.identity.command_code!=12u)return TR2_ERROR_CORRUPTED;
        if(i==126u&&s->entries[i].request.command.identity.command_code!=4u&&s->entries[i].request.command.identity.command_code!=14u)return TR2_ERROR_CORRUPTED;
        for(j=0;j<i;j++)if(s->present[j]&&(s->entries[j].generation==s->entries[i].generation||
            (s->entries[j].request.epoch==s->entries[i].request.epoch&&
             s->entries[j].request.command.transaction_id==s->entries[i].request.command.transaction_id)))return TR2_ERROR_CORRUPTED;
    }
    if(nonterminal>1u)return TR2_ERROR_CORRUPTED;
    if(s->epoch.flags) {
        const LiveCommandRecord *r=&s->entries[127];
        if(!s->present[127]||r->phase!=2u||r->request.epoch!=s->epoch.previous||
           r->generation!=s->epoch.transition)return TR2_ERROR_CORRUPTED;
    }
    s->mounted=true;return TR2_OK;
}
Tr2Result live_command_store_init(LiveCommandStore *s,LiveConfiguration *c)
{
    if(s==NULL||c==NULL)return TR2_ERROR_INVALID_ARGUMENT;
    memset(s,0,sizeof(*s));s->configuration=c;
    return live_configuration_bind_control(c,s,validate_control);
}
Tr2Result live_command_store_recover(LiveCommandStore *s)
{
    Tr2Result result;
    if(s==NULL||s->configuration==NULL)return TR2_ERROR_INVALID_ARGUMENT;
    s->mounted=false;result=live_configuration_recover(s->configuration);
    if(result!=TR2_OK)s->mounted=false;
    return result;
}
int live_command_store_lookup(const LiveCommandStore *s,uint32_t epoch,uint16_t txid)
{
    unsigned i;for(i=0;i<128;i++)if(s->present[i]&&s->entries[i].request.epoch==epoch&&s->entries[i].request.command.transaction_id==txid)return (int)i;
    return -1;
}
int live_command_store_allocate(const LiveCommandStore *s,uint16_t code)
{
    unsigned i;if(code==12u)return !s->present[127]||s->entries[127].request.epoch<s->epoch.current?127:-1;
    for(i=0;i<126;i++)if(!s->present[i]||s->entries[i].request.epoch<s->epoch.current)return (int)i;
    if((code==4u||code==14u)&&(!s->present[126]||s->entries[126].request.epoch<s->epoch.current))return 126;
    return -1;
}
Tr2Result live_command_store_commit(LiveCommandStore *s,unsigned slot,const LiveCommandRecord *r,
    const LiveEpochRecord *epoch,const LiveEffectRecord *effect)
{
    uint8_t b[112];Tr2Result result=TR2_OK;PersistentMedia *media;unsigned target;
    if(s==NULL||!s->mounted||slot>=128u||r==NULL)return TR2_ERROR_INVALID_STATE;
    media=transactional_image_media_interface(s->configuration->live);
    target=1u-s->copy[slot];
    /* A retired last may occupy the opposite copy during namespace reuse.
     * Keep it until a new terminal exists. The outer image transaction makes
     * rewriting the current logical copy power-loss safe (no in-place FRAM).
     * Otherwise two renewals with no intervening command would lose last. */
    if(r->phase<3u&&s->has_terminal) {
        const uint8_t *old=s->configuration->live->candidate+15922u+224u*slot+112u*target;
        if(!blank(old,112u)&&g64(old+8)==s->terminal.generation&&g16(old+34)==3u&&
           g32(old+16)<r->request.epoch)target=s->copy[slot];
    }
    result=live_command_record_encode(r,b);
    if(result==TR2_OK)result=media->write(media->context,15922u+224u*slot+112u*target,b,112);
    if(result==TR2_OK&&epoch!=NULL) {
        encode_epoch(epoch,b);result=media->write(media->context,51914u+64u*(1u-s->epoch_copy),b,64);
    }
    if(result==TR2_OK&&effect!=NULL) {
        encode_effect(effect,b);result=media->write(media->context,52042u+64u*(1u-s->effect_copy),b,64);
    }
    if(result==TR2_OK)result=media->commit(media->context);
    if(result!=TR2_OK){s->mounted=false;return result;}
    return live_command_store_recover(s);
}
