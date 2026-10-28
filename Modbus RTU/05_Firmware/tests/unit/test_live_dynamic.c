#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "tr2/application/live_metrology.h"

static ActiveConfigurationSnapshot configuration(void)
{
 ActiveConfigurationSnapshot c={0};c.config_id=4;c.generation=c.revision_counter=4;
 c.payload.sampling_frequency_hz=26667;c.payload.axes_enable_mask=7;c.payload.full_scale_code=2;
 c.payload.acquisition_mode=2;c.payload.window_size_samples=4096;c.payload.indicator_period_ms=2000;
 c.payload.supervision_enable_mask=1;c.payload.campaign_context_id=c.payload.mission_id=1;
 c.payload.rms_alarm_threshold_mg=10000;c.payload.peak_alarm_threshold_mg=15000;
 return c;
}
static void feed(LiveMetrology *m,const int16_t xyz[3],uint64_t i)
{assert(live_metrology_sample(m,m->run_id,4,4,i,xyz,i*1000/26667,false,0)==TR2_OK);}
static void constant(void)
{
 LiveMetrology m;ActiveConfigurationSnapshot c=configuration();int16_t xyz[3]={4000,-1200,300};
 live_metrology_init(&m);m.dynamic.enabled=true;assert(live_metrology_start(&m,1,&c)==TR2_OK);
 for(unsigned i=0;i<70000;i++)feed(&m,xyz,i);
 assert(m.value.rms_global_mg>1000&&m.dynamic.valid&&m.dynamic.sequence==1);
 assert(m.dynamic.value.rms_global_mg==0&&m.dynamic.value.peak_global_mg==0);
 assert(m.dynamic.value.valid_sample_count==4096);
 live_metrology_stop(&m);assert(!m.dynamic.running&&m.dynamic.valid&&m.dynamic.has_value);
 assert(live_metrology_start(&m,2,&c)==TR2_OK&&!m.dynamic.valid&&!m.dynamic.has_value);
 assert(live_metrology_sample(&m,2,4,4,0,xyz,3000,false,0)==TR2_OK);assert(!m.dynamic.valid&&m.dynamic.samples==1);
 live_metrology_invalidate(&m,true,false);assert(!m.dynamic.valid&&!m.dynamic.running);
}
static void sine(double frequency,double amplitude)
{
 LiveMetrology m;ActiveConfigurationSnapshot c=configuration();live_metrology_init(&m);
 m.dynamic.enabled=true;assert(live_metrology_start(&m,1,&c)==TR2_OK);
 const double pi=acos(-1.0),omega=2*pi*frequency/26667;
 const unsigned end=26667*2+4096;
 for(unsigned i=0;i<end;i++) {
  int16_t xyz[3]={(int16_t)lround((1000+amplitude*sin(omega*i))/.244),0,0};feed(&m,xyz,i);
 }
 assert(m.dynamic.valid);const double a=(double)TR2_AC1_ALPHA_Q30/1073741824.0;
 double real=(1-a)*(1-cos(omega)),imag=(1-a)*sin(omega);
 double denominator_r=1-(1-a)*cos(omega),denominator_i=(1-a)*sin(omega);
 double gain=hypot(real,imag)/hypot(denominator_r,denominator_i);
 double phase=atan2(imag,real)-atan2(denominator_i,denominator_r);
 const unsigned start=end-4096;
 double sum_cos=sin(4096*omega)/sin(omega)*cos(2*(omega*start+phase)+(4096-1)*omega);
 double expected=amplitude*gain*sqrt(.5-sum_cos/(2*4096));
 assert(fabs((double)m.dynamic.value.rms_x_mg-expected)<3);
 assert(fabs((double)m.dynamic.value.peak_x_mg-amplitude*gain)<3);
 assert(m.dynamic.value.rms_global_mg==m.dynamic.value.rms_x_mg);
 assert(m.dynamic.value.rms_y_mg==0&&m.dynamic.value.rms_z_mg==0);
 printf("AC1 deterministic %.0fHz %.0fmg: RMS%u expected%.3f peak%u gain%.6f PASS\n",frequency,amplitude,m.dynamic.value.rms_x_mg,expected,m.dynamic.value.peak_x_mg,gain);
}
static void rails_and_overflow(void)
{
 ActiveConfigurationSnapshot c=configuration();
 for(unsigned n=0;n<2;n++) {
  LiveMetrology m;live_metrology_init(&m);m.dynamic.enabled=true;assert(live_metrology_start(&m,1,&c)==TR2_OK);
  int16_t xyz[3]={n?INT16_MIN:INT16_MAX,0,0};
  assert(live_metrology_sample(&m,1,4,4,0,xyz,0,false,0)!=TR2_OK);assert(!m.dynamic.valid&&!m.dynamic.running);
 }
 /* Largest representable non-rail sign reversals, three axes, all four FS.
  * Exercise the Q16 signed64 multiply under UBSan; no fabricated validity. */
 for(unsigned fs=0;fs<4;fs++) {
  LiveMetrology m;c.payload.full_scale_code=(uint16_t)fs;
  live_metrology_init(&m);m.dynamic.enabled=true;assert(live_metrology_start(&m,1,&c)==TR2_OK);
  for(unsigned i=0;i<70000;i++){int16_t v=(i&1)?32766:-32767,xyz[3]={v,v,v};feed(&m,xyz,i);}
  assert(m.dynamic.valid&&m.dynamic.value.rms_global_mg>0&&m.dynamic.value.peak_global_mg<60000);
 }
}
int main(void)
{constant();sine(20,500);sine(50,500);sine(1000,500);sine(100,6500);rails_and_overflow();puts("AC1 DC rejection, warmup, stop/restart/fault, signed FS rails and analytic sine oracles: PASS");return 0;}
