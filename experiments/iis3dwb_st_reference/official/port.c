#include "platform.h"
#include "port.h"
#include <setjmp.h>
#include <string.h>
#ifndef BENCH_WINDOW_XYZ
#define BENCH_WINDOW_XYZ 0
#endif
#if BENCH_WINDOW_XYZ
#include "fifo_window.h"
FifoWindow fifo_window;
#endif
#ifndef OFFICIAL_PACKETS
#define OFFICIAL_PACKETS 1U
#endif
BenchResult bench_result;
#if BENCH_WINDOW_XYZ
uint8_t official_bus,official_words[WINDOW_MAX_WORDS*7],official_text[2048];
#else
uint8_t official_bus,official_words[1024*7],official_text[2048];
#endif
volatile uint32_t official_packets,official_text_count,official_raw_level,official_raw_flags,official_ctrl10;
static stmdev_ctx_t transport;
static BenchClock clock_ops;
static jmp_buf exit_point;
static uint32_t started,reset_started;
static unsigned in_cleanup;
extern void iis3dwb_fifo(void);
/* Exit codes: 0 bounded complete; 1 IO; 4 timeout; 7 FIFO overflow; 10 capacity;
 * 11 zero-length FIFO request from official getter. Never swallow ignored ST errors. */
static void finish(uint32_t reason){bench_result.result=reason;longjmp(exit_point,1);}
void official_prepare(void)
{
  if(platform_init(&transport,&clock_ops))bench_result.result=BENCH_IO;
}
void official_init(void){clock_ops.delay(1000);started=clock_ops.tick();}
void official_delay(uint32_t ms){clock_ops.delay(ms);}
static void deadline(void)
{
  if(!in_cleanup && clock_ops.tick()-started>=500)finish(BENCH_TIMEOUT);
}
int32_t official_write(void *h,uint8_t reg,const uint8_t *data,uint16_t n)
{
  (void)h;deadline();
  if(transport.write_reg(transport.handle,reg,data,n))finish(BENCH_IO);
  if(reg==IIS3DWB_CTRL3_C && n==1 && (data[0]&1))reset_started=clock_ops.tick();
  return 0;
}
int32_t official_read(void *h,uint8_t reg,uint8_t *data,uint16_t n)
{
  (void)h;deadline();
#if BENCH_WINDOW_XYZ
  /* The original ST setup has completed before its first FIFO status request.
   * Replace only the drain/console loop, never its initialization or driver. */
  if(!in_cleanup && reg==IIS3DWB_FIFO_STATUS1 && n==2){
    uint32_t code=(uint32_t)fifo_window_run(&transport,&clock_ops,BENCH_WINDOW_XYZ,
                          official_words,WINDOW_MAX_WORDS,&fifo_window);
    bench_result.count=fifo_window.received;
    bench_result.xyz_count=fifo_window.xyz;
    bench_result.timestamp_count=fifo_window.timestamps;
    bench_result.raw_level=fifo_window.last_level;bench_result.flags=fifo_window.flags_or;
    bench_result.polls=fifo_window.polls;
    finish(code);
  }
#endif
  if(reg==IIS3DWB_FIFO_DATA_OUT_TAG && !n)finish(11);
  if(!n || n>3584 || (reg==IIS3DWB_FIFO_DATA_OUT_TAG && (n%7 || bench_result.count+n/7>1024)))finish(BENCH_CAPACITY);
  /* SPI3 TSIZE limit: split at whole-word boundaries; explicit CS reassertion. */
  uint16_t left=n;uint8_t *at=data;
  while(left){uint16_t chunk=left>896?896:left;
    if(transport.read_reg(transport.handle,reg,at,chunk))finish(BENCH_IO);
    left-=chunk;at+=chunk;
  }
  if(reg==IIS3DWB_WHO_AM_I && n==1){bench_result.who=*data;if(*data!=IIS3DWB_ID)finish(BENCH_WHO);}
  if(reg==IIS3DWB_CTRL3_C && n==1 && (*data&1) && clock_ops.tick()-reset_started>=100)finish(BENCH_RESET_TIMEOUT);
  if(reg==IIS3DWB_FIFO_STATUS1 && n==2){
    official_raw_level=(data[0]|((uint32_t)data[1]<<8))&1023;
    official_raw_flags=data[1];bench_result.raw_level=official_raw_level;
    bench_result.st_level=official_raw_level&63;bench_result.flags=data[1];
    if(data[1]&0x68)finish(BENCH_OVERFLOW);
    /* Platform scheduling bound; the original loop and decisions remain intact. */
    clock_ops.delay(1);
  }
  if(reg==IIS3DWB_FIFO_DATA_OUT_TAG){
    memcpy(official_words+bench_result.count*7,data,n);
    for(unsigned i=0;i<n;i+=7){unsigned tag=data[i]>>3;
      if(tag==2)bench_result.xyz_count++;else if(tag==4)bench_result.timestamp_count++;
      else finish(BENCH_TAG);
    }
    bench_result.count+=n/7;
  }
  return 0;
}
int32_t official_status10(const stmdev_ctx_t *ctx,iis3dwb_fifo_status_t *st)
{
  int32_t code=iis3dwb_fifo_status_get(ctx,st);
  if(!code)st->fifo_level=(uint16_t)official_raw_level;
  return code;
}
void official_console(uint8_t *data,uint16_t n)
{
  if(official_text_count+n>sizeof(official_text))finish(BENCH_CAPACITY);
  memcpy(official_text+official_text_count,data,n);official_text_count+=n;
  if(n>=6 && !memcmp(data,"------",6)){
    official_packets++;if(official_packets>=OFFICIAL_PACKETS)finish(BENCH_OK);
  }
}
void official_run(void)
{
  started=clock_ops.tick();bench_result.stage=4;
  if(!setjmp(exit_point))iis3dwb_fifo();
  in_cleanup=1;clock_ops.cleanup_begin();
  int bad=transport.read_reg(transport.handle,7,bench_result.controls,4)!=0;
  if(transport.read_reg(transport.handle,15,bench_result.controls+4,10))bad=1;
  if(transport.read_reg(transport.handle,25,bench_result.cleanup_controls,1))bad=1;
  /* Preserve pre-cleanup CTRL10 separately from final readbacks. */
  official_ctrl10=bench_result.cleanup_controls[0];
  if(iis3dwb_xl_data_rate_set(&transport,IIS3DWB_XL_ODR_OFF))bad=1;
  if(iis3dwb_fifo_mode_set(&transport,IIS3DWB_BYPASS_MODE))bad=1;
  if(iis3dwb_timestamp_set(&transport,PROPERTY_DISABLE))bad=1;
  if(transport.read_reg(transport.handle,16,bench_result.cleanup_controls,1))bad=1;
  if(transport.read_reg(transport.handle,10,bench_result.cleanup_controls+1,1))bad=1;
  if(transport.read_reg(transport.handle,25,bench_result.cleanup_controls+2,1))bad=1;
  if((bench_result.cleanup_controls[0]&0xE0) || (bench_result.cleanup_controls[1]&7) || (bench_result.cleanup_controls[2]&0x20))bad=1;
  bench_result.cleanup_result=bad?BENCH_IO:BENCH_OK;bench_result.stage=6;
}
