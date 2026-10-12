#include "stm32_iis3dwb_active_readback.h"
#if TR2_STM32_ACTIVE_READBACK
#include <string.h>
volatile uint32_t tr2_active_readback_trigger;
volatile Stm32ActiveReadbackInfo tr2_active_readback_info;
volatile Stm32ActiveReadbackRecord tr2_active_readback_records[TR2_ACTIVE_READBACK_COUNT];
static const uint8_t controls[7]={7U,8U,9U,10U,16U,18U,20U};
static SPI_HandleTypeDef *bus;
static GPIO_TypeDef *cs_port;
static uint16_t cs_pin;
/* Raw diagnostics, no DATA_OUT, correction or expected-level substitution. */
static HAL_StatusTypeDef transfer(uint8_t reg,uint8_t length,uint8_t *data,int write)
{
 Stm32ActiveReadbackRecord record;
 uint32_t i=tr2_active_readback_info.count;
 if(i>=TR2_ACTIVE_READBACK_COUNT||length==0U||length>6U||reg>=0x78U) return HAL_ERROR;
 memset(&record,0,sizeof(record));record.phase=tr2_active_readback_info.stage;
 Stm32RegisterProbeRecord *v=&record.io;
 v->reg=reg;v->length=length;v->tx[0]=write?reg:(uint8_t)(reg|0x80U);
 if(write)v->tx[1]=*data;
 v->sr_before=bus->Instance->SR;v->cfg1=bus->Instance->CFG1;v->cfg2=bus->Instance->CFG2;
 v->cs_before=(cs_port->ODR&cs_pin)!=0U;v->begin_cycles=DWT->CYCCNT;
 HAL_GPIO_WritePin(cs_port,cs_pin,GPIO_PIN_RESET);
 v->cs_during=(cs_port->ODR&cs_pin)!=0U;
 v->status=(uint32_t)HAL_SPI_TransmitReceive(bus,v->tx,v->rx,(uint16_t)(length+1U),10U);
 v->sr_after=bus->Instance->SR;HAL_GPIO_WritePin(cs_port,cs_pin,GPIO_PIN_SET);
 v->end_cycles=DWT->CYCCNT;v->cs_after=(cs_port->ODR&cs_pin)!=0U;
 v->error=bus->ErrorCode;v->tx_left=bus->TxXferCount;v->rx_left=bus->RxXferCount;
 tr2_active_readback_records[i]=record;tr2_active_readback_info.count=i+1U;
 if(v->status!=HAL_OK)tr2_active_readback_info.errors++;
 if(!write&&v->status==HAL_OK)memcpy(data,v->rx+1U,length);
 return (HAL_StatusTypeDef)v->status;
}
static HAL_StatusTypeDef checked_write(uint8_t reg,uint8_t value)
{
 uint8_t actual=0U;int allowed=0;
 for(unsigned i=0;i<7;i++)if(controls[i]==reg)allowed=1;
 if(!allowed)return HAL_ERROR;
 HAL_StatusTypeDef result=transfer(reg,1U,&value,1);
 if(result==HAL_OK)result=transfer(reg,1U,&actual,0);
 return result==HAL_OK&&actual!=value?HAL_ERROR:result;
}
/* BDU order is always STATUS1 then STATUS2; grouped/single reads interleaved. */
static HAL_StatusTypeDef group(uint32_t *last)
{
 uint8_t data[6]={0};HAL_StatusTypeDef result=transfer(0x3AU,2U,data,0);
 *last=data[0]|((uint32_t)data[1]<<8U);
 if(result==HAL_OK)result=transfer(0x3AU,1U,data,0);
 if(result==HAL_OK)result=transfer(0x3BU,1U,data,0);
 if(result==HAL_OK)result=transfer(8U,1U,data,0);
 if(result==HAL_OK)result=transfer(15U,1U,data,0);
 if(result==HAL_OK)result=transfer(7U,4U,data,0);
 if(result==HAL_OK)result=transfer(18U,1U,data,0);
 return result;
}
__attribute__((noreturn)) void stm32_iis3dwb_active_readback_run(SPI_HandleTypeDef *spi,GPIO_TypeDef *port,uint16_t pin)
{
 uint8_t value=0U;uint32_t status=0U,start=0U;int saved=0,changed=0;
 HAL_StatusTypeDef result=HAL_OK,restoration=HAL_OK;
 bus=spi;cs_port=port;cs_pin=pin;tr2_active_readback_info.stage=1U;
 while(tr2_active_readback_trigger!=0x4D313544U)HAL_Delay(10U);
 tr2_active_readback_trigger=0U;
 CoreDebug->DEMCR|=CoreDebug_DEMCR_TRCENA_Msk;DWT->CTRL|=DWT_CTRL_CYCCNTENA_Msk;
 tr2_active_readback_info.core_hz=HAL_RCC_GetHCLKFreq();
 tr2_active_readback_info.spi_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3)/256U;
 for(unsigned i=0;i<7U;i++){
  result=transfer(controls[i],1U,&value,0);if(result!=HAL_OK)goto finish;
  tr2_active_readback_info.saved[i]=value;
  if(value!=(i==5U?4U:0U)){result=HAL_ERROR;goto finish;}
 }
 saved=1;
 result=transfer(15U,1U,&value,0);if(result!=HAL_OK||value!=0x7BU){result=HAL_ERROR;goto finish;}
 {uint8_t empty[2];result=transfer(0x3AU,2U,empty,0);if(result!=HAL_OK||empty[0]!=0U||empty[1]!=0U){result=HAL_ERROR;goto finish;}}
 changed=1;tr2_active_readback_info.stage=2U;
 {const uint8_t values[7]={0U,0x81U,0x0AU,0U,0xA8U,0x44U,0U};
  /* IF_INC/BDU before enabling sensor; FIFO bypass while settling. */
  const uint8_t order[7]={5U,4U,0U,1U,2U,3U,6U};
  for(unsigned j=0;j<7U;j++){unsigned i=order[j];result=checked_write(controls[i],values[i]);if(result!=HAL_OK)goto finish;}
 }
 HAL_Delay(100U);
 result=checked_write(10U,1U);if(result!=HAL_OK)goto finish;
 tr2_active_readback_info.stage=3U;start=HAL_GetTick();
 do{
  result=group(&status);if(result!=HAL_OK)goto finish;
  if(tr2_active_readback_info.count>=700U||(uint32_t)(HAL_GetTick()-start)>=15U)break;
 }while((status&0x8000U)==0U);
 tr2_active_readback_info.elapsed_ms=HAL_GetTick()-start;
 /* Freeze input without draining/clearing FIFO, then compare identical high levels.
  * STOP_ON_WTM remains configured; no intentional overrun or continuous mode. */
 tr2_active_readback_info.stage=4U;result=checked_write(16U,0U);if(result!=HAL_OK)goto finish;
 HAL_Delay(2U);
 for(unsigned i=0;i<32U;i++){result=group(&status);if(result!=HAL_OK)goto finish;}
finish:
 tr2_active_readback_info.result=(uint32_t)result;
 tr2_active_readback_info.stage=5U;
 if(saved){
  if(changed){const uint8_t order[7]={4U,3U,2U,1U,0U,6U,5U};
   for(unsigned j=0;j<7U;j++){unsigned i=order[j];HAL_StatusTypeDef step=checked_write(controls[i],(uint8_t)tr2_active_readback_info.saved[i]);if(step!=HAL_OK)restoration=step;}
  }
  for(unsigned i=0;i<7U;i++){HAL_StatusTypeDef step=transfer(controls[i],1U,&value,0);tr2_active_readback_info.restored[i]=value;if(step!=HAL_OK||value!=tr2_active_readback_info.saved[i])restoration=HAL_ERROR;}
 }else restoration=HAL_ERROR;
 tr2_active_readback_info.restore_result=(uint32_t)restoration;
 tr2_active_readback_info.stage=10U;
 for(;;)HAL_Delay(100U);
}
#else
typedef int Tr2ActiveReadbackDisabled;
#endif
