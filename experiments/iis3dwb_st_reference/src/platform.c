#include "platform.h"
#include <string.h>
_Static_assert(TRACE_RECORDS==1024U && TRACE_BYTES==65536U && PRECLOSE_RECORDS==2048U,"Budget policy mismatch");
static SPI_HandleTypeDef spi;
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
int platform_init(stmdev_ctx_t *ctx,BenchClock *clock)
{
  GPIO_InitTypeDef g={0};RCC_OscInitTypeDef osc={0};RCC_ClkInitTypeDef clk={0};
  __HAL_RCC_PWR_CLK_ENABLE();
  if(HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY)!=HAL_OK)return -1;
  osc.OscillatorType=RCC_OSCILLATORTYPE_MSI;osc.MSIState=RCC_MSI_ON;
  osc.MSIClockRange=RCC_MSIRANGE_2;osc.MSICalibrationValue=RCC_MSICALIBRATION_DEFAULT;
  osc.PLL.PLLState=RCC_PLL_NONE;
  if(HAL_RCC_OscConfig(&osc)!=HAL_OK)return -1;
  clk.ClockType=RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|
    RCC_CLOCKTYPE_PCLK2|RCC_CLOCKTYPE_PCLK3;
  clk.SYSCLKSource=RCC_SYSCLKSOURCE_MSI;clk.AHBCLKDivider=RCC_SYSCLK_DIV1;
  clk.APB1CLKDivider=RCC_HCLK_DIV1;clk.APB2CLKDivider=RCC_HCLK_DIV1;
  clk.APB3CLKDivider=RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&clk,FLASH_LATENCY_0)!=HAL_OK)return -1;
  RCC_PeriphCLKInitTypeDef peripheral={0};
  peripheral.PeriphClockSelection=RCC_PERIPHCLK_SPI3;
  peripheral.Spi3ClockSelection=RCC_SPI3CLKSOURCE_PCLK3;
  if(HAL_RCCEx_PeriphCLKConfig(&peripheral)!=HAL_OK ||
     HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3)!=16000000U)return -1;
  __HAL_RCC_GPIOC_CLK_ENABLE();__HAL_RCC_SPI3_CLK_ENABLE();
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
  g.Pin=GPIO_PIN_9;g.Mode=GPIO_MODE_OUTPUT_PP;g.Pull=GPIO_NOPULL;
  g.Speed=GPIO_SPEED_FREQ_LOW;HAL_GPIO_Init(GPIOC,&g);
  g.Pin=GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;g.Mode=GPIO_MODE_AF_PP;
  g.Speed=GPIO_SPEED_FREQ_HIGH;g.Alternate=GPIO_AF6_SPI3;HAL_GPIO_Init(GPIOC,&g);
  spi.Instance=SPI3;spi.Init.Mode=SPI_MODE_MASTER;spi.Init.Direction=SPI_DIRECTION_2LINES;
  spi.Init.DataSize=SPI_DATASIZE_8BIT;spi.Init.CLKPolarity=SPI_POLARITY_LOW;
  spi.Init.CLKPhase=SPI_PHASE_1EDGE;
#if BENCH_SPI_MODE3
  spi.Init.CLKPolarity=SPI_POLARITY_HIGH;spi.Init.CLKPhase=SPI_PHASE_2EDGE;
#endif
  spi.Init.NSS=SPI_NSS_SOFT;
  spi.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_8;
  spi.Init.FirstBit=SPI_FIRSTBIT_MSB;spi.Init.TIMode=SPI_TIMODE_DISABLE;
  spi.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;spi.Init.CRCPolynomial=7;
  spi.Init.NSSPMode=SPI_NSS_PULSE_DISABLE;spi.Init.NSSPolarity=SPI_NSS_POLARITY_LOW;
  spi.Init.FifoThreshold=SPI_FIFO_THRESHOLD_01DATA;
  spi.Init.MasterSSIdleness=SPI_MASTER_SS_IDLENESS_00CYCLE;
  spi.Init.MasterInterDataIdleness=SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  spi.Init.MasterReceiverAutoSusp=SPI_MASTER_RX_AUTOSUSP_DISABLE;
  spi.Init.MasterKeepIOState=SPI_MASTER_KEEP_IO_STATE_ENABLE;spi.Init.IOSwap=SPI_IO_SWAP_DISABLE;
  spi.Init.ReadyMasterManagement=SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  spi.Init.ReadyPolarity=SPI_RDY_POLARITY_HIGH;
  if(HAL_SPI_Init(&spi)!=HAL_OK)return -1;
  CoreDebug->DEMCR|=CoreDebug_DEMCR_TRCENA_Msk;
  if(DWT->CTRL&DWT_CTRL_NOCYCCNT_Msk)return -1;
  DWT->CYCCNT=0;DWT->CTRL|=DWT_CTRL_CYCCNTENA_Msk;
  *ctx=(stmdev_ctx_t){.write_reg=platform_write,.read_reg=platform_read,
    .mdelay=HAL_Delay,.handle=&spi};
  *clock=(BenchClock){HAL_GetTick,HAL_Delay,stop,cleanup_begin};
  bench_uid[0]=HAL_GetUIDw0();bench_uid[1]=HAL_GetUIDw1();bench_uid[2]=HAL_GetUIDw2();
  bench_core_hz=HAL_RCC_GetHCLKFreq();return 0;
}
