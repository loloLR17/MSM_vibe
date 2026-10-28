#include "tr2/application/live_dynamic.h"
#include <string.h>
#include <limits.h>

static int32_t rounded(int64_t n,int64_t divisor)
{return (int32_t)(n>=0?(n+divisor/2)/divisor:-((-n+divisor/2)/divisor));}
void live_dynamic_start(LiveDynamic *s,uint64_t run,const ActiveConfigurationSnapshot *config)
{
 bool enabled=s->enabled;memset(s,0,sizeof(*s));s->enabled=enabled;
 if(enabled){s->config=*config;s->run_id=run;s->running=true;}
}
void live_dynamic_stop(LiveDynamic *s){s->running=false;s->window_active=false;}
void live_dynamic_invalidate(LiveDynamic *s)
{s->valid=false;live_dynamic_stop(s);}
Tr2Result live_dynamic_sample(LiveDynamic *s,const int16_t xyz[3],uint64_t now)
{
 static const int32_t sensitivity[4]={61,122,244,488};
 if(!s||!xyz||!s->enabled||!s->running||s->config.payload.full_scale_code>3)
  return TR2_ERROR_INVALID_STATE;
 VibrationSample sample={0};int32_t hp[3]={0};
 if(!s->initialized){s->first_ms=now;s->initialized=true;
  for(unsigned i=0;i<3;i++)s->dc_q16[i]=(int64_t)xyz[i]*sensitivity[s->config.payload.full_scale_code]*65536LL;
 }
 if(now<s->first_ms||s->samples==UINT64_MAX)return TR2_ERROR_INVALID_STATE;
 for(unsigned i=0;i<3;i++) {
  if(xyz[i]==INT16_MIN||xyz[i]==INT16_MAX)return TR2_ERROR_INVALID_STATE;
  int64_t input=(int64_t)xyz[i]*sensitivity[s->config.payload.full_scale_code]*65536LL;
  /* Bounds at +/-16g including a full sign reversal: product <1.1e18,
   * signed64 safe. No left-shift of a negative signed number. */
  s->dc_q16[i]+=(input-s->dc_q16[i])*TR2_AC1_ALPHA_Q30/1073741824LL;
  if(s->config.payload.axes_enable_mask&(1u<<i))hp[i]=rounded(input-s->dc_q16[i],65536000LL);
 }
 s->samples++;
 if(s->samples<TR2_AC1_WARMUP_SAMPLES)return TR2_OK;
 if(!s->grid_ready) {
  uint64_t p=s->config.payload.indicator_period_ms,elapsed=now-s->first_ms;
  if(!p||now>UINT64_MAX-p)return TR2_ERROR_INVALID_STATE;
  s->next_window_ms=s->first_ms+((elapsed+p-1)/p)*p;s->grid_ready=true;
 }
 if(!s->window_active) {
  if(now<s->next_window_ms)return TR2_OK;
  if(now>UINT64_MAX-s->config.payload.indicator_period_ms)return TR2_ERROR_INVALID_STATE;
  s->next_window_ms=now+s->config.payload.indicator_period_ms;
  memset(&s->window,0,sizeof(s->window));s->window.configuration.generation=s->config.generation;
  s->window.configuration.config_id=s->config.config_id;
  s->window.configuration.revision_counter=s->config.revision_counter;
  s->window.configuration.payload=s->config.payload;
  s->window.start_monotonic_ms=now;s->window_active=true;
 }
 sample.x_mg=hp[0];sample.y_mg=hp[1];sample.z_mg=hp[2];sample.valid=true;
 if(!acquisition_window_accumulate_valid_sample(&s->window,&sample))return TR2_ERROR_INVALID_STATE;
 s->window.valid_sample_count++;s->window.acquired_sample_count++;
 if(s->window.valid_sample_count==s->config.payload.window_size_samples) {
  s->window.end_monotonic_ms=now;s->window.complete=true;
  if(vibration_indicators_from_window(&s->window,&s->value)!=TR2_OK)return TR2_ERROR_INVALID_STATE;
  for(unsigned i=0;i<3;i++)s->value_dc_ug[i]=rounded(s->dc_q16[i],65536LL);
  s->value_ms=now;s->has_value=s->valid=true;s->window_active=false;
  if(s->sequence<UINT32_MAX)s->sequence++;
 }
 return TR2_OK;
}
