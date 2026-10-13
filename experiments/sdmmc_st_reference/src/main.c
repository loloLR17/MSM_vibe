/* Adapted ST SD_ReadWrite_DMALinkedList application.
 * Copyright (c) 2021 STMicroelectronics; vendor/LICENSE.examples.md.
 * Local adaptations: SDMMC2, single noncircular node, bounded RAM trigger,
 * no erase/format, no automatic write, exact byte oracle and telemetry.
 */
#include "stm32u5xx_hal.h"
#include <stdint.h>
#include <string.h>
#define BYTES 4096U
#define BLOCKS (BYTES / 512U)
#define ROUNDS 3U
#define TIMEOUT 2000U
#define TRIGGER 0x4D313642U
SD_HandleTypeDef hsd2;
SDMMC_DMALinkNodeTypeDef node;
SDMMC_DMALinkedListTypeDef list;
_Alignas(32) uint8_t tx[BYTES], rx[BYTES];
volatile uint32_t bench_stage, bench_trigger, bench_error, bench_phase;
volatile uint32_t bench_uid[3], bench_cid[4], bench_blocks, bench_clkcr;
volatile uint32_t bench_kernel_hz, bench_clock_hz, bench_bus_width;
volatile uint32_t bench_rounds, bench_mismatch=UINT32_MAX, bench_elapsed_ms[ROUNDS];
volatile uint32_t bench_write_ms[ROUNDS], bench_read_ms[ROUNDS];
volatile uint32_t bench_last_r1, bench_cmd13_error, bench_hal_return;
volatile uint32_t bench_done, bench_dma_error;
volatile uint32_t bench_refusal;
/* RAM CID interlock must be set to the observed card before triggering.
 * It is additional to compiled range authorization, not permission itself. */
