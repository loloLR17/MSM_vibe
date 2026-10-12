#include "platform.h"
#include <string.h>
static SPI_HandleTypeDef spi;
BenchTrace bench_trace[TRACE_RECORDS];
uint8_t bench_payload[TRACE_BYTES];
volatile uint32_t bench_trace_count,bench_payload_count,bench_stop;
volatile uint32_t bench_command,bench_ready,bench_uid[3],bench_core_hz;
static uint32_t stop(void){return bench_stop;}
static int32_t transfer(void *handle,uint8_t reg,uint8_t *data,uint16_t len,unsigned write)
{
  if(handle!=&spi || !data || !len || len>896 || reg>0x7F ||
     bench_trace_count>=TRACE_RECORDS || bench_payload_count+len>TRACE_BYTES)
    return -1;
  BenchTrace *t=&bench_trace[bench_trace_count++];
  t->tick=HAL_GetTick();t->reg=reg;t->len=len;t->write=write;
  t->offset=bench_payload_count;bench_payload_count+=len;
  uint8_t command=write?reg:(uint8_t)(reg|0x80);
  /* ST callbacks: command TX then payload TX/RX, CS low across both calls. */
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
  HAL_StatusTypeDef s=HAL_SPI_Transmit(&spi,&command,1,20);
  if(s==HAL_OK) {
    if(write)s=HAL_SPI_Transmit(&spi,data,len,20);
    else {memset(data,0,len);s=HAL_SPI_Receive(&spi,data,len,20);}
  }
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
  t->status=s;t->error=HAL_SPI_GetError(&spi);
  t->rx_left=spi.RxXferCount;t->tx_left=spi.TxXferCount;
  memcpy(bench_payload+t->offset,data,len);
  t->valid=s==HAL_OK && !t->error && !t->rx_left && !t->tx_left;
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
  spi.Init.CLKPhase=SPI_PHASE_1EDGE;spi.Init.NSS=SPI_NSS_SOFT;
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
  *ctx=(stmdev_ctx_t){.write_reg=platform_write,.read_reg=platform_read,
    .mdelay=HAL_Delay,.handle=&spi};
  *clock=(BenchClock){HAL_GetTick,HAL_Delay,stop};
  bench_uid[0]=HAL_GetUIDw0();bench_uid[1]=HAL_GetUIDw1();bench_uid[2]=HAL_GetUIDw2();
  bench_core_hz=HAL_RCC_GetHCLKFreq();return 0;
}
