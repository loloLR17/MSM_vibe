#include "stm32_iis3dwb_register_probe.h"
#include <assert.h>
#include <setjmp.h>
#include <string.h>
DWT_Type test_dwt;
CoreDebug_Type test_core_debug;
static jmp_buf finish;
static GPIO_TypeDef port;
static uint32_t tick,calls;
void HAL_GPIO_WritePin(GPIO_TypeDef *p,uint16_t pin,GPIO_PinState value){assert(p==&port);if(value)p->ODR|=pin;else p->ODR&=~pin;}
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *s,const uint8_t *tx,uint8_t *rx,uint16_t size,uint32_t timeout){
 assert((port.ODR&1U)==0U); assert(tx[0]&0x80U);assert(size>=2U&&size<=7U&&timeout==10U);
 assert((tx[0]&127U)<0x78U); for(unsigned i=1;i<size;i++)assert(tx[i]==0U);
 memset(rx,0,size);rx[0]=0xAAU;rx[1]=tx[0]==0x8FU?0x7BU:tx[0]==0x92U?4U:0U;
 s->RxXferCount=0;s->TxXferCount=0;s->Instance->SR=2U;test_dwt.CYCCNT+=100U;
 calls++;s->ErrorCode=calls==17U?8U:0U; return calls==17U?HAL_TIMEOUT:HAL_OK;
}
void HAL_Delay(uint32_t ms){tick+=ms;test_dwt.CYCCNT+=ms*160000U;if(tr2_register_probe_info.stage==10U)longjmp(finish,1);}
uint32_t HAL_GetTick(void){return tick;}
uint32_t HAL_RCC_GetHCLKFreq(void){return 160000000U;}
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t id){assert(id==RCC_PERIPHCLK_SPI3);return 160000000U;}
int main(void){SPI_TypeDef registers={0,0x1234,0x5678};SPI_HandleTypeDef spi={&registers,0,0,0};port.ODR=1U;tr2_register_probe_trigger=0x4D313543U;
 if(setjmp(finish)==0)stm32_iis3dwb_register_probe_run(&spi,&port,1U);
 assert(calls==512U&&tr2_register_probe_info.count==512U&&tr2_register_probe_info.errors==1U);
 assert(tr2_register_probe_trigger==0U&&port.ODR==1U);
 for(unsigned i=0;i<512;i++){volatile Stm32RegisterProbeRecord *r=&tr2_register_probe_records[i];assert(r->cs_before==1U&&r->cs_during==0U&&r->cs_after==1U);assert(r->rx[0]==0xAAU);assert(r->cfg1==0x1234&&r->cfg2==0x5678);assert(r->tx_left==0&&r->rx_left==0);}
 assert(tr2_register_probe_records[16].status==HAL_TIMEOUT&&tr2_register_probe_records[16].error==8U);
 return 0;}
