#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "tr2/application/live_metrology.h"
#include "tr2/platform/iis3dwb_units.h"
#include "live_metrology_oracles.h"
static ActiveConfigurationSnapshot config(void)
{
 ActiveConfigurationSnapshot c={0};c.generation=c.config_id=1;
 c.payload.sampling_frequency_hz=26667;c.payload.axes_enable_mask=7;c.payload.acquisition_mode=2;
 c.payload.window_size_samples=4096;c.payload.indicator_period_ms=2000;
 c.payload.supervision_enable_mask=1;c.payload.campaign_context_id=c.payload.mission_id=1;
 c.payload.rms_warn_threshold_mg=c.payload.peak_warn_threshold_mg=100;
 c.payload.rms_alarm_threshold_mg=c.payload.peak_alarm_threshold_mg=200;
 return c;
}
static uint32_t val(const ModbusBlock3Image *b,unsigned i)
{return (uint32_t)b->registers[i]*65536u+b->registers[i+1];}
static void window_utc(LiveMetrology *s,int16_t raw,uint64_t first,bool utc_valid,uint64_t utc)
{
 int16_t xyz[3]={raw,raw,raw};uint64_t base=s->next_index;
 for(unsigned i=0;i<s->active.payload.window_size_samples;i++)
  assert(live_metrology_sample(s,s->run_id,s->active.config_id,s->active.generation,
   base+i,xyz,first+(uint64_t)i*1000/26667,utc_valid,utc)==TR2_OK);
}
static void window(LiveMetrology *s,int16_t raw,uint64_t first)
{window_utc(s,raw,first,true,123);}
static void numeric_oracles(void)
{
 for(unsigned v=0;v<sizeof(live_oracles)/sizeof(live_oracles[0]);v++) {
  const LiveOracle *o=&live_oracles[v];LiveMetrology s;ActiveConfigurationSnapshot c=config();ModbusBlock3Image b;
  c.payload.full_scale_code=(uint16_t)o->fs;c.payload.axes_enable_mask=(uint16_t)o->axes;c.payload.window_size_samples=(uint16_t)o->n;
  live_metrology_init(&s);assert(live_metrology_start(&s,1,&c)==TR2_OK);
  for(unsigned i=0;i<o->n;i++) {
   assert(live_metrology_sample(&s,1,1,1,i,o->raw[i%o->length],(uint64_t)i*1000/26667,false,0)==TR2_OK);
   if(i==o->n-2)assert(!s.has_value&&s.sequence==0);
  }
  assert(live_metrology_project_b3(&s,s.last_ms,&b)==TR2_OK);
  assert(val(&b,14)==o->rms_global&&val(&b,16)==o->peak_global);
  for(unsigned a=0;a<3;a++)assert(val(&b,18+2*a)==o->rms[a]&&val(&b,24+2*a)==o->peak[a]);
  assert(val(&b,12)==o->n&&val(&b,8)==1&&b.registers[0]==4);
  assert((b.registers[1]&0xDF)==0x9F && !(b.registers[1]&0x100));
  for(unsigned i=40;i<48;i++)assert(!b.registers[i]);
 }
 puts("448 independent Python numeric oracles: four FS, seven masks, four N, constants/AC/near-saturation: PASS");
}
static void quality_and_time(void)
{
 LiveMetrology s;ActiveConfigurationSnapshot c=config();ModbusBlock3Image b;int16_t xyz[3]={100,100,100};
 live_metrology_init(&s);assert(live_metrology_project_b3(&s,0,&b)==TR2_OK&&b.registers[0]==0&&!b.registers[1]);
 assert(live_metrology_start(&s,1,&c)==TR2_OK);
 assert(live_metrology_project_b3(&s,0,&b)==TR2_OK&&b.registers[0]==2&&!val(&b,8));
 window(&s,100,0);uint64_t end=s.value.value_monotonic_ms;
 assert(live_metrology_project_b3(&s,end+4000,&b)==TR2_OK&&(b.registers[1]&4));
 assert(live_metrology_project_b3(&s,end+4001,&b)==TR2_OK&&!(b.registers[1]&4)&&(b.registers[1]&512));
 assert(live_metrology_project_b3(&s,end+UINT32_MAX+10ull,&b)==TR2_OK&&val(&b,6)==UINT32_MAX);
 live_metrology_stop(&s);assert(live_metrology_project_b3(&s,end+UINT32_MAX+10ull,&b)==TR2_OK&&b.registers[0]==1&&(b.registers[1]&4096));
 assert((b.registers[1]&2)&&!(b.registers[1]&4));
 uint32_t seq=s.sequence;assert(live_metrology_start(&s,2,&c)==TR2_OK);
 window(&s,100,end+UINT32_MAX+20ull);assert(s.sequence==seq+1&&s.last_ms>UINT32_MAX);
 uint32_t date=s.value.civil_timestamp;
 assert(live_metrology_project_b3(&s,s.last_ms,&b)==TR2_OK&&val(&b,4)==date);
 live_metrology_invalidate(&s,false,true);assert(live_metrology_project_b3(&s,s.last_ms,&b)!=TR2_OK);
 assert(!(b.registers[1]&(2|4|128))&&(b.registers[1]&512)&&val(&b,4)==date);
 assert(!b.registers[30]);
 assert(live_metrology_start(&s,3,&c)==TR2_OK);s.sequence=UINT32_MAX;s.alarm_count=s.exceed_count=UINT32_MAX;
 window(&s,10000,UINT64_C(6048010000));assert(s.sequence==UINT32_MAX&&s.running);
 assert(s.alarm_count==UINT32_MAX&&s.exceed_count==UINT32_MAX);
 assert(live_metrology_sample(&s,3,1,1,s.next_index,xyz,0,true,0)!=TR2_OK&&!s.running);
 puts("B3 age2P/2P+1, retained STOP/config loss, rollover64, >7d, saturated counters, reversal: PASS");
}
static void alarm_lifecycle(void)
{
 LiveMetrology s;ActiveConfigurationSnapshot c=config();ModbusBlock3Image b;
 c.payload.axes_enable_mask=1;c.payload.threshold_hysteresis_mg=20;c.payload.alarm_hold_time_ms=4000;
 live_metrology_init(&s);assert(live_metrology_start(&s,1,&c)==TR2_OK);
 window(&s,4000,0);assert(s.alarms[0].pending&&!s.alarms[0].alarm&&s.exceed_count==1);
 window(&s,4000,2000);assert(!s.alarms[0].alarm);
 window(&s,4000,4000);assert(s.alarms[0].alarm&&s.alarm_count==1&&s.latched);
 assert(live_metrology_ack(&s,s.last_ms)!=TR2_OK);
 window(&s,3279,6000);assert(s.value.rms_x_mg==200&&s.alarms[0].alarm);
 window(&s,2951,8000);assert(s.value.rms_x_mg==180&&!s.alarms[0].alarm);
 assert(live_metrology_ack(&s,s.last_ms)==TR2_OK&&!s.latched);
 window(&s,1311,10000);assert(s.value.rms_x_mg==80&&!s.alarms[0].warning);
 window(&s,1639,12000);assert(s.value.rms_x_mg==100&&!s.alarms[0].warning); /* equality */
 window(&s,4000,14000);assert(s.alarms[0].pending);
 live_metrology_stop(&s);assert(!s.alarms[0].pending);
 assert(live_metrology_start(&s,2,&c)==TR2_OK);window(&s,4000,16000);assert(!s.alarms[0].alarm);
 assert(live_metrology_project_b3(&s,s.last_ms,&b)==TR2_OK&&b.registers[30]==1);
 live_metrology_stop(&s);c.payload.axes_enable_mask=7;c.payload.alarm_hold_time_ms=0;c.payload.threshold_hysteresis_mg=65535;
 assert(live_metrology_start(&s,3,&c)==TR2_OK);window(&s,4000,20000);
 assert(s.alarm_count==2&&s.exceed_count==2); /* union, not four axes */
 assert(live_metrology_project_b3(&s,s.last_ms,&b)==TR2_OK&&b.registers[30]==4&&b.registers[2]==4);
 window(&s,0,22000);assert(!s.alarms[0].alarm&&s.latched);
 assert(live_metrology_ack(&s,s.last_ms)==TR2_OK);
 puts("Alarms: equality, hold boundary, cancellation, hysteresis floor, latch ACK, union counters, dominance tie: PASS");
}
static void strict_faults(void)
{
 ActiveConfigurationSnapshot c=config();int16_t raw[3]={0,0,0};
 for(unsigned kind=0;kind<7;kind++) {
  LiveMetrology s;ModbusBlock3Image before,after;live_metrology_init(&s);assert(live_metrology_start(&s,1,&c)==TR2_OK);
  window(&s,100,0);assert(live_metrology_project_b3(&s,s.last_ms,&before)==TR2_OK);
  if(kind<3) {raw[kind]=INT16_MAX;assert(live_metrology_sample(&s,1,1,1,s.next_index,raw,s.last_ms,false,0)!=TR2_OK);raw[kind]=0;}
  if(kind==3)assert(live_metrology_sample(&s,2,1,1,s.next_index,raw,s.last_ms,false,0)!=TR2_OK);
  if(kind==4)assert(live_metrology_sample(&s,1,2,1,s.next_index,raw,s.last_ms,false,0)!=TR2_OK);
  if(kind==5)assert(live_metrology_sample(&s,1,1,2,s.next_index,raw,s.last_ms,false,0)!=TR2_OK);
  if(kind==6)assert(live_metrology_sample(&s,1,1,1,s.next_index+1,raw,s.last_ms,false,0)!=TR2_OK);
  assert(live_metrology_project_b3(&s,s.last_ms,&after)!=TR2_OK);
  assert(!s.running&&after.registers[0]==5&&!(after.registers[1]&6));
  for(unsigned i=4;i<30;i++)assert(before.registers[i]==after.registers[i]);
 }
 LiveMetrology s;live_metrology_init(&s);c.payload.axes_enable_mask=1;assert(live_metrology_start(&s,1,&c)==TR2_OK);
 raw[1]=INT16_MIN;assert(live_metrology_sample(&s,1,1,1,0,raw,0,false,0)!=TR2_OK);raw[1]=0;
 assert(live_metrology_start(&s,2,&c)==TR2_OK);window(&s,0,0);
 assert(live_metrology_start(&s,3,&c)!=TR2_OK);
 live_metrology_stop(&s);assert(live_metrology_start(&s,3,&c)==TR2_OK);
 assert(live_metrology_sample(&s,3,1,1,0,raw,2000,false,0)==TR2_OK);
 s.window.sum_square_vector_mg2=UINT64_MAX;raw[0]=100;
 assert(live_metrology_sample(&s,3,1,1,1,raw,2001,false,0)!=TR2_OK&&s.calc_error);
 assert(live_metrology_start(&s,4,&c)==TR2_OK);
 assert(live_metrology_sample(&s,4,1,1,0,raw,UINT64_MAX-1,false,0)!=TR2_OK);
 puts("Causal/index/config faults, all-axis saturation including disabled axis, accumulator/time overflow: PASS");
}
static void missed_window_cancels_hold(void)
{
 LiveMetrology s;ActiveConfigurationSnapshot c=config();
 c.payload.alarm_hold_time_ms=6000;live_metrology_init(&s);
 assert(live_metrology_start(&s,1,&c)==TR2_OK);
 window(&s,4000,0);assert(s.alarms[0].candidate_ms==153);
 window(&s,4000,4000); /* expected period2000 did not provide a valid window */
 assert(!s.alarms[0].alarm&&s.alarms[0].candidate_ms==4153);
 puts("A missing valid window cancels the pending hold without extrapolation: PASS");
}
static void chunking_and_utc(void)
{
 LiveMetrology a,b;ActiveConfigurationSnapshot c=config();ModbusBlock3Image x,y;
 live_metrology_init(&a);live_metrology_init(&b);
 assert(live_metrology_start(&a,1,&c)==TR2_OK&&live_metrology_start(&b,1,&c)==TR2_OK);
 const unsigned total=200000;
 for(unsigned chunk=0;chunk<total;) {
  unsigned size=1u+chunk%128u;if(size>total-chunk)size=total-chunk;
  for(unsigned i=chunk;i<chunk+size;i++) {
   int16_t raw[3]={(int16_t)(i%1000),-300,200};
   assert(live_metrology_sample(&a,1,1,1,i,raw,(uint64_t)i*1000/26667,true,UINT64_C(4294967296))==TR2_OK);
  }
  chunk+=size;
 }
 for(unsigned i=0;i<total;i++) {
  int16_t raw[3]={(int16_t)(i%1000),-300,200};
  assert(live_metrology_sample(&b,1,1,1,i,raw,(uint64_t)i*1000/26667,true,UINT64_C(4294967296))==TR2_OK);
 }
 assert(live_metrology_project_b3(&a,a.last_ms,&x)==TR2_OK&&live_metrology_project_b3(&b,b.last_ms,&y)==TR2_OK);
 assert(!memcmp(&x,&y,sizeof(x))&&a.sequence==4&&!a.value.civil_timestamp_available);
 assert(a.value.value_monotonic_ms==6153); /* starts0/2000/4000/6000, exact N4096 */
 assert(a.value.window_duration_ms==153);
 /* Query clock is separate from sample event time: queued older samples are
  * allowed, but neither stream may reverse its own observed monotone. */
 assert(live_metrology_project_b3(&a,a.last_ms+1,&x)==TR2_OK);
 int16_t raw[3]={0,0,0};
 assert(live_metrology_sample(&a,1,1,1,a.next_index,raw,a.last_ms,true,0)==TR2_OK);
 live_metrology_stop(&a);assert(live_metrology_start(&a,2,&c)==TR2_OK);
 window_utc(&a,100,8000,true,0); /* actual per-sample trustworthy epoch0 */
 assert(live_metrology_project_b3(&a,a.last_ms,&x)==TR2_OK&&(x.registers[1]&64)&&val(&x,4)==0);
 assert(live_metrology_project_b3(&a,a.last_read_ms-1,&x)!=TR2_OK&&!a.running);
 puts("Burst chunking invariance, four exact periodic windows, UTC overflow/epoch0, independent monotone streams: PASS");
}
int main(void)
{numeric_oracles();quality_and_time();alarm_lifecycle();strict_faults();missed_window_cancels_hold();chunking_and_utc();return 0;}
