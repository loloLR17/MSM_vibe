/* R4U board qualification adapter. The reusable runtime is live_runtime.c.
 * Shared, already observed board/calibration helpers only; NO retirement call.
 * Every boot WAIT. New authority is the full R4T final snapshot371a...6634. */
#ifndef TR2_LIVE_OPERATOR
#define TR2_LIVE_OPERATOR 0
#endif
#define TR2_R4S_ENTRY unused_r4s_entry
#include "stm32_r4s_bench.c"
#include "stm32_serial_transport.h"
#include "tr2/application/live_runtime.h"
/* Defaults preserve the historical R4U image; R4V supplies exact new guards. */
#ifndef R4U_STOP_HOLD_US
#define R4U_STOP_HOLD_US 2000000
#endif
#ifndef R4U_INITIAL_SHA
#define R4U_INITIAL_SHA 0x37,0x1a,0x9d,0x16,0xc6,0x28,0x5d,0x01,0x50,0x60,0x87,0x51,0xcc,0xfd,0xd5,0x21,0x7f,0xf3,0x03,0xd4,0x2d,0x8e,0x54,0x46,0xd6,0x6a,0x81,0x30,0x34,0x2e,0x66,0x34
#define R4U_INITIAL_JOURNAL 38
#define R4U_INITIAL_ROOT 51
#define R4U_INITIAL_RUN 6
#define R4U_INITIAL_STOP 2
#define R4U_FINAL_RUN 7
#define R4U_FINAL_STOP 4
#define R4U_EFFECT(code,point,result) ((void)0)
#define R4U_PORT(p) ((void)0)
#define R4U_PREPARED() ((void)0)
#define R4U_BOUNDED_INIT() ((void)0)
#endif
static LiveRuntime runtime;
static SerialTransport uart;
static LiveAcquisitionPort real_port;
static bool uart_started;
volatile struct {
 uint32_t stage,error,mode,starts,stops;uint64_t begin_us,end_us,max_drain_us,max_start_us,max_stop_us;
} r4u_result;
__attribute__((noinline)) void r4u_checkpoint(void){__asm volatile("":::"memory");}
__attribute__((noinline)) void r4u_uart_checkpoint(void){__asm volatile("":::"memory");}
static Tr2Result u_mount(void)
{
#if !TR2_LIVE_OPERATOR
 static const uint8_t expected[32]={R4U_INITIAL_SHA};
 uint8_t hash[32];
#endif
 REQUIRE(tr2_cooperative_enter(&work));
 for(uint32_t at=0;at<262144;at+=32){REQUIRE(read_physical(NULL,at,baseline+at,32));REQUIRE(tr2_cooperative_checkpoint(&work));}
 REQUIRE(tr2_cooperative_leave(&work));
#if !TR2_LIVE_OPERATOR
 REQUIRE(tr2_sha256_bytes_cooperative(baseline,262144,hash,&work));
 if(memcmp(hash,expected,32))return TR2_ERROR_INVALID_STATE;
#endif
 TransactionalImagePhysicalStorage storage={NULL,read_physical,write_physical};
 TransactionalImageGeometry geometry=transactional_image_geometry_qualification_profile();
 static const PersistentIdentityAssignment assignment={{0x004A003D,0x38425018,0x20333157},1};
 REQUIRE(transactional_image_media_init_v2(&historical,&storage,&geometry,candidate,sizeof(candidate)));
 REQUIRE(transactional_image_media_init_live(&live,&storage,candidate,sizeof(candidate),NULL,ownership));
 REQUIRE(transactional_image_media_bind_work(&historical,&work));REQUIRE(transactional_image_media_bind_work(&live,&work));
 REQUIRE(live_configuration_init(&configuration,&historical,&live,uid,&assignment,1));
 REQUIRE(live_command_store_init(&store,&configuration));
 real_port=live_fifo_acquisition_port(&owner.acquisition);
 REQUIRE(live_command_engine_init(&engine,&store,&real_port));REQUIRE(live_command_engine_boot(&engine));
#if TR2_LIVE_OPERATOR
 /* Fixed snapshot/one-shot IDs belong to the historical bench, not rebootable
  * operation. Real codecs validate CRC/identity/journal/effects at every mount;
  * any active or indeterminate recovery fails closed, never replays an effect. */
 if(engine.indeterminate||engine.has_active||!engine.has_last||engine.last.status!=4||engine.last.result||
    !store.epoch.current||configuration.active.config_id!=3||configuration.active.generation!=3)
  return TR2_ERROR_INVALID_STATE;
#else
 if(engine.indeterminate||engine.has_active||!engine.has_last||engine.last.status!=4||engine.last.result||
  engine.last.request.epoch!=5||engine.last.request.command.transaction_id!=R4U_INITIAL_STOP||engine.last.request.command.identity.command_code!=14||
  store.generation!=R4U_INITIAL_JOURNAL||live.generation!=R4U_INITIAL_ROOT||store.epoch.current!=5||store.effect.run_id!=R4U_INITIAL_RUN||store.effect.phase!=3||
  configuration.active.config_id!=3||configuration.active.generation!=3)return TR2_ERROR_INVALID_STATE;
#endif
 return TR2_OK;
}
static Tr2Result u_start(void *ctx,uint64_t run,const ActiveConfigurationSnapshot *cfg)
{
 (void)ctx;uint64_t t=now_us(NULL);r4_result.last_poll_us=0;memset(&event_time,0,sizeof(event_time));next_check=0;
 R4U_EFFECT(13,1,0);Tr2Result result=real_port.start(real_port.context,run,cfg);R4U_EFFECT(13,2,result);uint64_t cost=now_us(NULL)-t;
 if(cost>r4u_result.max_start_us)r4u_result.max_start_us=cost;
 r4u_result.starts++;return cost>1000?TR2_ERROR_UNAVAILABLE:result;
}
static Tr2Result u_stop(void *ctx)
{
 (void)ctx;uint64_t t=now_us(NULL);R4U_EFFECT(14,1,0);Tr2Result result=real_port.stop(real_port.context);R4U_EFFECT(14,2,result);
 uint64_t cost=now_us(NULL)-t;if(cost>r4u_result.max_stop_us)r4u_result.max_stop_us=cost;r4u_result.stops++;return result;
}
static Tr2Result u_drain(void *ctx)
{
 (void)ctx;uint64_t t=now_us(NULL);
 if(owner.acquisition.fifo.running) {
  if(!event_time.initialized||now_us(NULL)>=next_check) {
   ClockBracket c;c.before_us=now_us(NULL);
   if(iis3dwb_timestamp_raw_get(&owner.acquisition.fifo.ctx,&c.raw))return TR2_ERROR_UNAVAILABLE;
   c.after_us=now_us(NULL);c.index=(uint32_t)owner.acquisition.fifo.total_xyz;
   Tr2Result result;
   if(!event_time.initialized)result=fifo_event_time_init(&event_time,&owner.acquisition.fifo,&metric,c.raw,(c.before_us+c.after_us)/2,r4r_result.span_us,r4r_result.span_ticks);
   else {r4s_clock_last=c;if(r4s_result.clock_checks<128)r4s_clock_brackets[r4s_result.clock_checks]=c;
    result=fifo_event_time_track(&event_time,c.raw,c.before_us,c.after_us,100);r4s_result.clock_checks++;}
   if(result!=TR2_OK)return result;
   next_check=c.after_us+100000;
  }
  REQUIRE(service(NULL));
 }
 uint64_t cost=now_us(NULL)-t;if(cost>r4u_result.max_drain_us)r4u_result.max_drain_us=cost;return TR2_OK;
}
static Tr2Result u_service(void *ctx){(void)ctx;return live_runtime_service(&runtime);}
static Tr2Result u_prepare(bool physical,bool writable)
{
 work.service=service; /* stopped: safely compose before binding the new service */
 if(owner.acquisition.fifo.running||engine.has_active)return TR2_ERROR_INVALID_STATE;
 if(!owner.acquisition.metrology) {
  live_metrology_init(&metric);
  REQUIRE(live_fifo_acquisition_bind_event_metrology(&owner.acquisition,&metric,s_consume,&event_time));
  real_port=live_fifo_acquisition_port(&owner.acquisition);LiveAcquisitionPort port=real_port;port.start=u_start;port.stop=u_stop;R4U_PORT(port);
  REQUIRE(live_command_engine_init(&engine,&store,&port));REQUIRE(live_fifo_acquisition_bind_engine(&owner.acquisition,&engine));
 } else {
  /* Binding is single-use. Reuse the same stopped composition and callbacks,
   * never clear acquisition.engine/metrology to bypass the binding guard. */
  if(owner.acquisition.metrology!=&metric||owner.acquisition.engine!=&engine)return TR2_ERROR_INVALID_STATE;
  void (*safe_stop)(void *)=metric.safe_stop;void *stop_context=metric.stop_context;
  live_metrology_init(&metric);metric.safe_stop=safe_stop;metric.stop_context=stop_context;
 }
 memset(&event_time,0,sizeof(event_time));next_check=0;
 REQUIRE(live_command_engine_boot(&engine));
 PersistentIdentityRecord id;ModbusBlock0Image b0;REQUIRE(live_configuration_identity(&configuration,&id));REQUIRE(modbus_project_b0(&id.identity.functional,&b0));
 static SerialTransport ram={NULL,r_receive,r_transmit,r_event};SerialTransport *transport=&ram;
 if(physical) {
  if(!uart_started){REQUIRE(stm32_serial_transport_init(&uart));}
  transport=&uart;
 } else {
  frame_index=0;next_request=0;const uint8_t pdu[5]={3,0x0b,0xb8,0,48};
  if(modbus_rtu_adu_encode(1,pdu,5,frame,sizeof(frame),&frame_bytes)!=MODBUS_RTU_CODEC_OK)return TR2_ERROR_INVALID_STATE;
 }
 REQUIRE(live_modbus_server_init(&server,&engine,&metric,transport,1,&b0,r_ms,NULL));
 REQUIRE(live_modbus_server_enable_unavailable_inventory(&server));server.read_only=!writable;
 REQUIRE(live_runtime_init(&runtime,&server,&work,NULL,u_drain,8));R4U_PREPARED();
 if(physical&&!uart_started){REQUIRE(modbus_rtu_server_runtime_start(&server.rtu));uart_started=true;}
 r4_result.last_poll_us=0;r4_result.max_gap_us=0;r4u_result.max_drain_us=0;
 work.service=u_service;return TR2_OK;
}
static Tr2Result u_readonly(bool physical,uint64_t duration)
{
 REQUIRE(u_prepare(physical,false));r4_result.ownership=0;
 if(physical)r4u_uart_checkpoint();
 REQUIRE(u_start(NULL,UINT64_MAX-1,&configuration.active));metric.authority=false;
 r4u_result.begin_us=now_us(NULL);
 while(now_us(NULL)-r4u_result.begin_us<duration)REQUIRE(live_runtime_step(&runtime));
 r4u_result.end_us=now_us(NULL);REQUIRE(u_stop(NULL));REQUIRE(compare_protected());
 if(r4_result.write_bytes||engine.has_active||!metric.has_value||owner.acquisition.fifo.error||owner.acquisition.fifo.stop_error)return TR2_ERROR_INVALID_STATE;
 return TR2_OK;
}
__attribute__((noreturn)) void stm32_r4_benchmark_run(SPI_HandleTypeDef *spi1,SPI_HandleTypeDef *spi3)
{
 r4_result.core_hz=HAL_RCC_GetHCLKFreq();r4_result.spi1_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI1)/64;
 r4_result.spi3_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3)/4;
 if(!identity_ok()||r4_result.core_hz!=160000000||r4_result.spi1_hz!=2500000||r4_result.spi3_hz!=4000000||
 stm32_fram_storage_init(&fram,spi1,GPIOD,GPIO_PIN_14,2)!=TR2_OK||stm32_live_acquisition_attach(&owner,spi3,5,consume,NULL)!=TR2_OK){r4u_result.error=1;goto end;}
 owner.acquisition.fifo.first_data_timeout_ms=20;owner.acquisition.budget=admission;previous_cycles=DWT->CYCCNT;elapsed_cycles=0;
 bounded=stm32_fram_storage_bounded_physical(&fram);R4U_BOUNDED_INIT();
 if(tr2_cooperative_init(&work,NULL,now_us,service,stop,32,1000,2500)!=TR2_OK){r4u_result.error=2;goto end;}
 r4u_result.stage=1;
