/* Experimental TR2-002 only. One FIFO owner and cooperative RAW SPI2 SD, no UART/FRAM. RAM command gates
 * calibration, paired short captures and the long capture independently. */
#include "platform.h"
#include "fifo_event_time.h"
#include "tr2/domain/acquisition/fir_decimator10.h"
#include <string.h>
#include "sd_writer.h"
#include "stream_hash.h"
#include "capture_queue.h"
#define HIST_BINS 5001u
#define QUEUE_SIZE 32768u
#define TRACE_COUNT 512u
#define MAGIC 0x52334300u
typedef struct {uint32_t service,work,gap,xyz,sd_step,queue;} Record;
typedef struct {uint64_t sum;uint32_t count,max;uint32_t hist[HIST_BINS];} Distribution;
typedef struct {uint32_t before,after,raw;} Bracket;
typedef struct {uint32_t write_us,busy_us,queue_at_begin,lba;} SdWriteRecord;
typedef struct {uint64_t sum;uint32_t count,max;uint32_t hist_ms[1001];} SdDistribution;
typedef struct {
 uint32_t version,stage,error,uid[3],clock,who,run,mode,duration_us,records;
 uint32_t span_us,span_ticks,cal_residual,cal_brackets,cal_anchors;
 uint32_t accepted,outputs,warmup,rails,clips,source_hash,output_hash;
 uint32_t first_us,last_us,residual,clock_checks,level,flags,stop_us,stopped;
 uint32_t fifo_error,stop_error,trace_refused,preclose_overflow,irqs;
 uint32_t stack_deepest,stack_floor,primask,basepri,faultmask,max_words;
 uint32_t raw_words,pending_xyz,ctrl1,cal_xyz_span,cal_tick_span;
 uint32_t sd_error,sd_steps,sd_peak_queue,sd_completed,sd_step_max,sd_write_max_us,sd_busy_max_us;
 uint32_t sd_total_bytes,sd_lba,sd_written,drain_us,header_written,return_offset,return_bytes,returned;
 uint32_t sd_sectors,marker_ok,who_ok,trace_count,queue_overflows,sd_pending_polls;
} Result;
Result r3_result;
Record r3_records[TRACE_COUNT];
SdWriteRecord r3_sd_records[2048];
SdDistribution r3_sd_distributions[2];
static uint32_t writer_queue;
Distribution r3_distributions[4]; /* service/work/gap/SD quantum, rawcycles histogramceil1us */
SdSpi r3_card;SdWriter r3_writer;
R3Hash source_hash;
uint8_t r3_source_sha[32],r3_header[512],r3_media_header[512],r3_return[65536];
static CaptureQueue q;static uint8_t last_sector[512];
static uint32_t next_lba;
static bool writer_enabled;
int r3_sd_prepare(void);int r3_sd_init(SdSpi *);void r3_sd_deselect(void);
static void distribution(unsigned id,uint32_t cycles)
{Distribution *d=&r3_distributions[id];d->sum+=cycles;d->count++;if(cycles>d->max)d->max=cycles;uint32_t bin=(cycles+159u)/160u;if(bin>=HIST_BINS)bin=HIST_BINS-1;d->hist[bin]++;}
Bracket r3_brackets[256];
uint8_t r3_raw[2048u*7u];
FirDecimator10Input r3_sources[1024];
FirDecimator10Output r3_outputs[128];
uint32_t r3_source_count,r3_output_count;
volatile uint32_t r3_command,r3_irqs,r3_return_offset;
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
uint64_t r3_now(void *unused){(void)unused;uint32_t c=DWT->CYCCNT;elapsed+=(uint32_t)(c-last_cycles);last_cycles=c;return elapsed/160u;}
#define now() r3_now(NULL)
static uint32_t cyc(void){__DSB();__ISB();return DWT->CYCCNT;}
void SysTick_Handler(void){HAL_IncTick();r3_irqs++;}
void Error_Handler(void){r3_result.error=r3_result.error?r3_result.error:1;r3_result.stage=99;for(;;)__WFI();}
__attribute__((noinline)) void r3_finished(void){__DSB();__ISB();}
static void reset_trace(void){bench_trace_count=bench_payload_count=bench_preclose_count=0;bench_cleanup_mode=0;}
static uint32_t hash(uint32_t h,const int16_t xyz[3]){for(unsigned j=0;j<3;j++){h^=(uint16_t)xyz[j];h*=16777619u;}return h;}
static Tr2Result sink(void *unused,uint64_t index,const int16_t xyz[3],uint64_t us)
{
 (void)unused;
 if(index!=r3_result.accepted||us>UINT32_MAX||(index&&us<=r3_result.last_us))return TR2_ERROR_INVALID_STATE;
 if(r3_source_count<1024)r3_sources[r3_source_count++]=(FirDecimator10Input){.source_index=index,.time_us=us,.xyz={xyz[0],xyz[1],xyz[2]},.valid=true,.time_valid=true};
 if(!index)r3_result.first_us=(uint32_t)us;
 r3_result.last_us=(uint32_t)us;r3_result.accepted++;r3_result.source_hash=hash(r3_result.source_hash,xyz);
 FirDecimator10Input in={.source_index=index,.time_us=us,.xyz={xyz[0],xyz[1],xyz[2]},.valid=true,.time_valid=true};
 FirDecimator10Output out;bool produced=false;
 if(fir_decimator10_push(&fir,&in,&out,&produced)!=TR2_OK)return TR2_ERROR_INVALID_STATE;
 if(produced){
  if(out.source_index!=index||out.causal_time_us!=us)return TR2_ERROR_INVALID_STATE;
  if(r3_output_count<128)r3_outputs[r3_output_count++]=out;
  r3_result.outputs++;r3_result.warmup+=!!(out.flags&TR2_D10_WARMUP);
  r3_result.rails+=!!(out.flags&TR2_D10_SOURCE_RAIL);r3_result.clips+=!!(out.flags&TR2_D10_OUTPUT_CLIP);
  r3_result.output_hash=hash(r3_result.output_hash,out.xyz);
  uint8_t packed[6];for(unsigned j=0;j<3;j++){packed[2*j]=(uint8_t)out.xyz[j];packed[2*j+1]=(uint8_t)((uint16_t)out.xyz[j]>>8);}
  /* Independent producer digest before queue/transmit buffers; all outputs, not
   * just the captured prefix. PC compares actual whole SD payload to this. */
  r3_hash_update(&source_hash,packed,6);r3_result.sd_total_bytes+=6;
  if(writer_enabled){
   if(!capture_queue_push(&q,packed,6)){r3_result.queue_overflows=q.overflows;return TR2_ERROR_NOT_AVAILABLE;}
   r3_result.sd_peak_queue=q.peak;
  }
 }
 return TR2_OK;
}
static int writer_step(bool final)
{
 if(!writer_enabled)return 0;
 if(!r3_writer.active&&(q.count>=512||(final&&q.count))){
  const uint8_t *b=capture_queue_acquire(&q,final,last_sector);if(!b)return -3;
  writer_queue=q.count;
  if(sd_writer_begin(&r3_writer,&r3_card,next_lba,b))return -1;
 }
 if(r3_writer.active){
  if(sd_writer_step(&r3_writer))return -2;
  r3_result.sd_steps++;if(q.count>=512)r3_result.sd_pending_polls++;
  if(r3_writer.done){
   uint32_t n=q.borrowed;if(!capture_queue_commit(&q))return -4;next_lba++;r3_result.sd_completed++;r3_result.sd_written+=n;
   uint32_t total=(uint32_t)(r3_writer.complete_us-r3_writer.begin_us);
   uint32_t id=r3_result.sd_completed-1,slot=id<256?id:256+(id-256)%1792;
   r3_sd_records[slot]=(SdWriteRecord){total,(uint32_t)r3_writer.busy_us,writer_queue,r3_writer.lba};
   for(unsigned k=0;k<2;k++){uint32_t value=k?(uint32_t)r3_writer.busy_us:total;SdDistribution *d=&r3_sd_distributions[k];d->sum+=value;d->count++;if(value>d->max)d->max=value;uint32_t bin=(value+999u)/1000u;if(bin>1000)bin=1000;d->hist_ms[bin]++;}
   if(total>r3_result.sd_write_max_us)r3_result.sd_write_max_us=total;
   if(r3_writer.busy_us>r3_result.sd_busy_max_us)r3_result.sd_busy_max_us=(uint32_t)r3_writer.busy_us;
  }
 }
 return 0;
}
static int consume(void *unused,const uint8_t *b,uint32_t n,uint64_t index)
{
 (void)unused;uint32_t start=cyc();
 if(n>r3_result.max_words)r3_result.max_words=n;
 for(uint32_t j=0;j<n;j++){
  if(r3_result.raw_words<2048u)memcpy(r3_raw+7u*r3_result.raw_words++,b+7u*j,7);
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
   fifo_event_time_init_sink(&mapper,&fifo,sink,NULL,b.raw,((uint64_t)b.before+b.after)/2u,r3_result.span_us,r3_result.span_ticks);
  r3_result.clock_checks++;if(r!=TR2_OK)return -3;next_clock=(uint64_t)b.after+100000u;
 }
 consumer_cycles=0;uint64_t before=fifo.total_xyz;
 int r=fifo_live_poll(&fifo,now()/1000u);uint32_t sd_start=cyc();
 if(!r&&keep&&writer_step(false))r=-10;
 uint32_t sd_cycles=cyc()-sd_start,service=cyc()-start;
 if(keep){
  distribution(0,service);distribution(1,consumer_cycles);distribution(2,gap);distribution(3,sd_cycles);
  if(sd_cycles>r3_result.sd_step_max)r3_result.sd_step_max=sd_cycles;
  uint32_t slot=r3_result.records<256?r3_result.records:256+(r3_result.records-256)%256;
  r3_records[slot]=(Record){service,consumer_cycles,gap,(uint32_t)(fifo.total_xyz-before),sd_cycles,q.count};r3_result.records++;
  if(r3_result.trace_count<TRACE_COUNT)r3_result.trace_count++;
 }
 if(fifo.parser.max_level>r3_result.level)r3_result.level=fifo.parser.max_level;
 r3_result.flags|=fifo.parser.flags_or;
 if(r||bench_log_refused||bench_preclose_overflow)return -5;
 if(service>2500u*160u||gap>5000u*160u||consumer_cycles>1000u*160u||sd_cycles>1000u*160u)return -6;
 uint64_t end=now();while(now()-end<250u){}
 return 0;
}
static int calibrate(void)
{
 calibration=true;uint64_t previous=0;anchors=0;cal_xyz=0;
 reset_trace();if(fifo_live_start(&fifo,1,1,1,7,2,now()/1000u))return -1;
 uint64_t start=now(),next=start;
 while(now()-start<2000000u){
  if(poll(&previous,false))return -2;
  if(now()>=next&&r3_result.cal_brackets<256){
   Bracket *b=&r3_brackets[r3_result.cal_brackets];if(bracket(b))return -3;
   r3_result.cal_brackets++;next=(uint64_t)b->after+10000u;
  }
 }
 reset_trace();if(fifo_live_stop(&fifo))return -4;
 r3_result.cal_anchors=anchors;
 if(anchors<100||r3_result.cal_brackets<100)return -5;
 Bracket *a=&r3_brackets[10],*z=&r3_brackets[r3_result.cal_brackets-1];
 uint64_t base=((uint64_t)a->before+a->after)/2u;
 r3_result.span_us=(uint32_t)(((uint64_t)z->before+z->after)/2u-base);r3_result.span_ticks=z->raw-a->raw;
 if(!r3_result.span_us||!r3_result.span_ticks)return -6;
 for(unsigned i=10;i<r3_result.cal_brackets;i++){
  Bracket *b=&r3_brackets[i];uint64_t predicted=base+(uint64_t)(uint32_t)(b->raw-a->raw)*r3_result.span_us/r3_result.span_ticks;
  uint64_t mid=((uint64_t)b->before+b->after)/2u,d=predicted>mid?predicted-mid:mid-predicted;
  if(d>r3_result.cal_residual)r3_result.cal_residual=(uint32_t)d;
 }
 r3_result.cal_xyz_span=last_cal_index-first_cal_index;r3_result.cal_tick_span=last_anchor-first_cal_raw;
 if(!r3_result.cal_xyz_span||!r3_result.cal_tick_span)return -8;
 cal_num=(uint64_t)r3_result.cal_xyz_span*1000000ull*r3_result.span_ticks;
 cal_den=(uint64_t)r3_result.cal_tick_span*r3_result.span_us;
 return r3_result.cal_residual>100?-7:0;
}
static uint32_t crc32(const uint8_t *b,uint32_t n){uint32_t c=~0u;for(uint32_t i=0;i<n;i++){c^=b[i];for(unsigned j=0;j<8;j++)c=(c>>1)^((c&1)?0xedb88320u:0);}return ~c;}
static void put32(uint8_t *b,uint32_t v){for(unsigned i=0;i<4;i++)b[i]=(uint8_t)(v>>(8*i));}
static int session(uint32_t run,uint32_t duration)
{
 calibration=false;mode=1;writer_enabled=run>=3;r3_result.mode=writer_enabled;r3_result.run=run;
 r3_result.accepted=r3_result.outputs=r3_result.warmup=r3_result.rails=r3_result.clips=0;
 r3_result.records=r3_result.raw_words=r3_result.max_words=r3_result.flags=r3_result.level=r3_result.clock_checks=0;
 r3_result.sd_steps=r3_result.sd_peak_queue=r3_result.sd_completed=r3_result.sd_step_max=r3_result.sd_write_max_us=r3_result.sd_busy_max_us=0;
 r3_result.sd_total_bytes=r3_result.sd_written=r3_result.drain_us=r3_result.header_written=r3_result.returned=r3_result.return_bytes=0;
 r3_result.trace_count=r3_result.queue_overflows=r3_result.sd_pending_polls=0;r3_source_count=r3_output_count=0;
 r3_result.first_us=r3_result.last_us=r3_result.residual=r3_result.stop_us=0;
 r3_result.source_hash=r3_result.output_hash=2166136261u;capture_queue_init(&q);
 memset(r3_distributions,0,sizeof r3_distributions);memset(r3_sd_distributions,0,sizeof r3_sd_distributions);memset(&r3_writer,0,sizeof r3_writer);r3_hash_init(&source_hash);
 r3_result.sd_lba=run==3?8192:run==4?12288:16384;next_lba=r3_result.sd_lba+1;
 memset(&mapper,0,sizeof mapper);fir_decimator10_init(&fir);next_clock=0;
 if(fir_decimator10_start(&fir,run,0,(FirDecimator10Clock){cal_num,cal_den,true})!=TR2_OK)return -1;
 reset_trace();if(fifo_live_start(&fifo,run,1,1,7,2,now()/1000u))return -2;
 begin_us=now();uint64_t previous=0;uint32_t irq_start=r3_irqs;int error=0;
 while(now()-begin_us<duration){if((error=poll(&previous,true)))break;}
 r3_result.duration_us=(uint32_t)(now()-begin_us);r3_result.irqs=r3_irqs-irq_start;
 if(!error)reset_trace();
 uint64_t stop=now();int stopped=fifo_live_stop(&fifo);r3_result.stop_us=(uint32_t)(now()-stop);
 r3_result.stopped=fifo.stopped;r3_result.fifo_error=fifo.error;r3_result.stop_error=fifo.stop_error;
 r3_result.trace_refused=bench_log_refused;r3_result.preclose_overflow=bench_preclose_overflow;
 r3_result.pending_xyz=mapper.count;r3_result.ctrl1=fifo.ctrl1_verified;r3_result.residual=(uint32_t)mapper.max_residual_us;
 if(error||stopped||!mapper.emitted){fir_decimator10_invalidate(&fir,TR2_D10_SOURCE_FAULT);return error?error:-3;}
 fir_decimator10_stop(&fir);r3_hash_finish(&source_hash,r3_source_sha);
 uint64_t drain=now();
 if(writer_enabled){
  while(q.count||r3_writer.active){if(writer_step(true))return -20;}
  r3_result.drain_us=(uint32_t)(now()-drain);
  if(r3_result.sd_written!=r3_result.sd_total_bytes)return -21;
  memset(r3_header,0,512);memcpy(r3_header,"TR2R3V1",7);
  put32(r3_header+8,1);put32(r3_header+12,run);put32(r3_header+16,r3_result.sd_total_bytes);put32(r3_header+20,r3_result.outputs);
  put32(r3_header+24,r3_result.accepted);put32(r3_header+28,10);put32(r3_header+32,r3_result.first_us);put32(r3_header+36,r3_result.last_us);
  put32(r3_header+40,r3_result.warmup);put32(r3_header+44,r3_result.rails);put32(r3_header+48,r3_result.clips);
  put32(r3_header+52,0);put32(r3_header+56,fir.ended);put32(r3_header+60,r3_result.pending_xyz);
  memcpy(r3_header+64,r3_source_sha,32);memcpy(r3_header+96,r3_card.cid,16);
  put32(r3_header+112,r3_result.ctrl1);put32(r3_header+116,r3_result.clock);put32(r3_header+508,crc32(r3_header,508));
  SdWriter header={0};if(sd_writer_begin(&header,&r3_card,r3_result.sd_lba,r3_header))return -22;
  while(header.active)if(sd_writer_step(&header))return -23;
  r3_result.header_written=1;
 }
 return 0;
}
static int read_return(void)
{
 uint32_t offset=r3_return_offset,size=(r3_result.sd_total_bytes+511u)/512u*512u;
 if(r3_result.run<3||!r3_result.header_written||offset!=r3_result.returned||offset>=size||offset%512)return -30;
 uint32_t n=size-offset;if(n>sizeof r3_return)n=sizeof r3_return;
 r3_result.return_offset=offset;r3_result.return_bytes=0;
 if(!offset){if(sd_spi_read(&r3_card,r3_result.sd_lba,r3_media_header)||memcmp(r3_header,r3_media_header,512))return -32;}
 for(uint32_t i=0;i<n;i+=512)if(sd_spi_read(&r3_card,r3_result.sd_lba+1+(offset+i)/512,r3_return+i))return -31;
 r3_result.return_bytes=n;r3_result.returned+=n;return 0;
}
static void stack_paint(void){stack_begin=(uintptr_t)&_estack-4096u;stack_end=(__get_MSP()-256u)&~3u;if(stack_begin<(uintptr_t)&_ebss+512u||stack_end<=stack_begin)Error_Handler();for(volatile uint32_t *p=(uint32_t *)stack_begin;(uintptr_t)p<stack_end;p++)*p=0xA53CC35Au;}
static void stack_scan(void){uintptr_t p=stack_begin;while(p<stack_end&&*(volatile uint32_t *)p==0xA53CC35Au)p+=4;r3_result.stack_floor=(uint32_t)(p-stack_begin);r3_result.stack_deepest=p<stack_end?(uint32_t)((uintptr_t)&_estack-p):0;if(p==stack_begin)r3_result.error=91;}
int main(void)
{
 r3_result.version=1;r3_result.stage=1;r3_result.uid[0]=HAL_GetUIDw0();r3_result.uid[1]=HAL_GetUIDw1();r3_result.uid[2]=HAL_GetUIDw2();
 if(r3_result.uid[0]!=0x001b0046||r3_result.uid[1]!=0x38425018||r3_result.uid[2]!=0x20333157)Error_Handler();
 stmdev_ctx_t ctx;BenchClock clock;
 if(HAL_Init()!=HAL_OK)Error_Handler();
 HAL_PWREx_DisableUCPDDeadBattery();
 if(platform_init(&ctx,&clock)||bench_core_hz!=160000000u||r3_sd_prepare())Error_Handler();
 r3_result.clock=bench_core_hz;last_cycles=DWT->CYCCNT;r3_result.primask=__get_PRIMASK();r3_result.basepri=__get_BASEPRI();r3_result.faultmask=__get_FAULTMASK();
 if(r3_result.primask||r3_result.basepri||r3_result.faultmask)Error_Handler();
 if(fifo_live_init(&fifo,&ctx,5,consume,NULL)||fifo_live_set_poll_limit(&fifo,64))Error_Handler();
 fifo.cleanup_begin=clock.cleanup_begin;fifo.first_data_timeout_ms=20;stack_paint();r3_result.stage=2;
 for(;;){
  while(!r3_command){(void)now();__WFI();}
  uint32_t cmd=r3_command;r3_command=0;r3_result.stage=3;int r=-90;
  if(cmd==(MAGIC|1u)&&r3_result.run==0){
   uint8_t who=0;reset_trace();r=iis3dwb_device_id_get(&ctx,&who);r3_result.who=who;
   if(!r&&who==0x7b){r3_result.who_ok=1;r=r3_sd_init(&r3_card);}else r=-91;
   uint8_t marker[512];
   if(!r){r3_result.sd_sectors=r3_card.sectors;r=sd_spi_read(&r3_card,0,marker);}
   if(!r&&(memcmp(marker,"TR2 R2 SPI2 RAW TEST V1",23)||memcmp(marker+32,r3_card.cid,16)))r=-92;
   if(!r){r3_result.marker_ok=1;r=sd_writer_r3_gate(&r3_card);}
   if(!r){r=calibrate();r3_result.run=1;}
  }else if((cmd==(MAGIC|2u)&&r3_result.run==1)||(cmd==(MAGIC|3u)&&r3_result.run==2)||
    (cmd==(MAGIC|4u)&&r3_result.run==3&&r3_result.returned==(r3_result.sd_total_bytes+511u)/512u*512u)||
    (cmd==(MAGIC|5u)&&r3_result.run==4&&r3_result.returned==(r3_result.sd_total_bytes+511u)/512u*512u)){
   uint32_t run=cmd&255;r=session(run,run==4?10000000u:run==5?60000000u:2000000u);
  }else if(cmd==(MAGIC|0x10u)&&!fifo.running)r=read_return();
  if(fifo.running){if(!r)reset_trace();if(fifo_live_stop(&fifo))r=-93;}
  if(r3_writer.active){while(r3_writer.active){if(sd_writer_step(&r3_writer))break;}}
  if(r3_writer.uncertain&&sd_spi_quiesce(&r3_card)){r3_result.error=94;r3_result.stage=98;for(;;){(void)now();__WFI();}}
  r3_sd_deselect();r3_result.stopped=fifo.stopped;r3_result.fifo_error=fifo.error;r3_result.stop_error=fifo.stop_error;
  r3_result.trace_refused=bench_log_refused;r3_result.preclose_overflow=bench_preclose_overflow;r3_result.sd_error=r3_card.error;
  if(r)r3_result.error=(uint32_t)(r<0?-r:r);
  stack_scan();r3_result.stage=r3_result.error?99:4;r3_finished();
  if(r3_result.error)for(;;){(void)now();__WFI();}
 }
}
