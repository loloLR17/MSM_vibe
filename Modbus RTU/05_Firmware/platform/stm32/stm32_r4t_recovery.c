/* Exact R4S plan. Every boot CLOSED/WAIT; no generic RTU ingress.
 * Readonly phase1 must PASS before host grants the explicit backup token. */
#define TR2_R4S_ENTRY unused_r4s_readonly_entry
#include "stm32_r4s_bench.c"
#include "r4t_maintenance.h"
volatile uint32_t r4s_backup_token,r4s_continue_token;
volatile struct {uint32_t stage,error,old_key_rejected;uint64_t writes_before_live,run_begin_us,run_end_us;} r4t_recovery;
static uint8_t saved_intent[128];
static R4tRetirement retirement;
static Tr2Result retire(void)
{
 REQUIRE(mount_snapshot(true));
 if(r4s_backup_token!=0x53345342u||r4_result.write_bytes)return TR2_ERROR_INVALID_STATE;
 r4s_backup_token=0;REQUIRE(r4t_capture(&retirement,&engine,baseline));r4m_guard=retirement.gate;
 memcpy(saved_intent,r4m_guard.intent,128);
 LiveCommandRequest old=engine.active.request;
 r4_result.ownership=1;r4m_enabled=true;r4_result.stage=40;r4t_recovery.stage=2;
 REQUIRE(r4t_retire_and_admit(&retirement,&engine));
 r4m_guard=retirement.gate;
 REQUIRE(compare_protected());
 r4_result.stage=41;r4t_recovery.stage=3;
 while(engine.has_active){REQUIRE(live_command_engine_step(&engine));}
 REQUIRE(r4m_open_after_renew(&r4m_guard,&engine));
 uint64_t written=r4_result.write_bytes;
 if(r4m_authorize(&r4m_guard,&engine,&old))return TR2_ERROR_INVALID_STATE;
 REQUIRE(live_command_engine_submit(&engine,&old));
 if(engine.result!=24||engine.has_active||owner.acquisition.fifo.running||r4_result.write_bytes!=written)return TR2_ERROR_INVALID_STATE;
 r4t_recovery.old_key_rejected=engine.result;REQUIRE(compare_protected());
 r4_result.ownership=0;r4t_recovery.writes_before_live=r4_result.write_bytes;
 return TR2_OK;
}
static Tr2Result composed(void)
{
 if(r4s_continue_token!=0x53345033u||r4m_guard.phase!=3||store.epoch.current!=5||
  store.effect.run_id!=5||owner.acquisition.fifo.running)return TR2_ERROR_INVALID_STATE;
 r4s_continue_token=0;
 REQUIRE(s_calibrate());
 live_metrology_init(&metric);
 REQUIRE(live_fifo_acquisition_bind_event_metrology(&owner.acquisition,&metric,s_consume,&event_time));
 r4m_real_port=live_fifo_acquisition_port(&owner.acquisition);
 LiveAcquisitionPort port=r4m_real_port;port.start=guarded_start;port.stop=timed_stop;
 owner.acquisition.engine=NULL;REQUIRE(live_command_engine_init(&engine,&store,&port));
 REQUIRE(live_fifo_acquisition_bind_engine(&owner.acquisition,&engine));REQUIRE(live_command_engine_boot(&engine));
 if(engine.indeterminate||engine.has_active||store.epoch.current!=5||store.effect.run_id!=5)return TR2_ERROR_INVALID_STATE;
 ModbusBlock0Image b0;PersistentIdentityRecord id;
 REQUIRE(live_configuration_identity(&configuration,&id));REQUIRE(modbus_project_b0(&id.identity.functional,&b0));
 static SerialTransport transport={NULL,r_receive,r_transmit,r_event};
 REQUIRE(live_modbus_server_init(&server,&engine,&metric,&transport,1,&b0,r_ms,NULL));
 const uint8_t pdu[5]={3,0x0b,0xb8,0,48};
 if(modbus_rtu_adu_encode(1,pdu,5,frame,sizeof(frame),&frame_bytes)!=MODBUS_RTU_CODEC_OK)return TR2_ERROR_INVALID_STATE;
 work.service=s_service;r4_result.ownership=1;r4_result.last_poll_us=0;r4_result.max_gap_us=0;
 r4_result.stage=42;r4t_recovery.stage=5;REQUIRE(command(5,1,13));
 if(owner.acquisition.fifo.run_id!=6||event_time.fault)return TR2_ERROR_INVALID_STATE;
 r4_result.stage=43;r4t_recovery.stage=6;r4t_recovery.run_begin_us=now_us(NULL);
 REQUIRE(tr2_cooperative_enter(&work));
 for(uint32_t at=0;now_us(NULL)-r4t_recovery.run_begin_us<10000000u;at=(at+32u)%262144u){
  uint8_t bytes[32];REQUIRE(read_physical(NULL,at,bytes,32));
  if(memcmp(bytes,baseline+at,32)&&!(at>=122880u&&at<245760u))return TR2_ERROR_STORAGE;
  REQUIRE(tr2_cooperative_checkpoint(&work));
 }
 REQUIRE(tr2_cooperative_leave(&work));r4t_recovery.run_end_us=now_us(NULL);
 r4_result.stage=44;r4t_recovery.stage=7;REQUIRE(command(5,2,14));
 REQUIRE(tr2_cooperative_enter(&work));uint64_t begin=now_us(NULL);
 while(now_us(NULL)-begin<100000u)REQUIRE(tr2_cooperative_checkpoint(&work));
 REQUIRE(tr2_cooperative_leave(&work));REQUIRE(compare_protected());
 if(!r4r_result.valid_responses||!r4r_result.held_responses||!metric.has_value||event_time.fault||
  !owner.acquisition.fifo.stopped||owner.acquisition.fifo.error||owner.acquisition.fifo.stop_error)return TR2_ERROR_INVALID_STATE;
 return TR2_OK;
}
__attribute__((noreturn)) void stm32_r4_benchmark_run(SPI_HandleTypeDef *spi1,SPI_HandleTypeDef *spi3)
{
 r4_result.core_hz=HAL_RCC_GetHCLKFreq();r4_result.spi1_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI1)/64u;
 r4_result.spi3_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3)/4u;
 if(!identity_ok()||r4_result.core_hz!=160000000u||r4_result.spi1_hz!=2500000u||r4_result.spi3_hz!=4000000u||
 stm32_fram_storage_init(&fram,spi1,GPIOD,GPIO_PIN_14,2)!=TR2_OK||stm32_live_acquisition_attach(&owner,spi3,5,consume,NULL)!=TR2_OK){r4t_recovery.error=1;goto end;}
 owner.acquisition.fifo.first_data_timeout_ms=20;previous_cycles=DWT->CYCCNT;elapsed_cycles=0;bounded=stm32_fram_storage_bounded_physical(&fram);
 if(tr2_cooperative_init(&work,NULL,now_us,service,stop,32,1000,2500)!=TR2_OK){r4t_recovery.error=2;goto end;}
 r4t_recovery.stage=1;while(r4_trigger!=0x5234000Du){(void)now_us(NULL);HAL_Delay(1);}
 r4_trigger=0;
 if(retire()!=TR2_OK){r4t_recovery.error=3;goto end;}
 r4t_recovery.stage=4;r4r_checkpoint();
 /* Host verifies full FRAM equals model-renew before this distinct grant. No
  * ordinary request, startup, implicit replay or new epoch occurs in WAIT. */
 while(r4_trigger!=0x5234000Eu){(void)now_us(NULL);HAL_Delay(1);}
 r4_trigger=0;
 if(composed()!=TR2_OK){r4t_recovery.error=4;live_metrology_invalidate(&metric,false,false);goto end;}
 r4t_recovery.stage=10;
end:
 if(diag_fifo.running)(void)fifo_live_stop(&diag_fifo);
 stop(NULL);r4_result.ownership=0;snapshot();r4r_checkpoint();for(;;){(void)now_us(NULL);HAL_Delay(1);}
}
