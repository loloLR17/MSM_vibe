#include "stm32_iis3dwb_ctrl1_differential.h"
#include <assert.h>
#include <setjmp.h>
#include <string.h>
DWT_Type test_dwt;
CoreDebug_Type test_core_debug;
TestNvic test_nvic;
TestScb test_scb;
static GPIO_TypeDef port;
static uint8_t regs[128];
static jmp_buf finish;
static unsigned fault,calls,writes,activations,injected;
void HAL_GPIO_WritePin(GPIO_TypeDef *p,uint16_t pin,GPIO_PinState state){assert(p==&port);if(state)p->ODR|=pin;else p->ODR&=~pin;}
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *s,const uint8_t *tx,uint8_t *rx,uint16_t n,uint32_t timeout)
{
 uint8_t reg=tx[0]&127U;unsigned phase=tr2_ctrl1_info.stage;int write=(tx[0]&128U)==0U;
 assert(!(port.ODR&1U)&&n>=2U&&n<=7U&&timeout==10U);assert(reg!=0x28U&&reg!=0x78U);
 calls++;test_dwt.CYCCNT+=n*512U;memset(rx,0,n);rx[0]=0xEE;s->ErrorCode=0;s->RxXferCount=s->TxXferCount=0;
 if(write){assert(n==2U&&((reg==16U&&(tx[1]==0U||tx[1]==0xA8U))||(reg==18U&&(tx[1]==4U||tx[1]==0x44U))));regs[reg]=tx[1];writes++;if(reg==16U&&tx[1]==0xA8U)activations++;}
 else{
  for(unsigned i=1;i<n;i++){assert(tx[i]==0U);rx[i]=regs[reg+i-1U];}
  if((phase==3U||phase==4U)&&reg<=16U&&reg+n-1U>16U){
   int corrupt=(fault==1U&&n==2U)||fault==2U||(fault==3U&&phase==3U)||(fault==4U&&phase==4U&&n==2U);
   if(corrupt)rx[1U+16U-reg]=0x50U;
  }
  if(fault==6U&&reg==15U)rx[1]^=1U;
  if(fault==7U&&phase==2U&&reg==15U)rx[4]^=1U;
 }
 if(fault>=10U&&calls==fault-9U){injected=1U;return HAL_TIMEOUT;}
 return HAL_OK;
}
void HAL_Delay(uint32_t ms){if(fault!=9U)test_dwt.CYCCNT+=ms*160000U;if(tr2_ctrl1_info.stage==10U)longjmp(finish,1);}
uint32_t HAL_GetTick(void){return test_dwt.CYCCNT/160000U;}
uint32_t HAL_RCC_GetHCLKFreq(void){return 160000000U;}
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t id){assert(id==RCC_PERIPHCLK_SPI3);return 160000000U;}
int main(void)
{
 for(fault=0;fault<53U;fault++){
  SPI_TypeDef peripheral={0,0x50070067,0x87400000};SPI_HandleTypeDef spi={&peripheral,0,0,0};
  memset(regs,0,sizeof(regs));regs[15]=0x7B;regs[18]=4;if(fault==5U)regs[16]=1;
  memset((void*)&tr2_ctrl1_info,0,sizeof(tr2_ctrl1_info));memset((void*)&tr2_ctrl1_first_divergence,0,sizeof(tr2_ctrl1_first_divergence));
  memset((void*)tr2_ctrl1_traces,0,sizeof(tr2_ctrl1_traces));memset((void*)tr2_ctrl1_nvic,0,sizeof(tr2_ctrl1_nvic));memset(&test_dwt,0,sizeof(test_dwt));memset(&test_nvic,0,sizeof(test_nvic));
  if(fault==8U)for(unsigned i=0;i<3U;i++)test_nvic.ISPR[i]=0xFFFFFFFFU;
  calls=writes=activations=injected=0U;port.ODR=1U;tr2_ctrl1_trigger=TR2_CTRL1_TRIGGER;
  if(setjmp(finish)==0)stm32_iis3dwb_ctrl1_differential_run(&spi,&port,1U);
  assert(tr2_ctrl1_info.stage==10U&&port.ODR==1U&&tr2_ctrl1_info.count==calls&&calls<=TR2_CTRL1_TRACE_COUNT&&activations<=1U);
  volatile Stm32ConfigRefusal *v=&tr2_ctrl1_first_divergence;
  if(fault>=10U){assert(injected&&v->valid&&v->reason==1U&&v->observed_valid==0U&&v->io.hal_status==HAL_TIMEOUT&&v->io.valid_mask==7U);assert(tr2_ctrl1_info.result!=TR2_OK||tr2_ctrl1_info.restore_result!=TR2_OK);continue;}
  if(fault==8U||fault==9U){assert(calls==0U&&writes==0U&&tr2_ctrl1_info.result!=TR2_OK);continue;}
  if(fault==5U){assert(writes==0U&&v->valid&&v->reg==16U&&v->observed==1U&&v->io.reg==16U);continue;}
  assert(regs[16]==0U&&regs[18]==4U&&tr2_ctrl1_info.restore_result==TR2_OK);
  if(fault==6U||fault==7U){assert(activations==0U&&v->valid&&v->reg==(fault==6U?15U:18U));continue;}
  assert(calls==43U&&writes==5U&&activations==1U&&tr2_ctrl1_info.delay_end_cycles-tr2_ctrl1_info.delay_begin_cycles>=TR2_CTRL1_DELAY_MS*160000U);
  if(fault==0U){assert(tr2_ctrl1_info.result==TR2_OK&&v->valid==0U&&tr2_ctrl1_info.mismatches==0U);}
  else{
   assert(tr2_ctrl1_info.result==TR2_ERROR_INTERNAL&&v->valid&&v->reg==16U&&v->expected==0xA8U&&v->observed==0x50U&&v->expected_valid&&v->observed_valid);
   assert(v->phase==(fault==4U?4U:3U)&&v->io.method==1U&&v->io.tx[0]==0x90U&&v->io.rx[1]==0x50U&&v->io.valid_mask==15U);
   assert(tr2_ctrl1_info.mismatches==(fault==2U?8U:(fault==3U?4U:(fault==4U?2U:4U))));
   assert(memcmp((const void*)&v->io,(const void*)&tr2_ctrl1_traces[v->trace_index],sizeof(v->io))==0);
  }
 }
 return 0;
}
