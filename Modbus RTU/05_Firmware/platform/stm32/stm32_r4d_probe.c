/* R4-D volatile-only timing probe: no R2 intake, no FRAM write capability.
 * Reuses the qualified R4 hardware recipe and real continuous sensor port. */
#include "stm32_live_acquisition.h"
#include "stm32_fram_storage.h"
#include "tr2/common/cooperative_work.h"
#include "tr2/common/sha256.h"
#include "platform.h"
#include <string.h>
extern void stm32_st_transport_next_window(void);
extern volatile uint32_t bench_log_refused,bench_preclose_overflow;
typedef struct {uint32_t stage,error,count,trace_count,overflow,core_hz;uint32_t calibration_direct,calibration_plain,calibration_trace,calibration_max;} Diagnostic;
typedef struct {uint32_t stage,reg,len,write,cycles;int32_t result;} Transfer;
typedef struct {uint32_t kind,start_cycles,observe_cycles,stop_cycles,combined_cycles,start_status,observe_status,stop_status;uint32_t max_work_us,max_service_us,deadline,work_error,stopped,trace_first,trace_last;} Exercise;
volatile Diagnostic r4d;
volatile Exercise r4d_exercises[6];
volatile Transfer r4d_transfers[512];
volatile uint32_t r4d_trigger;
uint8_t r4d_backup[262144];
static Stm32FramStorage fram;
static Stm32LiveAcquisition owner;
static stmdev_ctx_t real_io;
static bool tracing;
static uint32_t phase,previous;
static uint64_t cycles;
static Tr2CooperativeWork work;
static uint64_t now(void *ctx){(void)ctx;uint32_t c=DWT->CYCCNT;cycles+=(uint32_t)(c-previous);previous=c;return cycles/160;}
static Tr2Result idle(void *ctx){(void)ctx;return TR2_OK;}
static void safety(void *ctx){(void)ctx;(void)fifo_live_stop(&owner.acquisition.fifo);}
static int consume(void *c,const uint8_t *b,uint32_t n,uint64_t i){(void)c;(void)b;(void)n;(void)i;return 0;}
static int32_t io(void *h,uint8_t r,uint8_t *d,uint16_t n,bool wr)
{
 uint32_t t=DWT->CYCCNT;int32_t result=wr?real_io.write_reg(h,r,d,n):real_io.read_reg(h,r,d,n);
 uint32_t elapsed=DWT->CYCCNT-t;
 if(tracing) {
  if(r4d.trace_count>=512)r4d.overflow=1;
  else {volatile Transfer *x=&r4d_transfers[r4d.trace_count++];x->stage=phase;x->reg=r;x->len=n;x->write=wr;x->cycles=elapsed;x->result=result;}
 }
 return result;
}
static int32_t rd(void *h,uint8_t r,uint8_t *d,uint16_t n){return io(h,r,d,n,false);}
static int32_t wr(void *h,uint8_t r,const uint8_t *d,uint16_t n){return io(h,r,(uint8_t*)d,n,true);}
static int32_t empty(void *h,uint8_t r,uint8_t *d,uint16_t n){(void)h;(void)r;(void)d;(void)n;__asm volatile("":::"memory");return 0;}
static int32_t empty_write(void *h,uint8_t r,const uint8_t *d,uint16_t n){return empty(h,r,(uint8_t*)d,n);}
static void calibrate(void)
{
 stmdev_ctx_t saved=real_io;real_io.read_reg=empty;real_io.write_reg=empty_write;uint8_t b=0;
 for(unsigned k=0;k<3;k++) {
  tracing=k==2;uint32_t sum=0;
  for(unsigned i=0;i<128;i++){uint32_t t=DWT->CYCCNT;if(k)(void)rd(real_io.handle,15,&b,1);else(void)empty(real_io.handle,15,&b,1);uint32_t dt=DWT->CYCCNT-t;sum+=dt;if(dt>r4d.calibration_max)r4d.calibration_max=dt;}
  if(k==2)r4d.calibration_trace=sum;else if(k==1)r4d.calibration_plain=sum;else r4d.calibration_direct=sum;
 }
 real_io=saved;tracing=false;r4d.trace_count=0;
}
static ActiveConfigurationSnapshot configuration(void)
{
 ActiveConfigurationSnapshot c={0};c.generation=2;c.config_id=2;
 c.payload.sampling_frequency_hz=26667;c.payload.axes_enable_mask=7;c.payload.acquisition_mode=2;
 c.payload.window_size_samples=4096;c.payload.indicator_period_ms=2000;c.payload.supervision_enable_mask=1;
 c.payload.campaign_context_id=1;c.payload.mission_id=4;return c;
}
static void exercise(unsigned kind)
{
 volatile Exercise *x=&r4d_exercises[r4d.count++];x->kind=kind;
 stm32_st_transport_next_window();
 owner.acquisition.fifo.ctx=real_io;
 if(kind==1){owner.acquisition.fifo.ctx.read_reg=rd;owner.acquisition.fifo.ctx.write_reg=wr;}
 tracing=kind==1;x->trace_first=r4d.trace_count;
 LiveAcquisitionPort p=live_fifo_acquisition_port(&owner.acquisition);LiveAcquisitionObservation o={0};
 ActiveConfigurationSnapshot c=configuration();
 if(kind==3)(void)tr2_cooperative_enter(&work);
 phase=1;uint32_t start=DWT->CYCCNT;
 x->start_status=p.start(p.context,UINT64_C(0x5234440000000001)+kind,&c);x->start_cycles=DWT->CYCCNT-start;
 phase=2;uint32_t t=DWT->CYCCNT;x->observe_status=p.observe(p.context,&o);x->observe_cycles=DWT->CYCCNT-t;
 x->combined_cycles=DWT->CYCCNT-start;
 if(kind==3){(void)tr2_cooperative_leave(&work);x->max_work_us=work.max_work_us;x->max_service_us=work.max_service_us;x->deadline=work.deadline_missed;x->work_error=work.error;}
 phase=3;t=DWT->CYCCNT;x->stop_status=p.stop(p.context);x->stop_cycles=DWT->CYCCNT-t;
 x->stopped=owner.acquisition.fifo.stopped;x->trace_last=r4d.trace_count;
 if(x->start_status||x->observe_status||x->stop_status||!x->stopped||r4d.overflow||bench_log_refused||bench_preclose_overflow)r4d.error=1;
 tracing=false;
}
__attribute__((noreturn)) void stm32_r4_benchmark_run(SPI_HandleTypeDef *f,SPI_HandleTypeDef *s)
{
 r4d.core_hz=HAL_RCC_GetHCLKFreq();
 if(r4d.core_hz!=160000000||stm32_live_acquisition_attach(&owner,s,5,consume,NULL)!=TR2_OK||
    stm32_fram_storage_init(&fram,f,GPIOD,GPIO_PIN_14,10)!=TR2_OK){r4d.error=2;for(;;)__WFI();}
 real_io=owner.acquisition.fifo.ctx;previous=DWT->CYCCNT;calibrate();
 r4d.stage=1;while(r4d_trigger!=0x52344401u){(void)now(NULL);HAL_Delay(1);}
 r4d_trigger=0;r4d.stage=2;
 if(stm32_fram_storage_read(&fram,0,r4d_backup,sizeof(r4d_backup))!=TR2_OK)r4d.error=3;
 uint8_t hash[32];static const uint8_t expected[32]={0xf2,0xbd,0x7a,0x99,0xd3,0xd0,0xe9,0xf1,0x72,0x92,0x31,0x0f,0x2c,0xec,0x51,0xfe,0x3a,0x3f,0x52,0x1b,0xa3,0xa4,0xd2,0xf0,0x7d,0xa5,0xb6,0x30,0x22,0x58,0xa0,0xc1};
 if(tr2_sha256_bytes(r4d_backup,sizeof(r4d_backup),hash)!=TR2_OK||memcmp(hash,expected,32))r4d.error=4;
 if(!r4d.error&&fifo_live_stop(&owner.acquisition.fifo))r4d.error=5;
 if(!r4d.error) {
  (void)tr2_cooperative_init(&work,NULL,now,idle,safety,32,1000,2500);
  for(unsigned kind=0;kind<4&&!r4d.error;kind++){r4d.stage=10+kind;exercise(kind);}
 }
 (void)fifo_live_stop(&owner.acquisition.fifo);r4d.stage=r4d.error?99:100;
 for(;;){(void)now(NULL);HAL_Delay(1);}
}
