#include "r4m_maintenance.h"
#include <string.h>
Tr2Result r4m_capture(R4mMaintenance *m,const LiveCommandEngine *e)
{
 if(!m||!e||!e->store||!e->store->configuration)return TR2_ERROR_INVALID_ARGUMENT;
 memset(m,0,sizeof(*m));const LiveCommandStore *s=e->store;unsigned count=0;
 for(unsigned i=0;i<128;i++)if(s->present[i])count++;
 /* This narrowly reviewed plan applies only to the single R4 STARTED case.
  * Require an authentic intent already durable; never synthesize a watermark. */
 if(!e->booted||!e->indeterminate||!e->has_active||e->active.phase!=2||
    e->active.request.command.identity.command_code!=13||count!=1||
    s->epoch.current!=1||s->epoch.flags||!s->has_effect||s->effect.phase!=1||
    s->effect.epoch!=e->active.request.epoch||s->effect.txid!=e->active.request.command.transaction_id||
    s->effect.code!=13||s->effect.run_id!=e->active.run_id||
    s->effect.config_id!=e->active.config_id||s->effect.config_generation!=e->active.config_generation||
    e->active.request.command.transaction_id==65535||!s->mounted)
  return TR2_ERROR_INVALID_STATE;
 m->epoch_floor=s->epoch.current;m->run_floor=s->effect.run_id;
 if(!m->run_floor||m->run_floor==UINT64_MAX)return TR2_ERROR_INVALID_STATE;
 memcpy(m->intent,s->configuration->live->candidate+TR2_LIVE_EFFECT_OFFSET,128);
 m->phase=1;return TR2_OK;
}
Tr2Result r4m_isolate_journal(R4mMaintenance *m,LiveCommandEngine *e)
{
 if(!m||m->phase!=1||!e||!e->indeterminate||!e->store->mounted)return TR2_ERROR_INVALID_STATE;
 TransactionalImageMedia *live=e->store->configuration->live;
 PersistentMedia *media=transactional_image_media_interface(live);const uint8_t zero[32]={0};
 m->phase=0; /* one attempt only, including an interrupted/failed publication */
 Tr2Result r=tr2_cooperative_enter(live->work);if(r!=TR2_OK)return r;
 /* Logical staging only. The old published bank/STARTED stays authoritative
  * until payload/readback/header/root publication completes atomically.
  * Epoch, both authentic intent copies and both configurations stay unchanged.
  * CLOSED/RETIRING frontend forbids old START throughout this bridge state. */
 for(uint32_t at=0;at<28672&&r==TR2_OK;at+=32)
  r=media->write(media->context,15922+at,zero,32);
 if(r==TR2_OK)r=media->commit(media->context);
 Tr2Result end=tr2_cooperative_leave(live->work);if(r==TR2_OK)r=end;
 if(r==TR2_OK)m->phase=2;
 return r;
}
bool r4m_authorize(const R4mMaintenance *m,const LiveCommandEngine *e,const LiveCommandRequest *q)
{
 if(!m||!e||!q||!e->store||!e->booted)return false;
 if(m->phase==2)return q->epoch==m->epoch_floor&&e->store->epoch.current==m->epoch_floor&&
  q->command.transaction_id==65535&&q->command.identity.command_code==12;
 return m->phase==3&&e->store->epoch.current>m->epoch_floor&&q->epoch==e->store->epoch.current&&
  e->store->has_effect&&e->store->effect.run_id>=m->run_floor;
}
Tr2Result r4m_open_after_renew(R4mMaintenance *m,const LiveCommandEngine *e)
{
 if(!m||m->phase!=2||!e||!e->booted||e->has_active||!e->has_last||!e->store->mounted||
    e->store->epoch.current!=m->epoch_floor+1||e->store->epoch.previous!=m->epoch_floor||e->store->epoch.flags||
    e->last.request.epoch!=m->epoch_floor||e->last.request.command.transaction_id!=65535||
    e->last.request.command.identity.command_code!=12||e->last.status!=4||e->last.result||
    !e->store->has_effect||e->store->effect.run_id!=m->run_floor||
    memcmp(m->intent,e->store->configuration->live->candidate+TR2_LIVE_EFFECT_OFFSET,128))
  return TR2_ERROR_INVALID_STATE;
 m->phase=3;return TR2_OK;
}

