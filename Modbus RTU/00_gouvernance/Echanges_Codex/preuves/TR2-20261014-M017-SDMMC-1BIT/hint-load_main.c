/* M016 1-bit LOAD-ONLY: ST SD_ReadWrite_DMALinkedList transport unchanged.
 * ST example provenance/licence: vendor/; no campaign/runtime integration.
 * Fixed single 600-second producer, real bounded SRAM queue, full byte oracle.
 */
#include "stm32u5xx_hal.h"
#include <stdint.h>
#include <string.h>
#define CPU_HZ 160000000U
#define SAMPLE_HZ 26667U
#define RECORD 16U
#define SECONDS 600U
#define TOTAL (SAMPLE_HZ*RECORD*SECONDS)
#define CHUNK 65536U
#define RING (2U*CHUNK)
#define PHYSICAL ((TOTAL+511U)&~511U)
#define MAX_CHUNKS 4096U
#define INIT_KEY 0x4C31494EU
#define RUN_KEY 0x4C314C44U
#define DIAG_KEY 0x4D313744U
#define TIMEOUT 5000U
SD_HandleTypeDef hsd2;
_Alignas(32) static uint8_t ring[RING], rx[CHUNK];
static SDMMC_DMALinkNodeTypeDef nodes[2];
static SDMMC_DMALinkedListTypeDef list;
volatile uint32_t load_stage, load_trigger, load_error, load_phase;
volatile uint32_t load_generated, load_produced, load_consumed, load_backlog_max;
volatile uint32_t load_overflow, load_lost, load_producer_enabled, load_producer_done;
volatile uint32_t load_written, load_written_logical, load_verified, load_verified_logical;
volatile uint32_t load_write_chunks, load_read_chunks, load_mismatches;
volatile uint32_t load_first_mismatch=UINT32_MAX, load_first_io_error, load_first_cmd_error;
volatile uint32_t load_kernel_hz, load_clock_hz, load_clkcr, load_bus_card, load_blocks;
volatile uint32_t load_transfer_phase;
volatile uint32_t load_cid[4], load_uid[3], load_dma_done, load_dma_error, load_hal_return;
volatile uint32_t load_last_r1, load_cmd_error, load_max_producer_gap_us;
volatile uint32_t load_generation_us, load_write_elapsed_us, load_verify_elapsed_us;
volatile uint32_t load_write_latency_us[MAX_CHUNKS];
volatile uint32_t load_dma_latency_us[MAX_CHUNKS], load_ready_latency_us[MAX_CHUNKS];
volatile uint32_t load_last_data_us, load_last_ready_us, load_last_before_us;
volatile uint32_t load_producer_cycles, load_producer_max_cycles;
volatile uint32_t diag_done, diag_error, diag_mismatches, diag_latency_us[16], diag_data_us[16], diag_ready_us[16];
volatile uint32_t load_preerase_error, load_preerase_r1, load_preerase_count;
static uint32_t polling_transport; /* Diagnostic selector only; campaign retains DMA baseline. */
static volatile uint64_t clock_cycles, load_started_cycles;
static volatile uint32_t clock_last;
void SystemClock_Config(void);
void HAL_MspInit(void) { __HAL_RCC_PWR_CLK_ENABLE(); }
static uint8_t pattern(uint32_t offset) {
  return (uint8_t)(offset*37U+(offset>>8)+(offset>>16)+(offset>>24)*113U);
}
static uint64_t cycles_now(void) {
  uint32_t old=__get_PRIMASK(); __disable_irq();
  uint64_t value=clock_cycles+(uint32_t)(DWT->CYCCNT-clock_last);
  if(!old) __enable_irq();
  return value;
}
void SysTick_Handler(void) {
  HAL_IncTick();
  uint32_t now=DWT->CYCCNT, delta=now-clock_last;
  clock_last=now; clock_cycles+=delta;
  if(!load_producer_enabled) return;
  uint32_t cpu_begin=DWT->CYCCNT;
  uint64_t elapsed=clock_cycles-load_started_cycles;
  const uint64_t end=(uint64_t)CPU_HZ*SECONDS;
  if(elapsed>end) elapsed=end;
  uint32_t desired=(uint32_t)((elapsed*SAMPLE_HZ)/CPU_HZ)*RECORD;
  uint32_t due=desired-load_generated;
  load_generated=desired;
  uint32_t used=load_produced-load_consumed, free=RING-used;
  uint32_t take=due<free?due:free;
  uint32_t offset=load_produced;
  for(uint32_t i=0;i<take;i++) ring[(offset+i)&(RING-1U)]=pattern(offset+i);
  __DMB(); load_produced=offset+take;
  used+=take;
  if(used>load_backlog_max) load_backlog_max=used;
  uint32_t gap=delta/(CPU_HZ/1000000U);
  if(gap>load_max_producer_gap_us) load_max_producer_gap_us=gap;
  uint32_t cpu_used=DWT->CYCCNT-cpu_begin;
  load_producer_cycles+=cpu_used;
  if(cpu_used>load_producer_max_cycles) load_producer_max_cycles=cpu_used;
  if(take!=due) {
    load_overflow=1; load_lost+=due-take; load_producer_enabled=0;
  } else if(elapsed==end) {
    load_producer_enabled=0; load_producer_done=1;
    load_generation_us=(uint32_t)((clock_cycles-load_started_cycles)/(CPU_HZ/1000000U));
  }
}
void SDMMC2_IRQHandler(void) { HAL_SD_IRQHandler(&hsd2); }
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *h) { if(h==&hsd2) load_dma_done=1; }
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *h) { if(h==&hsd2) load_dma_done=1; }
void HAL_SD_ErrorCallback(SD_HandleTypeDef *h) { if(h==&hsd2) { load_dma_error=h->ErrorCode; load_dma_done=2; } }
void Error_Handler(void) { load_producer_enabled=0; load_error=90; load_stage=99; while(1) {} }
void HAL_SD_MspInit(SD_HandleTypeDef *h) {
  if(h->Instance!=SDMMC2) Error_Handler();
  RCC_PeriphCLKInitTypeDef c={0};
  c.PeriphClockSelection=RCC_PERIPHCLK_SDMMC;
  c.SdmmcClockSelection=RCC_SDMMCCLKSOURCE_PLL1;
  if(HAL_RCCEx_PeriphCLKConfig(&c)!=HAL_OK) Error_Handler();
  __HAL_RCC_SDMMC2_CLK_ENABLE(); __HAL_RCC_GPIOB_CLK_ENABLE(); __HAL_RCC_GPIOD_CLK_ENABLE();
  GPIO_InitTypeDef g={0};
  /* Release the former GPIO-test outputs explicitly. No card operation here. */
  g.Pin=GPIO_PIN_15|GPIO_PIN_3|GPIO_PIN_4; g.Mode=GPIO_MODE_INPUT; g.Pull=GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB,&g);
  g.Pin=GPIO_PIN_6|GPIO_PIN_7; g.Mode=GPIO_MODE_AF_PP; g.Speed=GPIO_SPEED_FREQ_HIGH;
  g.Alternate=GPIO_AF11_SDMMC2; HAL_GPIO_Init(GPIOD,&g);
  g.Pin=GPIO_PIN_14; g.Alternate=GPIO_AF12_SDMMC2; HAL_GPIO_Init(GPIOB,&g);
  HAL_NVIC_SetPriority(SysTick_IRQn,0,0);
  HAL_NVIC_SetPriority(SDMMC2_IRQn,1,0); HAL_NVIC_EnableIRQ(SDMMC2_IRQn);
}
static int ready(void) {
  uint32_t start=HAL_GetTick();
  do {
    load_cmd_error=SDMMC_CmdSendStatus(hsd2.Instance,hsd2.SdCard.RelCardAdd<<16);
    if(load_cmd_error) { hsd2.ErrorCode|=load_cmd_error; return 0; }
    load_last_r1=SDMMC_GetResponse(hsd2.Instance,SDMMC_RESP1);
    if(((load_last_r1>>9)&15U)==4U && (load_last_r1&0x100U)) return 1;
  } while(HAL_GetTick()-start<TIMEOUT);
  return 0;
}
static int transfer(int writing,uint8_t *buffer,uint32_t lba,uint32_t bytes) {
  if(!bytes || bytes%512U || bytes>CHUNK) return 0;
  memset(nodes,0,sizeof nodes); memset(&list,0,sizeof list);
  /* Keep the official two contiguous nodes; equal halves, word aligned. */
  SD_DMALinkNodeConfTypeDef conf={0};
  for(unsigned i=0;i<2;i++) {
    conf.BufferAddress=(uint32_t)(buffer+i*(bytes/2)); conf.BufferSize=bytes/2;
    if(HAL_SDEx_DMALinkedList_BuildNode(&nodes[i],&conf)!=HAL_OK ||
       HAL_SDEx_DMALinkedList_InsertNode(&list,i?&nodes[0]:NULL,&nodes[i])!=HAL_OK) return 0;
  }
  uint64_t begin=cycles_now();
  load_transfer_phase=1;
  if(!ready()) return 0;
  load_last_before_us=(uint32_t)((cycles_now()-begin)/(CPU_HZ/1000000U));
  /* SD Association v7.10 ACMD23: announce exactly the next CMD25 count.
   * Normal write preparation only; never CMD32/CMD33/CMD38 or media format. */
  if(writing) {
    load_preerase_error=SDMMC_CmdAppCommand(hsd2.Instance,hsd2.SdCard.RelCardAdd<<16);
    load_preerase_r1=SDMMC_GetResponse(hsd2.Instance,SDMMC_RESP1);
    if(!load_preerase_error && !(load_preerase_r1&0x20U)) load_preerase_error=HAL_SD_ERROR_ILLEGAL_CMD;
    if(!load_preerase_error) load_preerase_error=SDMMC_CmdBlockCount(hsd2.Instance,bytes/512U);
    load_preerase_r1=SDMMC_GetResponse(hsd2.Instance,SDMMC_RESP1);
    if(load_preerase_error) { hsd2.ErrorCode|=load_preerase_error; return 0; }
    load_preerase_count++;
  }
  begin=cycles_now();
  if(polling_transport) {
    load_hal_return=writing?HAL_SD_WriteBlocks(&hsd2,buffer,lba,bytes/512U,TIMEOUT):
      HAL_SD_ReadBlocks(&hsd2,buffer,lba,bytes/512U,TIMEOUT);
    load_last_data_us=(uint32_t)((cycles_now()-begin)/(CPU_HZ/1000000U));
    if(load_hal_return!=HAL_OK) return 0;
    begin=cycles_now(); load_transfer_phase=4;
    int ok=ready();
    load_last_ready_us=(uint32_t)((cycles_now()-begin)/(CPU_HZ/1000000U));
    return ok;
  }
  load_transfer_phase=2;
  load_dma_done=0; load_dma_error=0;
  load_hal_return=writing?HAL_SDEx_DMALinkedList_WriteBlocks(&hsd2,&list,lba,bytes/512U):
    HAL_SDEx_DMALinkedList_ReadBlocks(&hsd2,&list,lba,bytes/512U);
  if(load_hal_return!=HAL_OK) return 0;
  load_transfer_phase=3;
  uint32_t start=HAL_GetTick();
  while(!load_dma_done && HAL_GetTick()-start<TIMEOUT) {}
  if(load_dma_done!=1 || load_dma_error) return 0;
  load_last_data_us=(uint32_t)((cycles_now()-begin)/(CPU_HZ/1000000U));
  begin=cycles_now(); load_transfer_phase=4;
  int ok=ready();
  load_last_ready_us=(uint32_t)((cycles_now()-begin)/(CPU_HZ/1000000U));
  return ok;
}
static int campaign(void) {
  uint32_t old=__get_PRIMASK(); __disable_irq();
  load_started_cycles=clock_cycles+(uint32_t)(DWT->CYCCNT-clock_last);
  load_producer_enabled=1;
  if(!old) __enable_irq();
  load_stage=3; load_phase=1;
  while(load_consumed<TOTAL) {
    if(load_overflow) { load_error=3; break; }
    uint32_t available=load_produced-load_consumed;
    if(available<CHUNK && !load_producer_done) continue;
    uint32_t logical=available<CHUNK?available:CHUNK;
    if(!logical) { load_error=4; break; }
    uint32_t bytes=(logical+511U)&~511U;
    uint8_t *src=ring+(load_consumed&(RING-1U));
    if(bytes>logical) memset(src+logical,0xA5,bytes-logical);
    uint64_t begin=cycles_now();
    int ok=transfer(1,src,BENCH_TEST_LBA+load_written/512U,bytes);
    uint32_t us=(uint32_t)((cycles_now()-begin)/(CPU_HZ/1000000U));
    if(load_write_chunks>=MAX_CHUNKS) { load_error=5; break; }
    load_write_latency_us[load_write_chunks]=us;
    load_dma_latency_us[load_write_chunks]=load_last_data_us;
    load_ready_latency_us[load_write_chunks]=load_last_ready_us;
    if(!ok) { load_first_io_error=hsd2.ErrorCode; load_first_cmd_error=load_cmd_error; load_error=1; break; }
    load_written+=bytes; load_written_logical+=logical; load_write_chunks++;
    __DMB(); load_consumed+=logical;
  }
  load_producer_enabled=0;
  load_write_elapsed_us=(uint32_t)((cycles_now()-load_started_cycles)/(CPU_HZ/1000000U));
  if(load_error) return 0;
  if(load_generated!=TOTAL || load_written!=PHYSICAL || load_written_logical!=TOTAL || load_lost) { load_error=6; return 0; }
  /* Integral verification belongs to this same run; no additional write. */
  load_stage=4; load_phase=2;
  uint64_t start=cycles_now();
  while(load_verified<load_written) {
    uint32_t count=load_written-load_verified; if(count>CHUNK) count=CHUNK;
    if(!transfer(0,rx,BENCH_TEST_LBA+load_verified/512U,count)) {
      load_first_io_error=hsd2.ErrorCode; load_first_cmd_error=load_cmd_error; load_error=2; return 0;
    }
    uint32_t offset=load_verified;
    for(uint32_t i=0;i<count;i++) {
      uint32_t pos=offset+i; uint8_t expected=pos<TOTAL?pattern(pos):0xA5;
      if(rx[i]!=expected) { if(load_first_mismatch==UINT32_MAX) load_first_mismatch=pos; load_mismatches++; }
    }
    load_verified+=count; load_read_chunks++;
    load_verified_logical=load_verified<TOTAL?load_verified:TOTAL;
  }
  load_verify_elapsed_us=(uint32_t)((cycles_now()-start)/(CPU_HZ/1000000U));
  if(load_mismatches) { load_error=7; return 0; }
  return 1;
}
/* Short differential: same buffer, bus, card and LBA range; no producer.
 * Eight DMA writes then eight polling writes, each fully read/compared.
 * HAL stays untouched. Only explicit DIAG_KEY can enter, once per reset. */
