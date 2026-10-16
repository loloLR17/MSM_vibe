/* Explicit NON-PRODUCTION R4R. Distinct volatile diagnostic FIFO, then exclusive
 * handback to the genuine R2 owner. No UART/SD, no legacy maintenance dispatch. */
#define stm32_r4_benchmark_run unused_r4p_benchmark_entry
#include "stm32_r4_bench.c"
#undef stm32_r4_benchmark_run
#include "fifo_event_time.h"
#include "tr2/application/live_modbus_server.h"
#include "tr2/modbus/projection.h"
extern void stm32_st_transport_next_window(void);
extern volatile uint32_t bench_cleanup_mode,bench_log_refused,bench_preclose_overflow;
typedef struct {uint64_t before_us,after_us;uint32_t raw,index;} ClockBracket;
volatile struct {
 uint32_t stage,error,words,anchors,counter_mismatch,reserved_nonzero;
 uint32_t min_delta,max_delta,raw_words,brackets,fine,first_ts,last_ts;
 uint64_t first_index,last_index,begin_us,end_us,max_poll_us;
 uint64_t span_us,span_ticks,calibration_residual_us,live_begin_us,live_end_us;
 uint64_t max_publish_us,max_pdu_us,max_event_age_us,stop_us;
 uint32_t responses,valid_responses,held_responses,b3_status,b3_flags,coherence_checks;
 uint32_t b3_sequence,b3_age,first_window_checked,stop_status;
 uint64_t first_event_us,first_window_end_us,hardware_stop_us;
} r4r_result;
uint8_t r4r_raw[2048*7];
int16_t r4r_xyz[4096][3];
ClockBracket r4r_brackets[256];
uint8_t r4r_valid_adu[101],r4r_held_adu[101];
SupervisionSnapshot r4r_first_value;
static FifoLive diag_fifo;
static FifoEventTime event_time;
static LiveMetrology metric;
static LiveModbusServer server;
static uint32_t last_anchor;
static bool pending_counter;
static uint8_t ts_counter;
static uint64_t next_check;
static uint8_t frame[8];static size_t frame_bytes;static unsigned frame_index;
static uint64_t next_request;
static uint32_t read32(const uint8_t *b){return (uint32_t)b[0]<<24|(uint32_t)b[1]<<16|(uint32_t)b[2]<<8|b[3];}
static uint64_t r_ms(void *ctx){(void)ctx;return now_us(NULL)/1000u;}
__attribute__((noinline)) void r4r_checkpoint(void){__asm volatile("":::"memory");}
static int r_consume(void *ctx,const uint8_t *b,uint32_t n,uint64_t index)
{
 (void)ctx;
 if(!r4_result.first_data_us)r4_result.first_data_us=now_us(NULL)-r4_result.start_us;
 for(uint32_t i=0;i<n;i++,b+=7) {
  if(r4r_result.raw_words<2048)memcpy(r4r_raw+7*r4r_result.raw_words++,b,7);
  r4r_result.words++;
  if((b[0]>>3)==2){
   if(pending_counter){if(((b[0]>>1)&3)!=ts_counter)r4r_result.counter_mismatch++;pending_counter=false;}
   index++;continue;
  }
  if((b[0]>>3)!=4)return -1;
  uint32_t raw=(uint32_t)b[1]|(uint32_t)b[2]<<8|(uint32_t)b[3]<<16|(uint32_t)b[4]<<24;
  if(pending_counter)r4r_result.counter_mismatch++;
  pending_counter=true;ts_counter=(b[0]>>1)&3;
  if(b[5]||b[6]!=10)r4r_result.reserved_nonzero++;
  if(r4r_result.anchors){uint32_t delta=raw-last_anchor;if(delta<r4r_result.min_delta)r4r_result.min_delta=delta;if(delta>r4r_result.max_delta)r4r_result.max_delta=delta;}
  else{r4r_result.first_ts=raw;r4r_result.first_index=index;}
  last_anchor=raw;r4r_result.last_ts=raw;r4r_result.last_index=index;r4r_result.anchors++;
 }
 return 0;
}
static Tr2Result diag_service(void)
{
 if(bench_log_refused||bench_preclose_overflow)return TR2_ERROR_UNAVAILABLE;
 stm32_st_transport_next_window();bench_cleanup_mode=0;
 uint64_t begin=now_us(NULL);
 if(r4_result.last_poll_us&&begin-r4_result.last_poll_us>r4_result.max_gap_us)r4_result.max_gap_us=begin-r4_result.last_poll_us;
 r4_result.last_poll_us=begin;r4_result.polls++;
 int error=fifo_live_poll(&diag_fifo,r_ms(NULL));
 uint64_t duration=now_us(NULL)-begin;if(duration>r4r_result.max_poll_us)r4r_result.max_poll_us=duration;
 return error||duration>2500u||r4_result.max_gap_us>5000u?TR2_ERROR_UNAVAILABLE:TR2_OK;
}
static int live_consume(void *ctx,const uint8_t *b,uint32_t n,uint64_t index)
{
 (void)ctx;uint64_t at=index;
 for(uint32_t i=0;i<n;i++)if((b[7*i]>>3)==2) {
  if(at<4096)for(unsigned j=0;j<3;j++) {
   uint16_t bits=(uint16_t)b[7*i+1+2*j]|(uint16_t)b[7*i+2+2*j]<<8;
   r4r_xyz[at][j]=(int16_t)(bits<=INT16_MAX?(int32_t)bits:(int32_t)bits-65536);
  }
  at++;
 }
 uint64_t before=event_time.emitted;
 int r=fifo_event_time_consume(&event_time,b,n,index);
 if(!before&&event_time.emitted)r4r_result.first_event_us=event_time.first_event_us;
 if(metric.sequence==1&&!r4r_result.first_window_checked){r4r_first_value=metric.value;r4r_result.first_window_checked=1;r4r_result.first_window_end_us=metric.value.value_monotonic_ms*1000u;}
 if(event_time.emitted){uint64_t age=now_us(NULL)-event_time.last_event_us;if(age>r4r_result.max_event_age_us)r4r_result.max_event_age_us=age;}
 return r;
}
static Tr2Result r_receive(void *ctx){(void)ctx;return TR2_OK;}
static bool r_event(void *ctx,SerialTransportEvent *e)
{
 (void)ctx;*e=(SerialTransportEvent){SERIAL_TRANSPORT_EVENT_NONE,0,0};
 if(now_us(NULL)<next_request)return true;
 if(frame_index<frame_bytes)*e=(SerialTransportEvent){SERIAL_TRANSPORT_EVENT_BYTE,frame[frame_index],0};
 else if(frame_index==frame_bytes)*e=(SerialTransportEvent){SERIAL_TRANSPORT_EVENT_SILENCE_T1_5,0,0};
 else if(frame_index==frame_bytes+1)*e=(SerialTransportEvent){SERIAL_TRANSPORT_EVENT_SILENCE_T3_5,0,0};
 frame_index++;return true;
}
static Tr2Result r_transmit(void *ctx,const uint8_t *bytes,size_t n)
{
 (void)ctx;ModbusRtuAduView v;
 if(modbus_rtu_adu_decode(bytes,n,&v)!=MODBUS_RTU_CODEC_OK||v.unit_id!=1||v.pdu_length!=98||v.pdu[0]!=3||v.pdu[1]!=96)return TR2_ERROR_CORRUPTED;
 r4r_result.responses++;r4r_result.b3_status=(uint16_t)v.pdu[2]<<8|v.pdu[3];
 uint16_t flags=(uint16_t)v.pdu[4]<<8|v.pdu[5];r4r_result.b3_flags=flags;
 if(flags&64)return TR2_ERROR_CORRUPTED;
 if((flags&6)==6){r4r_result.valid_responses++;memcpy(r4r_valid_adu,bytes,101);}
 if(flags&512){r4r_result.held_responses++;memcpy(r4r_held_adu,bytes,101);}
 r4r_result.b3_sequence=read32(v.pdu+2+8*2);r4r_result.b3_age=read32(v.pdu+2+6*2);
 LiveMetrology captured=server.images[server.published].metric;ModbusBlock3Image expected;
 if(live_metrology_project_b3(&captured,r_ms(NULL),&expected)!=TR2_OK)return TR2_ERROR_CORRUPTED;
 for(unsigned i=0;i<48;i++)if(i!=6&&i!=7) {
  uint16_t actual=(uint16_t)v.pdu[2+2*i]<<8|v.pdu[3+2*i];
  if(actual!=expected.registers[i])return TR2_ERROR_CORRUPTED;
 }
 r4r_result.coherence_checks++;
 /* Every returned register must belong to the same captured image/projection. */
 if((flags&2)&&read32(v.pdu+2+12*2)!=4096u)return TR2_ERROR_CORRUPTED;
 for(unsigned i=40;i<48;i++)if(v.pdu[2+2*i]||v.pdu[3+2*i])return TR2_ERROR_CORRUPTED;
 frame_index=0;next_request=now_us(NULL)+10000u;return TR2_OK;
}
static Tr2Result live_service(void *ctx)
{
 (void)ctx;uint64_t begin;Tr2Result r;
 if(owner.acquisition.fifo.running) {
  if(!event_time.initialized||now_us(NULL)>=next_check){
   ClockBracket c;c.before_us=now_us(NULL);
   if(iis3dwb_timestamp_raw_get(&owner.acquisition.fifo.ctx,&c.raw))return TR2_ERROR_UNAVAILABLE;
   c.after_us=now_us(NULL);
   if(!event_time.initialized)r=fifo_event_time_init(&event_time,&owner.acquisition.fifo,&metric,c.raw,(c.before_us+c.after_us)/2,r4r_result.span_us,r4r_result.span_ticks);
   else r=fifo_event_time_check(&event_time,c.raw,c.before_us,c.after_us,100);
   if(r!=TR2_OK)return r;
   next_check=c.after_us+100000u;
  }
  begin=now_us(NULL);r=service(NULL);uint64_t us=now_us(NULL)-begin;
  if(us>r4r_result.max_poll_us)r4r_result.max_poll_us=us;
  if(r!=TR2_OK)return r;
 }
 /* Loopback-specific publication policy: capture immediately before the
  * next request starts. AGE still projects at PDU time. Do not repeatedly copy
  * the same image at every FRAM32 checkpoint while the client is waiting.
  * This is a software candidate, not physically qualified after STARTED5. */
 if(!frame_index&&now_us(NULL)>=next_request) {
  begin=now_us(NULL);r=live_modbus_server_publish(&server,r_ms(NULL));
  uint64_t us=now_us(NULL)-begin;
  if(us>r4r_result.max_publish_us)r4r_result.max_publish_us=us;
  if(r!=TR2_OK)return r;
 }
 uint64_t us;
 for(unsigned i=0;i<8;i++) {
  begin=now_us(NULL);r=live_modbus_server_poll_once(&server);us=now_us(NULL)-begin;
  if(us>r4r_result.max_pdu_us)r4r_result.max_pdu_us=us;
  if(r!=TR2_OK)return r;
 }
 return TR2_OK;
}
static Tr2Result hardware_stop(void *ctx)
{
 uint64_t t=now_us(NULL);LiveAcquisitionPort p=live_fifo_acquisition_port(ctx);
 Tr2Result r=p.stop(p.context);r4r_result.hardware_stop_us=now_us(NULL)-t;return r;
}
static Tr2Result genuine_live(void)
{
 /* The readonly final snapshot was verified before the diagnostic, and no NVM
  * write or command has occurred. Rebind the recovered real R2 engine, no
  * quarantine isolation/epoch renewal/fake authority. One new START is needed
  * to qualify causal physical B3. Never change or rewind run_floor. */
 live_metrology_init(&metric);
 REQUIRE(live_fifo_acquisition_bind_event_metrology(&owner.acquisition,&metric,live_consume,&event_time));
 LiveAcquisitionPort port=live_fifo_acquisition_port(&owner.acquisition);port.stop=hardware_stop;
 owner.acquisition.engine=NULL;REQUIRE(live_command_engine_init(&engine,&store,&port));
 REQUIRE(live_fifo_acquisition_bind_engine(&owner.acquisition,&engine));REQUIRE(live_command_engine_boot(&engine));
 if(engine.indeterminate||engine.has_active||store.epoch.current!=4||store.effect.run_id!=4)return TR2_ERROR_INVALID_STATE;
 ModbusBlock0Image b0;PersistentIdentityRecord identity;
 REQUIRE(live_configuration_identity(&configuration,&identity));REQUIRE(modbus_project_b0(&identity.identity.functional,&b0));
 static SerialTransport transport={NULL,r_receive,r_transmit,r_event};
 REQUIRE(live_modbus_server_init(&server,&engine,&metric,&transport,1,&b0,r_ms,NULL));
 const uint8_t pdu[5]={3,0x0b,0xb8,0,48};
 if(modbus_rtu_adu_encode(1,pdu,5,frame,sizeof(frame),&frame_bytes)!=MODBUS_RTU_CODEC_OK)return TR2_ERROR_INVALID_STATE;
 work.service=live_service;r4_result.ownership=1;r4_result.last_poll_us=0;r4_result.max_gap_us=0;
 r4r_result.stage=12;r4_result.start_us=now_us(NULL);REQUIRE(command_impl(4,5,13));
 if(owner.acquisition.fifo.run_id!=5||event_time.fault)return TR2_ERROR_INVALID_STATE;
 r4r_result.stage=13;r4r_result.live_begin_us=now_us(NULL);REQUIRE(tr2_cooperative_enter(&work));
 for(uint32_t at=0;now_us(NULL)-r4r_result.live_begin_us<10000000u;at=(at+32u)%262144u){
  uint8_t bytes[32];REQUIRE(read_physical(NULL,at,bytes,32));
  if(memcmp(bytes,baseline+at,32)&&!(at>=122880u&&at<245760u))return TR2_ERROR_STORAGE;
  REQUIRE(tr2_cooperative_checkpoint(&work));
 }
 REQUIRE(tr2_cooperative_leave(&work));r4r_result.live_end_us=now_us(NULL);
 r4r_result.stage=14;uint64_t t=now_us(NULL);REQUIRE(command_impl(4,6,14));r4r_result.stop_us=now_us(NULL)-t;
 r4r_result.stop_status=engine.status;
 /* Continue reads after STOP to observe HELD and stable event dating. */
 REQUIRE(tr2_cooperative_enter(&work));t=now_us(NULL);
 while(now_us(NULL)-t<100000u)REQUIRE(tr2_cooperative_checkpoint(&work));
 REQUIRE(tr2_cooperative_leave(&work));REQUIRE(compare_protected());
 if(!r4r_result.valid_responses||!r4r_result.held_responses||!metric.has_value||event_time.fault)return TR2_ERROR_INVALID_STATE;
 return TR2_OK;
}
__attribute__((noreturn)) void stm32_r4_benchmark_run(SPI_HandleTypeDef *spi1,SPI_HandleTypeDef *spi3)
{
 r4_result.core_hz=HAL_RCC_GetHCLKFreq();r4_result.spi1_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI1)/64u;
 r4_result.spi3_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3)/4u;
 if(!identity_ok()||r4_result.core_hz!=160000000u||r4_result.spi1_hz!=2500000u||r4_result.spi3_hz!=4000000u||
 stm32_fram_storage_init(&fram,spi1,GPIOD,GPIO_PIN_14,2)!=TR2_OK||stm32_live_acquisition_attach(&owner,spi3,5,consume,NULL)!=TR2_OK){r4r_result.error=1;goto end;}
 owner.acquisition.fifo.first_data_timeout_ms=20;
 previous_cycles=DWT->CYCCNT;elapsed_cycles=0;bounded=stm32_fram_storage_bounded_physical(&fram);
 if(tr2_cooperative_init(&work,NULL,now_us,service,stop,32,1000,2500)!=TR2_OK){r4r_result.error=2;goto end;}
 r4r_result.stage=1;while(r4_trigger!=0x5234000Au){(void)now_us(NULL);HAL_Delay(1);}
 r4_trigger=0;r4r_result.stage=2;
 if(qualify(5)!=TR2_OK||r4_result.ownership||r4_result.write_bytes){r4r_result.error=3;goto end;}
 uint8_t fine=0;if(iis3dwb_odr_cal_reg_get(&owner.acquisition.fifo.ctx,&fine)){r4r_result.error=4;goto end;}
 r4r_result.fine=fine;r4r_result.min_delta=UINT32_MAX;r4r_result.stage=3;
 if(fifo_live_init(&diag_fifo,&owner.acquisition.fifo.ctx,5,r_consume,NULL)){r4r_result.error=5;goto end;}
 diag_fifo.cleanup_begin=owner.acquisition.fifo.cleanup_begin;diag_fifo.first_data_timeout_ms=20;
 r4_result.start_us=now_us(NULL);
 if(fifo_live_start(&diag_fifo,UINT64_MAX-1,3,3,7,0,r_ms(NULL))){r4r_result.error=5;goto end;}
 r4r_result.begin_us=now_us(NULL);uint64_t next_bracket=r4r_result.begin_us;
 while(now_us(NULL)-r4r_result.begin_us<2000000u){
  if(diag_service()!=TR2_OK){r4r_result.error=6;break;}
  if(now_us(NULL)>=next_bracket&&r4r_result.brackets<256){
   ClockBracket *c=&r4r_brackets[r4r_result.brackets];c->before_us=now_us(NULL);
   if(iis3dwb_timestamp_raw_get(&diag_fifo.ctx,&c->raw)){r4r_result.error=7;break;}
   c->after_us=now_us(NULL);c->index=(uint32_t)diag_fifo.total_xyz;r4r_result.brackets++;next_bracket=c->after_us+10000u;
  }
 }
 r4r_result.end_us=now_us(NULL);
 if(fifo_live_stop(&diag_fifo)||r4r_result.error||r4r_result.anchors<100||r4r_result.brackets<100||r4r_result.counter_mismatch||r4r_result.reserved_nonzero){r4r_result.error=8;goto end;}
 ClockBracket *a=&r4r_brackets[10],*b=&r4r_brackets[r4r_result.brackets-1];
 r4r_result.span_us=(b->before_us+b->after_us-a->before_us-a->after_us)/2;
 r4r_result.span_ticks=(uint32_t)(b->raw-a->raw);
 if(!r4r_result.span_ticks){r4r_result.error=9;goto end;}
 for(uint32_t i=10;i<r4r_result.brackets;i++){
  ClockBracket *c=&r4r_brackets[i];uint64_t predicted=(a->before_us+a->after_us)/2+(uint32_t)(c->raw-a->raw)*r4r_result.span_us/r4r_result.span_ticks;
  uint64_t mid=(c->before_us+c->after_us)/2,residual=mid>predicted?mid-predicted:predicted-mid;
  if(residual>r4r_result.calibration_residual_us)r4r_result.calibration_residual_us=residual;
 }
 if(r4r_result.calibration_residual_us>100){r4r_result.error=10;goto end;}
 r4r_result.stage=11;r4r_checkpoint();
 while(r4_trigger!=0x5234000Bu){(void)now_us(NULL);HAL_Delay(1);}
 r4_trigger=0;
 if(genuine_live()!=TR2_OK){r4r_result.error=11;live_metrology_invalidate(&metric,false,false);goto end;}
 r4r_result.stage=20;
end:
 if(diag_fifo.running)(void)fifo_live_stop(&diag_fifo);
 stop(NULL);r4_result.ownership=0;snapshot();r4r_checkpoint();for(;;){(void)now_us(NULL);HAL_Delay(1);}
}
