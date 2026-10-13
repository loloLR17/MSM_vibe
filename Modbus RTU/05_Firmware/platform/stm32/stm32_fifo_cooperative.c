#include "stm32_fifo_cooperative.h"
#include "fifo_cooperative.h"
#include "platform.h"
#include "stm32_fram_storage.h"
#include "stm32_irq_observer.h"
#include <string.h>
extern int stm32_st_transport_attach(SPI_HandleTypeDef *,stmdev_ctx_t *,BenchClock *);
extern void stm32_st_transport_next_window(void);
extern volatile uint32_t bench_log_refused,bench_preclose_overflow,bench_cleanup_mode;
extern volatile uint32_t tr2_fifo_state,tr2_fifo_fault;
BenchResult bench_result;
FifoCooperative cooperative;
uint8_t official_words[262144];
/* Every full-window RAM sample retained. Full SPI journal retained for last4096;
 * independent per-burst CRCs and byte comparison retain earlier integrity. */
typedef struct {uint32_t first_word,words,crc_payload,begin_cycles,end_cycles,rx_before,rx_after;} BurstEvidence;
BurstEvidence cooperative_bursts[320];
volatile uint32_t cooperative_service_max_gap_cycles,cooperative_service_last_cycles;
volatile uint32_t cooperative_modbus_max_cycles,cooperative_modbus_capture_max_cycles;
volatile uint32_t cooperative_command_io_denied,cooperative_command_snapshot_failures;
volatile uint32_t cooperative_burst_count,cooperative_start_tick,cooperative_end_tick;
volatile uint32_t cooperative_arm,cooperative_trigger_rx,cooperative_max_cycles;
volatile uint32_t cooperative_fram_crc_before,cooperative_fram_crc_after,cooperative_fram_state;
volatile uint32_t cooperative_log_first_word,cooperative_prefix_xyz[8],cooperative_prefix_tick[8],cooperative_prefix_count;
typedef struct {uint32_t tick,state,xyz,address,count,rx_bytes,tx_count,tx_complete_tick;} ModbusEvidence;
ModbusEvidence cooperative_modbus[8];volatile uint32_t cooperative_modbus_count;
static uint32_t verify_offset,verify_crc;
static Stm32FramStorage fram;
static uint32_t crc(uint32_t c,const uint8_t *data,uint32_t n)
{for(uint32_t i=0;i<n;i++){c^=data[i];for(unsigned bit=0;bit<8;bit++)c=(c>>1)^((c&1)?0xEDB88320U:0);}return c;}
void stm32_fifo_cooperative_prepare(SPI_HandleTypeDef *spi,SPI_HandleTypeDef *fram_spi)
{
 stmdev_ctx_t ctx;BenchClock clock;uint8_t verify[512];
 tr2_fifo_state=0;bench_ready=0;
 if(stm32_st_transport_attach(spi,&ctx,&clock) ||
    stm32_fram_storage_init(&fram,fram_spi,GPIOD,GPIO_PIN_14,100)!=TR2_OK ||
    stm32_fram_storage_read(&fram,0,official_words,sizeof(official_words))!=TR2_OK){tr2_fifo_fault=BENCH_IO;return;}
 for(uint32_t i=0;i<sizeof(official_words);i+=sizeof(verify)){
  if(stm32_fram_storage_read(&fram,i,verify,sizeof(verify))!=TR2_OK || memcmp(verify,official_words+i,sizeof(verify))){tr2_fifo_fault=BENCH_IO;return;}
 }
 cooperative_fram_crc_before=crc(UINT32_MAX,official_words,sizeof(official_words))^UINT32_MAX;
 cooperative_fram_state=1;
 if(fifo_cooperative_init(&cooperative,&ctx,&clock,official_words,36864,32768)){tr2_fifo_fault=BENCH_CONFIG;return;}
}
void stm32_fifo_cooperative_poll(void)
{
 if(tr2_fifo_fault)return;
 uint32_t begin=DWT->CYCCNT,before=cooperative.window.received;
 uint32_t rx_before=tr2_irq_observer.rx_callback;
 if(cooperative.state==COOP_ARMED){
  bench_ready=1;tr2_fifo_state=1;
  if(bench_command==0x4D313556){bench_command=0;cooperative_arm=1;cooperative_trigger_rx=tr2_irq_observer.rx_callback;}
  if(cooperative_arm && tr2_irq_observer.rx_callback!=cooperative_trigger_rx){
   cooperative_start_tick=HAL_GetTick();bench_ready=0;tr2_fifo_state=2;
   cooperative_service_max_gap_cycles=cooperative_service_last_cycles=0;
   cooperative_modbus_capture_max_cycles=0;
   if(fifo_cooperative_start(&cooperative))tr2_fifo_fault=BENCH_STOP;
  }
 }
 for(uint32_t i=0;i<cooperative_modbus_count;i++){
  ModbusEvidence *e=&cooperative_modbus[i];
  if(e->tx_count && !e->tx_complete_tick && tr2_irq_observer.tx_callback>=e->tx_count)e->tx_complete_tick=HAL_GetTick();
 }
 if((cooperative.state==COOP_CONFIGURING && cooperative.phase>=8) ||
    cooperative.state==COOP_CAPTURING || cooperative.state==COOP_CLEANUP){
  if(cooperative_service_last_cycles){
   uint32_t gap=begin-cooperative_service_last_cycles;
   if(gap>cooperative_service_max_gap_cycles)cooperative_service_max_gap_cycles=gap;
  }
  cooperative_service_last_cycles=begin;
 }
 uint32_t trace_begin=bench_trace_count;
 fifo_cooperative_poll(&cooperative);
 if(cooperative.window.received>before){
  for(uint32_t i=trace_begin;i<bench_trace_count;i++){
   BenchTrace *t=&bench_trace[i];if(t->reg!=120 || t->write)continue;
   if(cooperative_burst_count>=320 || memcmp(bench_payload+t->offset,official_words+before*7,t->len)){cooperative.result=BENCH_CAPACITY;cooperative.state=COOP_CLEANUP;cooperative.phase=0;bench_cleanup_mode=1;break;}
   uint32_t end=DWT->CYCCNT;
   cooperative_bursts[cooperative_burst_count++]=(BurstEvidence){before,t->len/7,crc(UINT32_MAX,bench_payload+t->offset,t->len)^UINT32_MAX,begin,end,rx_before,tr2_irq_observer.rx_callback};
  }
  if(cooperative.window.xyz%4096==0 && cooperative_prefix_count<8){
   uint32_t k=cooperative_prefix_count++;cooperative_prefix_xyz[k]=cooperative.window.xyz;cooperative_prefix_tick[k]=HAL_GetTick();
   if(cooperative.window.xyz<cooperative.window.target_xyz && cooperative.state==COOP_CAPTURING){
    if(bench_log_refused || bench_preclose_overflow){cooperative.result=BENCH_CAPACITY;cooperative.state=COOP_CLEANUP;cooperative.phase=0;bench_cleanup_mode=1;}
    else {cooperative_log_first_word=cooperative.window.received;stm32_st_transport_next_window();}
   }
  }
 }
 uint32_t cycles=DWT->CYCCNT-begin;if(cycles>cooperative_max_cycles)cooperative_max_cycles=cycles;
 if((cooperative.state==COOP_READY || cooperative.state==COOP_INVALID) && !cooperative_end_tick){
  cooperative_end_tick=HAL_GetTick();bench_result.stage=6;bench_result.result=cooperative.result;
  bench_result.cleanup_result=cooperative.cleanup_result;bench_result.count=cooperative.window.received;
  bench_result.xyz_count=cooperative.window.xyz;bench_result.timestamp_count=cooperative.window.timestamps;
  bench_result.who=cooperative.who;bench_result.flags=cooperative.window.flags_or;
  memcpy(bench_result.cleanup_controls,cooperative.cleanup,3);
  tr2_fifo_state=cooperative.state==COOP_READY?4:0; /* verification pending */
  /* Sensor is already cleaned up; preserve service availability on invalid data. */
  verify_offset=0;verify_crc=UINT32_MAX;
 }
 if(cooperative_end_tick && cooperative_fram_state==1){
  uint8_t verify[512];
  if(stm32_fram_storage_read(&fram,verify_offset,verify,sizeof(verify))!=TR2_OK){cooperative_fram_state=2;tr2_fifo_state=0;return;}
  verify_crc=crc(verify_crc,verify,sizeof(verify));verify_offset+=sizeof(verify);
  if(verify_offset==262144){cooperative_fram_crc_after=verify_crc^UINT32_MAX;cooperative_fram_state=cooperative_fram_crc_after==cooperative_fram_crc_before?3:2;tr2_fifo_state=cooperative_fram_state==3 && cooperative.state==COOP_READY?3:0;}
 }
}

