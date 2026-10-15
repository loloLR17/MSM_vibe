/* Reuse the existing real-store R1 fixture without rerunning its main/test suite.
 * The renamed external entry point keeps its private helpers referenced. */
#define main r1_regression_main
#include "test_live_configuration.c"
#undef main
#include "tr2/application/live_command_engine.h"
#include "live_context_vectors.h"

typedef struct {
    bool running,unconfirmed,fail_start,fail_stop,budget,time_valid;
    uint64_t run;
    uint32_t config_id,config_generation,time;
    unsigned starts,stops,observations;
} Acquisition;
typedef struct {Fixture *f;LiveCommandStore store;LiveCommandEngine engine;Acquisition acquisition;} Bench;
static Tr2Result start_acq(void *ctx,uint64_t run,const ActiveConfigurationSnapshot *c)
{
    Acquisition *a=ctx;a->starts++;if(a->fail_start)return TR2_ERROR_UNAVAILABLE;
    a->running=true;a->run=run;a->config_id=c->config_id;a->config_generation=c->generation;return TR2_OK;
}
static Tr2Result stop_acq(void *ctx)
{Acquisition *a=ctx;a->stops++;if(a->fail_stop)return TR2_ERROR_UNAVAILABLE;a->running=false;return TR2_OK;}
static Tr2Result observe_acq(void *ctx,LiveAcquisitionObservation *o)
{
    Acquisition *a=ctx;a->observations++;memset(o,0,sizeof(*o));o->running=a->running;
    o->stopped=!a->running;o->configuration_confirmed=!a->unconfirmed;
    o->run_id=a->run;o->config_id=a->config_id;o->config_generation=a->config_generation;return TR2_OK;
}
static bool io_budget(void *ctx,uint16_t code){(void)code;return ((Acquisition*)ctx)->budget;}
static CommandTerminalTimestamp clock_now(void *ctx)
{Acquisition *a=ctx;CommandTerminalTimestamp t={a->time_valid,a->time};return t;}
static void engine_boot(Bench *b)
{
    LiveAcquisitionPort port={&b->acquisition,start_acq,stop_acq,observe_acq,io_budget,clock_now};
    boot(b->f);assert(live_command_store_init(&b->store,&b->f->service)==TR2_OK);
    assert(live_command_engine_init(&b->engine,&b->store,&port)==TR2_OK);
    assert(live_command_engine_boot(&b->engine)==TR2_OK);
}
static Bench *bench(void)
{
    Bench *b=calloc(1,sizeof(*b));PreparedConfiguration q=prepared(vector_1_prepared,2u);
    assert(b);b->f=fixture();assert(live_configuration_migrate(&b->f->service)==TR2_OK);
    assert(live_configuration_apply(&b->f->service,&q,false)==TR2_OK);
    b->acquisition.budget=true;engine_boot(b);return b;
}
static void destroy(Bench *b){free(b->f);free(b);}
static LiveCommandRequest request(uint32_t epoch,uint16_t id,uint16_t code)
{LiveCommandRequest q={0};q.epoch=epoch;q.command.transaction_id=id;q.command.identity.command_code=code;return q;}
static void run_command(Bench *b,const LiveCommandRequest *q)
{
    unsigned steps=0;assert(live_command_engine_submit(&b->engine,q)==TR2_OK);
    assert(b->engine.has_active);
    while(b->engine.has_active&&steps++<5u)assert(live_command_engine_step(&b->engine)==TR2_OK);
    assert(!b->engine.has_active&&b->engine.status==4u&&b->engine.result==0u);
}
static void contexts_test(void)
{
    unsigned i;for(i=0;i<sizeof(contexts)/sizeof(contexts[0]);i++) {
        LiveCommandContext c={contexts[i].config,contexts[i].acq,contexts[i].fault!=0,
            contexts[i].maintenance!=0,contexts[i].budget!=0,contexts[i].busy!=0};
        LiveCommandDecision d=live_command_context_decide((uint16_t)contexts[i].code,&c);
        if(d.status!=contexts[i].status||d.result!=contexts[i].result) {
            fprintf(stderr,"context %u: %u/%u expected %u/%u\n",i,d.status,d.result,contexts[i].status,contexts[i].result);assert(false);
        }
    }
    puts("384 independent R0 START/STOP context rows: PASS");
}
static void lifecycle_and_epoch(void)
{
    Bench *b=bench();LiveCommandRequest start=request(1,1,13),stop=request(1,2,14),renew=request(1,65535,12),q;
    uint16_t image[29];unsigned starts,stops;size_t writes;uint8_t *historical=malloc(122880u);assert(historical);
    memcpy(historical,b->f->physical.bytes,122880u);
    b->acquisition.time_valid=true;b->acquisition.time=123u;
    {
        size_t reads=b->f->physical.read_bytes,written=b->f->physical.written;
        run_command(b,&start);
        printf("START real-store I/O: %zu bytes read, %zu bytes written\n",
            b->f->physical.read_bytes-reads,b->f->physical.written-written);
        assert(b->f->physical.written-written==4u*52426u);
    }
    assert(b->engine.state==LIVE_RUNNING&&b->acquisition.starts==1u);
    assert(b->store.has_effect&&b->store.effect.phase==2u&&b->store.effect.run_id==1u);
    starts=b->acquisition.starts;writes=b->f->physical.writes;
    assert(live_command_engine_submit(&b->engine,&start)==TR2_OK&&b->engine.status==4u);
    assert(b->acquisition.starts==starts&&b->f->physical.writes==writes);
    q=start;q.command.identity.param1=1u;
    assert(live_command_engine_submit(&b->engine,&q)==TR2_OK&&b->engine.result==27u);
    assert(b->engine.last.timestamp.available&&b->engine.last.timestamp.value==123u);
    q=request(0,0,99);assert(live_command_engine_submit(&b->engine,&q)==TR2_OK&&b->engine.result==23u);
    q=request(2,0,99);assert(live_command_engine_submit(&b->engine,&q)==TR2_OK&&b->engine.result==25u);
    q=request(1,0,99);assert(live_command_engine_submit(&b->engine,&q)==TR2_OK&&b->engine.result==14u);
    q=request(1,65535,13);assert(live_command_engine_submit(&b->engine,&q)==TR2_OK&&b->engine.result==2u);
    q=request(1,3,15);assert(live_command_engine_submit(&b->engine,&q)==TR2_OK&&b->engine.status==7u&&b->engine.result==1u);
    q=request(1,4,1);writes=b->f->physical.writes;
    assert(live_command_engine_submit(&b->engine,&q)==TR2_OK&&b->engine.result==5u&&b->f->physical.writes==writes);
    /* Renew while LIVE does not affect acquisition or create a campaign. */
    assert(live_command_engine_submit(&b->engine,&renew)==TR2_OK);
    assert(live_command_engine_step(&b->engine)==TR2_OK);
    assert(live_command_engine_step(&b->engine)==TR2_OK&&b->store.epoch.current==2u);
    assert(live_command_engine_project_b5(&b->engine,image)==TR2_OK);
    assert(image[23]==2u&&image[25]==1u&&image[9]==65535u&&image[8]==12u);
    assert(live_command_engine_submit(&b->engine,&renew)==TR2_OK&&b->store.epoch.current==2u);
    assert(live_command_engine_step(&b->engine)==TR2_OK&&!b->engine.has_active);
    assert(b->acquisition.starts==1u&&b->acquisition.running);
    assert(live_command_engine_submit(&b->engine,&start)==TR2_OK&&b->engine.result==24u);
    stop.epoch=2u;
    assert(live_command_engine_submit(&b->engine,&stop)==TR2_OK);
    assert(live_command_engine_step(&b->engine)==TR2_OK);
    assert(live_command_engine_project_b5(&b->engine,image)==TR2_OK&&(image[13]&16u));
    assert(live_command_engine_step(&b->engine)==TR2_OK);
    assert(live_command_engine_step(&b->engine)==TR2_OK);
    assert(b->engine.state==LIVE_READY&&!b->acquisition.running);
    stops=b->acquisition.stops;assert(live_command_engine_submit(&b->engine,&stop)==TR2_OK&&b->engine.status==4u&&b->acquisition.stops==stops);
    engine_boot(b);assert(!b->acquisition.running&&!b->engine.has_active&&b->engine.state==LIVE_READY);
    assert(live_command_engine_submit(&b->engine,&stop)==TR2_OK&&b->engine.status==4u);
    assert(b->engine.last.request.epoch==2u&&b->engine.last.timestamp.value==123u);
    assert(memcmp(historical,b->f->physical.bytes,122880u)==0);
    free(historical);destroy(b);puts("Durable START/STOP, retry/collision priority, renewal/reboot/timestamps, original FRAM: PASS");
}
static void recovery_phases(void)
{
    unsigned phase;for(phase=0;phase<=3;phase++) {
        Bench *b=bench();LiveCommandRequest q=request(1,1,13);unsigned i,starts;
        assert(live_command_engine_submit(&b->engine,&q)==TR2_OK);
        for(i=0;i<phase;i++)assert(live_command_engine_step(&b->engine)==TR2_OK);
        starts=b->acquisition.starts;engine_boot(b);
        assert(!b->acquisition.running&&b->acquisition.starts==starts);
        if(phase==0u)assert(!b->engine.has_active&&b->engine.status==6u&&b->engine.result==28u);
        if(phase==1u)assert(b->engine.has_active&&b->engine.status==9u&&b->engine.indeterminate);
        if(phase>=2u)assert(!b->engine.has_active&&b->engine.has_last&&b->engine.last.status==4u);
        assert(live_command_engine_submit(&b->engine,&q)==TR2_OK&&b->acquisition.starts==starts);
        if(phase==1u) {
            LiveCommandRequest another=request(1,2,13);assert(live_command_engine_submit(&b->engine,&another)==TR2_OK&&b->engine.result==13u);
        }
        if(phase==0u) { /* aborted RESERVED run IDs cannot be reused */
            q.command.transaction_id=2u;run_command(b,&q);assert(b->store.effect.run_id==2u);
        }
        destroy(b);
    }
    for(phase=1u;phase<=2u;phase++) {
        Bench *b=bench();LiveCommandRequest q=request(1,65535,12);unsigned i;
        assert(live_command_engine_submit(&b->engine,&q)==TR2_OK);
        for(i=0;i<phase;i++)assert(live_command_engine_step(&b->engine)==TR2_OK);
        engine_boot(b);
        if(phase==1u)assert(b->engine.status==9u&&b->engine.epoch_status==5u);
        else assert(b->store.epoch.current==2u&&!b->engine.has_active&&b->engine.last.status==4u);
        assert(live_command_engine_submit(&b->engine,&q)==TR2_OK&&b->store.epoch.current<=2u);
        destroy(b);
    }
    {
        Bench *b=bench();LiveCommandRequest q=request(1,65535,12);
        run_command(b,&q);q.epoch=2u;
        assert(live_command_engine_submit(&b->engine,&q)==TR2_OK);
        assert(live_command_engine_step(&b->engine)==TR2_OK);engine_boot(b);
        assert(b->engine.status==9u&&b->engine.has_last&&b->engine.last.request.epoch==1u);
        destroy(b);
    }
    puts("Recovery RESERVED28 / STARTED9 / durable receipt / renewal N+1, no redispatch: PASS");
}
static void effect_and_storage_faults(void)
{
    Bench *b=bench();LiveCommandRequest q=request(1,1,13);uint16_t out[29];
    b->acquisition.unconfirmed=true;assert(live_command_engine_submit(&b->engine,&q)==TR2_OK);
    assert(live_command_engine_step(&b->engine)==TR2_OK);
    assert(live_command_engine_step(&b->engine)==TR2_ERROR_UNAVAILABLE);
    assert(b->engine.status==9u&&!b->acquisition.running&&b->engine.state==LIVE_FAULT);
    assert(live_command_engine_project_b5(&b->engine,out)==TR2_OK&&out[8]==13u&&out[9]==1u&&out[10]==9u);
    destroy(b);
    b=bench();run_command(b,&q);q=request(1,2,14);
    assert(live_command_engine_submit(&b->engine,&q)==TR2_OK);
    b->f->physical.budget=0u;
    assert(live_command_engine_step(&b->engine)!=TR2_OK);
    assert(!b->acquisition.running&&b->engine.state==LIVE_FAULT&&b->engine.status!=4u);
    b->f->physical.budget=SIZE_MAX;engine_boot(b);
    assert(b->engine.last.result==28u&&!b->acquisition.running);
    destroy(b);puts("Unconfirmed sensor and FRAM failure: safety stop, no false success: PASS");
}
static void mailbox_and_apply(void)
{
    Bench *b=bench();uint16_t epoch[2]={0,1},fields[8]={13,7,0,0,0,0,0,1},out[29];
    PreparedConfiguration p=prepared(vector_2_prepared,3u);LiveCommandRequest apply=request(1,8,1);
    assert(live_command_engine_write_b5(&b->engine,5020,epoch,2)==TR2_OK);
    assert(live_command_engine_write_b5(&b->engine,5000,fields,8)==TR2_OK);
    assert(b->engine.has_active&&b->engine.active.request.command.transaction_id==7u);
    assert(live_command_engine_read_b5(&b->engine,5000,out,29)==TR2_OK&&out[7]==0&&out[9]==7u);
    fields[0]=99;fields[1]=99;
    assert(live_command_engine_write_b5(&b->engine,5000,fields,2)==TR2_OK);
    assert(b->engine.active.request.command.identity.command_code==13u);
    assert(live_command_engine_write_b5(&b->engine,5007,fields,2)==TR2_ERROR_INVALID_ARGUMENT);
    {uint16_t clear=4;assert(live_command_engine_write_b5(&b->engine,5007,&clear,1)==TR2_OK);assert(b->engine.mailbox_epoch==0u);}
    /* Reboot RESERVED, not a second physical START. Then APPLY real R1 store. */
    engine_boot(b);assert(b->engine.last.result==28u);
    assert(live_command_engine_prepare(&b->engine,&p)==TR2_OK);run_command(b,&apply);
    {ActiveConfigurationSnapshot active;assert(live_configuration_active(&b->f->service,&active)&&active.config_id==3u);}
    engine_boot(b);assert(b->engine.last.request.command.identity.command_code==1u&&b->engine.last.status==4u);
    destroy(b);puts("29-register mailbox, RO atomic refusal, immutable capture, actual APPLY: PASS");
}
static void saturation_and_historical_time(void)
{
    Bench *b=bench();PreparedConfiguration p=prepared(vector_1_prepared,2u);
    LiveCommandRequest q=request(1,1,1),retry=q;unsigned i;uint16_t image[29];
    assert(live_command_engine_prepare(&b->engine,&p)==TR2_OK);run_command(b,&q);
    for(i=2u;i<=126u;i++) {q=request(1,(uint16_t)i,i%2u?14u:13u);run_command(b,&q);}
    assert(b->engine.state==LIVE_RUNNING&&live_command_store_allocate(&b->store,13u)<0);
    assert(live_command_store_allocate(&b->store,14u)==126&&live_command_store_allocate(&b->store,12u)==127);
    assert(live_command_engine_submit(&b->engine,&retry)==TR2_OK&&b->engine.status==4u);
    q=request(1,127,14);run_command(b,&q);assert(b->store.present[126]);
    q=request(1,65535,12);run_command(b,&q);assert(b->store.epoch.current==2u);
    assert(!b->engine.last.timestamp.available);
    b->acquisition.time_valid=true;b->acquisition.time=999u;engine_boot(b);
    assert(!b->engine.last.timestamp.available&&b->engine.last.timestamp.value==0u);
    assert(live_command_engine_project_b5(&b->engine,image)==TR2_OK&&!(image[13]&2048u)&&image[18]==0u&&image[19]==0u);
    q=request(2,1,13);run_command(b,&q);assert(b->acquisition.running);
    destroy(b);puts("126 real admissions, no eviction, reserved STOP126/renew127, new epoch reuse and timestamp non-retroactivity: PASS");
}
static void absence_proof_and_record_corruption(void)
{
    Bench *b=bench();LiveCommandRequest q=request(1,1,13);LiveCommandRecord r;LiveEffectRecord effect;
    assert(live_command_engine_submit(&b->engine,&q)==TR2_OK);
    assert(live_command_engine_step(&b->engine)==TR2_OK);
    r=b->engine.active;r.generation=b->store.generation+1u;r.effect_state=3u;
    effect=b->store.effect;effect.generation++;effect.phase=4u;
    assert(live_command_store_commit(&b->store,0,&r,NULL,&effect)==TR2_OK);
    engine_boot(b);assert(!b->engine.has_active&&b->engine.last.status==6u&&b->engine.last.result==29u);
    assert(b->acquisition.starts==0u);
    {uint8_t record[112];LiveCommandRecord decoded;
     r=b->engine.last;assert(live_command_record_encode(&r,record)==TR2_OK);
     record[65]^=1u;assert(live_command_record_decode(record,&decoded)==TR2_ERROR_CORRUPTED);}
    destroy(b);
    b=bench();q=request(1,1,13);assert(live_command_engine_submit(&b->engine,&q)==TR2_OK);
    assert(live_command_engine_step(&b->engine)==TR2_OK);
    r=b->engine.active;r.generation=b->store.generation+1u;r.effect_state=3u;
    effect=b->store.effect;effect.generation++;effect.phase=4u;effect.txid=2u;
    assert(live_command_store_commit(&b->store,0,&r,NULL,&effect)==TR2_OK);
    engine_boot(b);assert(b->engine.has_active&&b->engine.status==9u);destroy(b);
    puts("Explicit causal ABSENCE_PROVEN29, mismatched receipt9 and journal CRC refusal: PASS");
}
static void cut_campaign(unsigned stage)
{
    Bench *b=bench();LiveCommandRequest q=request(1,1,stage>=4u&&stage<=6u?12u:13u);
    uint8_t *physical=malloc(262144u),*candidate=malloc(52298u);size_t cut,cases=0;
    TransactionalImageMedia historical,live;LiveConfiguration configuration;
    LiveCommandStore store;LiveCommandEngine engine;Acquisition acquisition;
    unsigned i,setup_steps=stage==0u?0u:stage<=3u?stage-1u:stage<=6u?stage-4u:0u;
    assert(physical&&candidate);
    if(q.command.identity.command_code==12u)q.command.transaction_id=65535u;
    if(stage==8u) {run_command(b,&q);q=request(1,2,14);setup_steps=1u;}
    if(stage==9u) {
        PreparedConfiguration p=prepared(vector_2_prepared,3u);q=request(1,2,1);setup_steps=1u;
        assert(live_command_engine_prepare(&b->engine,&p)==TR2_OK);
    }
    if(stage!=0u)assert(live_command_engine_submit(&b->engine,&q)==TR2_OK);
    for(i=0;i<setup_steps;i++)assert(live_command_engine_step(&b->engine)==TR2_OK);
    memcpy(physical,b->f->physical.bytes,262144u);memcpy(candidate,b->f->candidate,52298u);
    historical=b->f->historical;live=b->f->live;configuration=b->f->service;
    store=b->store;engine=b->engine;acquisition=b->acquisition;
    for(cut=0u;cut<=(stage==9u?104852u:52426u);) {
        unsigned starts;Tr2Result result;
        memcpy(b->f->physical.bytes,physical,262144u);memcpy(b->f->candidate,candidate,52298u);
        b->f->historical=historical;b->f->live=live;b->f->service=configuration;
        b->store=store;b->engine=engine;b->acquisition=acquisition;b->f->physical.budget=cut;
        if(stage==0u)result=live_command_engine_submit(&b->engine,&q);
        else if(stage==7u)result=live_command_engine_boot(&b->engine);
        else result=live_command_engine_step(&b->engine);
        (void)result;starts=b->acquisition.starts;b->f->physical.budget=SIZE_MAX;
        engine_boot(b);
        assert(!b->acquisition.running&&b->acquisition.starts==starts);
        assert(memcmp(physical,b->f->physical.bytes,122880u)==0);
        assert(memcmp(physical+245760u,b->f->physical.bytes+245760u,16384u)==0);
        if(stage==1u)assert(b->engine.status==9u||b->engine.last.result==28u);
        if(stage==2u)assert(b->engine.status==9u||b->engine.last.status==4u);
        if(stage==3u)assert(b->engine.has_last&&b->engine.last.status==4u);
        if(stage==5u)assert((b->engine.epoch_status==5u&&b->engine.status==9u)||
            (b->store.epoch.current==2u&&b->engine.last.status==4u));
        if(stage==6u)assert(b->store.epoch.current==2u&&b->engine.last.status==4u);
        if(stage==7u)assert(b->engine.last.result==28u);
        if(stage==9u&&b->engine.has_last&&b->engine.last.request.command.identity.command_code==1u&&b->engine.last.status==4u) {
            ActiveConfigurationSnapshot active;assert(live_configuration_active(&b->f->service,&active)&&active.config_id==3u);
        }
        cases++;
        /* Every physical header/root byte, payload stride4096; repeat for APPLY's second commit. */
        {size_t local=cut%52426u;
         if(local<49152u)cut+=4096u;
         else if(local<52298u)cut+=52298u-local;
         else cut++;}
    }
    printf("Power-loss stage %u: %zu budgets, protected bytes/no redispatch: PASS\n",stage,cases);
    free(physical);free(candidate);destroy(b);
}
static void fixture_u32(uint8_t *p,uint32_t value)
{p[0]=(uint8_t)(value>>24);p[1]=(uint8_t)(value>>16);p[2]=(uint8_t)(value>>8);p[3]=(uint8_t)value;}
static void fixture_u64(uint8_t *p,uint64_t value)
{fixture_u32(p,(uint32_t)(value>>32));fixture_u32(p+4,(uint32_t)value);}
static void epoch_boundary_record(uint8_t bytes[64],uint64_t gen,uint16_t flags)
{
    memset(bytes,0,64u);memcpy(bytes,"TR2Q",4u);bytes[5]=1;bytes[7]=64;
    fixture_u64(bytes+8,gen);fixture_u32(bytes+16,UINT32_MAX);bytes[21]=1;
    fixture_u32(bytes+24,UINT32_MAX-1u);bytes[28]=255;bytes[29]=255;
    bytes[31]=(uint8_t)flags;fixture_u64(bytes+32,100u);root_crc(bytes);
}
static void boundaries_and_unavailable_port(void)
{
    Bench *b=bench();LiveCommandRequest q=request(1,1,13);uint8_t bytes[112];LiveCommandRecord r={0};size_t writes;
    run_command(b,&q);
    /* Synthetic valid near-limit checkpoint, not billions of real renewals. */
    epoch_boundary_record(bytes,101,0);assert(b->f->live.interface.write(&b->f->live,51914u,bytes,64)==TR2_OK);
    epoch_boundary_record(bytes,100,1);assert(b->f->live.interface.write(&b->f->live,51978u,bytes,64)==TR2_OK);
    r.generation=100u;r.request=request(UINT32_MAX-1u,65535,12);r.phase=2;
    assert(live_command_record_encode(&r,bytes)==TR2_OK);
    assert(b->f->live.interface.write(&b->f->live,15922u+224u*127u,bytes,112)==TR2_OK);
    r.generation=101u;r.phase=3;r.status=4;
    assert(live_command_record_encode(&r,bytes)==TR2_OK);
    assert(b->f->live.interface.write(&b->f->live,15922u+224u*127u+112u,bytes,112)==TR2_OK);
    assert(b->f->live.interface.commit(&b->f->live)==TR2_OK);
    assert(live_command_store_recover(&b->store)==TR2_OK);
    writes=b->f->physical.writes;q=request(UINT32_MAX,65535,12);
    assert(live_command_engine_submit(&b->engine,&q)==TR2_OK&&b->engine.result==26u);
    assert(b->acquisition.running&&writes==b->f->physical.writes);
    destroy(b);
    b=bench();b->engine.acquisition.start=NULL;q=request(1,1,13);writes=b->f->physical.writes;
    assert(live_command_engine_submit(&b->engine,&q)==TR2_OK&&b->engine.result==8u);
    assert(b->acquisition.starts==0&&b->f->physical.writes==writes);
    assert(TR2_B5_REGISTER_COUNT==20u); /* historical profile stays frozen */
    {PersistentIdentityRecord id;assert(live_configuration_identity(&b->f->service,&id)==TR2_OK);
     assert(id.identity.functional.protocol_version==2u&&!(id.identity.functional.device_capabilities&16u));}
    destroy(b);
    b=bench();b->acquisition.time_valid=true;b->acquisition.time=0u;q=request(1,1,13);run_command(b,&q);
    {uint16_t out[29];assert(live_command_engine_project_b5(&b->engine,out)==TR2_OK);
     assert((out[13]&2048u)&&out[18]==0u&&out[19]==0u);}
    b->acquisition.time=999u;engine_boot(b);
    assert(b->engine.last.timestamp.available&&b->engine.last.timestamp.value==0u);
    destroy(b);puts("Synthetic epoch ceiling without wrap/STOP loss, absent acquisition port refusal, no false LIVE capability: PASS");
}
int live_r2_regression(void)
{
    unsigned stage;contexts_test();lifecycle_and_epoch();recovery_phases();effect_and_storage_faults();
    mailbox_and_apply();boundaries_and_unavailable_port();saturation_and_historical_time();absence_proof_and_record_corruption();
    for(stage=0u;stage<10u;stage++)cut_campaign(stage);
    return 0;
}

#ifndef TR2_LIVE_R2_TEST_LIBRARY
int main(void) { return live_r2_regression(); }
#endif
