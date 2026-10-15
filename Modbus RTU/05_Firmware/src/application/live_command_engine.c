#include "tr2/application/live_command_engine.h"
#include "tr2/application/command_policy.h"
#include "tr2/modbus/b4_configuration_codec.h"
#include <string.h>

static LiveCommandDecision decision(uint16_t status,uint16_t result)
{LiveCommandDecision d={status,result,status==2u};return d;}
LiveCommandDecision live_command_context_decide(uint16_t code,const LiveCommandContext *c)
{
    if(c==NULL)return decision(5,3);
    if(c->nonterminal)return decision(5,13);
    if(code==13u) {
        if(c->maintenance)return decision(8,18);
        if(c->blocking_fault)return decision(5,8);
        if(c->configuration==0||c->configuration==3)return decision(5,22);
        if(c->configuration!=1||c->acquisition!=0)return decision(5,3);
        if(!c->journal_budget)return decision(5,7);
        return decision(2,0);
    }
    if(code==14u) {
        if(c->acquisition==2)return decision(5,3);
        if(c->acquisition!=1)return decision(5,21);
        if(!c->journal_budget)return decision(5,7);
        return decision(2,0);
    }
    return decision(5,3);
}
static bool ready(const LiveCommandEngine *e)
{return e!=NULL&&e->store!=NULL&&e->store->configuration!=NULL;}
static bool same_request(const LiveCommandRequest *a,const LiveCommandRequest *b)
{return a->epoch==b->epoch&&a->command.transaction_id==b->command.transaction_id&&
    command_request_identity_equal(&a->command.identity,&b->command.identity);}
