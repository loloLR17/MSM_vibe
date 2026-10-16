#include "fifo_event_time.h"
#include <string.h>
#include <limits.h>
static bool scale(const FifoEventTime *t,uint64_t eighths,uint64_t *us)
{
 if(!t->span_ticks||t->span_ticks>UINT64_MAX/8u||eighths>UINT64_MAX/t->span_us)return false;
 uint64_t delta=eighths*t->span_us/(8u*t->span_ticks);
 if(delta>UINT64_MAX-t->base_us)return false;
 *us=t->base_us+delta;return true;
}
Tr2Result fifo_event_time_init(FifoEventTime *t,FifoLive *f,LiveMetrology *m,
 uint32_t raw,uint64_t base_us,uint64_t span_us,uint64_t span_ticks)
{
 if(!t||!f||!m||!span_us||!span_ticks||span_ticks>UINT32_MAX||span_us>UINT32_MAX)
  return TR2_ERROR_INVALID_ARGUMENT;
 memset(t,0,sizeof(*t));t->fifo=f;t->metric=m;t->raw=raw;t->base_us=base_us;
 t->span_us=span_us;t->span_ticks=span_ticks;t->initialized=true;return TR2_OK;
}
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
     uint64_t eighths=t->anchor_ticks*8u,us;
     if(width>UINT64_MAX/8u||eighths>UINT64_MAX-j*width||!scale(t,eighths+j*width,&us)||
        (t->emitted&&us<t->last_event_us))goto error;
     if(live_metrology_sample(t->metric,t->fifo->run_id,t->fifo->config_id,t->fifo->config_generation,
       t->emitted,t->xyz[j],us/1000u,false,0)!=TR2_OK)goto error;
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
