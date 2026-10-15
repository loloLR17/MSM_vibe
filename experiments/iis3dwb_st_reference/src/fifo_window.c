#include "fifo_window.h"
#include <string.h>
/* Configuration ST unchanged. Reserved addresses 11/18 are recorded, not asserted. */
int fifo_window_controls(const uint8_t c[14], uint8_t ts)
{
 static const uint8_t expected[14]={0,1,0x0A,0x86,0x7B,0xAC,0,0x44,0,0,0,0,0,0};
 for(unsigned i=0;i<14;i++)if(i!=6 && i!=13 && c[i]!=expected[i])return 0;
 return ts==0x20;
}
static int config(stmdev_ctx_t *c)
{
 uint8_t regs[14],ts;
 if(c->read_reg(c->handle,7,regs,4) || c->read_reg(c->handle,15,regs+4,10) ||
    c->read_reg(c->handle,25,&ts,1))return BENCH_IO;
 return fifo_window_controls(regs,ts)?BENCH_OK:BENCH_CONFIG;
}
int fifo_window_read_status(stmdev_ctx_t *c,FifoWindow *w)
{
 uint8_t b[2];
 if(c->read_reg(c->handle,58,b,2))return BENCH_IO;
 uint32_t level=b[0]+((uint32_t)(b[1]&3)<<8);
 w->polls++;w->last_level=level;w->flags_or|=b[1];
 if(level>w->max_level)w->max_level=level;
 if((b[1]&0x68) || level>=512)return BENCH_OVERFLOW;
 return BENCH_OK;
}
int fifo_window_parse_word(const uint8_t *b,FifoWindow *w)
{
 unsigned parity=b[0];parity^=parity>>4;parity^=parity>>2;parity^=parity>>1;
 if(parity&1){w->reason=1;return BENCH_TAG;}
 unsigned tag=b[0]>>3;
 if(tag==2){
  unsigned ctr=(b[0]>>1)&3;
  if(w->counter_valid && ctr!=((w->previous_counter+1)&3)){w->reason=2;return BENCH_TAG;}
  for(unsigned i=1;i<7;i+=2){uint16_t v=(uint16_t)b[i]|((uint16_t)b[i+1]<<8);
   if(v==0x8000 || v==0x7FFF){w->reason=5;return BENCH_TAG;}}
  w->previous_counter=ctr;w->counter_valid=1;w->xyz++;w->since_ts++;
 }else if(tag==4){
  uint32_t ts=b[1]|((uint32_t)b[2]<<8)|((uint32_t)b[3]<<16)|((uint32_t)b[4]<<24);
  uint32_t delta=ts-w->previous_ts;
  if(w->ts_valid && (!delta || delta>=0x80000000U)){w->reason=3;return BENCH_TAG;}
  if(w->ts_valid && w->since_ts!=8){w->reason=4;return BENCH_TAG;}
  w->previous_ts=ts;w->ts_valid=1;w->since_ts=0;w->timestamps++;
 }else{w->reason=6;return BENCH_TAG;}
 return BENCH_OK;
}
int fifo_window_run(stmdev_ctx_t *c,const BenchClock *clock,uint32_t target,
                    uint8_t *words,uint32_t capacity,FifoWindow *w)
{
 memset(w,0,sizeof(*w));w->target_xyz=target;w->first_tick=clock->tick();
 if((target!=512 && target!=4096) || capacity>WINDOW_MAX_WORDS || capacity<target || !words)return BENCH_CAPACITY;
 int result=config(c);if(result)return result;
 while(w->xyz<target){
  if(clock->stop())return BENCH_STOP;
  if(clock->tick()-w->first_tick>=500)return BENCH_TIMEOUT;
  if(w->polls>=400)return BENCH_CAPACITY;
  result=fifo_window_read_status(c,w);if(result)return result;
  uint32_t n=target-w->xyz;if(n>128)n=128;
  if(w->last_level<n){clock->delay(1);continue;}
  if(n>capacity-w->received)return BENCH_CAPACITY;
  uint8_t *at=words+w->received*7;
  if(c->read_reg(c->handle,120,at,(uint16_t)(n*7)))return BENCH_IO;
  uint32_t first=w->received;w->received+=n;w->bursts++;
  /* Preserve the complete received burst even when parsing rejects it. */
  for(uint32_t i=first;i<w->received;i++){result=fifo_window_parse_word(words+i*7,w);if(result)return result;}
  result=fifo_window_read_status(c,w);if(result)return result;
  result=config(c);if(result)return result;
 }
 w->last_tick=clock->tick();
 if(!w->timestamps)return BENCH_TAG;
 return BENCH_OK;
}

/* V: one scheduling quantum, same guards/parser/config as the qualified drain.
 * Prefix boundaries preserve counter and timestamp history, never reset sensor. */
int fifo_window_step(stmdev_ctx_t *c,const BenchClock *clock,uint8_t *words,uint32_t capacity,FifoWindow *w)
{
 if(!c || !clock || !words || !w ||
    (w->target_xyz!=4096 && w->target_xyz!=8192 && w->target_xyz!=16384 && w->target_xyz!=32768) ||
    capacity>WINDOW_MAX_WORDS || capacity<w->target_xyz)return BENCH_CAPACITY;
 if(w->xyz>=w->target_xyz){w->last_tick=clock->tick();return w->timestamps?BENCH_OK:BENCH_TAG;}
 if(clock->stop())return BENCH_STOP;
 uint32_t scale=w->target_xyz/4096;
 if(clock->tick()-w->first_tick>=500*scale)return BENCH_TIMEOUT;
 if(w->polls>=400*scale)return BENCH_CAPACITY;
 int result=fifo_window_read_status(c,w);if(result)return result;
 uint32_t n=4096-w->xyz%4096;
 if(n>w->target_xyz-w->xyz)n=w->target_xyz-w->xyz;
 if(n>128)n=128;
 if(w->last_level<n)return FIFO_WINDOW_PENDING;
 if(n>capacity-w->received)return BENCH_CAPACITY;
 if(c->read_reg(c->handle,120,words+w->received*7,(uint16_t)(n*7)))return BENCH_IO;
 uint32_t first=w->received;w->received+=n;w->bursts++;
 for(uint32_t i=first;i<w->received;i++){result=fifo_window_parse_word(words+i*7,w);if(result)return result;}
 result=fifo_window_read_status(c,w);if(result)return result;
 result=config(c);if(result)return result;
 if(w->xyz==w->target_xyz){w->last_tick=clock->tick();return w->timestamps?BENCH_OK:BENCH_TAG;}
 return FIFO_WINDOW_PENDING;
}

int fifo_window_check_configuration(stmdev_ctx_t *ctx){return config(ctx);}
