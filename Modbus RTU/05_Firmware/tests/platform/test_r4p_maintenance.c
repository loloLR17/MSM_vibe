/* Reuse real R4M stores/transport fixture, not its old campaign. */
#define main r4m_fixture_main
#include "test_r4m_maintenance.c"
#undef main
int main(int argc,char **argv)
{
 assert(argc==2||argc==3);FILE *f=fopen(argv[1],"rb");assert(f);uint8_t *original=malloc(262144);assert(original);
 assert(fread(original,1,262144,f)==262144&&fgetc(f)==EOF);fclose(f);
 Bench *b=calloc(1,sizeof(*b));assert(b);b->owner=true;b->budget=SIZE_MAX;memcpy(b->bytes,original,262144);boot(b);
 assert(live_command_engine_boot(&b->engine)==TR2_OK&&b->engine.indeterminate&&b->engine.status==9);
 LiveCommandRequest old=b->engine.active.request;size_t before=b->writes;
 R4mMaintenance rejected;LiveCommandStore badstore=b->store;LiveCommandEngine bad=b->engine;bad.store=&badstore;badstore.has_effect=false;
 assert(r4p_capture(&rejected,&bad)!=TR2_OK&&!rejected.phase&&b->writes==before);
 assert(r4p_capture(&b->guard,&b->engine)==TR2_OK&&b->guard.epoch_floor==2&&b->guard.run_floor==2);
 assert(r4m_isolate_journal(&b->guard,&b->engine)==TR2_OK);R4mMaintenance retiring=b->guard;
 assert(r4m_isolate_journal(&b->guard,&b->engine)!=TR2_OK);
 boot(b);assert(live_command_engine_boot(&b->engine)==TR2_OK&&!b->engine.has_active&&!b->engine.has_last&&b->store.epoch.current==2);
 b->guard=retiring;assert(!r4m_authorize(&b->guard,&b->engine,&old));
 uint8_t *bridge=malloc(262144);assert(bridge);memcpy(bridge,b->bytes,262144);
 assert(!memcmp(retiring.intent,b->config.live->candidate+TR2_LIVE_EFFECT_OFFSET,128));
 LiveCommandRequest renew={0};renew.epoch=2;renew.command.transaction_id=65535;renew.command.identity.command_code=12;
 run(b,&renew);assert(r4m_open_after_renew(&b->guard,&b->engine)==TR2_OK&&b->store.epoch.current==3);
 before=b->writes;assert(!r4m_authorize(&b->guard,&b->engine,&old));
 assert(live_command_engine_submit(&b->engine,&old)==TR2_OK&&b->engine.result==24&&b->writes==before&&!b->starts);
 LiveCommandRequest older=old;older.epoch=1;assert(live_command_engine_submit(&b->engine,&older)==TR2_OK&&b->engine.result==24&&b->writes==before);
 LiveCommandRequest startq=old;startq.epoch=3;run(b,&startq);assert(b->run==3&&b->starts==1);
 renew.epoch=3;run(b,&renew);assert(b->store.epoch.current==4&&b->running);
 LiveCommandRequest stopq={0};stopq.epoch=4;stopq.command.transaction_id=2;stopq.command.identity.command_code=14;run(b,&stopq);
 PreparedConfiguration cfg={0};cfg.config_id=3;cfg.payload=b->config.active.payload;cfg.supplied_crc=tr2_b4_prepared_payload_crc(&cfg.payload);
 assert(live_command_engine_prepare(&b->engine,&cfg)==TR2_OK);LiveCommandRequest apply=stopq;apply.command.transaction_id=3;apply.command.identity.command_code=1;run(b,&apply);
 startq.epoch=4;startq.command.transaction_id=4;run(b,&startq);assert(b->run==4&&b->starts==2);
 assert(live_command_engine_fault(&b->engine)==TR2_OK&&!b->running&&!b->engine.has_active);
 assert(!memcmp(original,b->bytes,122880)&&!memcmp(original+245760,b->bytes+245760,16384));
 if(argc==3){FILE *out=fopen(argv[2],"wb");assert(out);assert(fwrite(b->bytes,1,262144,out)==262144);fclose(out);}
 puts("R4P model: authentic STARTED2/run2 archived; one isolation; genuine RENEW2->3; old epochs1/2 rejected24/0write; next runs3/4, STOP/APPLY and final receipt, protected bytes identical: PASS");
 unsigned cuts=0;
 for(size_t cut=0;cut<=52426;cut=cut<49152?cut+4096:cut==49152?52298:cut+1) {
  memcpy(b->bytes,original,262144);b->budget=SIZE_MAX;boot(b);assert(live_command_engine_boot(&b->engine)==TR2_OK);
  assert(r4p_capture(&b->guard,&b->engine)==TR2_OK);b->budget=cut;(void)r4m_isolate_journal(&b->guard,&b->engine);
  b->budget=SIZE_MAX;boot(b);assert(!r4m_authorize(&b->guard,&b->engine,&old));assert(live_command_engine_boot(&b->engine)==TR2_OK);
  assert(b->engine.indeterminate||(!b->engine.has_active&&b->store.epoch.current==2&&b->store.effect.run_id==2));
  assert(!memcmp(original,b->bytes,122880)&&!memcmp(original+245760,b->bytes+245760,16384));cuts++;
 }
 for(unsigned phase=0;phase<4;phase++) {
  memcpy(b->bytes,bridge,262144);b->budget=SIZE_MAX;boot(b);assert(live_command_engine_boot(&b->engine)==TR2_OK);b->guard=retiring;renew.epoch=2;
  assert(live_command_engine_submit(&b->engine,&renew)==TR2_OK);for(unsigned i=0;i<phase;i++)assert(live_command_engine_step(&b->engine)==TR2_OK);
  unsigned starts=b->starts;boot(b);assert(!r4m_authorize(&b->guard,&b->engine,&old));assert(live_command_engine_boot(&b->engine)==TR2_OK&&b->starts==starts&&b->store.effect.run_id==2);
  if(phase==0)assert(!b->engine.has_active&&b->engine.last.result==28&&b->store.epoch.current==2);
  if(phase==1)assert(b->engine.indeterminate&&b->engine.status==9&&b->store.epoch.current==2);
  if(phase>=2)assert(!b->engine.has_active&&b->store.epoch.current==3&&b->engine.last.status==4);
 }
 /* The bridge remains firmware-quarantined, never generic product recovery. */
 memcpy(b->bytes,bridge,262144);b->budget=SIZE_MAX;boot(b);assert(live_command_engine_boot(&b->engine)==TR2_OK);
 assert(!r4m_authorize(&b->guard,&b->engine,&old));assert(live_command_engine_submit(&b->engine,&old)==TR2_OK&&b->engine.has_active);
 printf("R4P isolation cuts=%u +4 genuine RENEW cut phases: CLOSED after reset, no dispatch; negative control confirms bridge must never receive product ingress: PASS\n",cuts);
 free(bridge);free(original);free(b);return 0;
}
