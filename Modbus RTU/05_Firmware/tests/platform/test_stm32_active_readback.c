#include "stm32_iis3dwb_active_readback.h"
#include <assert.h>
#include <setjmp.h>
#include <string.h>
DWT_Type test_dwt;
CoreDebug_Type test_core_debug;
static GPIO_TypeDef port;
static jmp_buf finish;
static uint8_t regs[128];
static uint32_t tick,level,writes,fifo_starts;
static int scenario,injected;
static uint32_t injection_index,injected_byte;
void HAL_GPIO_WritePin(GPIO_TypeDef *p,uint16_t pin,GPIO_PinState v){assert(p==&port);if(v)p->ODR|=pin;else p->ODR&=~pin;}
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *s,const uint8_t *tx,uint8_t *rx,uint16_t size,uint32_t timeout){
 assert((port.ODR&1U)==0U&&size>=2U&&size<=5U&&timeout==10U);
 uint8_t reg=tx[0]&127U;assert(reg<0x78U);memset(rx,0,size);rx[0]=0xFFU;
 if((tx[0]&128U)==0U){
  assert(size==2U);assert(reg==7U||reg==8U||reg==9U||reg==10U||reg==16U||reg==18U||reg==20U);
  regs[reg]=tx[1];writes++;if(reg==10U){if(tx[1]==0U)level=0;else {assert(tx[1]==1U);fifo_starts++;}}
 }else{
  for(unsigned i=1U;i<size;i++)assert(tx[i]==0U);
  if(reg==0x3AU&&regs[10]==1U&&regs[16]!=0U&&level<256U)level+=4U;
  uint32_t status=level|(level==256U?0xA000U:0U);
  for(unsigned i=1;i<size;i++){uint8_t address=(uint8_t)(reg+i-1U);rx[i]=address==0x3AU?(uint8_t)status:address==0x3BU?(uint8_t)(status>>8U):regs[address];}
  if(scenario==1&&!injected&&reg==0x3AU&&size==3U&&level>=128U&&level<256U){rx[1]&=127U;injection_index=tr2_active_readback_info.count;injected_byte=rx[1];injected=1;}
  if(scenario==2&&!injected&&reg==8U&&regs[8]==0x81U){rx[1]=0x80U;injected=1;}
 }
 s->TxXferCount=0;s->RxXferCount=0;s->ErrorCode=0;test_dwt.CYCCNT+=8000U;
 return HAL_OK;
}
void HAL_Delay(uint32_t ms){tick+=ms;test_dwt.CYCCNT+=ms*160000U;if(tr2_active_readback_info.stage==10U)longjmp(finish,1);}
uint32_t HAL_GetTick(void){return tick;}
uint32_t HAL_RCC_GetHCLKFreq(void){return 160000000U;}
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t id){assert(id==RCC_PERIPHCLK_SPI3);return 160000000U;}
int main(void){
 for(scenario=0;scenario<4;scenario++){
  SPI_TypeDef peripheral={0,0x70070067,0x87400000};SPI_HandleTypeDef spi={&peripheral,0,0,0};
  memset(regs,0,sizeof(regs));regs[15]=0x7B;regs[18]=4;if(scenario==3)regs[16]=0xA8;
  memset((void*)&tr2_active_readback_info,0,sizeof(tr2_active_readback_info));tick=level=writes=fifo_starts=0;injected=0;port.ODR=1;
  tr2_active_readback_trigger=0x4D313544U;
  if(setjmp(finish)==0)stm32_iis3dwb_active_readback_run(&spi,&port,1U);
  assert(tr2_active_readback_info.count<TR2_ACTIVE_READBACK_COUNT&&tr2_active_readback_info.stage==10U&&port.ODR==1U);
  for(unsigned i=0;i<tr2_active_readback_info.count;i++){volatile Stm32RegisterProbeRecord *r=&tr2_active_readback_records[i].io;assert(r->cs_before==1U&&r->cs_during==0U&&r->cs_after==1U);assert(r->rx[0]==0xFFU);}
  if(scenario==3){assert(writes==0&&fifo_starts==0&&tr2_active_readback_info.result!=HAL_OK);continue;}
  assert(regs[7]==0&&regs[8]==0&&regs[9]==0&&regs[10]==0&&regs[16]==0&&regs[18]==4&&regs[20]==0);
  assert(tr2_active_readback_info.restore_result==HAL_OK);
  if(scenario==2){assert(tr2_active_readback_info.result!=HAL_OK&&fifo_starts==0&&injected);continue;}
  assert(tr2_active_readback_info.result==HAL_OK&&fifo_starts==1);
  if(scenario==1){assert(injected);assert(tr2_active_readback_records[injection_index].phase==3U);assert(tr2_active_readback_records[injection_index].io.rx[1]==injected_byte);}
 }
 return 0;
}