Tr2Result r4m_capture_bridge(R4mMaintenance *m,const LiveCommandEngine *e,const uint8_t intent[128])
{
 if(!m||!e||!intent||!e->store||!e->store->configuration)return TR2_ERROR_INVALID_ARGUMENT;
 memset(m,0,sizeof(*m));const LiveCommandStore *s=e->store;
 if(!e->booted||e->has_active||e->has_last||e->indeterminate||!s->mounted||
    s->epoch.current!=1||s->epoch.generation!=1||s->epoch.flags||!s->has_effect||
    s->effect.phase!=1||s->effect.run_id!=1||s->effect.epoch!=1||s->effect.txid!=1||
    s->effect.code!=13||s->effect.config_id!=2||s->effect.config_generation!=2||
    memcmp(intent,s->configuration->live->candidate+TR2_LIVE_EFFECT_OFFSET,128))return TR2_ERROR_INVALID_STATE;
 for(unsigned i=0;i<128;i++)if(s->present[i])return TR2_ERROR_INVALID_STATE;
 m->epoch_floor=1;m->run_floor=1;memcpy(m->intent,intent,128);m->phase=2;return TR2_OK;
}

Tr2Result r4p_capture(R4mMaintenance *m,const LiveCommandEngine *e)
{
 if(!m||!e||!e->store||!e->store->configuration)return TR2_ERROR_INVALID_ARGUMENT;
 memset(m,0,sizeof(*m));const LiveCommandStore *s=e->store;unsigned count=0;
 for(unsigned i=0;i<128;i++)if(s->present[i])count++;
 if(!e->booted||!e->indeterminate||!e->has_active||!e->has_last||!s->mounted||count!=2||
    !s->present[0]||!s->present[127]||e->active.phase!=2||e->active.generation!=6||
    e->active.request.epoch!=2||e->active.request.command.transaction_id!=1||
    e->active.request.command.identity.command_code!=13||e->active.run_id!=2||
    e->active.config_id!=2||e->active.config_generation!=2||e->active.effect_state!=1||
    e->last.phase!=3||e->last.generation!=4||e->last.request.epoch!=1||
    e->last.request.command.transaction_id!=65535||e->last.request.command.identity.command_code!=12||
    e->last.status!=4||e->last.result||s->epoch.current!=2||s->epoch.previous!=1||
    s->epoch.generation!=3||s->epoch.flags||!s->has_effect||s->effect.phase!=1||
    s->effect.epoch!=2||s->effect.txid!=1||s->effect.code!=13||s->effect.run_id!=2||
    s->effect.config_id!=2||s->effect.config_generation!=2)return TR2_ERROR_INVALID_STATE;
 m->epoch_floor=2;m->run_floor=2;
 memcpy(m->intent,s->configuration->live->candidate+TR2_LIVE_EFFECT_OFFSET,128);
 m->phase=1;return TR2_OK;
}

Tr2Result r4p_capture_bridge(R4mMaintenance *m,const LiveCommandEngine *e,const uint8_t intent[128])
{
 if(!m||!e||!intent||!e->store||!e->store->configuration)return TR2_ERROR_INVALID_ARGUMENT;
 memset(m,0,sizeof(*m));const LiveCommandStore *s=e->store;
 if(!e->booted||e->has_active||e->has_last||e->indeterminate||!s->mounted||
    s->epoch.current!=2||s->epoch.previous!=1||s->epoch.generation!=3||s->epoch.flags||
    !s->has_effect||s->effect.phase!=1||s->effect.run_id!=2||s->effect.epoch!=2||
    s->effect.txid!=1||s->effect.code!=13||s->effect.config_id!=2||s->effect.config_generation!=2||
    memcmp(intent,s->configuration->live->candidate+TR2_LIVE_EFFECT_OFFSET,128))return TR2_ERROR_INVALID_STATE;
 for(unsigned i=0;i<128;i++)if(s->present[i])return TR2_ERROR_INVALID_STATE;
 m->epoch_floor=2;m->run_floor=2;memcpy(m->intent,intent,128);m->phase=2;return TR2_OK;
}
