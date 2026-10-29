#include "fifo_event_time.h"
#include <string.h>
#include <limits.h>
#if TR2_FIFO_TIME_PROFILE
extern uint32_t fifo_event_time_cycles(void);
#endif
static bool scale(const FifoEventTime *t,uint64_t eighths,uint64_t *us)
{
 if(!t->span_ticks||t->span_ticks>UINT64_MAX/8u||eighths>UINT64_MAX/t->span_us)return false;
 uint64_t delta=eighths*t->span_us/(8u*t->span_ticks);
 if(delta>UINT64_MAX-t->base_us)return false;
 *us=t->base_us+delta;return true;
}
static Tr2Result init(FifoEventTime *t,FifoLive *f,LiveMetrology *m,
 FifoTimedSink sink,void *context,uint32_t raw,uint64_t base_us,uint64_t span_us,uint64_t span_ticks)
{
 if(!t||!f||(!m&&!sink)||(m&&sink)||!span_us||!span_ticks||span_ticks>UINT32_MAX||span_us>UINT32_MAX)
  return TR2_ERROR_INVALID_ARGUMENT;
 memset(t,0,sizeof(*t));t->fifo=f;t->metric=m;t->sink=sink;t->sink_context=context;t->raw=raw;t->base_us=base_us;
 t->span_us=span_us;t->span_ticks=span_ticks;t->calibration_raw=raw;t->calibration_us=base_us;t->initialized=true;return TR2_OK;
}
Tr2Result fifo_event_time_init(FifoEventTime *t,FifoLive *f,LiveMetrology *m,
 uint32_t raw,uint64_t base_us,uint64_t span_us,uint64_t span_ticks)
{return init(t,f,m,NULL,NULL,raw,base_us,span_us,span_ticks);}
Tr2Result fifo_event_time_init_sink(FifoEventTime *t,FifoLive *f,FifoTimedSink sink,void *context,
 uint32_t raw,uint64_t base_us,uint64_t span_us,uint64_t span_ticks)
{return init(t,f,NULL,sink,context,raw,base_us,span_us,span_ticks);}
static Tr2Result failed(FifoEventTime *t)
{
 t->fault=true;return TR2_ERROR_INVALID_STATE;
}
Tr2Result fifo_event_time_check(FifoEventTime *t,uint32_t raw,uint64_t before,uint64_t after,uint64_t allowed)
{
 if(!t||!t->initialized||t->fault)return TR2_ERROR_INVALID_STATE;
 uint32_t d=raw-t->raw;uint64_t predicted;
 if(after<before||d>=0x80000000u||t->ticks>UINT64_MAX-d||t->ticks+d>UINT64_MAX/8u||
  !scale(t,(t->ticks+d)*8u,&predicted))return failed(t);
 uint64_t residual=predicted<before?before-predicted:predicted>after?predicted-after:0;
 if(residual>t->max_residual_us)t->max_residual_us=residual;
 if(residual>allowed)return failed(t);
 return TR2_OK;
}
int fifo_event_time_consume(void *context,const uint8_t *words,uint32_t n,uint64_t index)
{
 FifoEventTime *t=context;
 if(!t||!words||!t->initialized||t->fault||index!=t->input_index)return -1;
 for(uint32_t i=0;i<n;i++) {
  const uint8_t *b=words+7u*i;unsigned tag=b[0]>>3,counter=(b[0]>>1)&3;
  if(tag==4) {
   uint32_t raw=(uint32_t)b[1]|(uint32_t)b[2]<<8|(uint32_t)b[3]<<16|(uint32_t)b[4]<<24;
   uint32_t delta=raw-t->raw;
   if(b[5]||b[6]!=10||delta>=0x80000000u||t->ticks>UINT64_MAX-delta)goto error;
   t->ticks+=delta;t->raw=raw;
   if(t->anchor) {
    uint64_t width=t->ticks-t->anchor_ticks;
    if(t->count!=8||width<23u||width>25u||counter!=t->counter||t->anchor_ticks>UINT64_MAX/8u)goto error;
    for(unsigned j=0;j<8;j++) {
#if TR2_FIFO_TIME_PROFILE
     uint32_t started=fifo_event_time_cycles();
#endif
     uint64_t eighths=t->anchor_ticks*8u,us;
     if(width>UINT64_MAX/8u||eighths>UINT64_MAX-j*width||!scale(t,eighths+j*width,&us)||
        (t->emitted&&us<t->last_event_us))goto error;
#if TR2_FIFO_TIME_PROFILE
     t->mapping_cycles+=(uint32_t)(fifo_event_time_cycles()-started);
     started=fifo_event_time_cycles();
#endif
     Tr2Result result=t->sink?t->sink(t->sink_context,t->emitted,t->xyz[j],us):
       live_metrology_sample(t->metric,t->fifo->run_id,t->fifo->config_id,t->fifo->config_generation,
         t->emitted,t->xyz[j],us/1000u,false,0);
     if(result!=TR2_OK)goto error;
#if TR2_FIFO_TIME_PROFILE
     t->metrology_cycles+=(uint32_t)(fifo_event_time_cycles()-started);
#endif
     if(!t->emitted)t->first_event_us=us;
     t->last_event_us=us;t->emitted++;
    }
    t->groups++;
   }else if(t->input_index||t->count)goto error;
   t->anchor=true;t->anchor_ticks=t->ticks;t->anchor_raw=raw;t->counter=(uint8_t)counter;t->count=0;
  }else if(tag==2) {
   if(!t->anchor||t->count>=8||counter!=((t->counter+t->count)&3u)||t->input_index==UINT64_MAX)goto error;
   for(unsigned j=0;j<3;j++) {
    uint16_t bits=(uint16_t)b[1+2*j]|(uint16_t)b[2+2*j]<<8;
    t->xyz[t->count][j]=(int16_t)(bits<=INT16_MAX?(int32_t)bits:(int32_t)bits-65536);
   }
   t->count++;t->input_index++;
  }else goto error;
 }
 return 0;
error:
 (void)failed(t);return -1;
}

Tr2Result fifo_event_time_track(FifoEventTime *t,uint32_t raw,uint64_t before,uint64_t after,uint64_t allowed)
{
 Tr2Result r=fifo_event_time_check(t,raw,before,after,allowed);
 if(r!=TR2_OK)return r;
 uint64_t mid=before+(after-before)/2u,predicted,pivot;
 uint32_t ticks=raw-t->calibration_raw,delta=raw-t->raw;
 if(!ticks||ticks>=0x80000000u||mid<=t->calibration_us||
    t->ticks>UINT64_MAX/8u||t->ticks+delta>UINT64_MAX/8u||
    !scale(t,(t->ticks+delta)*8u,&predicted)||!scale(t,t->ticks*8u,&pivot))return failed(t);
 uint64_t measured=mid-t->calibration_us,corrected=measured;
 if(mid>=predicted) {
  if(corrected>UINT64_MAX-(mid-predicted))return failed(t);
  corrected+=mid-predicted;
 }else {
  if(corrected<=predicted-mid)return failed(t);
  corrected-=predicted-mid;
 }
 if(!corrected||corrected>UINT32_MAX)return failed(t);
 /* Preserve the already observed anchor exactly and leave the <8 pending XYZ
  * unpublished. The new affine interval only applies beyond that anchor. */
 t->base_us=pivot;t->ticks=t->anchor_ticks=0;
 t->span_us=corrected;t->span_ticks=ticks;
 t->calibration_us=mid;t->calibration_raw=raw;return TR2_OK;
}
