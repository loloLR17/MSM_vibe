#include "live_fifo_acquisition.h"
#include <string.h>
static Tr2Result start(void *ctx,uint64_t run,const ActiveConfigurationSnapshot *config)
{
 LiveFifoAcquisition *a=ctx;PreparedConfiguration p={0};
 if(!a||!config)return TR2_ERROR_INVALID_ARGUMENT;
 p.config_id=config->config_id;p.payload=config->payload;
 if(configuration_validate_live(&p,NULL).status!=CONFIGURATION_VALIDATION_VALID)return TR2_ERROR_INVALID_ARGUMENT;
 return fifo_live_start(&a->fifo,run,config->config_id,config->generation,
   config->payload.axes_enable_mask,config->payload.full_scale_code,a->now_ms(a->time_context))?TR2_ERROR_UNAVAILABLE:TR2_OK;
}
static Tr2Result stop(void *ctx)
{return fifo_live_stop(&((LiveFifoAcquisition*)ctx)->fifo)?TR2_ERROR_UNAVAILABLE:TR2_OK;}
static Tr2Result observe(void *ctx,LiveAcquisitionObservation *o)
{
 LiveFifoAcquisition *a=ctx;
 if(!o)return TR2_ERROR_INVALID_ARGUMENT;
 memset(o,0,sizeof(*o));
 if(fifo_live_observe(&a->fifo))return TR2_ERROR_UNAVAILABLE;
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
 if(a->engine)(void)live_command_engine_fault(a->engine);
 return TR2_ERROR_UNAVAILABLE;
}