#if !TR2_LIVE_OPERATOR
 while(r4_trigger!=0x52345501u&&r4_trigger!=0x52345502u&&r4_trigger!=0x52345503u){(void)now_us(NULL);HAL_Delay(1);}
#else
 r4_trigger=0x52345503u; /* boot stopped, explicit B5 START remains mandatory */
#endif
 uint32_t mode=r4_trigger;r4_trigger=0;r4u_result.mode=mode;r4u_result.stage=2;
 if(u_mount()!=TR2_OK||s_calibrate()!=TR2_OK){r4u_result.error=3;goto end;}
 /* Each diagnostic has a fresh volatile owner boot. Never clear/lower its
  * monotone run_id, and never reuse a diagnostic owner for durable run7. */
 if(mode==0x52345501u) {
  if(u_readonly(false,5000000)!=TR2_OK){r4u_result.error=3;goto end;}
  r4u_result.stage=3;goto end;
 }
 if(mode==0x52345502u) {
  r4u_result.stage=4;
  if(u_readonly(true,10000000)!=TR2_OK){r4u_result.error=4;goto end;}
  r4u_result.stage=5;goto end;
 }
 r4u_result.stage=6;
 r4r_result.first_window_checked=0;memset(&r4r_first_value,0,sizeof(r4r_first_value));memset(r4r_xyz,0,sizeof(r4r_xyz));
 if(u_prepare(true,true)!=TR2_OK){r4u_result.error=5;goto end;}
 r4_result.ownership=1;r4m_enabled=true;r4_result.stage=60;r4u_result.stage=7;r4u_result.begin_us=now_us(NULL);
