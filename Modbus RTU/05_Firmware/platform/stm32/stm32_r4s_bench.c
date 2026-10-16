/* R4S phase1: strictly readonly FRAM, volatile diagnostic acquisition only.
 * R2 remains indeterminate. B3 V/F and configuration authority stay false. */
#define TR2_R4R_ENTRY unused_r4r_entry
#include "stm32_r4r_bench.c"

typedef struct {
 uint64_t total,poll,consumer,clock,publish,rtu,mapping,metrology;
 uint32_t words,level,sequence,published;
} R4sService;
volatile struct {uint32_t stage,error,write_attempts,clock_checks,trace_count;uint64_t begin_us,end_us,max_consumer_us,start_us,stop_us;} r4s_result;
R4sService r4s_trace[64],r4s_max_service;
ClockBracket r4s_clock_brackets[128];
uint32_t fifo_event_time_cycles(void){return DWT->CYCCNT;}
static uint64_t consumer_us;
static int s_consume(void *ctx,const uint8_t *b,uint32_t n,uint64_t index)
{
 uint64_t t=now_us(NULL);int r=live_consume(ctx,b,n,index);consumer_us+=now_us(NULL)-t;return r;
}
static Tr2Result s_deny_write(void *ctx,uint32_t at,const void *b,size_t n)
{(void)ctx;(void)at;(void)b;(void)n;r4s_result.write_attempts++;return TR2_ERROR_STORAGE;}
static Tr2Result mount_readonly(void)
{
 static const uint8_t expected[32]={0x14,0xfb,0x17,0x2f,0x29,0x17,0xd0,0xa8,0xe4,0x40,0x43,0x80,0x51,0x83,0x1c,0x84,0xf3,0xf3,0xe7,0x46,0x16,0x17,0x02,0xa7,0xde,0x9e,0x9a,0x1a,0x25,0x1b,0x2a,0xa9};
 uint8_t hash[32];REQUIRE(tr2_cooperative_enter(&work));
 for(uint32_t at=0;at<sizeof(baseline);at+=32){REQUIRE(read_physical(NULL,at,baseline+at,32));REQUIRE(tr2_cooperative_checkpoint(&work));}
 REQUIRE(tr2_cooperative_leave(&work));REQUIRE(tr2_sha256_bytes_cooperative(baseline,sizeof(baseline),hash,&work));
 if(memcmp(hash,expected,32))return TR2_ERROR_STORAGE;
 TransactionalImagePhysicalStorage storage={NULL,read_physical,s_deny_write};
 TransactionalImageGeometry geometry=transactional_image_geometry_qualification_profile();
 static const PersistentIdentityAssignment assignment={{0x004A003D,0x38425018,0x20333157},1};
 REQUIRE(transactional_image_media_init_v2(&historical,&storage,&geometry,candidate,sizeof(candidate)));
 REQUIRE(transactional_image_media_init_live(&live,&storage,candidate,sizeof(candidate),NULL,ownership));
 REQUIRE(transactional_image_media_bind_work(&historical,&work));REQUIRE(transactional_image_media_bind_work(&live,&work));
 REQUIRE(live_configuration_init(&configuration,&historical,&live,uid,&assignment,1));
 REQUIRE(live_command_store_init(&store,&configuration));REQUIRE(live_configuration_recover(&configuration));
 LiveAcquisitionPort port=live_fifo_acquisition_port(&owner.acquisition);
 REQUIRE(live_command_engine_init(&engine,&store,&port));REQUIRE(live_command_engine_boot(&engine));
 if(!engine.indeterminate||engine.status!=9||!engine.has_active||store.epoch.current!=4||store.effect.run_id!=5||configuration.active.config_id!=3||configuration.active.generation!=3)return TR2_ERROR_INVALID_STATE;
 return TR2_OK;
}
static Tr2Result s_service(void *ctx)
{
 (void)ctx;R4sService sample={0};uint64_t start=now_us(NULL),t;Tr2Result r;
 if(owner.acquisition.fifo.running) {
  if(!event_time.initialized||now_us(NULL)>=next_check){
   ClockBracket c;c.before_us=now_us(NULL);
   if(iis3dwb_timestamp_raw_get(&owner.acquisition.fifo.ctx,&c.raw))return TR2_ERROR_UNAVAILABLE;
   c.after_us=now_us(NULL);c.index=(uint32_t)owner.acquisition.fifo.total_xyz;
   if(!event_time.initialized)r=fifo_event_time_init(&event_time,&owner.acquisition.fifo,&metric,c.raw,(c.before_us+c.after_us)/2,r4r_result.span_us,r4r_result.span_ticks);
   else {
    if(r4s_result.clock_checks>=128)return TR2_ERROR_INVALID_STATE;
    r4s_clock_brackets[r4s_result.clock_checks]=c;
    r=fifo_event_time_track(&event_time,c.raw,c.before_us,c.after_us,100);r4s_result.clock_checks++;
   }
   sample.clock=c.after_us-c.before_us;if(r!=TR2_OK)return r;next_check=c.after_us+100000u;
  }
  uint64_t mapping=event_time.mapping_cycles,metrology=event_time.metrology_cycles;
  uint64_t before=owner.acquisition.fifo.total_xyz;consumer_us=0;t=now_us(NULL);r=service(NULL);
  sample.mapping=(event_time.mapping_cycles-mapping+159u)/160u;
  sample.metrology=(event_time.metrology_cycles-metrology+159u)/160u;
  sample.poll=now_us(NULL)-t;sample.consumer=consumer_us;sample.words=(uint32_t)(owner.acquisition.fifo.total_xyz-before);sample.level=owner.acquisition.fifo.parser.last_level;
  if(sample.consumer>r4s_result.max_consumer_us)r4s_result.max_consumer_us=sample.consumer;
  if(r!=TR2_OK)return r;
 }
 if(!frame_index&&now_us(NULL)>=next_request){
  t=now_us(NULL);r=live_modbus_server_publish(&server,r_ms(NULL));sample.publish=now_us(NULL)-t;sample.published=1;if(r!=TR2_OK)return r;
 }
 t=now_us(NULL);
 for(unsigned i=0;i<8;i++){r=live_modbus_server_poll_once(&server);if(r!=TR2_OK)return r;}
 sample.rtu=now_us(NULL)-t;sample.sequence=metric.sequence;sample.total=now_us(NULL)-start;
 /* Every new record maximum retains a fully paired composition, not a sum
  * of unrelated maxima. Also retain every publication and clock observation. */
 if(sample.total>r4s_max_service.total)r4s_max_service=sample;
 if(r4s_result.trace_count<64&&(sample.published||sample.clock||!r4s_result.trace_count||sample.total>r4s_trace[r4s_result.trace_count-1].total))r4s_trace[r4s_result.trace_count++]=sample;
 return TR2_OK;
}
__attribute__((noreturn)) void stm32_r4_benchmark_run(SPI_HandleTypeDef *spi1,SPI_HandleTypeDef *spi3)
{
 r4_result.core_hz=HAL_RCC_GetHCLKFreq();r4_result.spi1_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI1)/64u;
 r4_result.spi3_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3)/4u;
 if(!identity_ok()||r4_result.core_hz!=160000000u||r4_result.spi1_hz!=2500000u||r4_result.spi3_hz!=4000000u||
 stm32_fram_storage_init(&fram,spi1,GPIOD,GPIO_PIN_14,2)!=TR2_OK||stm32_live_acquisition_attach(&owner,spi3,5,consume,NULL)!=TR2_OK){r4s_result.error=1;goto end;}
 owner.acquisition.fifo.first_data_timeout_ms=20;previous_cycles=DWT->CYCCNT;elapsed_cycles=0;
 bounded=stm32_fram_storage_bounded_physical(&fram);
 if(tr2_cooperative_init(&work,NULL,now_us,service,stop,32,1000,2500)!=TR2_OK){r4s_result.error=2;goto end;}
 r4s_result.stage=1;while(r4_trigger!=0x5234000Cu){(void)now_us(NULL);HAL_Delay(1);}
 r4_trigger=0;r4s_result.stage=2;
 if(mount_readonly()!=TR2_OK){r4s_result.error=3;goto end;}
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

 r4s_result.stage=3;
 live_metrology_init(&metric);
 if(live_fifo_acquisition_bind_event_metrology(&owner.acquisition,&metric,s_consume,&event_time)!=TR2_OK){r4s_result.error=4;goto end;}
 ModbusBlock0Image b0;PersistentIdentityRecord id;
 if(live_configuration_identity(&configuration,&id)!=TR2_OK||modbus_project_b0(&id.identity.functional,&b0)!=TR2_OK){r4s_result.error=5;goto end;}
 static SerialTransport transport={NULL,r_receive,r_transmit,r_event};
 if(live_modbus_server_init(&server,&engine,&metric,&transport,1,&b0,r_ms,NULL)!=TR2_OK){r4s_result.error=6;goto end;}
 const uint8_t pdu[5]={3,0x0b,0xb8,0,48};
 if(modbus_rtu_adu_encode(1,pdu,5,frame,sizeof(frame),&frame_bytes)!=MODBUS_RTU_CODEC_OK){r4s_result.error=7;goto end;}
 LiveAcquisitionPort port=live_fifo_acquisition_port(&owner.acquisition);
 r4_result.last_poll_us=0;r4_result.max_gap_us=0;
 uint64_t start=now_us(NULL);
 Tr2Result started=port.start(port.context,UINT64_MAX-1,&configuration.active);
 r4s_result.start_us=now_us(NULL)-start;
 if(started!=TR2_OK||r4s_result.start_us>1000u){r4s_result.error=8;goto end;}
 metric.authority=false; /* Diagnostic context, NEVER an admitted R2 measurement. */
 work.service=s_service;r4s_result.begin_us=now_us(NULL);r4s_result.stage=4;
 if(tr2_cooperative_enter(&work)!=TR2_OK){r4s_result.error=9;goto end;}
 for(uint32_t at=0;now_us(NULL)-r4s_result.begin_us<10000000u;at=(at+32u)%262144u){
  uint8_t bytes[32];Tr2Result r=read_physical(NULL,at,bytes,32);
  if(r!=TR2_OK||memcmp(bytes,baseline+at,32)||tr2_cooperative_checkpoint(&work)!=TR2_OK){r4s_result.error=10;break;}
 }
 if(tr2_cooperative_leave(&work)!=TR2_OK)r4s_result.error=11;
 r4s_result.end_us=now_us(NULL);
 start=now_us(NULL);
 if(port.stop(port.context)!=TR2_OK)r4s_result.error=12;
 r4s_result.stop_us=now_us(NULL)-start;
 if(r4s_result.write_attempts||r4_result.write_bytes||!metric.has_value||r4r_result.valid_responses)r4s_result.error=13;
 if(!r4s_result.error)r4s_result.stage=10;
end:
 if(diag_fifo.running)(void)fifo_live_stop(&diag_fifo);
 stop(NULL);snapshot();r4r_checkpoint();for(;;){(void)now_us(NULL);HAL_Delay(1);}
}
