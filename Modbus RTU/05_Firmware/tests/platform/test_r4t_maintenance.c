/* Genuine snapshot/stores, RAM byte cuts only; no sensor effect claims. */
#define main old_fixture_main
#include "test_r4m_maintenance.c"
#undef main
#include "r4t_maintenance.h"
static void check(const Bench *b,const uint8_t *original)
{
 assert(b->store.generation>=26&&b->live.generation>=39&&b->store.epoch.current>=4);
 assert(b->store.effect.run_id>=5&&!b->starts);
 assert(!memcmp(original,b->bytes,122880)&&!memcmp(original+245760,b->bytes+245760,16384));
 assert(b->store.has_effect&&b->store.effect.phase==1&&b->store.effect.run_id==5);
}
static void dump(const char *name,const uint8_t *b)
{FILE *f=fopen(name,"wb");assert(f);assert(fwrite(b,1,262144,f)==262144&&!fclose(f));}
int main(int argc,char **argv)
{
 assert(argc==2||argc==4);FILE *f=fopen(argv[1],"rb");assert(f);
 uint8_t *original=malloc(262144),*admitted=malloc(262144);assert(original&&admitted);
 assert(fread(original,1,262144,f)==262144&&fgetc(f)==EOF);fclose(f);
 Bench *b=calloc(1,sizeof(*b));assert(b);b->owner=true;b->budget=SIZE_MAX;memcpy(b->bytes,original,262144);boot(b);
 assert(live_command_engine_boot(&b->engine)==TR2_OK&&b->engine.indeterminate);
 LiveCommandRequest old[5];unsigned slots[5]={0,1,2,3,127};
 for(unsigned i=0;i<5;i++)old[i]=b->store.entries[slots[i]].request;
 R4tRetirement t;assert(r4t_capture(&t,&b->engine,original)==TR2_OK);
 assert(t.archived.phase==2&&t.archived.generation==26&&t.archived.run_id==5);
 uint8_t intent[128];memcpy(intent,t.gate.intent,128);
 /* Alter every domain, including protected regions and journal CRC: exact hash refuses. */
 unsigned changed[]={0,122880,131072,147733,188416,245760,262143};
 for(unsigned i=0;i<sizeof(changed)/sizeof(changed[0]);i++) {
  R4tRetirement bad;original[changed[i]]^=1;assert(r4t_capture(&bad,&b->engine,original)!=TR2_OK&&!bad.gate.phase);original[changed[i]]^=1;
 }
 assert(r4t_retire_and_admit(&t,&b->engine)==TR2_OK);
 assert(r4t_retire_and_admit(&t,&b->engine)!=TR2_OK);check(b,original);
 assert(b->store.generation==27&&b->engine.active.phase==1&&b->engine.active.request.command.identity.command_code==12);
 assert(!b->store.present[3]&&b->store.entries[2].generation==24&&b->engine.last.generation==24);
 assert(t.archived.phase==2&&t.archived.status==0&&t.archived.result==0);
 memcpy(admitted,b->bytes,262144);if(argc==4)dump(argv[2],admitted);
 for(unsigned i=0;i<3;i++){assert(live_command_engine_step(&b->engine)==TR2_OK);assert(b->store.generation==28+i);check(b,original);}
 assert(r4m_open_after_renew(&t.gate,&b->engine)==TR2_OK&&b->store.epoch.current==5);
 assert(!memcmp(intent,b->live.candidate+TR2_LIVE_EFFECT_OFFSET,128));
 if(argc==4)dump(argv[3],b->bytes);
 size_t before=b->writes;
 for(unsigned i=0;i<5;i++)assert(live_command_engine_submit(&b->engine,&old[i])==TR2_OK&&b->engine.result==24&&b->writes==before&&!b->starts);
 b->guard=t.gate;LiveCommandRequest startq={0};startq.epoch=5;startq.command.transaction_id=1;startq.command.identity.command_code=13;
 run(b,&startq);assert(b->run==6&&b->starts==1);
 LiveCommandRequest stopq={0};stopq.epoch=5;stopq.command.transaction_id=2;stopq.command.identity.command_code=14;run(b,&stopq);assert(!b->running);
 unsigned cuts=0;
 /* Every header/root byte plus payload strides, for admission and every RENEW barrier. */
 for(unsigned phase=0;phase<4;phase++)for(size_t cut=0;cut<=52426;cut=cut<49152?cut+4096:cut==49152?52298:cut+1) {
  memcpy(b->bytes,original,262144);b->budget=SIZE_MAX;b->starts=0;boot(b);assert(live_command_engine_boot(&b->engine)==TR2_OK);
  assert(r4t_capture(&t,&b->engine,original)==TR2_OK);
  if(!phase){b->budget=cut;(void)r4t_retire_and_admit(&t,&b->engine);}
  else {
   assert(r4t_retire_and_admit(&t,&b->engine)==TR2_OK);
   for(unsigned j=1;j<phase;j++)assert(live_command_engine_step(&b->engine)==TR2_OK);
   b->budget=cut;(void)live_command_engine_step(&b->engine);
  }
  b->budget=SIZE_MAX;boot(b);assert(!r4m_authorize(&b->guard,&b->engine,&old[3]));
  assert(live_command_store_recover(&b->store)==TR2_OK);check(b,original);
  assert(!memcmp(intent,b->live.candidate+TR2_LIVE_EFFECT_OFFSET,128));
  uint64_t journal=b->store.generation,root=b->live.generation;
  assert(live_command_engine_boot(&b->engine)==TR2_OK);check(b,original);
  assert(b->store.generation>=journal&&b->live.generation>=root);
  assert(!r4m_authorize(&b->guard,&b->engine,&old[3]));
  /* Original STARTED5 remains unresolved or is archived/absent, never terminalized. */
  int oldslot=live_command_store_lookup(&b->store,4,5);
  assert(oldslot<0||b->store.entries[oldslot].phase==2);
  assert(r4t_capture(&t,&b->engine,original)!=TR2_OK||b->store.generation==26);
  cuts++;
 }
 printf("R4T PASS:26->27->28->29->30; %u byte-cut cases across4barriers; exact hash negative tests; original STARTED5 never terminalized; INTENT/floors/protected exact;5oldkeys24; next run6 STOP; CLOSED reboot/no redispatch\n",cuts);
 free(b);free(original);free(admitted);return 0;
}
