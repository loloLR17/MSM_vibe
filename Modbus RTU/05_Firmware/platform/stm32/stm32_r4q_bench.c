/* Reuse qualified UID/clocks/FRAM read/recovery of R4P. Its maintenance entry is
 * never called: this entry only accepts trigger9 and calls qualify(5) readonly.
 * No serial peripheral, RX IRQ, RS485 TX or nonvolatile command is enabled. */
#define stm32_r4_benchmark_run unused_r4p_benchmark_entry
#include "stm32_r4_bench.c"
#undef stm32_r4_benchmark_run
#include "tr2/application/live_modbus_server.h"
#include "tr2/modbus/projection.h"

static LiveModbusServer qserver;
static LiveMetrology qmetric;
volatile struct {
 uint32_t stage,error,responses,b0,b3,b4,b5,unexpected,frame_index;
 uint64_t begin_us,end_us,max_pdu_us,max_publish_us;
 uint32_t b3_status,b3_flags,b0_protocol,b0_capabilities;
} r4q_result;
static uint8_t frame[8];static size_t frame_bytes;static unsigned frame_index;
static uint64_t next_request;
static unsigned selected;
static const uint16_t addresses[]={0,3000,4000,5000};
static const uint16_t quantities[]={21,48,64,29};
static uint64_t q_ms(void *ctx){(void)ctx;return now_us(NULL)/1000u;}
static void q_next(void)
{
 uint16_t a=addresses[selected%4],n=quantities[selected%4];
 uint8_t pdu[5]={3,(uint8_t)(a>>8),(uint8_t)a,0,(uint8_t)n};
 if(modbus_rtu_adu_encode(1,pdu,5,frame,sizeof(frame),&frame_bytes)!=MODBUS_RTU_CODEC_OK)r4q_result.unexpected++;
 frame_index=0;
}
static Tr2Result q_receive(void *ctx){(void)ctx;return TR2_OK;}
static bool q_event(void *ctx,SerialTransportEvent *event)
{
 (void)ctx;*event=(SerialTransportEvent){SERIAL_TRANSPORT_EVENT_NONE,0,0};
 if(now_us(NULL)<next_request)return true;
 if(frame_index<frame_bytes)*event=(SerialTransportEvent){SERIAL_TRANSPORT_EVENT_BYTE,frame[frame_index],0};
 else if(frame_index==frame_bytes)*event=(SerialTransportEvent){SERIAL_TRANSPORT_EVENT_SILENCE_T1_5,0,0};
 else if(frame_index==frame_bytes+1)*event=(SerialTransportEvent){SERIAL_TRANSPORT_EVENT_SILENCE_T3_5,0,0};
 else return true;
 frame_index++;r4q_result.frame_index=frame_index;return true;
}
static Tr2Result q_transmit(void *ctx,const uint8_t *bytes,size_t n)
{
 (void)ctx;ModbusRtuAduView view;
 if(modbus_rtu_adu_decode(bytes,n,&view)!=MODBUS_RTU_CODEC_OK||view.unit_id!=1||
  view.pdu[0]!=3||view.pdu_length!=2u+2u*quantities[selected%4])return TR2_ERROR_CORRUPTED;
 unsigned which=selected%4;r4q_result.responses++;
 if(which==0){r4q_result.b0++;r4q_result.b0_protocol=(uint16_t)view.pdu[14]<<8|view.pdu[15];r4q_result.b0_capabilities=(uint16_t)view.pdu[16]<<8|view.pdu[17];}
 if(which==1){r4q_result.b3++;r4q_result.b3_status=(uint16_t)view.pdu[2]<<8|view.pdu[3];r4q_result.b3_flags=(uint16_t)view.pdu[4]<<8|view.pdu[5];if(r4q_result.b3_flags&6u)return TR2_ERROR_CORRUPTED;}
 if(which==2)r4q_result.b4++;
 if(which==3)r4q_result.b5++;
 selected++;q_next();next_request=now_us(NULL)+10000u;return TR2_OK;
}
static Tr2Result q_service(void *ctx)
{
 (void)ctx;uint64_t begin=now_us(NULL);
 Tr2Result r=live_modbus_server_publish(&qserver,q_ms(NULL));
 uint64_t elapsed=now_us(NULL)-begin;if(elapsed>r4q_result.max_publish_us)r4q_result.max_publish_us=elapsed;
 if(r!=TR2_OK)return r;
 for(unsigned i=0;i<8;i++) {
  begin=now_us(NULL);r=live_modbus_server_poll_once(&qserver);
  elapsed=now_us(NULL)-begin;if(elapsed>r4q_result.max_pdu_us)r4q_result.max_pdu_us=elapsed;
  if(r!=TR2_OK)return r;
 }
 return TR2_OK;
}
__attribute__((noreturn)) void stm32_r4_benchmark_run(SPI_HandleTypeDef *spi1,SPI_HandleTypeDef *spi3)
{
 r4_result.core_hz=HAL_RCC_GetHCLKFreq();r4_result.spi1_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI1)/64u;
 r4_result.spi3_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3)/4u;
 if(!identity_ok()||r4_result.core_hz!=160000000u||r4_result.spi1_hz!=2500000u||r4_result.spi3_hz!=4000000u||
  stm32_fram_storage_init(&fram,spi1,GPIOD,GPIO_PIN_14,2)!=TR2_OK||stm32_live_acquisition_attach(&owner,spi3,5,consume,NULL)!=TR2_OK)
  {r4q_result.error=1;goto end;}
 previous_cycles=DWT->CYCCNT;elapsed_cycles=0;bounded=stm32_fram_storage_bounded_physical(&fram);
 if(tr2_cooperative_init(&work,NULL,now_us,service,stop,32,1000,2500)!=TR2_OK){r4q_result.error=2;goto end;}
 r4q_result.stage=1;
 while(r4_trigger!=0x52340009u){(void)now_us(NULL);HAL_Delay(1);}
 r4_trigger=0;r4q_result.stage=2;
 /* Exact a13b final snapshot guard, closed ownership, no recovered writes. */
 Tr2Result r=qualify(5);
 if(r!=TR2_OK||r4_result.write_bytes||r4_result.ownership||owner.acquisition.fifo.running){r4q_result.error=3;goto end;}
 ModbusBlock0Image b0;PersistentIdentityRecord id;
 live_metrology_init(&qmetric);
 if(live_configuration_identity(&configuration,&id)!=TR2_OK||modbus_project_b0(&id.identity.functional,&b0)!=TR2_OK){r4q_result.error=4;goto end;}
 static SerialTransport transport={NULL,q_receive,q_transmit,q_event};
 if(live_modbus_server_init(&qserver,&engine,&qmetric,&transport,1,&b0,q_ms,NULL)!=TR2_OK){r4q_result.error=5;goto end;}
 q_next();work.service=q_service;r4q_result.stage=3;r4q_result.begin_us=now_us(NULL);
 if(tr2_cooperative_enter(&work)!=TR2_OK){r4q_result.error=6;goto end;}
 for(uint32_t at=0;now_us(NULL)-r4q_result.begin_us<10000000u;at=(at+32u)%262144u) {
  uint8_t bytes[32];r=read_physical(NULL,at,bytes,32);
  if(r!=TR2_OK||memcmp(bytes,baseline+at,32)||tr2_cooperative_checkpoint(&work)!=TR2_OK){r4q_result.error=7;break;}
 }
 if(tr2_cooperative_leave(&work)!=TR2_OK){r4q_result.error=8;goto end;}
 r4q_result.end_us=now_us(NULL);
 if(r4_result.write_bytes||owner.acquisition.fifo.running||r4q_result.unexpected||!r4q_result.b0||!r4q_result.b3||!r4q_result.b4||!r4q_result.b5){r4q_result.error=9;goto end;}
 if(!r4q_result.error)r4q_result.stage=10;
end:
 r4_result.ownership=0;snapshot();for(;;){(void)now_us(NULL);HAL_Delay(1);}
}