static void differential(void) {
  for(uint32_t i=0;i<CHUNK;i++) ring[i]=pattern(i);
  for(uint32_t j=0;j<16;j++) {
    polling_transport=j>=8;
    uint32_t lba=503000U+(j%8U)*(CHUNK/512U);
    uint64_t begin=cycles_now();
    if(!transfer(1,ring,lba,CHUNK)) { diag_error=1; break; }
    diag_latency_us[j]=(uint32_t)((cycles_now()-begin)/(CPU_HZ/1000000U));
    diag_data_us[j]=load_last_data_us; diag_ready_us[j]=load_last_ready_us;
    if(!transfer(0,rx,lba,CHUNK)) { diag_error=2; break; }
    for(uint32_t i=0;i<CHUNK;i++) if(rx[i]!=pattern(i)) diag_mismatches++;
    diag_done++;
    if(diag_mismatches) { diag_error=3; break; }
  }
  polling_transport=0;
}
__attribute__((noinline)) void load_terminal(void) { while(1) {} }
int main(void) {
  for(unsigned i=0;i<3;i++) load_uid[i]=*(volatile uint32_t *)(0x0BFA0700U+i*4U);
  if(load_uid[0]!=0x001B0046U || load_uid[1]!=0x38425018U || load_uid[2]!=0x20333157U) Error_Handler();
  HAL_Init(); HAL_PWREx_DisableUCPDDeadBattery();
  if(HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY)!=HAL_OK) Error_Handler();
  SystemClock_Config(); if(SystemCoreClock!=CPU_HZ) Error_Handler();
  CoreDebug->DEMCR|=CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT=0; DWT->CTRL|=DWT_CTRL_CYCCNTENA_Msk;
  hsd2.Instance=SDMMC2; hsd2.Init.ClockEdge=SDMMC_CLOCK_EDGE_RISING;
  hsd2.Init.ClockPowerSave=SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd2.Init.BusWide=SDMMC_BUS_WIDE_1B; hsd2.Init.HardwareFlowControl=SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd2.Init.ClockDiv=8;
  HAL_SD_MspInit(&hsd2); load_stage=1; /* Safe with card absent; wait for human insertion. */
  while(load_trigger!=INIT_KEY) {} load_trigger=0;
  if(HAL_SD_Init(&hsd2)!=HAL_OK) { load_error=8; load_first_io_error=hsd2.ErrorCode; load_stage=99; while(1) {} }
  HAL_SD_CardStatusTypeDef status={0};
  if(HAL_SD_GetCardStatus(&hsd2,&status)!=HAL_OK) Error_Handler();
  load_bus_card=status.DataBusWidth; load_blocks=hsd2.SdCard.LogBlockNbr;
  load_kernel_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDMMC);
  load_clkcr=SDMMC2->CLKCR; load_clock_hz=load_kernel_hz/(2U*(load_clkcr&0x3ffU));
  for(unsigned i=0;i<4;i++) load_cid[i]=hsd2.CID[i];
  if(load_bus_card!=0 || (load_clkcr&SDMMC_CLKCR_WIDBUS)!=0 || load_clock_hz!=10000000U ||
     hsd2.CID[0]!=0x03534453U || hsd2.CID[1]!=0x44333247U || hsd2.CID[2]!=0x858BF092U || hsd2.CID[3]!=0x3C01927AU ||
     BENCH_TEST_LBA>=load_blocks || PHYSICAL/512U>load_blocks-BENCH_TEST_LBA || !ready()) Error_Handler();
  load_stage=2;
  while(load_trigger!=RUN_KEY) {
    if(load_trigger==DIAG_KEY && !diag_done && !diag_error) {
      load_trigger=0; differential();
    }
  } load_trigger=0;
  if(campaign()) load_stage=5; else { load_producer_enabled=0; load_stage=99; }
  load_terminal();
}
