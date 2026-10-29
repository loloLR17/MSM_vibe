/* Experimental TR2-002 only. One owner, no UART/FRAM/media. RAM command gates
 * calibration, paired short captures and the long capture independently. */
#include "platform.h"
#include "fifo_event_time.h"
#include "tr2/domain/acquisition/fir_decimator10.h"
#include <string.h>
#define CAP 26000u
#define MAGIC 0x52314600u
typedef struct {uint32_t service,work,gap,xyz;} Record;
typedef struct {uint32_t before,after,raw;} Bracket;
typedef struct {
 uint32_t version,stage,error,uid[3],clock,who,run,mode,duration_us,records;
 uint32_t span_us,span_ticks,cal_residual,cal_brackets,cal_anchors;
 uint32_t accepted,outputs,warmup,rails,clips,source_hash,output_hash;
 uint32_t first_us,last_us,residual,clock_checks,level,flags,stop_us,stopped;
 uint32_t fifo_error,stop_error,trace_refused,preclose_overflow,irqs;
 uint32_t stack_deepest,stack_floor,primask,basepri,faultmask,max_words;
 uint32_t raw_words,pending_xyz,ctrl1,cal_xyz_span,cal_tick_span;
} Result;
Result rf_result;
Record rf_records[CAP];
Bracket rf_brackets[256];
uint8_t rf_raw[2048u*7u];
FirDecimator10Input rf_sources[1024];
FirDecimator10Output rf_outputs[128];
uint32_t rf_source_count,rf_output_count;
volatile uint32_t rf_command,rf_irqs;
static FifoLive fifo;
static FifoEventTime mapper;
static FirDecimator10 fir;
static uint32_t last_cycles,consumer_cycles,mode,anchors,last_anchor,cal_xyz;
static uint64_t elapsed,begin_us,next_clock,cal_num,cal_den;
static uint32_t first_cal_raw,first_cal_index,last_cal_index;
static bool calibration;
extern volatile uint32_t bench_cleanup_mode,bench_log_refused;
extern uint32_t _estack,_ebss;
static uintptr_t stack_begin,stack_end;
static uint64_t now(void){uint32_t c=DWT->CYCCNT;elapsed+=(uint32_t)(c-last_cycles);last_cycles=c;return elapsed/160u;}
static uint32_t cyc(void){__DSB();__ISB();return DWT->CYCCNT;}
void SysTick_Handler(void){HAL_IncTick();rf_irqs++;}
void Error_Handler(void){rf_result.error=rf_result.error?rf_result.error:1;rf_result.stage=99;for(;;)__WFI();}
__attribute__((noinline)) void rf_finished(void){__DSB();__ISB();}
static void reset_trace(void){bench_trace_count=bench_payload_count=bench_preclose_count=0;bench_cleanup_mode=0;}
static uint32_t hash(uint32_t h,const int16_t xyz[3]){for(unsigned j=0;j<3;j++){h^=(uint16_t)xyz[j];h*=16777619u;}return h;}
static Tr2Result sink(void *unused,uint64_t index,const int16_t xyz[3],uint64_t us)
{
 (void)unused;
 if(index!=rf_result.accepted||us>UINT32_MAX||(index&&us<=rf_result.last_us))return TR2_ERROR_INVALID_STATE;
 if(rf_source_count<1024)rf_sources[rf_source_count++]=(FirDecimator10Input){.source_index=index,.time_us=us,.xyz={xyz[0],xyz[1],xyz[2]},.valid=true,.time_valid=true};
 if(!index)rf_result.first_us=(uint32_t)us;
 rf_result.last_us=(uint32_t)us;rf_result.accepted++;rf_result.source_hash=hash(rf_result.source_hash,xyz);
 if(mode){
  FirDecimator10Input in={.source_index=index,.time_us=us,.xyz={xyz[0],xyz[1],xyz[2]},.valid=true,.time_valid=true};
  FirDecimator10Output out;bool produced=false;
  if(fir_decimator10_push(&fir,&in,&out,&produced)!=TR2_OK)return TR2_ERROR_INVALID_STATE;
  if(produced){
   if(out.source_index!=index||out.causal_time_us!=us)return TR2_ERROR_INVALID_STATE;
   if(rf_output_count<128)rf_outputs[rf_output_count++]=out;
   rf_result.outputs++;rf_result.warmup+=!!(out.flags&TR2_D10_WARMUP);
   rf_result.rails+=!!(out.flags&TR2_D10_SOURCE_RAIL);rf_result.clips+=!!(out.flags&TR2_D10_OUTPUT_CLIP);
   rf_result.output_hash=hash(rf_result.output_hash,out.xyz);
  }
 }else if(index%10u==0){rf_result.outputs++;rf_result.output_hash=hash(rf_result.output_hash,xyz);}
 return TR2_OK;
}
static int consume(void *unused,const uint8_t *b,uint32_t n,uint64_t index)
{
 (void)unused;uint32_t start=cyc();
 if(n>rf_result.max_words)rf_result.max_words=n;
 for(uint32_t j=0;j<n;j++){
  if(rf_result.raw_words<2048u)memcpy(rf_raw+7u*rf_result.raw_words++,b+7u*j,7);
  if(calibration){const uint8_t *p=b+7u*j;
   if((p[0]>>3)==2)cal_xyz++;
   else if((p[0]>>3)==4){uint32_t raw=(uint32_t)p[1]|(uint32_t)p[2]<<8|(uint32_t)p[3]<<16|(uint32_t)p[4]<<24;
    if(p[5]||p[6]!=10)return -1;
    if(anchors&&((uint32_t)(raw-last_anchor)<23||(uint32_t)(raw-last_anchor)>25))return -1;
    if(!anchors){first_cal_raw=raw;first_cal_index=cal_xyz;}
    last_cal_index=cal_xyz;last_anchor=raw;anchors++;
   }else return -1;
  }
 }
 int r=calibration?0:fifo_event_time_consume(&mapper,b,n,index);
 consumer_cycles=(uint32_t)(cyc()-start);return r;
}
static int bracket(Bracket *b){b->before=(uint32_t)now();if(iis3dwb_timestamp_raw_get(&fifo.ctx,&b->raw))return -1;b->after=(uint32_t)now();return 0;}
static int poll(uint64_t *previous,bool keep)
{
 if(bench_log_refused||bench_preclose_overflow)return -1;
 reset_trace();uint64_t at=now();uint32_t start=cyc(),gap=*previous?(uint32_t)((at-*previous)*160u):0;*previous=at;
 if(!calibration&&(!mapper.initialized||at>=next_clock)){
  Bracket b;if(bracket(&b))return -2;
  Tr2Result r=mapper.initialized?fifo_event_time_track(&mapper,b.raw,b.before,b.after,100):
   fifo_event_time_init_sink(&mapper,&fifo,sink,NULL,b.raw,((uint64_t)b.before+b.after)/2u,rf_result.span_us,rf_result.span_ticks);
  rf_result.clock_checks++;if(r!=TR2_OK)return -3;next_clock=(uint64_t)b.after+100000u;
 }
 consumer_cycles=0;uint64_t before=fifo.total_xyz;
 int r=fifo_live_poll(&fifo,now()/1000u);uint32_t service=cyc()-start;
 if(keep){
  if(rf_result.records>=CAP)return -4;
  rf_records[rf_result.records++]=(Record){service,consumer_cycles,gap,(uint32_t)(fifo.total_xyz-before)};
 }
 if(fifo.parser.max_level>rf_result.level)rf_result.level=fifo.parser.max_level;
 rf_result.flags|=fifo.parser.flags_or;
 if(r||bench_log_refused||bench_preclose_overflow)return -5;
 if(service>2500u*160u||gap>5000u*160u||consumer_cycles>1000u*160u)return -6;
 uint64_t end=now();while(now()-end<250u){} /* fixed post-drain cooperative interval */
 return 0;
}
static int calibrate(void)
{
 calibration=true;uint64_t previous=0;anchors=0;cal_xyz=0;
 reset_trace();if(fifo_live_start(&fifo,1,1,1,7,2,now()/1000u))return -1;
 uint64_t start=now(),next=start;
 while(now()-start<2000000u){
  if(poll(&previous,false))return -2;
  if(now()>=next&&rf_result.cal_brackets<256){
   Bracket *b=&rf_brackets[rf_result.cal_brackets];if(bracket(b))return -3;
   rf_result.cal_brackets++;next=(uint64_t)b->after+10000u;
  }
 }
 reset_trace();if(fifo_live_stop(&fifo))return -4;
 rf_result.cal_anchors=anchors;
 if(anchors<100||rf_result.cal_brackets<100)return -5;
 Bracket *a=&rf_brackets[10],*z=&rf_brackets[rf_result.cal_brackets-1];
 uint64_t base=((uint64_t)a->before+a->after)/2u;
 rf_result.span_us=(uint32_t)(((uint64_t)z->before+z->after)/2u-base);rf_result.span_ticks=z->raw-a->raw;
 if(!rf_result.span_us||!rf_result.span_ticks)return -6;
 for(unsigned i=10;i<rf_result.cal_brackets;i++){
  Bracket *b=&rf_brackets[i];uint64_t predicted=base+(uint64_t)(uint32_t)(b->raw-a->raw)*rf_result.span_us/rf_result.span_ticks;
  uint64_t mid=((uint64_t)b->before+b->after)/2u,d=predicted>mid?predicted-mid:mid-predicted;
  if(d>rf_result.cal_residual)rf_result.cal_residual=(uint32_t)d;
 }
 rf_result.cal_xyz_span=last_cal_index-first_cal_index;rf_result.cal_tick_span=last_anchor-first_cal_raw;
 if(!rf_result.cal_xyz_span||!rf_result.cal_tick_span)return -8;
 cal_num=(uint64_t)rf_result.cal_xyz_span*1000000ull*rf_result.span_ticks;
 cal_den=(uint64_t)rf_result.cal_tick_span*rf_result.span_us;
 return rf_result.cal_residual>100?-7:0;
}
static int session(uint32_t run,uint32_t duration)
{
 calibration=false;mode=run!=2;rf_result.mode=mode;rf_result.run=run;
 rf_result.accepted=rf_result.outputs=rf_result.warmup=rf_result.rails=rf_result.clips=0;
 rf_result.records=rf_result.raw_words=rf_result.max_words=rf_result.flags=rf_result.level=rf_result.clock_checks=0;
 rf_source_count=rf_output_count=0;
 rf_result.first_us=rf_result.last_us=rf_result.residual=rf_result.stop_us=0;
 rf_result.source_hash=rf_result.output_hash=2166136261u;
 memset(&mapper,0,sizeof mapper);fir_decimator10_init(&fir);next_clock=0;
 if(mode&&fir_decimator10_start(&fir,run,0,(FirDecimator10Clock){cal_num,cal_den,true})!=TR2_OK)return -1;
 /* Measured calibration XYZ/anchor deltas and sensor/MCU clock ratio;
  * metadata refined from actual event endpoints at STOP. No nominal ODR. */
 reset_trace();if(fifo_live_start(&fifo,run,1,1,7,2,now()/1000u))return -2;
 begin_us=now();uint64_t previous=0;uint32_t irq_start=rf_irqs;
 int error=0;
 while(now()-begin_us<duration){if((error=poll(&previous,true)))break;}
 rf_result.duration_us=(uint32_t)(now()-begin_us);rf_result.irqs=rf_irqs-irq_start;
 /* Only STOP can precede the debugger terminal point, including error paths. */
 if(!error)reset_trace();
 uint64_t stop=now();int stopped=fifo_live_stop(&fifo);rf_result.stop_us=(uint32_t)(now()-stop);
 rf_result.stopped=fifo.stopped;rf_result.fifo_error=fifo.error;rf_result.stop_error=fifo.stop_error;
 rf_result.trace_refused=bench_log_refused;rf_result.preclose_overflow=bench_preclose_overflow;
 rf_result.pending_xyz=mapper.count;rf_result.ctrl1=fifo.ctrl1_verified;rf_result.residual=(uint32_t)mapper.max_residual_us;
 if(error||stopped||!mapper.emitted){fir_decimator10_invalidate(&fir,TR2_D10_SOURCE_FAULT);return error?error:-3;}
 if(mode){fir.clock=(FirDecimator10Clock){(mapper.emitted-1u)*1000000ull,mapper.last_event_us-mapper.first_event_us,true};fir_decimator10_stop(&fir);}
 return 0;
}
static void stack_paint(void){stack_begin=(uintptr_t)&_estack-4096u;stack_end=(__get_MSP()-256u)&~3u;if(stack_begin<(uintptr_t)&_ebss+512u||stack_end<=stack_begin)Error_Handler();for(volatile uint32_t *p=(uint32_t *)stack_begin;(uintptr_t)p<stack_end;p++)*p=0xA53CC35Au;}
static void stack_scan(void){uintptr_t p=stack_begin;while(p<stack_end&&*(volatile uint32_t *)p==0xA53CC35Au)p+=4;rf_result.stack_floor=(uint32_t)(p-stack_begin);rf_result.stack_deepest=p<stack_end?(uint32_t)((uintptr_t)&_estack-p):0;if(p==stack_begin)rf_result.error=91;}
int main(void)
{
 rf_result.version=1;rf_result.stage=1;rf_result.uid[0]=HAL_GetUIDw0();rf_result.uid[1]=HAL_GetUIDw1();rf_result.uid[2]=HAL_GetUIDw2();
 if(rf_result.uid[0]!=0x001B0046u||rf_result.uid[1]!=0x38425018u||rf_result.uid[2]!=0x20333157u)Error_Handler();
 stmdev_ctx_t ctx;BenchClock clock;
 if(HAL_Init()!=HAL_OK||platform_init(&ctx,&clock)||bench_core_hz!=160000000u)Error_Handler();
 rf_result.clock=bench_core_hz;last_cycles=DWT->CYCCNT;rf_result.primask=__get_PRIMASK();rf_result.basepri=__get_BASEPRI();rf_result.faultmask=__get_FAULTMASK();
 if(rf_result.primask||rf_result.basepri||rf_result.faultmask)Error_Handler();
 if(fifo_live_init(&fifo,&ctx,5,consume,NULL)||fifo_live_set_poll_limit(&fifo,64))Error_Handler();
 fifo.cleanup_begin=clock.cleanup_begin;fifo.first_data_timeout_ms=20;stack_paint();rf_result.stage=2;
 for(uint32_t run=1;run<=4;run++){
  while(rf_command!=(MAGIC|run)){(void)now();__WFI();}
  rf_command=0;rf_result.stage=3;rf_result.run=run;
  int r;
  if(run==1){uint8_t who=0;reset_trace();r=iis3dwb_device_id_get(&ctx,&who);rf_result.who=who;if(!r&&who==0x7b)r=calibrate();else r=-90;}
  else r=session(run,run==4?10000000u:2000000u);
  if(fifo.running){if(!r)reset_trace();if(fifo_live_stop(&fifo))r=-91;}
  rf_result.stopped=fifo.stopped;rf_result.fifo_error=fifo.error;rf_result.stop_error=fifo.stop_error;
  rf_result.trace_refused=bench_log_refused;rf_result.preclose_overflow=bench_preclose_overflow;
  if(r)rf_result.error=(uint32_t)(-r);
  stack_scan();rf_result.stage=rf_result.error?99:4;
  rf_finished();
  if(rf_result.error)break;
 }
 for(;;){(void)now();__WFI();}
}