volatile uint32_t bench_expected_cid[4];
void SystemClock_Config(void);
void Error_Handler(void) { bench_error=hsd2.ErrorCode; bench_stage=99; while(1) {} }
void SysTick_Handler(void) { HAL_IncTick(); }
void SDMMC2_IRQHandler(void) { HAL_SD_IRQHandler(&hsd2); }
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *h) { if(h==&hsd2) bench_done=1; }
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *h) { if(h==&hsd2) bench_done=1; }
void HAL_SD_ErrorCallback(SD_HandleTypeDef *h) { if(h==&hsd2) { bench_dma_error=h->ErrorCode; bench_done=2; } }
void HAL_MspInit(void) { __HAL_RCC_PWR_CLK_ENABLE(); }
void HAL_SD_MspInit(SD_HandleTypeDef *h) {
  if(h->Instance!=SDMMC2) Error_Handler();
  RCC_PeriphCLKInitTypeDef c={0};
  c.PeriphClockSelection=RCC_PERIPHCLK_SDMMC|RCC_PERIPHCLK_CLK48;
  c.IclkClockSelection=RCC_CLK48CLKSOURCE_HSI48;
  c.SdmmcClockSelection=RCC_SDMMCCLKSOURCE_CLK48;
  if(HAL_RCCEx_PeriphCLKConfig(&c)!=HAL_OK) Error_Handler();
  __HAL_RCC_SDMMC2_CLK_ENABLE(); __HAL_RCC_GPIOB_CLK_ENABLE(); __HAL_RCC_GPIOD_CLK_ENABLE();
  GPIO_InitTypeDef g={0};
  g.Mode=GPIO_MODE_AF_PP; g.Pull=GPIO_NOPULL; g.Speed=GPIO_SPEED_FREQ_HIGH;
  g.Pin=GPIO_PIN_6|GPIO_PIN_7; g.Alternate=GPIO_AF11_SDMMC2; HAL_GPIO_Init(GPIOD,&g);
  g.Pin=GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_3|GPIO_PIN_4; g.Alternate=GPIO_AF12_SDMMC2; HAL_GPIO_Init(GPIOB,&g);
  HAL_NVIC_SetPriority(SDMMC2_IRQn,0,0); HAL_NVIC_EnableIRQ(SDMMC2_IRQn);
}
static int ready(void) {
  uint32_t start=HAL_GetTick();
  do {
    bench_cmd13_error=SDMMC_CmdSendStatus(hsd2.Instance,hsd2.SdCard.RelCardAdd<<16);
    if(bench_cmd13_error) { hsd2.ErrorCode|=bench_cmd13_error; return 0; }
    bench_last_r1=SDMMC_GetResponse(hsd2.Instance,SDMMC_RESP1);
    if(((bench_last_r1>>9)&15U)==4U && (bench_last_r1&0x100U)) return 1;
  } while((uint32_t)(HAL_GetTick()-start)<TIMEOUT);
  return 0;
}
static int transfer(int writing) {
  bench_done=0; bench_dma_error=0;
  memset(&node,0,sizeof node); memset(&list,0,sizeof list);
  SD_DMALinkNodeConfTypeDef config={0};
  config.BufferAddress=(uint32_t)(writing?tx:rx); config.BufferSize=BYTES;
  if(HAL_SDEx_DMALinkedList_BuildNode(&node,&config)!=HAL_OK ||
     HAL_SDEx_DMALinkedList_InsertNode(&list,NULL,&node)!=HAL_OK) return 0;
  bench_hal_return=writing?
    HAL_SDEx_DMALinkedList_WriteBlocks(&hsd2,&list,BENCH_TEST_LBA,BLOCKS):
    HAL_SDEx_DMALinkedList_ReadBlocks(&hsd2,&list,BENCH_TEST_LBA,BLOCKS);
  if(bench_hal_return!=HAL_OK) return 0;
  uint32_t start=HAL_GetTick();
  while(!bench_done && (uint32_t)(HAL_GetTick()-start)<TIMEOUT) {}
  return bench_done==1 && !bench_dma_error && ready();
}
static int run(void) {
  if(!BENCH_WRITE_ENABLE || BENCH_TEST_LBA==0 ||
     BENCH_TEST_LBA>=bench_blocks || BLOCKS>bench_blocks-BENCH_TEST_LBA ||
     bench_bus_width!=SDMMC_BUS_WIDE_4B) { bench_refusal=1; return 0; }
  for(unsigned i=0;i<4;i++) if(bench_expected_cid[i]!=hsd2.CID[i]) { bench_refusal=2; return 0; }
  for(unsigned round=0;round<ROUNDS;round++) {
    for(unsigned i=0;i<BYTES;i++) tx[i]=(uint8_t)(i*37U+(i>>8)+round*71U);
    memset(rx,0xA5,sizeof rx);
    uint32_t begin=HAL_GetTick(); bench_phase=3;
    if(!ready()) return 0;
    bench_phase=4; uint32_t t=HAL_GetTick();
    if(!transfer(1)) return 0;
    bench_write_ms[round]=HAL_GetTick()-t;
    bench_phase=5; t=HAL_GetTick();
    if(!transfer(0)) return 0;
    bench_read_ms[round]=HAL_GetTick()-t;
    bench_phase=6;
    for(unsigned i=0;i<BYTES;i++) if(rx[i]!=tx[i]) { bench_mismatch=i; return 0; }
    bench_elapsed_ms[round]=HAL_GetTick()-begin;
    bench_rounds=round+1;
  }
  return 1;
}
int main(void) {
  bench_stage=1;
  for(unsigned i=0;i<3;i++) bench_uid[i]=*(volatile uint32_t *)(0x0BFA0700U+i*4U);
  /* Only the second board previously identified in M015-L may initialize SD. */
  if(bench_uid[0]!=0x001B0046U || bench_uid[1]!=0x38425018U || bench_uid[2]!=0x20333157U) Error_Handler();
  HAL_Init(); HAL_PWREx_DisableUCPDDeadBattery();
  if(HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY)!=HAL_OK) Error_Handler();
  SystemClock_Config();
  hsd2.Instance=SDMMC2;
  hsd2.Init.ClockEdge=SDMMC_CLOCK_EDGE_FALLING;
  hsd2.Init.ClockPowerSave=SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd2.Init.BusWide=SDMMC_BUS_WIDE_4B;
  hsd2.Init.HardwareFlowControl=SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd2.Init.ClockDiv=1;
  bench_phase=1;
  if(HAL_SD_Init(&hsd2)!=HAL_OK) Error_Handler();
  bench_kernel_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDMMC);
  bench_clkcr=SDMMC2->CLKCR; bench_bus_width=bench_clkcr&SDMMC_CLKCR_WIDBUS;
  bench_clock_hz=bench_kernel_hz/(2U*(bench_clkcr&SDMMC_CLKCR_CLKDIV));
  bench_blocks=hsd2.SdCard.LogBlockNbr;
  for(unsigned i=0;i<4;i++) bench_cid[i]=hsd2.CID[i];
  bench_phase=2;
  if(!ready() || bench_bus_width!=SDMMC_BUS_WIDE_4B) Error_Handler();
  bench_stage=2;
  while(bench_trigger!=TRIGGER) {}
  bench_trigger=0; bench_stage=3;
  if(!run()) Error_Handler();
  bench_stage=4;
  while(1) {}
}
