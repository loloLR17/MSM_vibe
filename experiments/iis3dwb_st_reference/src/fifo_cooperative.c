#include "fifo_cooperative.h"
#include <string.h>
int fifo_cooperative_init(FifoCooperative *c,const stmdev_ctx_t *ctx,const BenchClock *clock,uint8_t *words,uint32_t capacity,uint32_t target)
{
 if(!c || !ctx || !ctx->read_reg || !ctx->write_reg || !clock || !clock->tick || !clock->stop || !clock->cleanup_begin || !words ||
    (target!=4096 && target!=8192 && target!=16384 && target!=32768) || capacity>WINDOW_MAX_WORDS || capacity<target+target/8)return BENCH_CAPACITY;
 memset(c,0,sizeof(*c));c->ctx=*ctx;c->clock=*clock;c->words=words;c->capacity=capacity;c->window.target_xyz=target;
 c->state=COOP_CONFIGURING;c->next_tick=clock->tick()+10;return BENCH_OK;
}
static void finish(FifoCooperative *c,int reason)
{c->result=(uint32_t)reason;c->state=COOP_CLEANUP;c->phase=0;c->clock.cleanup_begin();}
int fifo_cooperative_start(FifoCooperative *c)
{if(!c || c->state!=COOP_ARMED)return BENCH_STOP;c->state=COOP_CONFIGURING;c->phase=8;c->window.first_tick=c->clock.tick();return BENCH_OK;}
void fifo_cooperative_poll(FifoCooperative *c)
{
 int code=0;uint8_t reset;
 if(!c || c->state==COOP_ARMED || c->state==COOP_READY || c->state==COOP_INVALID)return;
 uint32_t now=c->clock.tick();
 if((int32_t)(now-c->next_tick)<0)return;
 if(c->state!=COOP_CLEANUP && c->clock.stop()){finish(c,BENCH_STOP);return;}
 if(c->state==COOP_CONFIGURING){
  switch(c->phase){
   case 0:code=iis3dwb_device_id_get(&c->ctx,&c->who);if(!code && c->who!=IIS3DWB_ID){finish(c,BENCH_WHO);return;}break;
   case 1:code=iis3dwb_reset_set(&c->ctx,PROPERTY_ENABLE);c->reset_tick=now;break;
   case 2:code=iis3dwb_reset_get(&c->ctx,&reset);if(!code && reset){if(now-c->reset_tick>=100)finish(c,BENCH_RESET_TIMEOUT);else c->next_tick=now+1;return;}break;
   case 3:code=iis3dwb_block_data_update_set(&c->ctx,PROPERTY_ENABLE);break;
   case 4:code=iis3dwb_xl_full_scale_set(&c->ctx,IIS3DWB_8g);break;
   case 5:code=iis3dwb_fifo_watermark_set(&c->ctx,256);break;
   case 6:code=iis3dwb_fifo_xl_batch_set(&c->ctx,IIS3DWB_XL_BATCHED_AT_26k7Hz);break;
   case 7:code=iis3dwb_fifo_mode_set(&c->ctx,IIS3DWB_STREAM_MODE);if(!code)c->state=COOP_ARMED;break;
   case 8:code=iis3dwb_xl_data_rate_set(&c->ctx,IIS3DWB_XL_ODR_26k7Hz);break;
   case 9:code=iis3dwb_fifo_timestamp_batch_set(&c->ctx,IIS3DWB_DEC_8);break;
   case 10:code=iis3dwb_timestamp_set(&c->ctx,PROPERTY_ENABLE);break;
   case 11:code=fifo_window_check_configuration(&c->ctx);if(!code)c->state=COOP_CAPTURING;break;
   default:finish(c,BENCH_CONFIG);return;
  }
  if(code)finish(c,BENCH_IO);else c->phase++;
  return;
 }
 if(c->state==COOP_CAPTURING){
  uint32_t previous=c->window.received;
  code=fifo_window_step(&c->ctx,&c->clock,c->words,c->capacity,&c->window);
  if(code==FIFO_WINDOW_PENDING){if(previous==c->window.received)c->next_tick=now+1;return;}
  finish(c,code);return;
 }
 if(c->state==COOP_CLEANUP){
  switch(c->phase){
   case 0:code=iis3dwb_xl_data_rate_set(&c->ctx,IIS3DWB_XL_ODR_OFF);break;
   case 1:code=iis3dwb_fifo_mode_set(&c->ctx,IIS3DWB_BYPASS_MODE);break;
   case 2:code=iis3dwb_timestamp_set(&c->ctx,PROPERTY_DISABLE);break;
   case 3:code=c->ctx.read_reg(c->ctx.handle,16,c->cleanup,1);break;
   case 4:code=c->ctx.read_reg(c->ctx.handle,10,c->cleanup+1,1);break;
   case 5:code=c->ctx.read_reg(c->ctx.handle,25,c->cleanup+2,1);break;
  }
  if(code)c->cleanup_result=BENCH_IO;
  if(++c->phase>=6){
   if((c->cleanup[0]&0xE0)||(c->cleanup[1]&7)||(c->cleanup[2]&0x20))c->cleanup_result=BENCH_CONFIG;
   c->state=!c->result && !c->cleanup_result && c->window.xyz==c->window.target_xyz && c->window.timestamps==c->window.target_xyz/8?COOP_READY:COOP_INVALID;
  }
 }
}
