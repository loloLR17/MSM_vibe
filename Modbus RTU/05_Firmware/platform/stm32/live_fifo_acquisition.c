#include "live_fifo_acquisition.h"
#include <string.h>
static Tr2Result start(void *ctx,uint64_t run,const ActiveConfigurationSnapshot *config)
{
 LiveFifoAcquisition *a=ctx;PreparedConfiguration p={0};
 if(!a||!config)return TR2_ERROR_INVALID_ARGUMENT;
 p.config_id=config->config_id;p.payload=config->payload;
 if(configuration_validate_live(&p,NULL).status!=CONFIGURATION_VALIDATION_VALID)return TR2_ERROR_INVALID_ARGUMENT;
 if(fifo_live_start(&a->fifo,run,config->config_id,config->generation,
   config->payload.axes_enable_mask,config->payload.full_scale_code,a->now_ms(a->time_context))) {
  if(a->metrology)live_metrology_invalidate(a->metrology,false,false);
  return TR2_ERROR_UNAVAILABLE;
 }
 if(a->metrology && live_metrology_start(a->metrology,run,config)!=TR2_OK) {
  (void)fifo_live_stop(&a->fifo);return TR2_ERROR_INVALID_STATE;
 }
 return TR2_OK;
}
static Tr2Result stop(void *ctx)
{
 LiveFifoAcquisition *a=ctx;int error=fifo_live_stop(&a->fifo);
 if(a->metrology) {
  if(error)live_metrology_invalidate(a->metrology,false,false);
  else live_metrology_stop(a->metrology);
 }
 return error?TR2_ERROR_UNAVAILABLE:TR2_OK;
}
static Tr2Result observe(void *ctx,LiveAcquisitionObservation *o)
{
 LiveFifoAcquisition *a=ctx;
 if(!o)return TR2_ERROR_INVALID_ARGUMENT;
 memset(o,0,sizeof(*o));
 if(fifo_live_observe(&a->fifo)) {
  if(a->metrology)live_metrology_invalidate(a->metrology,false,false);
  return TR2_ERROR_UNAVAILABLE;
 }
 o->stopped=a->fifo.stopped;o->running=a->fifo.running;o->configuration_confirmed=a->fifo.running;
 o->run_id=a->fifo.run_id;o->config_id=a->fifo.config_id;o->config_generation=a->fifo.config_generation;return TR2_OK;
}
static bool budget(void *ctx,uint16_t code)
{LiveFifoAcquisition *a=ctx;return a->budget&&a->budget(a->budget_context,code);}
Tr2Result live_fifo_acquisition_init(LiveFifoAcquisition *a,const stmdev_ctx_t *ctx,
 uint64_t (*now)(void *),void *time,uint32_t gap,FifoLiveConsume consume,void *consumer)
{
 if(!a||!now)return TR2_ERROR_INVALID_ARGUMENT;
 memset(a,0,sizeof(*a));a->now_ms=now;a->time_context=time;
 return fifo_live_init(&a->fifo,ctx,gap,consume,consumer)?TR2_ERROR_INVALID_ARGUMENT:TR2_OK;
}
LiveAcquisitionPort live_fifo_acquisition_port(LiveFifoAcquisition *a)
{LiveAcquisitionPort p={a,start,stop,observe,budget,NULL};return p;}
Tr2Result live_fifo_acquisition_poll(LiveFifoAcquisition *a)
{
 if(!a||!a->fifo.initialized)return TR2_ERROR_INVALID_ARGUMENT;
 if(!a->fifo.running)return a->fifo.stopped?TR2_OK:TR2_ERROR_INVALID_STATE;
 if(!fifo_live_poll(&a->fifo,a->now_ms(a->time_context)))return TR2_OK;
 if(a->metrology)live_metrology_invalidate(a->metrology,false,false);
 if(a->engine)(void)live_command_engine_fault(a->engine);
 return TR2_ERROR_UNAVAILABLE;
}

static void metrology_safety_stop(void *context)
{ (void)fifo_live_stop(&((LiveFifoAcquisition *)context)->fifo); }
static int metrology_consume(void *context,const uint8_t *words,uint32_t count,uint64_t index)
{
 LiveFifoAcquisition *a=context;
 for(uint32_t i=0;i<count;i++) {
  const uint8_t *b=words+7u*i;
  if((b[0]>>3)!=2u)continue; /* tags/integrity already checked by shared parser */
  uint64_t mono=0,utc=0;bool valid=false;int16_t xyz[3];
  if(a->sample_time(a->sample_time_context,index,&mono,&valid,&utc)!=TR2_OK)return -1;
  for(unsigned j=0;j<3;j++) {
   uint16_t bits=(uint16_t)b[1+2*j]|((uint16_t)b[2+2*j]<<8);
   xyz[j]=(int16_t)(bits<=INT16_MAX?(int32_t)bits:(int32_t)bits-65536);
  }
  if(live_metrology_sample(a->metrology,a->fifo.run_id,a->fifo.config_id,
      a->fifo.config_generation,index,xyz,mono,valid,utc)!=TR2_OK)return -1;
  index++;
 }
 return 0;
}
Tr2Result live_fifo_acquisition_bind_metrology(LiveFifoAcquisition *a,LiveMetrology *m,
 Tr2Result (*time)(void *,uint64_t,uint64_t *,bool *,uint64_t *),void *context)
{
 if(!a || !m || !time || !a->fifo.initialized)return TR2_ERROR_INVALID_ARGUMENT;
 if(a->fifo.running || a->metrology || m->running)return TR2_ERROR_INVALID_STATE;
 a->metrology=m;a->sample_time=time;a->sample_time_context=context;
 a->fifo.consume=metrology_consume;a->fifo.consumer=a;
 m->safe_stop=metrology_safety_stop;m->stop_context=a;return TR2_OK;
}

static void engine_measurement_fault(void *context,bool authority_lost)
{ live_metrology_invalidate(context,false,authority_lost); }
Tr2Result live_fifo_acquisition_bind_engine(LiveFifoAcquisition *a,LiveCommandEngine *engine)
{
 if(!a || !engine || !a->metrology)return TR2_ERROR_INVALID_ARGUMENT;
 if(a->engine || engine->fault_observer || a->fifo.running || engine->booted)
  return TR2_ERROR_INVALID_STATE;
 a->engine=engine;engine->fault_context=a->metrology;
 engine->fault_observer=engine_measurement_fault;return TR2_OK;
}

Tr2Result live_fifo_acquisition_bind_event_metrology(LiveFifoAcquisition *a,LiveMetrology *m,
 FifoLiveConsume consume,void *context)
{
 if(!a||!m||!consume||!context||!a->fifo.initialized)return TR2_ERROR_INVALID_ARGUMENT;
 if(a->fifo.running||a->metrology||m->running)return TR2_ERROR_INVALID_STATE;
 a->metrology=m;a->fifo.consume=consume;a->fifo.consumer=context;
 m->safe_stop=metrology_safety_stop;m->stop_context=a;return TR2_OK;
}
