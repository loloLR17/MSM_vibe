#include "fifo_live.h"
#include <string.h>
static const iis3dwb_fs_xl_t scales[4]={IIS3DWB_2g,IIS3DWB_4g,IIS3DWB_8g,IIS3DWB_16g};
static int controls(FifoLive *s,bool on)
{
 uint8_t r[14],ts;static const uint8_t expected[14]={0,1,0x0A,0x86,0x7B,0,0,0x44,0,0,0,0,0,0};
 if(s->ctx.read_reg(s->ctx.handle,7,r,4)||s->ctx.read_reg(s->ctx.handle,15,r+4,10)||s->ctx.read_reg(s->ctx.handle,25,&ts,1))return BENCH_IO;
 if(!on)return !(r[5]&0xE0)&&!(r[3]&7)&&!(ts&0x20)?BENCH_OK:BENCH_CONFIG;
 for(unsigned i=0;i<14;i++)if(i!=5&&i!=6&&i!=13&&r[i]!=expected[i])return BENCH_CONFIG;
 return r[5]==(uint8_t)(0xA0u|((unsigned)scales[s->full_scale]<<2))&&ts==0x20?BENCH_OK:BENCH_CONFIG;
}
int fifo_live_init(FifoLive *s,const stmdev_ctx_t *ctx,uint32_t max_gap_ms,FifoLiveConsume consume,void *consumer)
{
 if(!s||!ctx||!ctx->read_reg||!ctx->write_reg||!max_gap_ms||!consume)return BENCH_CONFIG;
 memset(s,0,sizeof(*s));s->ctx=*ctx;s->max_gap_ms=max_gap_ms;s->consume=consume;s->consumer=consumer;s->initialized=true;
 /* No observation or write at init, and no claim that unknown hardware is off. */
 return BENCH_OK;
}
int fifo_live_stop(FifoLive *s)
{
 int error=0;
 if(!s||!s->initialized)return BENCH_CONFIG;
 if(s->cleanup_begin)s->cleanup_begin();
 s->running=false;s->stopped=false;
 /* Attempt every safety action even when an earlier transfer fails. */
 if(iis3dwb_xl_data_rate_set(&s->ctx,IIS3DWB_XL_ODR_OFF))error=BENCH_IO;
 if(iis3dwb_fifo_mode_set(&s->ctx,IIS3DWB_BYPASS_MODE))error=BENCH_IO;
 if(iis3dwb_timestamp_set(&s->ctx,PROPERTY_DISABLE))error=BENCH_IO;
 if(controls(s,false))error=BENCH_CONFIG;
 s->stop_error=(uint32_t)error;s->stopped=!error;return error;
}
static int fault(FifoLive *s,int error)
{s->error=(uint32_t)error;(void)fifo_live_stop(s);return error;}
int fifo_live_start(FifoLive *s,uint64_t run,uint32_t id,uint32_t generation,uint16_t axes,uint16_t fs,uint64_t now)
{
 uint8_t who;
 if(!s||!s->initialized||s->running||!run||!id||!generation||!axes||axes>7||fs>3)return BENCH_CONFIG;
 /* run IDs are opaque causal identities, but never reused by this owner. */
 if(run<=s->run_id)return BENCH_CONFIG;
 s->run_id=run;s->config_id=id;s->config_generation=generation;
 if(fifo_live_stop(s))return BENCH_IO;
 s->error=0;s->full_scale=fs;
 if(iis3dwb_device_id_get(&s->ctx,&who)||who!=IIS3DWB_ID)return fault(s,BENCH_WHO);
 if(iis3dwb_auto_increment_set(&s->ctx,PROPERTY_ENABLE)||
    iis3dwb_block_data_update_set(&s->ctx,PROPERTY_ENABLE)||
    iis3dwb_xl_axis_selection_set(&s->ctx,IIS3DWB_ENABLE_ALL)||
    iis3dwb_xl_full_scale_set(&s->ctx,scales[fs])||
    iis3dwb_fifo_watermark_set(&s->ctx,256)||
    iis3dwb_fifo_xl_batch_set(&s->ctx,IIS3DWB_XL_BATCHED_AT_26k7Hz)||
    iis3dwb_fifo_timestamp_batch_set(&s->ctx,IIS3DWB_DEC_8)||
    iis3dwb_timestamp_set(&s->ctx,PROPERTY_ENABLE)||
    iis3dwb_fifo_mode_set(&s->ctx,IIS3DWB_STREAM_MODE)||
    iis3dwb_xl_data_rate_set(&s->ctx,IIS3DWB_XL_ODR_26k7Hz))return fault(s,BENCH_IO);
 if(controls(s,true))return fault(s,BENCH_CONFIG);
 memset(&s->parser,0,sizeof(s->parser));s->run_id=run;s->config_id=id;s->config_generation=generation;
 s->axes=axes;s->total_xyz=s->total_ts=0;s->last_ms=s->last_data_ms=now;s->running=true;s->stopped=false;
 return BENCH_OK;
}
int fifo_live_observe(FifoLive *s)
{
 if(!s||!s->initialized)return BENCH_CONFIG;
 if(controls(s,s->running))return fault(s,BENCH_CONFIG);
 return s->running||s->stopped?BENCH_OK:BENCH_CONFIG;
}
int fifo_live_poll(FifoLive *s,uint64_t now)
{
 int error;uint32_t n;
 if(!s||!s->initialized||!s->running)return BENCH_STOP;
 if(now<s->last_ms||now-s->last_ms>s->max_gap_ms)return fault(s,BENCH_TIMEOUT);
 s->last_ms=now;
 error=fifo_window_read_status(&s->ctx,&s->parser);if(error)return fault(s,error);
 n=s->parser.last_level;if(n>128)n=128;
 if(!n)return now-s->last_data_ms>s->max_gap_ms?fault(s,BENCH_TIMEOUT):BENCH_OK;
 s->last_data_ms=now;
 if(s->ctx.read_reg(s->ctx.handle,120,s->words,(uint16_t)(n*7u)))return fault(s,BENCH_IO);
 /* Keep continuity history across bursts. Only unbounded local statistics reset;
  * totals are 64-bit and saturation is explicit, never a campaign duration. */
 s->parser.xyz=s->parser.timestamps=s->parser.polls=0;
 for(uint32_t i=0;i<n;i++){error=fifo_window_parse_word(s->words+i*7u,&s->parser);if(error)return fault(s,error);}
 error=fifo_window_read_status(&s->ctx,&s->parser);if(error)return fault(s,error);
 if(controls(s,true))return fault(s,BENCH_CONFIG);
 if(UINT64_MAX-s->total_xyz<s->parser.xyz||UINT64_MAX-s->total_ts<s->parser.timestamps)return fault(s,BENCH_CAPACITY);
 /* Reject the whole burst on parser/status/config failure, before the consumer. */
 if(s->consume(s->consumer,s->words,n,s->total_xyz))return fault(s,BENCH_CAPACITY);
 s->total_xyz+=s->parser.xyz;s->total_ts+=s->parser.timestamps;
 return BENCH_OK;
}
