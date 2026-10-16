#include "fifo_event_time.h"
#include <assert.h>
#include <stdio.h>
static unsigned parity(unsigned n){unsigned p=0;while(n){p^=n&1;n>>=1;}return p;}
static void anchor(uint8_t b[7],uint32_t ticks,unsigned ctr)
{b[0]=(uint8_t)(32+2*ctr);b[0]|=(uint8_t)parity(b[0]);for(unsigned i=0;i<4;i++)b[i+1]=(uint8_t)(ticks>>(8*i));b[5]=0;b[6]=10;}
static void xyz(uint8_t b[7],unsigned ctr)
{b[0]=(uint8_t)(16+2*ctr);b[0]|=(uint8_t)parity(b[0]);b[1]=0xe8;b[2]=3;b[3]=b[4]=b[5]=b[6]=0;}
static ActiveConfigurationSnapshot cfg(void)
{
 ActiveConfigurationSnapshot c={0};c.config_id=3;c.generation=3;c.payload.sampling_frequency_hz=26667;
 c.payload.axes_enable_mask=7;c.payload.acquisition_mode=2;c.payload.window_size_samples=4096;
 c.payload.indicator_period_ms=2000;c.payload.supervision_enable_mask=1;
 c.payload.campaign_context_id=c.payload.mission_id=1;
 c.payload.rms_warn_threshold_mg=c.payload.peak_warn_threshold_mg=100;
 c.payload.rms_alarm_threshold_mg=c.payload.peak_alarm_threshold_mg=200;return c;
}
int main(void)
{
 FifoLive f={0};LiveMetrology m;FifoEventTime t;uint8_t b[7];ActiveConfigurationSnapshot c=cfg();
 f.run_id=5;f.config_id=3;f.config_generation=3;live_metrology_init(&m);assert(live_metrology_start(&m,5,&c)==TR2_OK);
 /* Actual anchors in this oracle straddle raw32 rollover. All fractional
  * times arise from these anchors and the explicit observed clock ratio. */
 uint32_t origin=0xfffffff0u;assert(fifo_event_time_init(&t,&f,&m,origin,5000000,1000000,80000)==TR2_OK);
 for(unsigned i=0;i<=4096;i++) {
  if(i%8==0){anchor(b,origin+(i/8)*24,0);assert(!fifo_event_time_consume(&t,b,1,i));}
  if(i<4096){xyz(b,i%4);assert(!fifo_event_time_consume(&t,b,1,i));}
 }
 assert(t.emitted==4096&&t.groups==512&&t.count==0&&m.has_value&&m.sequence==1);
 assert(m.value.rms_x_mg==61&&m.value.rms_global_mg==61&&m.value.peak_global_mg==61);
 assert(m.value.window_duration_ms==153&&m.value.value_monotonic_ms==5153&&!m.value.civil_timestamp_available);
 ModbusBlock3Image out;assert(live_metrology_project_b3(&m,5200,&out)==TR2_OK);assert((out.registers[1]&6)==6&&!(out.registers[1]&64));
 assert(fifo_event_time_check(&t,origin+16000,5199990,5200010,30)==TR2_OK);
 assert(fifo_event_time_check(&t,origin+16000,5200200,5200210,30)!=TR2_OK&&t.fault);
 /* Never publish an unbracketed group; malformed counter/empty group fail. */
 live_metrology_init(&m);assert(live_metrology_start(&m,5,&c)==TR2_OK);
 assert(fifo_event_time_init(&t,&f,&m,100,1000000,1000000,80000)==TR2_OK);
 anchor(b,100,0);assert(!fifo_event_time_consume(&t,b,1,0));xyz(b,0);assert(!fifo_event_time_consume(&t,b,1,0));assert(!m.has_value&&t.emitted==0);
 anchor(b,124,0);assert(fifo_event_time_consume(&t,b,1,1)==-1&&t.fault);
 assert(fifo_event_time_init(&t,&f,&m,100,1000000,1000000,80000)==TR2_OK);
 anchor(b,100,0);assert(!fifo_event_time_consume(&t,b,1,0));xyz(b,1);assert(fifo_event_time_consume(&t,b,1,0)==-1);
 assert(fifo_event_time_init(&t,&f,&m,100,UINT64_MAX-1,1000000,80000)==TR2_OK);
 assert(fifo_event_time_check(&t,200,0,10,100)==TR2_ERROR_INVALID_STATE);
 puts("fifo_event_time PASS: real-anchor interpolation, wrap, exact indices, UTC absent, RMS oracle, drift and malformed groups fail closed");return 0;
}
