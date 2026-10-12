#include "stm32_iis3dwb_continuous.h"
#include <assert.h>
#include <setjmp.h>
#include <string.h>
DWT_Type test_dwt;
CoreDebug_Type test_core_debug;
TestNvic test_nvic;
TestScb test_scb;
static GPIO_TypeDef port;
static uint8_t regs[128];
static unsigned fault,popped,writes,activation_count;
static uint32_t enable_cycles;
static int fifo_on;
static jmp_buf finish;
static uint8_t tag_for(unsigned index){uint8_t tag=(uint8_t)(0x10U|((index&3U)<<1U));unsigned parity=0;for(unsigned i=0;i<8;i++)parity^=(tag>>i)&1U;return tag|(uint8_t)parity;}
void HAL_GPIO_WritePin(GPIO_TypeDef *p,uint16_t pin,GPIO_PinState state){assert(p==&port);if(state)p->ODR|=pin;else p->ODR&=~pin;}
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *s,const uint8_t *tx,uint8_t *rx,uint16_t n,uint32_t timeout)
{
 uint8_t reg=tx[0]&127U;unsigned stage=tr2_cont_info.stage;
 assert(!(port.ODR&1U)&&n>=2U&&n<=897U&&timeout==10U);memset(rx,0,n);rx[0]=0xEE;
 test_dwt.CYCCNT+=n*512U;s->ErrorCode=0;s->RxXferCount=s->TxXferCount=0;
 if(!(tx[0]&128U)){
  assert(n==2U&&(reg==7||reg==8||reg==9||reg==10||reg==16||reg==18||reg==20));regs[reg]=tx[1];writes++;
  if(reg==10U){assert(tx[1]==0U||tx[1]==6U);fifo_on=tx[1]==6U;if(fifo_on){activation_count++;enable_cycles=test_dwt.CYCCNT;popped=0;if(fault==17U)tr2_cont_info.start_cycles-=TR2_CONT_BUDGET_MS*160000U+1U;}}
  if(reg==16U&&tx[1]==0U&&stage==3U&&fault==0U)assert(popped==TR2_CONT_WORDS);
  return HAL_OK;
 }
 assert(n>2U);for(unsigned i=1;i<n;i++)assert(tx[i]==0U);
 if(reg==0x3AU){
  assert(n==3U);unsigned produced=fifo_on?(unsigned)(((uint64_t)(uint32_t)(test_dwt.CYCCNT-enable_cycles)*26667U)/160000000U):0U;
  if(fault==5U)produced=popped;
  unsigned level=produced-popped;uint32_t value=level>=512U?512U|0x6000U:level|(level>=128U?0x8000U:0U);
  if(stage==3U&&fault==3U&&level>=128U)value|=0x4000U;
  if(stage==3U&&fault==10U)value=384U;
  if(stage==3U&&fault==2U&&popped)value=10U;
  if(stage==3U&&fault==12U)tr2_cont_info.word_count=TR2_CONT_WORDS-6U;
  if(stage==3U&&fault==8U&&popped>=384U)tr2_cont_stop=1U;
  rx[1]=(uint8_t)value;rx[2]=(uint8_t)(value>>8U);
 }else if(reg==0x78U){
  assert(n==897U&&fifo_on&&regs[16]==0xA8U&&regs[10]==6U&&regs[8]==0U);
  assert(popped+128U<=TR2_CONT_WORDS);
  for(unsigned i=0;i<128U;i++){unsigned index=popped+i;uint8_t tag=tag_for(index);if(fault==6U&&index==7U)tag^=1U;if(fault==7U&&index==128U)tag=tag_for(index+1U);rx[1U+7U*i]=tag;for(unsigned axis=0;axis<3U;axis++){int16_t value=(int16_t)((int)index-2048+(int)axis*4096);rx[2U+7U*i+2U*axis]=(uint8_t)value;rx[3U+7U*i+2U*axis]=(uint8_t)((uint16_t)value>>8U);}}
  popped+=128U;
  if(fault==9U)enable_cycles+=n*512U; /* Producer frozen only during drain: must reject. */
  if(fault==1U)s->RxXferCount=1U;
  if(fault==4U)return HAL_TIMEOUT;
 }else{
  for(unsigned i=1;i<n;i++)rx[i]=regs[reg+i-1U];
  if(fault==13U&&stage==3U&&reg==15U)rx[2]^=1U;
  if(fault==18U&&stage==3U&&reg==7U&&popped>=128U){rx[3]=0x14U;rx[4]=0x0CU;}
  if(fault==14U&&reg==15U)rx[1]^=1U;
 }
 return HAL_OK;
}
void HAL_Delay(uint32_t ms){if(fault!=16U)test_dwt.CYCCNT+=ms*160000U;if(fault==11U&&ms==100U&&tr2_cont_info.stage==2U)test_dwt.CYCCNT=0xFFF80000U;if(tr2_cont_info.stage==10U)longjmp(finish,1);}
uint32_t HAL_GetTick(void){return test_dwt.CYCCNT/160000U;}
uint32_t HAL_RCC_GetHCLKFreq(void){return 160000000U;}
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t id){assert(id==RCC_PERIPHCLK_SPI3);return 160000000U;}
int main(void)
{
 for(fault=0;fault<19U;fault++){
  SPI_TypeDef peripheral={0,0x50070067,0x87400000};SPI_HandleTypeDef spi={&peripheral,0,0,0};
  memset(regs,0,sizeof(regs));regs[15]=0x7B;regs[18]=4;memset(&test_dwt,0,sizeof(test_dwt));memset(&test_nvic,0,sizeof(test_nvic));
  memset((void*)&tr2_cont_info,0,sizeof(tr2_cont_info));memset((void*)&tr2_cont_first_refusal,0,sizeof(tr2_cont_first_refusal));memset((void*)tr2_cont_meta,0,sizeof(tr2_cont_meta));memset((void*)tr2_cont_xyz,0,sizeof(tr2_cont_xyz));
  if(fault==15U)for(unsigned i=0;i<3;i++)test_nvic.ISPR[i]=UINT32_MAX;
  tr2_cont_stop=tr2_cont_trace_count=0;tr2_cont_trigger=TR2_CONT_TRIGGER;port.ODR=1U;popped=writes=activation_count=0;fifo_on=0;
  if(setjmp(finish)==0)stm32_iis3dwb_continuous_run(&spi,&port,1U);
  assert(tr2_cont_info.stage==10U&&port.ODR==1U&&tr2_cont_info.burst_count<=TR2_CONT_BURSTS&&tr2_cont_info.poll_count<=TR2_CONT_POLLS&&tr2_cont_trace_count<=TR2_CONT_TRACES&&activation_count<=1U);
  if(fault==0U||fault==11U){
   assert(tr2_cont_info.result==TR2_OK&&tr2_cont_info.restore_result==TR2_OK&&tr2_cont_info.accepted==1U&&tr2_cont_info.window_state==3U&&tr2_cont_info.word_count==4096U&&tr2_cont_info.burst_count==32U&&tr2_cont_info.loss_flags==0U);
   for(unsigned i=0;i<TR2_CONT_WORDS;i++){assert(tr2_cont_tags[i]==tag_for(i));for(unsigned axis=0;axis<3;axis++)assert(tr2_cont_xyz[i][axis]==(int16_t)((int)i-2048+(int)axis*4096));}
   for(unsigned i=0;i<32;i++){assert(tr2_cont_meta[i].valid_mask==63U&&tr2_cont_meta[i].produced_observed>0U&&tr2_cont_meta[i].produced_observed>=tr2_cont_meta[i].produced_lower&&tr2_cont_meta[i].produced_observed<=tr2_cont_meta[i].produced_upper);}
   assert((uint32_t)(tr2_cont_info.capture_end_cycles-tr2_cont_info.start_cycles)<250U*160000U);
  }else{
   assert(tr2_cont_info.accepted==0U&&tr2_cont_info.window_state==4U&&tr2_cont_info.result!=TR2_OK);
   if(fault==1U)assert(tr2_cont_first_refusal.valid&&tr2_cont_first_refusal.io.valid_mask==7U&&tr2_cont_first_refusal.io.rx_left==1U);
   if(fault==2U||fault==9U||fault==10U)assert(tr2_cont_info.loss_flags&2U);
   if(fault==3U)assert(tr2_cont_info.loss_flags&1U);
   if(fault==4U)assert(tr2_cont_first_refusal.io.hal_status==HAL_TIMEOUT);
   if(fault==5U||fault==17U)assert(tr2_cont_info.loss_flags&128U);
   if(fault==6U)assert(tr2_cont_info.parity_errors==1U&&tr2_cont_info.first_failure_word==7U);
   if(fault==7U)assert(tr2_cont_info.gap_events==1U&&tr2_cont_info.first_failure_word==128U);
   if(fault==8U)assert(tr2_cont_info.loss_flags&256U);
   if(fault==12U)assert(tr2_cont_info.loss_flags&32U&&tr2_cont_info.burst_count==0U);
   if(fault==13U)assert(tr2_cont_first_refusal.reg==16U&&tr2_cont_info.burst_count==0U);
   if(fault==14U||fault==15U||fault==16U)assert(writes==0U);
   if(fault==18U){assert(tr2_cont_first_refusal.reg==9U&&tr2_cont_first_refusal.expected==0x0AU&&tr2_cont_first_refusal.observed==0x14U);assert(tr2_cont_info.burst_count==1U&&tr2_cont_info.word_count==0U&&tr2_cont_meta[0].valid_mask==23U);}

  }
  if(fault!=14U&&fault!=15U&&fault!=16U){assert(tr2_cont_info.restore_result==TR2_OK);assert(regs[7]==0U&&regs[8]==0U&&regs[9]==0U&&regs[10]==0U&&regs[16]==0U&&regs[18]==4U&&regs[20]==0U);}
 }
 return 0;
}
