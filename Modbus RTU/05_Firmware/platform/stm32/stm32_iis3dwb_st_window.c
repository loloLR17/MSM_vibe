/* M015-U: transport/preclose port of qualified T platform.c; clock/GPIO setup
 * remains owned by main.c. No FRAM, SD or UART operations here. */
#include "platform.h"
#if defined(TR2_STM32_FIFO_SPI_MHZ) && TR2_STM32_FIFO_SPI_MHZ==8
#define TR2_ST_FIFO_PRESCALER SPI_BAUDRATEPRESCALER_2
#else
#define TR2_ST_FIFO_PRESCALER SPI_BAUDRATEPRESCALER_4
#endif
#include "stm32_fram_storage.h"
#include <string.h>
_Static_assert(TRACE_RECORDS==1024U && TRACE_BYTES==65536U && PRECLOSE_RECORDS==2048U,"Budget policy mismatch");
static SPI_HandleTypeDef *sensor_spi;
#define spi (*sensor_spi)
#if BENCH_DUPLEX
_Alignas(4) static uint8_t duplex_tx[897],duplex_rx[897];
#endif
uint8_t bench_command_rx[TRACE_RECORDS];
volatile uint32_t bench_transfer_method=BENCH_DUPLEX;
BenchTrace bench_trace[TRACE_RECORDS];
uint8_t bench_payload[TRACE_BYTES];
volatile uint32_t bench_trace_count,bench_payload_count,bench_stop;
volatile uint32_t bench_command,bench_ready,bench_uid[3],bench_core_hz;
BenchPreclose bench_preclose[PRECLOSE_RECORDS];
volatile uint32_t bench_preclose_count,bench_preclose_overflow;
static uint32_t active_trace,active_phase,active_begin,active_length;
static uintptr_t active_data;
/* Passive snapshot at HAL close entry; no hook inside the RX service loop. */
void bench_spi_preclose(SPI_HandleTypeDef *handle)
{
  if(handle!=&spi)return;
  if(bench_preclose_count>=PRECLOSE_RECORDS){bench_preclose_overflow=1;return;}
  BenchPreclose *r=&bench_preclose[bench_preclose_count++];
  r->sr=handle->Instance->SR;r->end_cycles=DWT->CYCCNT;
  r->rx_size=handle->RxXferSize;r->rx_remaining=handle->RxXferCount;
  r->tx_remaining=handle->TxXferCount;r->error_before_close=handle->ErrorCode;
  r->trace_index=active_trace;r->phase=active_phase;r->begin_cycles=active_begin;
  r->tick=HAL_GetTick();r->requested=active_length;r->state=handle->State;
  r->cr1=handle->Instance->CR1;r->cr2=handle->Instance->CR2;
  r->cfg1=handle->Instance->CFG1;r->cfg2=handle->Instance->CFG2;
  r->rx_offset=bench_rx_offset(active_data,(uintptr_t)handle->pRxBuffPtr,
                              active_length,&r->offset_valid);
  r->cs=(GPIOC->ODR&GPIO_PIN_9)?1:0;
}
volatile uint32_t bench_cleanup_mode,bench_log_refused;
/* Aggregate for repetitive level reads; raw observations remain reconstructible. */
volatile uint32_t bench_poll_aggregate[6]; /* count,first_tick,last_tick,min,max,flags_OR */
static void cleanup_begin(void){bench_cleanup_mode=1;}
static uint32_t stop(void){return bench_stop;}
static int32_t transfer(void *handle,uint8_t reg,uint8_t *data,uint16_t len,unsigned write)
{
  if(handle!=&spi || !data || !len || len>896 || reg>0x7F)
    return -1;
  uint32_t refusal=bench_log_refusal(bench_trace_count,bench_payload_count,
                                    bench_preclose_count,len,bench_cleanup_mode);
  if(refusal){bench_log_refused|=refusal;return -1;}
  if(bench_preclose_overflow)return -1;
  BenchTrace *t=&bench_trace[bench_trace_count++];
  t->tick=HAL_GetTick();t->reg=reg;t->len=len;t->write=write;
  t->offset=bench_payload_count;bench_payload_count+=len;
#if !BENCH_DUPLEX
  uint8_t command=write?reg:(uint8_t)(reg|0x80);
#endif
  /* ST callbacks: command TX then payload TX/RX, CS low across both calls. */
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
  HAL_StatusTypeDef s;
#if BENCH_DUPLEX
  uint16_t size=bench_build_frame(reg,data,len,write,duplex_tx,sizeof(duplex_tx));
  memset(duplex_rx,0,size);
  active_trace=bench_trace_count-1;active_phase=2;active_length=size;
  active_data=(uintptr_t)duplex_rx;active_begin=DWT->CYCCNT;
  s=HAL_SPI_TransmitReceive(&spi,duplex_tx,duplex_rx,size,20);
  bench_command_rx[active_trace]=duplex_rx[0];
  if(!write)memcpy(data,duplex_rx+1,len);
#else
  active_trace=bench_trace_count-1;active_phase=1;active_length=1;
  active_data=(uintptr_t)&command;active_begin=DWT->CYCCNT;
  s=HAL_SPI_Transmit(&spi,&command,1,20);
  if(s==HAL_OK) {
    active_phase=write?3:2;active_length=len;active_data=(uintptr_t)data;
    active_begin=DWT->CYCCNT;
    if(write)s=HAL_SPI_Transmit(&spi,data,len,20);
    else {memset(data,0,len);s=HAL_SPI_Receive(&spi,data,len,20);}
  }
#endif
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
  t->status=s;t->error=HAL_SPI_GetError(&spi);
  t->rx_left=spi.RxXferCount;t->tx_left=spi.TxXferCount;
  memcpy(bench_payload+t->offset,data,len);
  t->valid=s==HAL_OK && !t->error && !t->rx_left && !t->tx_left && !bench_preclose_overflow;
  if(!write || BENCH_DUPLEX) {
    const BenchPreclose *r=bench_preclose_count?&bench_preclose[bench_preclose_count-1]:0;
    t->valid=t->valid && bench_rx_complete(r,active_trace,BENCH_DUPLEX?len+1U:len) && !(r->sr&SPI_SR_OVR);
  }
  if(t->valid && !write && reg==IIS3DWB_FIFO_STATUS1 && len==2) {
    uint32_t level=(data[0]|((uint32_t)data[1]<<8))&0x3FFU;
    if(!bench_poll_aggregate[0]){bench_poll_aggregate[1]=t->tick;bench_poll_aggregate[3]=level;}
    bench_poll_aggregate[0]++;bench_poll_aggregate[2]=t->tick;
    if(level<bench_poll_aggregate[3])bench_poll_aggregate[3]=level;
    if(level>bench_poll_aggregate[4])bench_poll_aggregate[4]=level;
    bench_poll_aggregate[5]|=data[1];
  }
  return t->valid?0:-1;
}
int32_t platform_read(void *h,uint8_t r,uint8_t *d,uint16_t n){return transfer(h,r,d,n,0);}
int32_t platform_write(void *h,uint8_t r,const uint8_t *d,uint16_t n){return transfer(h,r,(uint8_t *)d,n,1);}

