/* R2 NON-PRODUCTION: TR2-002 only, SPI2 SDHC alone. No sensor/FRAM/RTU/SDMMC.
 * Inert before a RAM command. Terminal breakpoint only after CS deselected. */
#include "stm32u5xx_hal.h"
#include "sd_spi.h"
#include "pattern.h"
#include <string.h>
#define COMMAND 0x52325300u
#define START_LBA 2048u
#define RING_SIZE 32768u
#define LATENCIES 4096u
typedef struct {uint32_t kind,write_us,busy_us,backlog,read_us;} Latency;
typedef struct {
 uint32_t version,stage,error,uid[3],hclk,spi_hz,phase,operation,who_unused;
 uint32_t sectors,ocr,cid[4],csd[4],witness_lba[3],witness_crc[3];
 uint32_t start_lba,zone_sectors,raw_layout,seed,target_bytes,generated,written,verified;
 uint32_t records,elapsed_us,producer_us,max_backlog,overflows,returned_bytes,return_offset,return_bytes;
 uint32_t reads,writes,max_busy_us,timeouts,crc_errors,preclose_error,irq_count,cs_high;
 uint32_t pb_moder,pb_afrh,pd_moder,pd_afrl,stack_deepest,stack_floor,quiesced;
} R2Result;
R2Result r2_result;SdSpi r2_card;
Latency r2_latencies[LATENCIES];
uint8_t r2_witness[3][512],r2_return[65536];
volatile uint32_t r2_command,r2_return_offset,r2_irqs;
static uint8_t ring[RING_SIZE],block[512],readback[512];
typedef struct {uint32_t valid,length,status,error,rx_left,tx_left,sr,cr1,cr2,cfg1,cfg2;} SpiFault;
SpiFault r2_spi_fault;
static uint32_t active_length;
static SPI_HandleTypeDef spi;
static uint32_t previous,preclose_seen,preclose_error;
static uint64_t cycles64;
static uintptr_t stack_begin,stack_end;
extern uint32_t _estack,_ebss;
void SystemClock_Config(void);
void HAL_MspInit(void){__HAL_RCC_PWR_CLK_ENABLE();}
void SysTick_Handler(void){HAL_IncTick();r2_irqs++;}
void Error_Handler(void){r2_result.error=r2_result.error?r2_result.error:99;r2_result.stage=99;for(;;)__WFI();}
static uint64_t us(void *unused){(void)unused;uint32_t c=DWT->CYCCNT;cycles64+=(uint32_t)(c-previous);previous=c;return cycles64/160u;}
static void select_card(void *unused,bool on){(void)unused;HAL_GPIO_WritePin(GPIOF,GPIO_PIN_12,on?GPIO_PIN_RESET:GPIO_PIN_SET);}
/* Qualified HAL observer before it clears status. No trace inside RX loop. */
void bench_spi_preclose(SPI_HandleTypeDef *h)
{
 if(h!=&spi)return;
 preclose_seen++;
 if(h->RxXferCount||h->TxXferCount||h->ErrorCode||(h->Instance->SR&SPI_SR_OVR)){
  preclose_error=1;if(!r2_spi_fault.valid)r2_spi_fault=(SpiFault){1,active_length,0,h->ErrorCode,h->RxXferCount,h->TxXferCount,h->Instance->SR,h->Instance->CR1,h->Instance->CR2,h->Instance->CFG1,h->Instance->CFG2};
 }
}
static int exchange(void *unused,const uint8_t *tx,uint8_t *rx,uint16_t n)
{
 (void)unused;uint32_t seen=preclose_seen;active_length=n;
 HAL_StatusTypeDef result=HAL_SPI_TransmitReceive(&spi,(uint8_t *)tx,rx,n,30);
 bool failed=result!=HAL_OK||spi.ErrorCode||spi.RxXferCount||spi.TxXferCount||preclose_error||preclose_seen!=seen+1u;
 if(failed&&!r2_spi_fault.valid)r2_spi_fault=(SpiFault){1,n,result,spi.ErrorCode,spi.RxXferCount,spi.TxXferCount,SPI2->SR,SPI2->CR1,SPI2->CR2,SPI2->CFG1,SPI2->CFG2};
 return failed?-1:0;
}
static int frequency(bool fast)
{
 spi.Init.BaudRatePrescaler=fast?SPI_BAUDRATEPRESCALER_4:SPI_BAUDRATEPRESCALER_64;
 if(HAL_SPI_Init(&spi)!=HAL_OK)return -1;
 r2_result.spi_hz=16000000u/(fast?4u:64u);return 0;
}
static int platform(void)
{
 GPIO_InitTypeDef g={0};RCC_OscInitTypeDef osc={0};RCC_PeriphCLKInitTypeDef p={0};
 __HAL_RCC_GPIOB_CLK_ENABLE();__HAL_RCC_GPIOD_CLK_ENABLE();__HAL_RCC_GPIOC_CLK_ENABLE();__HAL_RCC_GPIOF_CLK_ENABLE();
 /* CS is latched HIGH before making PF12 an output; no JTAG/SWO or old DAT drive. */
 HAL_GPIO_WritePin(GPIOF,GPIO_PIN_12,GPIO_PIN_SET);g.Pin=GPIO_PIN_12;g.Mode=GPIO_MODE_OUTPUT_PP;g.Pull=GPIO_PULLUP;g.Speed=GPIO_SPEED_FREQ_LOW;HAL_GPIO_Init(GPIOF,&g);
 DBGMCU->CR&=~DBGMCU_CR_TRACE_IOEN;
 g.Pin=GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_14|GPIO_PIN_15;g.Mode=GPIO_MODE_INPUT;g.Pull=GPIO_NOPULL;HAL_GPIO_Init(GPIOB,&g);
 g.Pin=GPIO_PIN_6|GPIO_PIN_7;g.Mode=GPIO_MODE_ANALOG;HAL_GPIO_Init(GPIOD,&g);
 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);g.Pin=GPIO_PIN_9;g.Mode=GPIO_MODE_OUTPUT_PP;HAL_GPIO_Init(GPIOC,&g);
 osc.OscillatorType=RCC_OSCILLATORTYPE_HSI;osc.HSIState=RCC_HSI_ON;osc.HSICalibrationValue=RCC_HSICALIBRATION_DEFAULT;osc.PLL.PLLState=RCC_PLL_NONE;
 if(HAL_RCC_OscConfig(&osc)!=HAL_OK)return -1;
 p.PeriphClockSelection=RCC_PERIPHCLK_SPI2;p.Spi2ClockSelection=RCC_SPI2CLKSOURCE_HSI;
 if(HAL_RCCEx_PeriphCLKConfig(&p)!=HAL_OK||HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI2)!=16000000u)return -1;
 __HAL_RCC_SPI2_CLK_ENABLE();g.Pin=GPIO_PIN_13;g.Mode=GPIO_MODE_AF_PP;g.Pull=GPIO_NOPULL;g.Speed=GPIO_SPEED_FREQ_HIGH;g.Alternate=GPIO_AF5_SPI2;HAL_GPIO_Init(GPIOB,&g);
 g.Pin=GPIO_PIN_3|GPIO_PIN_4;HAL_GPIO_Init(GPIOD,&g);
 spi.Instance=SPI2;spi.Init.Mode=SPI_MODE_MASTER;spi.Init.Direction=SPI_DIRECTION_2LINES;spi.Init.DataSize=SPI_DATASIZE_8BIT;
 spi.Init.CLKPolarity=SPI_POLARITY_LOW;spi.Init.CLKPhase=SPI_PHASE_1EDGE;spi.Init.NSS=SPI_NSS_SOFT;spi.Init.FirstBit=SPI_FIRSTBIT_MSB;
 spi.Init.TIMode=SPI_TIMODE_DISABLE;spi.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;spi.Init.CRCPolynomial=7;
 spi.Init.NSSPMode=SPI_NSS_PULSE_DISABLE;spi.Init.NSSPolarity=SPI_NSS_POLARITY_LOW;spi.Init.FifoThreshold=SPI_FIFO_THRESHOLD_01DATA;
 spi.Init.MasterSSIdleness=SPI_MASTER_SS_IDLENESS_00CYCLE;spi.Init.MasterInterDataIdleness=SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
 spi.Init.MasterReceiverAutoSusp=SPI_MASTER_RX_AUTOSUSP_DISABLE;spi.Init.MasterKeepIOState=SPI_MASTER_KEEP_IO_STATE_ENABLE;
 spi.Init.IOSwap=SPI_IO_SWAP_DISABLE;spi.Init.ReadyMasterManagement=SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;spi.Init.ReadyPolarity=SPI_RDY_POLARITY_HIGH;
 return frequency(false);
}
static uint32_t crc32(const uint8_t *b,uint32_t n){uint32_t c=~0u;for(uint32_t i=0;i<n;i++){c^=b[i];for(unsigned j=0;j<8;j++)c=(c>>1)^((c&1)?0xedb88320u:0);}return ~c;}
static uint32_t be32(const uint8_t *b){return (uint32_t)b[0]<<24|(uint32_t)b[1]<<16|(uint32_t)b[2]<<8|b[3];}
static int p1(void)
{
 int r=sd_spi_init(&r2_card,(SdSpiIo){NULL,exchange,select_card,us});if(r)return r;
 r2_result.sectors=r2_card.sectors;r2_result.ocr=r2_card.ocr;
 for(unsigned i=0;i<4;i++){r2_result.cid[i]=be32(r2_card.cid+i*4);r2_result.csd[i]=be32(r2_card.csd+i*4);}
 if(frequency(true))return SD_IO;
 uint32_t lbas[3]={0,2048,r2_card.sectors-1};
 for(unsigned i=0;i<3;i++){
  r2_result.witness_lba[i]=lbas[i];
  if((r=sd_spi_read(&r2_card,lbas[i],r2_witness[i]))||(r=sd_spi_read(&r2_card,lbas[i],readback)))return r;
  if(memcmp(r2_witness[i],readback,512))return SD_CRC;
  r2_result.witness_crc[i]=crc32(readback,512);
 }
 return 0;
}
static int write_block(uint32_t lba,const uint8_t *data,uint32_t kind,uint32_t backlog)
{
 if(r2_result.records>=LATENCIES)return SD_RANGE;
 uint64_t start=us(NULL);int r=sd_spi_write(&r2_card,lba,data);uint32_t time=(uint32_t)(us(NULL)-start);
 r2_latencies[r2_result.records++]=(Latency){kind,time,r2_card.last_busy_us,backlog,0};return r;
}
static int p2(void)
{
 int r=sd_spi_test_gate(&r2_card,START_LBA,4096);if(r)return r;
 r2_result.start_lba=START_LBA;r2_result.zone_sectors=4096;
 /* Explicit sacrificial raw layout, not random writes into a mounted FAT.
  * Preserve old sector0 from P1 in evidence; invalidate its partition signature. */
 memset(block,0,512);memcpy(block,"TR2 R2 SPI2 RAW TEST V1",23);memcpy(block+32,r2_card.cid,16);
 if((r=write_block(0,block,0,0))||(r=sd_spi_read(&r2_card,0,readback)))return r;
 if(memcmp(block,readback,512))return SD_CRC;
 r2_result.raw_layout=1;
 uint32_t at=0;static const uint32_t sizes[4]={1,8,32,128};r2_result.seed=0x52325032;
 for(unsigned c=0;c<4;c++)for(unsigned j=0;j<sizes[c];j++,at++){
  r2_pattern(at*512u,r2_result.seed,block,512);
  if((r=write_block(START_LBA+at,block,sizes[c]*512,0)))return r;
  uint64_t read_started=us(NULL);r=sd_spi_read(&r2_card,START_LBA+at,readback);
  r2_latencies[r2_result.records-1].read_us=(uint32_t)(us(NULL)-read_started);if(r)return r;
  if(memcmp(block,readback,512)||crc32(block,512)!=crc32(readback,512))return SD_CRC;
  r2_result.written+=512;r2_result.verified+=512;
 }
 return 0;
}
static int load(uint32_t duration_us,uint32_t seed)
{
 if(!r2_result.raw_layout||!r2_card.write_enabled)return SD_REFUSED;
 r2_result.seed=seed;r2_result.target_bytes=(uint32_t)((uint64_t)duration_us*16000u/1000000u);
 r2_result.generated=r2_result.written=r2_result.verified=r2_result.records=r2_result.max_backlog=r2_result.overflows=r2_result.returned_bytes=0;
 uint32_t head=0,tail=0,backlog=0;uint64_t begin=us(NULL);int r=0;
 while(r2_result.generated<r2_result.target_bytes||backlog){
  uint64_t elapsed=us(NULL)-begin;uint32_t desired=elapsed>=duration_us?r2_result.target_bytes:(uint32_t)(elapsed*16000u/1000000u);
  uint32_t add=desired-r2_result.generated;
  if(add>RING_SIZE-backlog){r2_result.overflows++;return SD_RANGE;}
  uint32_t first=add<RING_SIZE-head?add:RING_SIZE-head;
  r2_pattern(r2_result.generated,seed,ring+head,first);if(add>first)r2_pattern(r2_result.generated+first,seed,ring,add-first);
  head=(head+add)%RING_SIZE;backlog+=add;r2_result.generated=desired;if(backlog>r2_result.max_backlog)r2_result.max_backlog=backlog;
  if(backlog>=512||(desired==r2_result.target_bytes&&backlog)){
   uint32_t size=backlog<512?backlog:512;memcpy(block,ring+tail,size);if(size<512)memset(block+size,255,512-size);
   if((r=write_block(START_LBA+r2_result.written/512u,block,512,backlog)))return r;
   tail=(tail+size)%RING_SIZE;backlog-=size;r2_result.written+=size;
  }
 }
 r2_result.producer_us=duration_us;r2_result.elapsed_us=(uint32_t)(us(NULL)-begin);
 uint32_t sectors=(r2_result.written+511u)/512u;
 for(uint32_t i=0;i<sectors;i++){
  uint32_t left=r2_result.written-i*512u,n=left<512?left:512;
  if((r=sd_spi_read(&r2_card,START_LBA+i,readback)))return r;
  r2_pattern(i*512u,seed,block,n);if(n<512)memset(block+n,255,512-n);
  if(memcmp(block,readback,512))return SD_CRC;
  r2_result.verified+=n;
 }
 return 0;
}
static int return_media(void)
{
 uint32_t size=((r2_result.written+511u)/512u)*512u,offset=r2_return_offset;
 if(r2_result.phase<3||offset!=r2_result.returned_bytes||offset>=size||offset%512u)return SD_REFUSED;
 uint32_t n=size-offset;if(n>sizeof r2_return)n=sizeof r2_return;
 r2_result.return_offset=offset;r2_result.return_bytes=0;
 for(uint32_t i=0;i<n;i+=512u){int r=sd_spi_read(&r2_card,START_LBA+(offset+i)/512u,r2_return+i);if(r)return r;}
 r2_result.return_bytes=n;r2_result.returned_bytes+=n;return 0;
}
static void paint(void){stack_begin=(uintptr_t)&_estack-4096;stack_end=(__get_MSP()-256u)&~3u;if(stack_begin<(uintptr_t)&_ebss+512||stack_end<=stack_begin)Error_Handler();for(volatile uint32_t *p=(uint32_t *)stack_begin;(uintptr_t)p<stack_end;p++)*p=0xa53cc35a;}
static void snapshot(void)
{
 r2_result.reads=r2_card.reads;r2_result.writes=r2_card.writes;r2_result.max_busy_us=r2_card.max_busy_us;r2_result.timeouts=r2_card.timeouts;r2_result.crc_errors=r2_card.crc_errors;r2_result.preclose_error=preclose_error;r2_result.irq_count=r2_irqs;
 r2_result.cs_high=!!(GPIOF->ODR&GPIO_PIN_12);r2_result.pb_moder=GPIOB->MODER;r2_result.pb_afrh=GPIOB->AFR[1];r2_result.pd_moder=GPIOD->MODER;r2_result.pd_afrl=GPIOD->AFR[0];
 uintptr_t p=stack_begin;while(p<stack_end&&*(volatile uint32_t *)p==0xa53cc35a)p+=4;r2_result.stack_floor=(uint32_t)(p-stack_begin);r2_result.stack_deepest=p<stack_end?(uint32_t)((uintptr_t)&_estack-p):0;if(p==stack_begin)r2_result.error=91;
}
__attribute__((noinline)) void r2_finished(void){__DSB();__ISB();}
int main(void)
{
 r2_result.version=1;r2_result.stage=1;r2_result.uid[0]=HAL_GetUIDw0();r2_result.uid[1]=HAL_GetUIDw1();r2_result.uid[2]=HAL_GetUIDw2();
 if(r2_result.uid[0]!=0x001b0046||r2_result.uid[1]!=0x38425018||r2_result.uid[2]!=0x20333157)Error_Handler();
 if(HAL_Init()!=HAL_OK)Error_Handler();
 HAL_PWREx_DisableUCPDDeadBattery();
 if(HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY)!=HAL_OK)Error_Handler();
 SystemClock_Config();
 r2_result.hclk=HAL_RCC_GetHCLKFreq();if(r2_result.hclk!=160000000u)Error_Handler();
 CoreDebug->DEMCR|=CoreDebug_DEMCR_TRCENA_Msk;DWT->CYCCNT=0;DWT->CTRL|=DWT_CTRL_CYCCNTENA_Msk;previous=DWT->CYCCNT;
 if(platform())Error_Handler();
 paint();r2_result.stage=2;
 for(;;){
  while(!r2_command){(void)us(NULL);__WFI();}
  uint32_t cmd=r2_command;r2_command=0;r2_result.stage=3;r2_result.operation=cmd;int r=SD_REFUSED;
  if(cmd==(COMMAND|1u)&&r2_result.phase==0){r=p1();if(!r)r2_result.phase=1;}
  else if(cmd==(COMMAND|2u)&&r2_result.phase==1){r=p2();if(!r)r2_result.phase=2;}
  else if(cmd==(COMMAND|3u)&&r2_result.phase==2){r=load(10000000u,0x52325333);if(!r)r2_result.phase=3;}
  else if(cmd==(COMMAND|4u)&&r2_result.phase==3&&r2_result.returned_bytes==((r2_result.written+511u)/512u)*512u){r=load(60000000u,0x52325334);if(!r)r2_result.phase=4;}
  else if(cmd==(COMMAND|0x10u))r=return_media();
  if(r==SD_TIMEOUT&&r2_card.last_cmd==24){
   if(sd_spi_quiesce(&r2_card)){r2_result.error=(uint32_t)r;r2_result.stage=98;select_card(NULL,false);for(;;){(void)us(NULL);__WFI();}}
   r2_result.quiesced=1;
  }
  select_card(NULL,false);if(r)r2_result.error=(uint32_t)r;snapshot();r2_result.stage=r2_result.error?99:4;r2_finished();
  if(r2_result.error)for(;;){(void)us(NULL);__WFI();}
 }
}