#if TR2_LIVE_OPERATOR
 /* Keep the real R2/FIFO/FRAM budgets and ownership guard. Finite bench trace
  * arrays are not an admission authority in this persistent diagnostic mode. */
 r4m_enabled=false;
 r4u_uart_checkpoint();
 for(;;) {
  if(live_runtime_step(&runtime)!=TR2_OK){r4u_result.error=6;goto end;}
 }
#else
 r4u_uart_checkpoint();uint64_t stopped_since=0;
 while(now_us(NULL)-r4u_result.begin_us<600000000) {
  if(live_runtime_step(&runtime)!=TR2_OK){r4u_result.error=6;goto end;}
  if(r4_trigger==0x52345504u&&!owner.acquisition.fifo.running&&!engine.has_active)break;
  if(!owner.acquisition.fifo.running&&!engine.has_active&&engine.has_last&&
   engine.last.request.epoch==5&&engine.last.request.command.transaction_id==R4U_FINAL_STOP&&
   engine.last.request.command.identity.command_code==14&&engine.last.status==4&&!engine.last.result) {
   if(!stopped_since)stopped_since=now_us(NULL);
   if(now_us(NULL)-stopped_since>R4U_STOP_HOLD_US)break;
  }
 }
 r4_trigger=0;r4u_result.end_us=now_us(NULL);
 if(owner.acquisition.fifo.running||engine.has_active||engine.status!=4||engine.result||store.effect.run_id!=R4U_FINAL_RUN||!metric.has_value){r4u_result.error=7;goto end;}
 if(compare_protected()!=TR2_OK){r4u_result.error=8;goto end;}r4u_result.stage=10;
#endif
end:
 if(diag_fifo.running)(void)fifo_live_stop(&diag_fifo);
 stop(NULL);r4_result.ownership=0;snapshot();r4u_checkpoint();
 for(;;){(void)now_us(NULL);HAL_Delay(1);}
}