#include "port.h"
extern BenchResult bench_result;
#include "tr2_fifo_admission.h"
volatile uint32_t tr2_fifo_state, tr2_fifo_fault;
volatile uint32_t tr2_fifo_fram_status;
uint8_t tr2_fifo_fram_backup[262144];
static Stm32FramStorage protected_fram;
static int preserve_fram(SPI_HandleTypeDef *fram)
{
 uint8_t verify[512];
 if(stm32_fram_storage_init(&protected_fram,fram,GPIOD,GPIO_PIN_14,100)!=TR2_OK)return -1;
 if(stm32_fram_storage_read(&protected_fram,0,tr2_fifo_fram_backup,sizeof(tr2_fifo_fram_backup))!=TR2_OK)return -1;
 for(uint32_t i=0;i<sizeof(tr2_fifo_fram_backup);i+=sizeof(verify)){
  if(stm32_fram_storage_read(&protected_fram,i,verify,sizeof(verify))!=TR2_OK ||
     memcmp(verify,tr2_fifo_fram_backup+i,sizeof(verify)))return -1;
 }
 return 0;
}
static int verify_fram(void)
{
 uint8_t verify[512];
 for(uint32_t i=0;i<sizeof(tr2_fifo_fram_backup);i+=sizeof(verify)){
  if(stm32_fram_storage_read(&protected_fram,i,verify,sizeof(verify))!=TR2_OK ||
     memcmp(verify,tr2_fifo_fram_backup+i,sizeof(verify)))return -1;
 }
 return 0;
}
int platform_init(stmdev_ctx_t *ctx, BenchClock *clock)
{
  if(!sensor_spi || sensor_spi->Instance!=SPI3 ||
     sensor_spi->State!=HAL_SPI_STATE_READY ||
     HAL_RCC_GetHCLKFreq()!=160000000U ||
     HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3)!=16000000U ||
     sensor_spi->Init.BaudRatePrescaler!=TR2_ST_FIFO_PRESCALER ||
     sensor_spi->Init.CLKPolarity!=SPI_POLARITY_HIGH ||
     sensor_spi->Init.CLKPhase!=SPI_PHASE_2EDGE ||
     !(GPIOC->ODR&GPIO_PIN_9) ||
     HAL_GetUIDw0()!=0x004A003D || HAL_GetUIDw1()!=0x38425018 ||
     HAL_GetUIDw2()!=0x20333157)return -1;
  CoreDebug->DEMCR|=CoreDebug_DEMCR_TRCENA_Msk;
  if(DWT->CTRL&DWT_CTRL_NOCYCCNT_Msk)return -1;
  DWT->CYCCNT=0;DWT->CTRL|=DWT_CTRL_CYCCNTENA_Msk;
  *ctx=(stmdev_ctx_t){.write_reg=platform_write,.read_reg=platform_read,
    .mdelay=HAL_Delay,.handle=sensor_spi};
  *clock=(BenchClock){HAL_GetTick,HAL_Delay,stop,cleanup_begin};
  bench_uid[0]=HAL_GetUIDw0();bench_uid[1]=HAL_GetUIDw1();bench_uid[2]=HAL_GetUIDw2();
  bench_core_hz=HAL_RCC_GetHCLKFreq();return 0;
}
__attribute__((noreturn)) void stm32_iis3dwb_st_window_run(SPI_HandleTypeDef *handle,SPI_HandleTypeDef *fram)
{
  sensor_spi=handle;
  tr2_fifo_state=TR2_FIFO_INVALID;
  if(HAL_GetUIDw0()!=0x004A003D || HAL_GetUIDw1()!=0x38425018 || HAL_GetUIDw2()!=0x20333157){tr2_fifo_fault=BENCH_CONFIG;for(;;)__WFI();}
  tr2_fifo_fram_status=preserve_fram(fram)?2:1;
  if(tr2_fifo_fram_status!=1){tr2_fifo_fault=BENCH_IO;for(;;)__WFI();}
  official_prepare();
  if(bench_result.result){tr2_fifo_fault=bench_result.result;for(;;)__WFI();}
  bench_ready=1;tr2_fifo_state=TR2_FIFO_ARMED;
  while(!bench_command)__WFI();
  bench_ready=0;
  if(bench_command!=0x4D31354B){tr2_fifo_fault=BENCH_STOP;tr2_fifo_state=TR2_FIFO_INVALID;for(;;)__WFI();}
  bench_command=0;tr2_fifo_state=TR2_FIFO_CAPTURING;
  official_run();
  tr2_fifo_fram_status=verify_fram()?2:3;
  tr2_fifo_fault=bench_result.result?bench_result.result:bench_result.cleanup_result;
  tr2_fifo_state=tr2_fifo_fram_status==3 && tr2_fifo_admit(&bench_result,bench_log_refused,bench_preclose_overflow)?TR2_FIFO_READY:TR2_FIFO_INVALID;
  /* Data are debugger-visible only. Never publish diagnostic READY as B1 READY. */
  for(;;)__WFI();
}

int stm32_st_transport_attach(SPI_HandleTypeDef *h,stmdev_ctx_t *ctx,BenchClock *clock)
{sensor_spi=h;return platform_init(ctx,clock);}
void stm32_st_transport_next_window(void)
{
 /* Only called after every transaction of the completed prefix passed.
  * Sticky errors are never cleared here. No sensor reset or parser reset. */
 bench_trace_count=bench_payload_count=bench_preclose_count=0;
}
