/* Maintenance-specific safety evidence on the genuine R1/R2 stores.
 * RAM-backed storage only. No expected product result is weakened. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "r4m_maintenance.h"
#include "tr2/common/sha256.h"
#include "tr2/modbus/b4_configuration_codec.h"
typedef struct {
 uint8_t bytes[262144],candidate[52298];TransactionalImageMedia historical,live;
 LiveConfiguration config;LiveCommandStore store;LiveCommandEngine engine;
 R4mMaintenance guard;bool owner,running;uint64_t run;uint32_t config_id,config_generation;size_t budget,writes;unsigned starts;
} Bench;
static const uint32_t uid[3]={0x004A003D,0x38425018,0x20333157};
static const PersistentIdentityAssignment assignment={ {0x004A003D,0x38425018,0x20333157},1};
static Tr2Result rd(void *c,uint32_t at,void *p,size_t n){Bench *b=c;assert(at+n<=262144);memcpy(p,b->bytes+at,n);return TR2_OK;}
static Tr2Result wr(void *c,uint32_t at,const void *p,size_t n)
{Bench *b=c;assert(at>=122880&&at+n<=245760);size_t take=n<b->budget?n:b->budget;
 memcpy(b->bytes+at,p,take);b->budget-=take;b->writes+=take;return take==n?TR2_OK:TR2_ERROR_STORAGE;}
static Tr2Result ownership(void *c,uint32_t at,size_t n){Bench *b=c;assert(at>=122880&&at+n<=245760);return b->owner?TR2_OK:TR2_ERROR_UNAVAILABLE;}
static Tr2Result stop(void *c){((Bench*)c)->running=false;return TR2_OK;}
static Tr2Result observe(void *c,LiveAcquisitionObservation *o){Bench *b=c;memset(o,0,sizeof(*o));o->stopped=!b->running;o->running=b->running;o->configuration_confirmed=b->running;o->run_id=b->run;o->config_id=b->config_id;o->config_generation=b->config_generation;return TR2_OK;}
static Tr2Result start(void *c,uint64_t run,const ActiveConfigurationSnapshot *cfg)
{Bench *b=c;b->starts++;b->running=true;b->run=run;b->config_id=cfg->config_id;b->config_generation=cfg->generation;return TR2_OK;}
static bool budget(void *c,uint16_t code){(void)c;(void)code;return true;}
static void boot(Bench *b)
{
 memset(&b->guard,0,sizeof(b->guard));TransactionalImagePhysicalStorage io={b,rd,wr};
 TransactionalImageGeometry g=transactional_image_geometry_qualification_profile();
 assert(transactional_image_media_init_v2(&b->historical,&io,&g,b->candidate,sizeof(b->candidate))==TR2_OK);
 assert(transactional_image_media_init_live(&b->live,&io,b->candidate,sizeof(b->candidate),b,ownership)==TR2_OK);
 assert(live_configuration_init(&b->config,&b->historical,&b->live,uid,&assignment,1)==TR2_OK);
 assert(live_command_store_init(&b->store,&b->config)==TR2_OK);
 LiveAcquisitionPort p={b,start,stop,observe,budget,NULL};assert(live_command_engine_init(&b->engine,&b->store,&p)==TR2_OK);
}
static void run(Bench *b,const LiveCommandRequest *q)
{
 assert(r4m_authorize(&b->guard,&b->engine,q));assert(live_command_engine_submit(&b->engine,q)==TR2_OK);
 for(unsigned i=0;i<5&&b->engine.has_active;i++)assert(live_command_engine_step(&b->engine)==TR2_OK);
 assert(!b->engine.has_active&&b->engine.status==4&&!b->engine.result);
}
int main(int argc,char **argv)
{
 assert(argc==2||argc==3);FILE *f=fopen(argv[1],"rb");assert(f);uint8_t *original=malloc(262144);assert(original);
 assert(fread(original,1,262144,f)==262144&&fgetc(f)==EOF);fclose(f);
 uint8_t sha[32];assert(tr2_sha256_bytes(original,262144,sha)==TR2_OK);
 static const uint8_t expected[]={0xc8,0x14,0xe9,0xcc,0x32,0xef,0xf0,0x60,0x16,0xf2,0x8e,0xf9,0x64,0x23,0x33,0x54,0xe3,0x5b,0xa4,0x58,0x65,0x3a,0x16,0x43,0x4e,0x44,0x16,0xb2,0x96,0x27,0x15,0x61};assert(!memcmp(sha,expected,32));
 Bench *b=calloc(1,sizeof(*b));assert(b);memcpy(b->bytes,original,262144);b->owner=true;b->budget=SIZE_MAX;boot(b);
 assert(live_command_engine_boot(&b->engine)==TR2_OK&&b->engine.indeterminate&&b->engine.status==9);
 LiveCommandRequest old=b->engine.active.request;size_t before=b->writes;
 assert(live_command_engine_submit(&b->engine,&old)==TR2_OK&&b->engine.status==9&&b->writes==before&&!b->starts);
 LiveCommandStore broken_store=b->store;LiveCommandEngine broken=b->engine;broken.store=&broken_store;
 R4mMaintenance rejected;broken_store.has_effect=false;
 assert(r4m_capture(&rejected,&broken)!=TR2_OK&&!rejected.phase&&b->writes==before);
 assert(r4m_capture(&b->guard,&b->engine)==TR2_OK&&b->guard.run_floor==1&&b->guard.epoch_floor==1);
 assert(!r4m_authorize(&b->guard,&b->engine,&old));
 assert(r4m_isolate_journal(&b->guard,&b->engine)==TR2_OK);R4mMaintenance retiring=b->guard;
 assert(r4m_isolate_journal(&b->guard,&b->engine)!=TR2_OK); /* no second isolation attempt */
 assert(live_command_store_recover(&b->store)==TR2_OK);
 LiveAcquisitionPort port={b,start,stop,observe,budget,NULL};assert(live_command_engine_init(&b->engine,&b->store,&port)==TR2_OK);
 assert(live_command_engine_boot(&b->engine)==TR2_OK&&!b->engine.has_active&&b->store.has_effect&&b->store.effect.run_id==1);
 assert(!r4m_authorize(&b->guard,&b->engine,&old));
 uint8_t *bridge=malloc(262144);assert(bridge);memcpy(bridge,b->bytes,262144);
 R4mMaintenance resumed,rejected_bridge;
 assert(r4m_capture_bridge(&resumed,&b->engine,retiring.intent)==TR2_OK&&resumed.phase==2);
 uint8_t altered[128];memcpy(altered,retiring.intent,128);altered[20]^=1;
 assert(r4m_capture_bridge(&rejected_bridge,&b->engine,altered)!=TR2_OK&&!rejected_bridge.phase);
 b->guard=resumed;
 LiveCommandRequest renew={0};renew.epoch=1;renew.command.transaction_id=65535;renew.command.identity.command_code=12;
 run(b,&renew);assert(r4m_open_after_renew(&b->guard,&b->engine)==TR2_OK);
 before=b->writes;assert(!r4m_authorize(&b->guard,&b->engine,&old));
 assert(live_command_engine_submit(&b->engine,&old)==TR2_OK&&b->engine.result==24&&b->writes==before&&!b->starts);
 LiveCommandRequest next=old;next.epoch=2;assert(r4m_authorize(&b->guard,&b->engine,&next));
 assert(live_command_engine_submit(&b->engine,&next)==TR2_OK&&b->engine.active.run_id==2&&!b->starts);
 assert(!memcmp(original,b->bytes,122880)&&!memcmp(original+245760,b->bytes+245760,16384));
 puts("REAL CORE: original STARTED9, gate rejects old request, authentic INTENT1 retained, durable RENEW1->2, retired old key24, next RESERVED run2, no acquisition dispatch: PASS");
 /* Complete only in the RAM model to derive the exact phase-C metadata oracle.
  * The physical probe must obtain genuine sensor readbacks for its receipts. */
 for(unsigned i=0;i<5&&b->engine.has_active;i++)assert(live_command_engine_step(&b->engine)==TR2_OK);
 assert(!b->engine.has_active&&b->engine.status==4&&b->running&&b->starts==1&&b->run==2);
 renew.epoch=2;run(b,&renew);assert(b->store.epoch.current==3&&b->running);
 LiveCommandRequest stopq={0};stopq.epoch=3;stopq.command.transaction_id=2;stopq.command.identity.command_code=14;run(b,&stopq);assert(!b->running);
 PreparedConfiguration cfg={0};cfg.config_id=3;cfg.payload=b->config.active.payload;
 cfg.supplied_crc=tr2_b4_prepared_payload_crc(&cfg.payload);
 assert(live_command_engine_prepare(&b->engine,&cfg)==TR2_OK);
 LiveCommandRequest apply={0};apply.epoch=3;apply.command.transaction_id=3;apply.command.identity.command_code=1;run(b,&apply);
 assert(b->config.active.config_id==3&&b->config.active.generation==3&&b->store.effect.run_id==2);
 LiveCommandRequest final_start={0};final_start.epoch=3;final_start.command.transaction_id=4;final_start.command.identity.command_code=13;
 run(b,&final_start);assert(b->running&&b->run==3&&b->starts==2);
 assert(live_command_engine_fault(&b->engine)==TR2_OK&&!b->running&&!b->engine.has_active);
 if(argc==3){FILE *out=fopen(argv[2],"wb");assert(out);assert(fwrite(b->bytes,1,262144,out)==262144);assert(!fclose(out));}
 puts("MODEL FINAL ORACLE: genuine core phases START epoch2/run2, RENEW->3, STOP/APPLY config3, completed START run3 then safety fault without journal mutation, protected regions unchanged: PASS");

 unsigned cuts=0;
 /* Atomically isolating one journal is a single outer commit. Test actual
  * byte cuts across payload plus EVERY header/root byte, not invented receipts. */
 for(size_t cut=0;cut<=52426;cut+=(cut<52298?4096:1)) {
  memcpy(b->bytes,original,262144);b->budget=SIZE_MAX;boot(b);assert(live_command_engine_boot(&b->engine)==TR2_OK);
  assert(r4m_capture(&b->guard,&b->engine)==TR2_OK);b->budget=cut;
  (void)r4m_isolate_journal(&b->guard,&b->engine);b->budget=SIZE_MAX;boot(b);
  assert(!r4m_authorize(&b->guard,&b->engine,&old)); /* CLOSED after every boot */
  assert(live_command_engine_boot(&b->engine)==TR2_OK);
  assert(b->engine.indeterminate||(!b->engine.has_active&&b->store.epoch.current==1&&b->store.effect.run_id==1));
  assert(!memcmp(original,b->bytes,122880)&&!memcmp(original+245760,b->bytes+245760,16384));cuts++;
 }
 for(size_t cut=52298;cut<=52426;cut++) {
  memcpy(b->bytes,original,262144);b->budget=SIZE_MAX;boot(b);assert(live_command_engine_boot(&b->engine)==TR2_OK);
  assert(r4m_capture(&b->guard,&b->engine)==TR2_OK);b->budget=cut;(void)r4m_isolate_journal(&b->guard,&b->engine);
  b->budget=SIZE_MAX;boot(b);assert(!r4m_authorize(&b->guard,&b->engine,&old));assert(live_command_engine_boot(&b->engine)==TR2_OK);
  assert(b->engine.indeterminate||(!b->engine.has_active&&b->store.effect.run_id==1));
  assert(!memcmp(original,b->bytes,122880)&&!memcmp(original+245760,b->bytes+245760,16384));cuts++;
 }
 for(unsigned stage=0;stage<=3;stage++) {
  memcpy(b->bytes,bridge,262144);b->budget=SIZE_MAX;boot(b);
  assert(live_command_engine_boot(&b->engine)==TR2_OK);b->guard=retiring;renew.epoch=1;
  assert(r4m_authorize(&b->guard,&b->engine,&renew));assert(live_command_engine_submit(&b->engine,&renew)==TR2_OK);
  for(unsigned step=0;step<stage;step++)assert(live_command_engine_step(&b->engine)==TR2_OK);
  unsigned starts_before=b->starts;boot(b);assert(!r4m_authorize(&b->guard,&b->engine,&old));
  assert(live_command_engine_boot(&b->engine)==TR2_OK&&b->starts==starts_before&&b->store.effect.run_id==1);
  assert(!r4m_authorize(&b->guard,&b->engine,&old));
  if(stage==0)assert(!b->engine.has_active&&b->engine.last.result==28&&b->store.epoch.current==1);
  if(stage==1)assert(b->engine.indeterminate&&b->engine.status==9&&b->store.epoch.current==1);
  if(stage>=2)assert(!b->engine.has_active&&b->engine.last.status==4&&b->store.epoch.current==2);
 }
 puts("RENEW four cut phases: CLOSED frontend at reset, RESERVED28/STARTED9/durable2 recovery, no acquisition or old-key replay: PASS");
 /* A bridge is NOT safe for generic product ingress. Negative control exposes
  * why the nonpersistent CLOSED-on-every-boot firmware gate is indispensable. */
 memcpy(b->bytes,bridge,262144);b->budget=SIZE_MAX;boot(b);assert(live_command_engine_boot(&b->engine)==TR2_OK);
 assert(!r4m_authorize(&b->guard,&b->engine,&old));
 assert(live_command_engine_submit(&b->engine,&old)==TR2_OK&&b->engine.has_active);
 puts("NEGATIVE CONTROL: bypassing the isolated maintenance frontend would re-admit old key in bridge; never expose it as product recovery: PASS");
 printf("MAINTENANCE_COMMIT_CUTS=%u,protected_bytes=139264,gate_closed_after_reset=PASS\n",cuts);
 free(bridge);free(original);free(b);return 0;
}
