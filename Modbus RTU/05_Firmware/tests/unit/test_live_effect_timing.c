#define TR2_LIVE_R2_TEST_LIBRARY 1
#include "test_live_command_engine.c"
#include "tr2/common/cooperative_work.h"
/* Independent nominal costs from R4D raw measurements, not hardware/WCET. */
static struct {uint64_t us;Bench *b;unsigned fail,observed,services;bool armed;uint32_t start_us;} m;
static uint64_t timing_now(void *ctx){(void)ctx;return m.us;}
static Tr2Result timing_start(void *ctx,uint64_t run,const ActiveConfigurationSnapshot *c)
{m.us+=m.start_us;return start_acq(ctx,run,c);}
static Tr2Result timing_observe(void *ctx,LiveAcquisitionObservation *o)
{m.us+=103;return observe_acq(ctx,o);}
static Tr2Result timing_stop(void *ctx){m.us+=236;return stop_acq(ctx);}
static void timing_safety(void *ctx){(void)ctx;(void)stop_acq(&m.b->acquisition);}
static Tr2Result timing_service(void *ctx)
{
 (void)ctx;m.services++;
 if(m.armed&&m.b->acquisition.starts) {
  if(m.fail==1&&m.b->acquisition.observations==m.observed)return TR2_ERROR_STORAGE;
  if(m.fail==2&&m.b->acquisition.observations>m.observed)return TR2_ERROR_STORAGE;
 }
 return TR2_OK;
}
static void scenario(unsigned fail,uint32_t duration)
{
 Bench *b=bench();Tr2CooperativeWork w;memset(&m,0,sizeof(m));m.b=b;m.fail=fail;m.start_us=duration;
 assert(tr2_cooperative_init(&w,NULL,timing_now,timing_service,timing_safety,32,1000,2500)==TR2_OK);
 assert(transactional_image_media_bind_work(&b->f->historical,&w)==TR2_OK);
 assert(transactional_image_media_bind_work(&b->f->live,&w)==TR2_OK);
 b->engine.acquisition.start=timing_start;b->engine.acquisition.stop=timing_stop;b->engine.acquisition.observe=timing_observe;
 LiveCommandRequest q=request(1,1,13);assert(live_command_engine_submit(&b->engine,&q)==TR2_OK);
 assert(live_command_engine_step(&b->engine)==TR2_OK&&b->engine.active.phase==2);
 size_t before=b->f->physical.writes;m.observed=b->acquisition.observations;m.armed=true;
 Tr2Result result=live_command_engine_step(&b->engine);
 if(!fail&&duration<=1000) {
  assert(result==TR2_OK);assert(w.max_work_us<=1000&&!w.deadline_missed);
  assert(live_command_engine_step(&b->engine)==TR2_OK&&!b->engine.has_active&&b->engine.status==4);
  m.armed=false;q=request(1,2,14);run_command(b,&q);
  Bench *plain=bench();q=request(1,1,13);run_command(plain,&q);q=request(1,2,14);run_command(plain,&q);
  assert(!memcmp(b->f->physical.bytes,plain->f->physical.bytes,262144));destroy(plain);
 } else {
  assert(result!=TR2_OK&&b->engine.indeterminate&&b->engine.status==9&&!b->acquisition.running);
  assert(b->f->physical.writes==before&&b->engine.active.phase==2&&b->store.effect.phase==1);
  if(duration>1000)assert(w.deadline_missed);
  unsigned dispatched=b->acquisition.starts;m.armed=false;engine_boot(b);
  assert(b->engine.indeterminate&&b->engine.status==9&&b->acquisition.starts==dispatched&&!b->acquisition.running);
 }
 destroy(b);
}
int main(void)
{
 scenario(0,905);scenario(1,905);scenario(2,905);scenario(0,1100);
 puts("R4D effects: 905us START +103us OBSERVE split without budget relaxation; identical durable START/STOP image; service failure at both causal boundaries preserves STARTED9/no writes/no replay; single1100us effect still fails safely: PASS");return 0;
}
