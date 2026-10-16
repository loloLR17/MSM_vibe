#include "r4t_maintenance.h"
#include "tr2/common/sha256.h"
#include <string.h>
static bool floor_ok(const LiveCommandStore *s)
{
 return s->mounted&&s->epoch.current==4&&s->epoch.previous==3&&s->epoch.generation==7&&
  !s->epoch.flags&&s->epoch.renew_txid==65535&&s->epoch.transition==11&&s->has_effect&&
  s->effect.generation==15&&s->effect.epoch==4&&s->effect.txid==5&&s->effect.code==13&&
  s->effect.run_id==5&&s->effect.config_id==3&&s->effect.config_generation==3&&s->effect.phase==1&&
  s->configuration->has_active&&s->configuration->active.config_id==3&&s->configuration->active.generation==3;
}
static Tr2Result semantic_capture(R4mMaintenance *m,const LiveCommandEngine *e)
{
 if(!m||!e||!e->store||!e->store->configuration)return TR2_ERROR_INVALID_ARGUMENT;
 memset(m,0,sizeof(*m));const LiveCommandStore *s=e->store;
 if(!e->booted||!e->indeterminate||!e->has_active||!e->has_last||!floor_ok(s)||
  e->active.generation!=26||e->active.phase!=2||e->active.effect_state!=1||
  e->active.request.epoch!=4||e->active.request.command.transaction_id!=5||
  e->active.request.command.identity.command_code!=13||e->active.run_id!=5||
  e->active.config_id!=3||e->active.config_generation!=3||e->last.generation!=24||
  e->last.phase!=3||e->last.status!=4||e->last.result||e->last.request.epoch!=4||
  e->last.request.command.transaction_id!=4||e->last.request.command.identity.command_code!=13||
  e->last.run_id!=4||e->last.config_id!=3||e->last.config_generation!=3||e->last.effect_state!=2)
  return TR2_ERROR_INVALID_STATE;
 static const unsigned slots[5]={0,1,2,3,127};
 static const uint64_t generations[5]={16,20,24,26,12},runs[5]={3,0,4,5,0};
 static const uint16_t tx[5]={2,3,4,5,65535},codes[5]={14,1,13,13,12};
 unsigned count=0;for(unsigned i=0;i<128;i++)if(s->present[i])count++;
 if(count!=5)return TR2_ERROR_INVALID_STATE;
 for(unsigned i=0;i<5;i++) {
  const LiveCommandRecord *r=&s->entries[slots[i]];
  if(!s->present[slots[i]]||r->generation!=generations[i]||r->run_id!=runs[i]||
   r->request.epoch!=(i==4?3u:4u)||r->request.command.transaction_id!=tx[i]||
   r->request.command.identity.command_code!=codes[i]||r->phase!=(i==3?2u:3u)||
   r->status!=(i==3?0u:4u)||r->result)return TR2_ERROR_INVALID_STATE;
 }
 m->epoch_floor=4;m->run_floor=5;m->phase=1;
 memcpy(m->intent,s->configuration->live->candidate+TR2_LIVE_EFFECT_OFFSET,128);return TR2_OK;
}

Tr2Result r4t_capture(R4tRetirement *t,const LiveCommandEngine *e,const uint8_t snapshot[262144])
{
 if(!t||!snapshot||!e||!e->store||!e->store->configuration)return TR2_ERROR_INVALID_ARGUMENT;
 memset(t,0,sizeof(*t));uint8_t hash[32];
 static const uint8_t expected[32]={0x14,0xfb,0x17,0x2f,0x29,0x17,0xd0,0xa8,0xe4,0x40,0x43,0x80,0x51,0x83,0x1c,0x84,0xf3,0xf3,0xe7,0x46,0x16,0x17,0x2,0xa7,0xde,0x9e,0x9a,0x1a,0x25,0x1b,0x2a,0xa9};
 if(tr2_sha256_bytes_cooperative(snapshot,262144,hash,e->store->configuration->live->work)!=TR2_OK||memcmp(hash,expected,32))return TR2_ERROR_INVALID_STATE;
 const TransactionalImageMedia *m=e->store->configuration->live;
 if(m->generation!=39||!m->recovered||m->active_image>1)return TR2_ERROR_INVALID_STATE;
 for(size_t at=0;at<52298;at+=32) {
  size_t n=52298-at<32?52298-at:32;
  if(tr2_cooperative_checkpoint(m->work)!=TR2_OK||memcmp(m->candidate+at,snapshot+(m->active_image?188416u:131072u)+64+at,n))return TR2_ERROR_INVALID_STATE;
 }
 Tr2Result result=semantic_capture(&t->gate,e);if(result!=TR2_OK)return result;
 t->generation_floor=e->store->generation;t->archived=e->active;return TR2_OK;
}
Tr2Result r4t_retire_and_admit(R4tRetirement *t,LiveCommandEngine *e)
{
 if(!t||!e||!e->store||!e->store->configuration||t->gate.phase!=1||t->generation_floor!=26)
  return TR2_ERROR_INVALID_STATE;
 R4mMaintenance current;if(semantic_capture(&current,e)!=TR2_OK||
  memcmp(current.intent,t->gate.intent,128)||e->store->generation!=t->generation_floor)return TR2_ERROR_INVALID_STATE;
 LiveCommandStore *s=e->store;TransactionalImageMedia *m=s->configuration->live;
 PersistentMedia *media=transactional_image_media_interface(m);const uint8_t zero[32]={0};
 t->gate.phase=0; /* consumed before staging, including failure: never retry */
 Tr2Result result=tr2_cooperative_enter(m->work);if(result!=TR2_OK)return result;
 /* Retire only slot3 in the candidate, WITHOUT publishing an empty journal.
  * The real R2 admission supplies generation27 in the SAME outer transaction.
  * Unchanged terminal receipts and authentic INTENT remain byte-identical.
  * This isolated maintenance frontend has no normal ingress, even transiently. */
 for(uint32_t at=0;at<224&&result==TR2_OK;at+=32)
  result=media->write(media->context,15922u+224u*3u+at,zero,32);
 if(result==TR2_OK) {
  s->present[3]=false; /* staged namespace retirement, no durable state yet */
  e->has_active=false;e->indeterminate=false;memset(&e->active,0,sizeof(e->active));
  LiveCommandRequest q={0};q.epoch=4;q.command.transaction_id=65535;q.command.identity.command_code=12;
  result=live_command_engine_submit(e,&q);
  if(result==TR2_OK&&(!e->has_active||e->active.phase!=1||e->active.generation!=27||s->generation!=27||
   e->status!=2||s->effect.run_id!=5||memcmp(t->gate.intent,m->candidate+TR2_LIVE_EFFECT_OFFSET,128)))result=TR2_ERROR_INVALID_STATE;
 }
 Tr2Result end=tr2_cooperative_leave(m->work);if(result==TR2_OK)result=end;
 if(result!=TR2_OK){e->booted=false;s->mounted=false;(void)live_command_engine_fault(e);return result;}
 t->gate.phase=2;return TR2_OK;
}