void stm32_fifo_cooperative_observe(const ModbusRtuAduView *request)
{
 if(!request || request->pdu_length!=5 || request->pdu[0]!=3 || cooperative_modbus_count>=8)return;
 const uint8_t *pdu=request->pdu;
 cooperative_modbus[cooperative_modbus_count++]=(ModbusEvidence){HAL_GetTick(),cooperative.state,cooperative.window.xyz,((uint32_t)pdu[1]<<8)|pdu[2],((uint32_t)pdu[3]<<8)|pdu[4],tr2_irq_observer.rx_callback,tr2_irq_observer.tx_started,0};
}

/* Single main-loop owner: no persistent command I/O while a capture can start,
 * while the sensor is producing, or until its cleanup has been verified. */
bool stm32_fifo_cooperative_command_io_available(void *context)
{
 (void)context;
 return fifo_cooperative_command_io_available(&cooperative,
  cooperative_arm || bench_command==0x4D313556);
}

void stm32_fifo_cooperative_observe_service(uint32_t cycles,uint32_t denied,uint32_t failures)
{
 if(cycles>cooperative_modbus_max_cycles)cooperative_modbus_max_cycles=cycles;
 if(!stm32_fifo_cooperative_command_io_available(NULL) && cycles>cooperative_modbus_capture_max_cycles)
  cooperative_modbus_capture_max_cycles=cycles;
 cooperative_command_io_denied=denied;cooperative_command_snapshot_failures=failures;
}