static bool port_available(const LiveCommandEngine *e)
{return e->acquisition.start&&e->acquisition.stop&&e->acquisition.observe;}
static Tr2Result stop_confirmed(LiveCommandEngine *e)
{
    LiveAcquisitionObservation observed={0};Tr2Result result;
    if(e->acquisition.stop==NULL||e->acquisition.observe==NULL)return TR2_ERROR_UNAVAILABLE;
    result=e->acquisition.stop(e->acquisition.context);
    if(result==TR2_OK)result=e->acquisition.observe(e->acquisition.context,&observed);
    return result==TR2_OK&&observed.stopped&&!observed.running?TR2_OK:TR2_ERROR_UNAVAILABLE;
}
Tr2Result live_command_engine_fault(LiveCommandEngine *e)
{
    Tr2Result result;if(!ready(e))return TR2_ERROR_INVALID_ARGUMENT;
    result=stop_confirmed(e);e->blocking_fault=true;e->state=LIVE_FAULT;return result;
}
static Tr2Result storage_fault(LiveCommandEngine *e,Tr2Result error)
{
    (void)live_command_engine_fault(e);e->epoch_status=e->has_active&&e->active.request.command.identity.command_code==12u?5u:3u;
    e->indeterminate=e->has_active;e->status=e->has_active?9u:6u;e->result=e->has_active?0u:8u;
    return error;
}
static void restore_last(LiveCommandEngine *e)
{
    e->has_last=e->store->has_terminal;
    if(e->has_last)e->last=e->store->terminal;
}
static CommandTerminalTimestamp terminal_time(const LiveCommandEngine *e)
{
    CommandTerminalTimestamp t={false,0};
    if(e->acquisition.terminal_time)t=e->acquisition.terminal_time(e->acquisition.context);
    if(!t.available)t.value=0;
    return t;
}
static Tr2Result complete(LiveCommandEngine *e,uint16_t status,uint16_t result)
{
    LiveCommandRecord r=e->active;LiveEpochRecord epoch=e->store->epoch;
    int slot=live_command_store_lookup(e->store,r.request.epoch,r.request.command.transaction_id);
    Tr2Result error;
    if(slot<0||e->store->generation>=UINT64_MAX-1u)return storage_fault(e,TR2_ERROR_UNSUPPORTED);
    r.generation=e->store->generation+1u;r.phase=3u;r.status=status;r.result=result;r.timestamp=terminal_time(e);
    if(r.request.command.identity.command_code==12u&&epoch.flags) {
        if(epoch.generation>=UINT64_MAX-1u)return storage_fault(e,TR2_ERROR_UNSUPPORTED);
        epoch.generation++;epoch.flags=0;
        error=live_command_store_commit(e->store,(unsigned)slot,&r,&epoch,NULL);
    } else error=live_command_store_commit(e->store,(unsigned)slot,&r,NULL,NULL);
    if(error!=TR2_OK)return storage_fault(e,error);
    e->last=r;e->has_last=true;e->has_active=false;e->indeterminate=false;e->status=status;e->result=result;e->detail=r.detail;
    memset(&e->active,0,sizeof(e->active));return TR2_OK;
}
static bool receipt_matches(const LiveCommandEngine *e)
{
    const LiveCommandRecord *r=&e->active;const LiveEffectRecord *f=&e->store->effect;
    uint16_t code=r->request.command.identity.command_code;
    return e->store->has_effect&&f->epoch==r->request.epoch&&f->txid==r->request.command.transaction_id&&
        f->code==code&&f->run_id==r->run_id&&f->config_id==r->config_id&&f->config_generation==r->config_generation&&
        ((r->effect_state==2u&&f->phase==(code==13u?2u:3u))||(r->effect_state==3u&&f->phase==4u));
}
Tr2Result live_command_engine_init(LiveCommandEngine *e,LiveCommandStore *s,const LiveAcquisitionPort *port)
{
    if(e==NULL||s==NULL||s->configuration==NULL||port==NULL)return TR2_ERROR_INVALID_ARGUMENT;
    memset(e,0,sizeof(*e));e->store=s;e->acquisition=*port;e->state=LIVE_BOOT;return TR2_OK;
}
Tr2Result live_command_engine_boot(LiveCommandEngine *e)
{
    unsigned i;Tr2Result result;ActiveConfigurationSnapshot active;
    if(!ready(e))return TR2_ERROR_INVALID_ARGUMENT;
    e->booted=false;e->has_active=false;e->has_last=false;e->indeterminate=false;e->epoch_status=0;
    memset(e->mailbox,0,sizeof(e->mailbox));e->mailbox_epoch=0;
    /* Safety happens even when FRAM cannot be read. It never proves old effect absence. */
    result=stop_confirmed(e);e->blocking_fault=result!=TR2_OK;
    e->state=result==TR2_OK?LIVE_NO_CONFIG:LIVE_FAULT;
    result=live_command_store_recover(e->store);
    if(result!=TR2_OK) {
        e->epoch_status=result==TR2_ERROR_UNSUPPORTED?4u:result==TR2_ERROR_CORRUPTED?2u:3u;
        (void)live_command_engine_fault(e);return result;
    }
    e->epoch_status=1u;e->status=0;e->result=0;e->detail=0;e->booted=true;restore_last(e);
    if(e->state!=LIVE_FAULT&&live_configuration_active(e->store->configuration,&active))e->state=LIVE_READY;
    for(i=0;i<128;i++)if(e->store->present[i]&&e->store->entries[i].phase<3u) {
        e->active=e->store->entries[i];e->has_active=true;
        if(e->active.phase==1u)return complete(e,6u,28u); /* no STARTED: durable effect barrier */
        if(e->active.request.command.identity.command_code==12u&&e->store->epoch.flags&&
            e->store->epoch.previous==e->active.request.epoch&&e->store->epoch.transition==e->active.generation)
            return complete(e,4u,0u);
        if((e->active.request.command.identity.command_code==13u||e->active.request.command.identity.command_code==14u)&&receipt_matches(e))
            return complete(e,e->active.effect_state==3u?6u:4u,e->active.effect_state==3u?29u:0u);
        if(e->active.request.command.identity.command_code==1u&&
           live_configuration_active(e->store->configuration,&active)&&
           active.config_id==e->active.config_id&&active.generation==e->active.config_generation)
            return complete(e,4u,0u);
        e->indeterminate=true;e->status=9u;e->result=0;
        if(e->active.request.command.identity.command_code==12u)e->epoch_status=5u;
    }
    return TR2_OK;
}
Tr2Result live_command_engine_prepare(LiveCommandEngine *e,const PreparedConfiguration *p)
{
    if(!ready(e)||p==NULL)return TR2_ERROR_INVALID_ARGUMENT;
    e->prepared=*p;e->prepared_present=true;return TR2_OK;
}
static Tr2Result refuse(LiveCommandEngine *e,uint16_t status,uint16_t result)
{e->status=status;e->result=result;e->detail=0;return TR2_OK;}
Tr2Result live_command_engine_submit(LiveCommandEngine *e,const LiveCommandRequest *request)
{
    LiveCommandRequest q;LiveCommandRecord r={0};ActiveConfigurationSnapshot config;
    LiveCommandContext context={0};LiveCommandDecision d;LiveEffectRecord intent={0};int slot;uint16_t code,tx;Tr2Result error;
    bool has_config;
    if(!ready(e)||request==NULL||!e->booted)return TR2_ERROR_INVALID_STATE;
    q=*request;code=q.command.identity.command_code;tx=q.command.transaction_id;
    if(q.epoch==0)return refuse(e,5,23);
    /* Only the pending renewal may retain its previous epoch identity. */
    if(!(e->has_active&&e->active.request.epoch==q.epoch)) {
        if(e->epoch_status!=1u||!e->store->mounted)return refuse(e,5,25);
        if(q.epoch<=e->store->epoch.previous)return refuse(e,5,24);
        if(q.epoch!=e->store->epoch.current)return refuse(e,5,25);
    }
    if(tx==0)return refuse(e,5,14);
    if((tx==65535u)!=(code==12u))return refuse(e,5,2);
    if(e->has_active&&e->active.request.epoch==q.epoch&&e->active.request.command.transaction_id==tx) {
        if(!same_request(&q,&e->active.request))return refuse(e,5,27);
        e->status=e->indeterminate?9u:e->active.phase==1u?2u:3u;e->result=0;return TR2_OK;
    }
    if(e->epoch_status!=1u||!e->store->mounted)return refuse(e,5,25);
    if(q.epoch!=e->store->epoch.current)return refuse(e,5,24);
    slot=live_command_store_lookup(e->store,q.epoch,tx);
    if(slot>=0) {
        const LiveCommandRecord *old=&e->store->entries[slot];
        if(!same_request(&q,&old->request))return refuse(e,5,27);
        e->status=old->phase==3u?old->status:old->phase==1u?2u:3u;
        e->result=old->result;e->detail=old->detail;return TR2_OK;
    }
    if(code==0||code>14u)return refuse(e,7,1);
    if(code>=12u||code==1u) {
        if(q.command.identity.param1||q.command.identity.param2||q.command.identity.param3||q.command.identity.confirm_key)return refuse(e,5,2);
    } else {
        CommandRequestPolicyResult policy=command_request_policy_validate(&q.command);
        if(policy!=COMMAND_REQUEST_POLICY_VALID)return refuse(e,5,policy==COMMAND_REQUEST_POLICY_CONFIRMATION_MISSING?9u:2u);
    }
    if(code==12u&&(e->has_active||e->store->epoch.current==UINT32_MAX))return refuse(e,5,26);
    if(e->has_active)return refuse(e,5,13);
    has_config=live_configuration_active(e->store->configuration,&config);
    context.configuration=has_config?(config.payload.acquisition_mode==2u?1u:2u):0u;
    context.acquisition=e->campaign_active?2u:e->state==LIVE_RUNNING||e->state==LIVE_STARTING||e->state==LIVE_STOPPING?1u:0u;
    context.blocking_fault=e->blocking_fault||!port_available(e);context.maintenance=e->maintenance;
    slot=live_command_store_allocate(e->store,code);
    context.journal_budget=slot>=0&&e->acquisition.io_admissible&&e->acquisition.io_admissible(e->acquisition.context,code);
    if(code==13u||code==14u) {
        d=live_command_context_decide(code,&context);if(!d.admit)return refuse(e,d.status,d.result);
    } else if(code==1u) {
        if(context.acquisition)return refuse(e,5,5);
        if(!e->prepared_present||e->prepared.config_id==0u)return refuse(e,5,20);
        if(tr2_b4_prepared_payload_crc(&e->prepared.payload)!=e->prepared.supplied_crc||
           configuration_validate_live(&e->prepared,NULL).status!=CONFIGURATION_VALIDATION_VALID)return refuse(e,5,4);
        if(e->store->configuration->last_generation==UINT32_MAX||e->store->configuration->last_revision==UINT32_MAX)return refuse(e,5,7);
    } else if(code!=12u) {
        /* Legacy production dispatcher is preserved. Unbound successor effects
         * are explicitly incompatible, never redirected into the V1 journal. */
        return refuse(e,5,3);
    }
    if(!context.journal_budget)return refuse(e,5,7);
    /* Numeric reserves do not prove SPI timing. Keep enough generations for
     * the whole operation, STOP and renewal, in addition to the I/O gate. */
    {
        uint64_t reserve=code==13u?12u:code==1u?5u:code==12u&&context.acquisition?8u:4u;
        if(e->store->generation>UINT64_MAX-1u-reserve||
           e->store->configuration->live->generation>UINT64_MAX-1u-reserve)return refuse(e,5,7);
        if(code==12u&&e->store->epoch.generation>UINT64_MAX-3u)return refuse(e,5,26);
    }
    if(e->store->generation>=UINT64_MAX-1u)return refuse(e,5,7);
    if(code==13u&&e->store->has_effect&&e->store->effect.run_id==UINT64_MAX)return refuse(e,5,7);
    r.generation=e->store->generation+1u;r.request=q;r.phase=1u;
    if(code==13u) {
        r.run_id=e->store->has_effect?e->store->effect.run_id+1u:1u;
        r.config_id=config.config_id;r.config_generation=config.generation;
    } else if(code==14u) {
        if(!e->store->has_effect)return storage_fault(e,TR2_ERROR_CORRUPTED);
        r.run_id=e->store->effect.run_id;r.config_id=e->store->effect.config_id;r.config_generation=e->store->effect.config_generation;
    } else if(code==1u) {
        e->captured_prepared=e->prepared;r.config_id=e->prepared.config_id;
        r.config_generation=e->store->configuration->last_generation+1u;
    }
    if(code==13u||code==14u) {
        if(e->store->has_effect&&e->store->effect.generation>=UINT64_MAX-1u)return refuse(e,5,7);
        intent.generation=e->store->has_effect?e->store->effect.generation+1u:1u;
        intent.epoch=q.epoch;intent.txid=tx;intent.code=code;intent.run_id=r.run_id;
        intent.config_id=r.config_id;intent.config_generation=r.config_generation;intent.phase=1u;
    }
    error=live_command_store_commit(e->store,(unsigned)slot,&r,NULL,code==13u||code==14u?&intent:NULL);
    if(error!=TR2_OK)return storage_fault(e,error);
    e->active=r;e->has_active=true;e->status=2u;e->result=0;e->detail=0;return TR2_OK;
}
Tr2Result live_command_engine_step(LiveCommandEngine *e)
{
    LiveCommandRecord r;LiveEpochRecord epoch;LiveEffectRecord effect={0};
    ActiveConfigurationSnapshot config;LiveAcquisitionObservation observation={0};
    Tr2Result error;int slot;uint16_t code;
    if(!ready(e)||!e->has_active||e->indeterminate||!e->store->mounted)return TR2_ERROR_INVALID_STATE;
    r=e->active;epoch=e->store->epoch;code=r.request.command.identity.command_code;
    slot=live_command_store_lookup(e->store,r.request.epoch,r.request.command.transaction_id);
    if(slot<0||e->store->generation>=UINT64_MAX-1u)return storage_fault(e,TR2_ERROR_CORRUPTED);
    r.generation=e->store->generation+1u;
    if(code==13u&&e->blocking_fault&&r.effect_state!=2u)return complete(e,6u,8u);
    if(r.phase==1u) {
        r.phase=2u;
        if(code==13u||code==14u) {
            if(e->store->has_effect&&e->store->effect.generation>=UINT64_MAX-1u)return storage_fault(e,TR2_ERROR_UNSUPPORTED);
            effect.generation=e->store->has_effect?e->store->effect.generation+1u:1u;
            effect.epoch=r.request.epoch;effect.txid=r.request.command.transaction_id;effect.code=code;
            effect.run_id=r.run_id;effect.config_id=r.config_id;effect.config_generation=r.config_generation;effect.phase=1u;r.effect_state=1u;
        }
        error=live_command_store_commit(e->store,(unsigned)slot,&r,NULL,code==13u||code==14u?&effect:NULL);
        if(error!=TR2_OK)return storage_fault(e,error);
        e->active=r;e->status=3u;
        if(code==13u)e->state=LIVE_STARTING;else if(code==14u)e->state=LIVE_STOPPING;
        return TR2_OK;
    }
    if(code==12u) {
        if(epoch.flags)return complete(e,4u,0u);
        if(epoch.current==UINT32_MAX||epoch.generation>=UINT64_MAX-1u)return storage_fault(e,TR2_ERROR_UNSUPPORTED);
        epoch.previous=epoch.current;epoch.current++;epoch.generation++;epoch.renew_txid=65535u;epoch.flags=1u;
        epoch.transition=r.generation;
        error=live_command_store_commit(e->store,(unsigned)slot,&r,&epoch,NULL);
        if(error!=TR2_OK)return storage_fault(e,error);
        e->active=r;return TR2_OK;
    }
    if(r.effect_state==2u)return complete(e,4u,0u);
    if(code==1u) {
        error=live_configuration_apply(e->store->configuration,&e->captured_prepared,false);
        if(error!=TR2_OK)return storage_fault(e,error);
        r.effect_state=2u;
        error=live_command_store_commit(e->store,(unsigned)slot,&r,NULL,NULL);
        if(error!=TR2_OK)return storage_fault(e,error);
        e->active=r;e->state=LIVE_READY;return TR2_OK;
    }
    /* No effect may occur until the immutable STARTED+intent transaction has
     * been verified. An error/uncertain observation is never retried. */
    if(code==13u) {
        if(!live_configuration_active(e->store->configuration,&config)||config.config_id!=r.config_id||config.generation!=r.config_generation)
            return storage_fault(e,TR2_ERROR_INVALID_STATE);
        error=e->acquisition.start(e->acquisition.context,r.run_id,&config);
    } else error=e->acquisition.stop(e->acquisition.context);
    if(error==TR2_OK)error=e->acquisition.observe(e->acquisition.context,&observation);
    if(error!=TR2_OK||(code==13u&&(!observation.running||observation.stopped||!observation.configuration_confirmed||
        observation.run_id!=r.run_id||observation.config_id!=r.config_id||observation.config_generation!=r.config_generation))||
        (code==14u&&(!observation.stopped||observation.running||observation.run_id!=r.run_id||
         observation.config_id!=r.config_id||observation.config_generation!=r.config_generation))) {
        (void)live_command_engine_fault(e);e->indeterminate=true;e->status=9u;e->result=0;return TR2_ERROR_UNAVAILABLE;
    }
    effect=e->store->effect;
    if(effect.generation>=UINT64_MAX-1u)return storage_fault(e,TR2_ERROR_UNSUPPORTED);
    effect.generation++;effect.phase=code==13u?2u:3u;r.effect_state=2u;
    error=live_command_store_commit(e->store,(unsigned)slot,&r,NULL,&effect);
    if(error!=TR2_OK)return storage_fault(e,error);
    e->active=r;e->state=code==13u?LIVE_RUNNING:LIVE_READY;return TR2_OK;
}
static void u32(uint16_t *p,uint32_t v){p[0]=(uint16_t)(v>>16);p[1]=(uint16_t)v;}
Tr2Result live_command_engine_project_b5(const LiveCommandEngine *e,uint16_t out[29])
{
    uint16_t flags=0;ActiveConfigurationSnapshot config;
    if(!ready(e)||out==NULL)return TR2_ERROR_INVALID_ARGUMENT;
    memset(out,0,58);memcpy(out,e->mailbox,16);u32(out+20,e->mailbox_epoch);
    if(e->has_active){out[8]=e->active.request.command.identity.command_code;out[9]=e->active.request.command.transaction_id;u32(out+24,e->active.request.epoch);flags|=2u;}
    out[10]=e->status;out[11]=e->result;out[12]=e->detail;
    if(e->epoch_status==1u&&e->store->mounted&&!e->indeterminate)flags|=1u;
    if(e->maintenance)flags|=8u;
    if(e->state==LIVE_RUNNING||e->state==LIVE_STOPPING)flags|=16u;
    if(e->blocking_fault)flags|=32u;
    if(live_configuration_active(e->store->configuration,&config))flags|=64u;
    if(e->prepared_present)flags|=256u;
    if(e->has_last) {
        const LiveCommandRecord *r=&e->last;out[14]=r->request.command.identity.command_code;out[15]=r->request.command.transaction_id;
        out[16]=r->status;out[17]=r->result;u32(out+26,r->request.epoch);flags|=1024u;
        if(r->timestamp.available){u32(out+18,r->timestamp.value);flags|=2048u;}
    }
    out[13]=flags;out[28]=e->epoch_status;if(e->epoch_status==1u)u32(out+22,e->store->epoch.current);
    return TR2_OK;
}
Tr2Result live_command_engine_read_b5(const LiveCommandEngine *e,uint16_t address,uint16_t *out,uint16_t count)
{
    uint16_t image[29];Tr2Result result;
    if(out==NULL||!count||address<5000u||address>5028u||count>5029u-address)return TR2_ERROR_INVALID_ARGUMENT;
    result=live_command_engine_project_b5(e,image);if(result==TR2_OK)memcpy(out,image+address-5000u,2u*count);return result;
}
Tr2Result live_command_engine_write_b5(LiveCommandEngine *e,uint16_t address,const uint16_t *values,uint16_t count)
{
    uint16_t staged[8];uint32_t epoch;unsigned i;LiveCommandRequest q={0};
    if(!ready(e)||values==NULL||!count||address<5000u||address>5021u||count>5022u-address)return TR2_ERROR_INVALID_ARGUMENT;
    for(i=0;i<count;i++)if(address+i>5007u&&address+i<5020u)return TR2_ERROR_INVALID_ARGUMENT;
    memcpy(staged,e->mailbox,sizeof(staged));epoch=e->mailbox_epoch;
    for(i=0;i<count;i++) {
        unsigned a=address+i;
        if(a<=5007u)staged[a-5000u]=values[i];
        else if(a==5020u)epoch=(epoch&0xFFFFu)|((uint32_t)values[i]<<16);
        else epoch=(epoch&0xFFFF0000u)|values[i];
    }
    memcpy(e->mailbox,staged,sizeof(staged));e->mailbox_epoch=epoch;
    if(staged[7]&0xFFF8u){e->mailbox[7]&=6u;return refuse(e,5,2);}
    if(staged[7]&4u){memset(e->mailbox,0,sizeof(e->mailbox));e->mailbox_epoch=0;return TR2_OK;}
    if(staged[7]&2u){e->mailbox[7]&=(uint16_t)~2u;return refuse(e,5,15);}
    if(!(staged[7]&1u))return TR2_OK;
    e->mailbox[7]&=(uint16_t)~1u;q.epoch=epoch;q.command.transaction_id=staged[1];
    q.command.identity.command_code=staged[0];q.command.identity.param1=staged[2];q.command.identity.param2=staged[3];
    q.command.identity.param3=((uint32_t)staged[4]<<16)|staged[5];q.command.identity.confirm_key=staged[6];
    return live_command_engine_submit(e,&q);
}
