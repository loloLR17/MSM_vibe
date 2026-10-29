/* Reuse the qualified bounded sensor model, not its finite acquisition tests. */
#define main bounded_fifo_main
#include "test_fifo_cooperative.c"
#undef main
#include "live_fifo_acquisition.h"
#define main r1_fixture_main
#include "../unit/test_live_configuration.c"
#undef main
static uint64_t time64,delivered;
static unsigned consumer_fail,consumer_limit=128;
static int consume(void *ctx,const uint8_t *b,uint32_t n,uint64_t first)
{
 (void)ctx;(void)first;assert(n<=consumer_limit);
 if(consumer_fail)return -1;
 for(uint32_t i=0;i<n;i++)if((b[i*7u]>>3)==2u)delivered++;
 return 0;
}
static uint64_t ms(void *ctx){(void)ctx;return time64;}
static LiveFifoAcquisition acquisition;
static ActiveConfigurationSnapshot configuration(void)
{
 ActiveConfigurationSnapshot c={0};c.generation=1;c.config_id=1;
 c.payload.sampling_frequency_hz=26667;c.payload.axes_enable_mask=7;c.payload.full_scale_code=2;
 c.payload.acquisition_mode=2;c.payload.window_size_samples=4096;c.payload.indicator_period_ms=2000;
 c.payload.supervision_enable_mask=1;c.payload.campaign_context_id=1;c.payload.mission_id=1;return c;
}
static void setup(void)
{
 reset(4096);time64=0;delivered=consumer_fail=0;consumer_limit=128;
 assert(live_fifo_acquisition_init(&acquisition,&ctx,ms,NULL,5,consume,NULL)==TR2_OK);
 assert(io_count==0);assert(!acquisition.fifo.running&&!acquisition.fifo.stopped);
}
static bool model_budget(void *ctx,uint16_t code){(void)ctx;(void)code;return true;}
static LiveMetrology metrology;
/* Explicit model clock only: rational sample grid independent of burst shape. */
static Tr2Result sample_clock(void *context,uint64_t index,uint64_t *mono,bool *utc_valid,uint64_t *utc)
{
 (void)context;*mono=index*1000u/26667u;*utc_valid=false;*utc=0;return TR2_OK;
}
static void metrology_stream(void)
{
 ActiveConfigurationSnapshot c=configuration();ModbusBlock3Image b;LiveAcquisitionPort p;
 setup();live_metrology_init(&metrology);
 assert(live_fifo_acquisition_bind_metrology(&acquisition,&metrology,NULL,NULL)!=TR2_OK);
 assert(live_fifo_acquisition_bind_metrology(&acquisition,&metrology,sample_clock,NULL)==TR2_OK);
 p=live_fifo_acquisition_port(&acquisition);assert(p.start(p.context,1,&c)==TR2_OK);
 timestamp=UINT32_MAX-50u; /* shared sensor timestamp guard crosses modulo32 */
 for(unsigned i=0;i<40000;i++) {
  time64=acquisition.fifo.total_xyz*1000u/26667u;
  assert(live_fifo_acquisition_poll(&acquisition)==TR2_OK);
 }
 assert(acquisition.fifo.total_xyz>4000000u&&metrology.sequence>80);
 assert(live_metrology_project_b3(&metrology,metrology.last_ms,&b)==TR2_OK&&(b.registers[1]&6)==6);
 mode=4;sequence=10; /* malformed next raw burst -> no current valid B3 */
 time64=acquisition.fifo.total_xyz*1000u/26667u;
 assert(live_fifo_acquisition_poll(&acquisition)!=TR2_OK);
 assert(!metrology.running&&!metrology.value_valid&&!acquisition.fifo.running);
 puts("Actual owner -> metrology: >4M checked XYZ, sensor timestamp rollover, periodic windows, FIFO error invalidates/stops: PASS");
}
static void real_engine_port(void)
{
 Fixture *f=fixture();PreparedConfiguration prep=prepared(vector_1_prepared,2u);
 LiveCommandStore store;LiveCommandEngine engine;LiveAcquisitionPort p;LiveCommandRequest q={0};
 assert(live_configuration_migrate(&f->service)==TR2_OK);
 assert(live_configuration_apply(&f->service,&prep,false)==TR2_OK);boot(f);setup();
 live_metrology_init(&metrology);
 assert(live_fifo_acquisition_bind_metrology(&acquisition,&metrology,sample_clock,NULL)==TR2_OK);
 assert(live_command_store_init(&store,&f->service)==TR2_OK);
 acquisition.budget=model_budget;p=live_fifo_acquisition_port(&acquisition);
 assert(live_command_engine_init(&engine,&store,&p)==TR2_OK);assert(live_fifo_acquisition_bind_engine(&acquisition,&engine)==TR2_OK);
 assert(live_command_engine_boot(&engine)==TR2_OK);
 q.epoch=1;q.command.transaction_id=1;q.command.identity.command_code=13;
 assert(live_command_engine_submit(&engine,&q)==TR2_OK);
 for(unsigned i=0;i<3;i++)assert(live_command_engine_step(&engine)==TR2_OK);
 assert(engine.state==LIVE_RUNNING&&acquisition.fifo.running&&(regs[16]&0xE0)==0xA0);
 for(unsigned i=0;i<100;i++){time64++;assert(live_fifo_acquisition_poll(&acquisition)==TR2_OK);}
 {size_t writes=f->physical.writes;uint32_t io=io_count;LiveCommandRequest apply=q;
  apply.command.transaction_id=2;apply.command.identity.command_code=1;
  assert(live_command_engine_submit(&engine,&apply)==TR2_OK&&engine.result==5u);
  assert(f->physical.writes==writes&&io_count==io);}
 q.command.transaction_id=3;q.command.identity.command_code=14;
 assert(live_command_engine_submit(&engine,&q)==TR2_OK);
 for(unsigned i=0;i<3;i++)assert(live_command_engine_step(&engine)==TR2_OK);
 assert(engine.status==4&&!acquisition.fifo.running&&!(regs[16]&0xE0));
 q.command.transaction_id=4;q.command.identity.command_code=13;
 assert(live_command_engine_submit(&engine,&q)==TR2_OK);
 for(unsigned i=0;i<3;i++)assert(live_command_engine_step(&engine)==TR2_OK);
 f->physical.budget=0;q.command.transaction_id=5;q.command.identity.command_code=14;
 assert(live_command_engine_submit(&engine,&q)!=TR2_OK);
 assert(!acquisition.fifo.running&&!(regs[16]&0xE0)&&engine.state==LIVE_FAULT&&!metrology.running&&!metrology.value_valid&&!metrology.authority);
 free(f);puts("R2 real journal + actual continuous owner on simulated SPI: START/STOP causal, APPLY refusal and FRAM-independent safety: PASS");
}
static int32_t delayed_read(void *handle,uint8_t reg,uint8_t *b,uint16_t n)
{
 if(reg==58 && n==2 && time64<12){memset(b,0,n);return 0;}
 return ctx.read_reg(handle,reg,b,n);
}
static void first_data_policy(void)
{
 ActiveConfigurationSnapshot c=configuration();setup();
 acquisition.fifo.ctx.read_reg=delayed_read;
 acquisition.fifo.first_data_timeout_ms=20;
 LiveAcquisitionPort p=live_fifo_acquisition_port(&acquisition);
 assert(p.start(p.context,1,&c)==TR2_OK);
 for(time64=1;time64<=12;time64++)assert(live_fifo_acquisition_poll(&acquisition)==TR2_OK);
 assert(acquisition.fifo.total_xyz>0);
 /* A startup allowance never changes the ordinary service-gap guard. */
 time64+=6;assert(live_fifo_acquisition_poll(&acquisition)!=TR2_OK&&!acquisition.fifo.running);
 setup();acquisition.fifo.ctx.read_reg=delayed_read;acquisition.fifo.first_data_timeout_ms=10;
 p=live_fifo_acquisition_port(&acquisition);assert(p.start(p.context,1,&c)==TR2_OK);
 for(time64=1;time64<=10;time64++)assert(live_fifo_acquisition_poll(&acquisition)==TR2_OK);
 time64=11;assert(live_fifo_acquisition_poll(&acquisition)!=TR2_OK&&!acquisition.fifo.running);
 puts("Explicit first-data allowance: delayed startup, finite timeout and unchanged service-gap guard: PASS");
}
static void bounded_poll_quantum(void)
{
 ActiveConfigurationSnapshot c=configuration();setup();
 assert(acquisition.fifo.poll_word_limit==128);
 assert(!fifo_live_set_poll_limit(&acquisition.fifo,8));consumer_limit=8;
 LiveAcquisitionPort p=live_fifo_acquisition_port(&acquisition);
 assert(p.start(p.context,1,&c)==TR2_OK);
 assert(fifo_live_set_poll_limit(&acquisition.fifo,64)==BENCH_CONFIG);
 for(unsigned i=0;i<100;i++){time64++;assert(live_fifo_acquisition_poll(&acquisition)==TR2_OK);}
 assert(delivered==acquisition.fifo.total_xyz&&delivered>600&&delivered<800);
 assert(p.stop(p.context)==TR2_OK);
 assert(!fifo_live_set_poll_limit(&acquisition.fifo,128));
 puts("Opt-in poll quantum:8 words across continuous parser boundaries, unchanged128 default and no mutation while running: PASS");
}
int main(void)
{
 bounded_poll_quantum();
 first_data_policy();real_engine_port();metrology_stream();
 ActiveConfigurationSnapshot c=configuration();LiveAcquisitionObservation o;
 LiveAcquisitionPort p;
 for(unsigned fs=0;fs<4;fs++) {
  setup();p=live_fifo_acquisition_port(&acquisition);c.payload.full_scale_code=(uint16_t)fs;
  assert(!p.io_admissible(p.context,13));assert(p.stop(p.context)==TR2_OK);
  assert(p.start(p.context,1,&c)==TR2_OK&&p.observe(p.context,&o)==TR2_OK);
  assert(o.running&&!o.stopped&&o.configuration_confirmed&&o.run_id==1&&o.config_generation==1);
  assert(p.start(p.context,2,&c)!=TR2_OK);
  time64=UINT32_MAX-5000u;acquisition.fifo.last_ms=acquisition.fifo.last_data_ms=time64;
  for(unsigned i=0;i<10000;i++){time64++;assert(live_fifo_acquisition_poll(&acquisition)==TR2_OK);}
  assert(delivered==acquisition.fifo.total_xyz&&delivered>1000000u&&acquisition.fifo.running);
  assert(p.stop(p.context)==TR2_OK&&p.stop(p.context)==TR2_OK);
  assert(p.observe(p.context,&o)==TR2_OK&&o.stopped&&!o.running&&o.run_id==1);
  assert(p.start(p.context,1,&c)!=TR2_OK);
 }
 for(unsigned m=1;m<=11;m++) {
  if(m==9)continue;
  setup();p=live_fifo_acquisition_port(&acquisition);assert(p.start(p.context,1,&c)==TR2_OK);mode=m;
  bool failed=false;
  for(unsigned i=0;i<20;i++){time64++;if(live_fifo_acquisition_poll(&acquisition)!=TR2_OK){failed=true;break;}}
  assert(failed&&!acquisition.fifo.running);
 }
 setup();p=live_fifo_acquisition_port(&acquisition);assert(p.start(p.context,1,&c)==TR2_OK);
 time64=6;assert(live_fifo_acquisition_poll(&acquisition)!=TR2_OK&&!acquisition.fifo.running);
 setup();p=live_fifo_acquisition_port(&acquisition);assert(p.start(p.context,1,&c)==TR2_OK);
 consumer_fail=1;time64++;assert(live_fifo_acquisition_poll(&acquisition)!=TR2_OK&&delivered==0);
 for(unsigned pos=1;pos<=40;pos++) {
  setup();p=live_fifo_acquisition_port(&acquisition);fail_io=pos;
  int result=p.start(p.context,1,&c);
  if(result!=TR2_OK)assert(!acquisition.fifo.running);
  else {fail_io=0;assert(p.stop(p.context)==TR2_OK);}
 }
 puts("Continuous FIFO: four full scales, >4M XYZ beyond finite capacity, 64-bit time boundary, guarded consumer, faults, STOP and40 start I/O cuts: PASS");
 return 0;
}
