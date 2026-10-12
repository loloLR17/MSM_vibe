#include "stm32_iis3dwb_burst_capture.h"
#include <assert.h>
#include <setjmp.h>
#include <string.h>
DWT_Type test_dwt;
CoreDebug_Type test_core_debug;
static GPIO_TypeDef port;
static jmp_buf finish;
static uint8_t regs[128];
static uint32_t level,popped,enable_cycles,writes,bursts;
static int fault,fifo_on,stopped;
void HAL_GPIO_WritePin(GPIO_TypeDef *p,uint16_t pin,GPIO_PinState state){assert(p==&port);if(state)p->ODR|=pin;else p->ODR&=~pin;}
static uint8_t tag_for(unsigned index){uint8_t tag=(uint8_t)(0x10U|((index&3U)<<1U));unsigned parity=0;for(unsigned i=0;i<8;i++)parity^=(tag>>i)&1U;return tag|(uint8_t)parity;}
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *s,const uint8_t *tx,uint8_t *rx,uint16_t n,uint32_t timeout){
 assert((port.ODR&1U)==0U&&n>=2U&&n<=897U&&timeout==10U);memset(rx,0,n);rx[0]=0xEE;
 uint8_t reg=tx[0]&127U;test_dwt.CYCCNT+=n*512U; /* wire model2.5MHz */
 s->ErrorCode=0;s->RxXferCount=0;s->TxXferCount=0;
 if((tx[0]&128U)==0U){assert(n==2U);assert(reg==7||reg==8||reg==9||reg==10||reg==16||reg==18||reg==20);regs[reg]=tx[1];writes++;if(reg==10){fifo_on=tx[1]==1;enable_cycles=test_dwt.CYCCNT;popped=0;stopped=0;if(!fifo_on)level=0;}return HAL_OK;}
 for(unsigned i=1;i<n;i++)assert(tx[i]==0U);
 if(reg==0x3A){
  assert(n==3U);if(fifo_on&&regs[16]){uint32_t produced=(uint32_t)(((uint64_t)(test_dwt.CYCCNT-enable_cycles)*26667U)/160000000U);if(produced>=256)stopped=1;level=(stopped?256:produced)-popped;}
  uint32_t value=level|(level==256?0xA000U:0U);if(fault==4&&level>=256)value|=0x4000;
  rx[1]=(uint8_t)value;rx[2]=(uint8_t)(value>>8U);
 }else if(reg==0x78){
  assert(n==897U&&fifo_on&&regs[16]==0&&level>=128);bursts++;
  for(unsigned i=0;i<128;i++){unsigned index=popped+i;uint8_t tag=tag_for(index);if(fault==1&&index==128)tag=tag_for(index+1);if(fault==2&&index==7)tag^=1;rx[1+i*7]=tag;for(unsigned axis=0;axis<3;axis++){int16_t v=(int16_t)((int)index-128+(int)axis*1024);rx[2+i*7+2*axis]=(uint8_t)v;rx[3+i*7+2*axis]=(uint8_t)((uint16_t)v>>8);}}
  popped+=128;level-=128;
  if(fault==3&&bursts==2)return HAL_TIMEOUT;
 }else for(unsigned i=1;i<n;i++)rx[i]=regs[reg+i-1U];
 return HAL_OK;
}
void HAL_Delay(uint32_t ms){test_dwt.CYCCNT+=ms*160000U;if(tr2_burst_info.stage==10)longjmp(finish,1);}
uint32_t HAL_GetTick(void){return test_dwt.CYCCNT/160000U;}
uint32_t HAL_RCC_GetHCLKFreq(void){return 160000000U;}
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t id){assert(id==RCC_PERIPHCLK_SPI3);return 160000000U;}
int main(void){
 for(fault=0;fault<6;fault++){
  SPI_TypeDef peripheral={0,0x50070067,0x87400000};SPI_HandleTypeDef spi={&peripheral,0,0,0};
  memset(regs,0,sizeof(regs));regs[15]=0x7B;regs[18]=4;if(fault==5)regs[10]=1;
  memset((void*)&tr2_burst_info,0,sizeof(tr2_burst_info));memset(&test_dwt,0,sizeof(test_dwt));level=popped=writes=bursts=0;fifo_on=stopped=0;port.ODR=1;tr2_burst_trigger=0x4D313546U;
  if(setjmp(finish)==0)stm32_iis3dwb_burst_capture_run(&spi,&port,1U);
  assert(tr2_burst_info.stage==10&&port.ODR==1&&tr2_burst_info.poll_count<=TR2_BURST_POLLS);
  if(fault==5){assert(writes==0&&bursts==0&&tr2_burst_info.accepted==0);continue;}
  assert(regs[7]==0&&regs[8]==0&&regs[9]==0&&regs[10]==0&&regs[16]==0&&regs[18]==4&&regs[20]==0&&tr2_burst_info.restore_result==TR2_OK);
  if(fault==0){assert(tr2_burst_info.result==TR2_OK&&tr2_burst_info.accepted==1&&tr2_burst_info.word_count==256&&bursts==2&&tr2_burst_info.final_status==0);assert(tr2_burst_rx[0][0]==0xEE&&tr2_burst_rx[1][0]==0xEE);assert(tr2_burst_meta[0].status_before==0xA100&&tr2_burst_meta[0].status_after==128&&tr2_burst_meta[1].status_after==0);}
  else{assert(tr2_burst_info.accepted==0&&tr2_burst_info.result!=TR2_OK);if(fault==1)assert(tr2_burst_info.gap_events>0);if(fault==2)assert(tr2_burst_info.parity_errors==1);if(fault==3)assert(tr2_burst_info.errors==1&&tr2_burst_info.burst_count==2);if(fault==4)assert(bursts==0&&(tr2_burst_info.loss_flags&1U));}
 }
 return 0;
}
