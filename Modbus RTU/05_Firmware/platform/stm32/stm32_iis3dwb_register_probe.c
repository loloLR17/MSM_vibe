#include "stm32_iis3dwb_register_probe.h"
#if TR2_STM32_REGISTER_PROBE
#include <string.h>
volatile uint32_t tr2_register_probe_trigger;
volatile Stm32RegisterProbeInfo tr2_register_probe_info;
volatile Stm32RegisterProbeRecord tr2_register_probe_records[TR2_REGISTER_PROBE_COUNT];
/* All commands are reads. Never access FIFO_DATA_OUT or start the sensor.
 * STATUS2 can clear OVR_LATCHED; its raw bytes are retained. */
__attribute__((noreturn)) void stm32_iis3dwb_register_probe_run(
 SPI_HandleTypeDef *spi,GPIO_TypeDef *port,uint16_t pin)
{
 static const uint8_t regs[8]={0x0FU,0x0FU,0x12U,0x08U,0x07U,0x3AU,0x3AU,0x3BU};
 static const uint8_t lengths[8]={1U,2U,1U,1U,6U,2U,1U,1U};
 uint32_t start;
 tr2_register_probe_info.stage=1U;
 while(tr2_register_probe_trigger!=0x4D313543U) HAL_Delay(10U);
 tr2_register_probe_trigger=0U;
 CoreDebug->DEMCR|=CoreDebug_DEMCR_TRCENA_Msk;
 DWT->CTRL|=DWT_CTRL_CYCCNTENA_Msk;
 tr2_register_probe_info.core_hz=HAL_RCC_GetHCLKFreq();
 tr2_register_probe_info.spi_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3)/256U;
 tr2_register_probe_info.stage=2U; start=HAL_GetTick();
 for(uint32_t i=0U;i<TR2_REGISTER_PROBE_COUNT;i++) {
  Stm32RegisterProbeRecord record;
  memset(&record,0,sizeof(record));
  record.reg=regs[i%8U]; record.length=lengths[i%8U];
  record.tx[0]=(uint8_t)(record.reg|0x80U);
  record.cs_before=(port->ODR&pin)!=0U;
  record.sr_before=spi->Instance->SR;
  record.cfg1=spi->Instance->CFG1; record.cfg2=spi->Instance->CFG2;
  record.begin_cycles=DWT->CYCCNT;
  HAL_GPIO_WritePin(port,pin,GPIO_PIN_RESET);
  record.cs_during=(port->ODR&pin)!=0U;
  record.status=(uint32_t)HAL_SPI_TransmitReceive(spi,record.tx,record.rx,(uint16_t)(record.length+1U),10U);
  record.sr_after=spi->Instance->SR;
  HAL_GPIO_WritePin(port,pin,GPIO_PIN_SET);
  record.end_cycles=DWT->CYCCNT;
  record.cs_after=(port->ODR&pin)!=0U;
  record.error=spi->ErrorCode; record.tx_left=spi->TxXferCount; record.rx_left=spi->RxXferCount;
  tr2_register_probe_records[i]=record;
  tr2_register_probe_info.count=i+1U;
  if(record.status!=HAL_OK) tr2_register_probe_info.errors++;
  if((uint32_t)(HAL_GetTick()-start)>=1000U) break;
  if(i%8U==7U) HAL_Delay(1U);
 }
 tr2_register_probe_info.elapsed_ms=HAL_GetTick()-start;
 tr2_register_probe_info.stage=10U;
 for(;;) HAL_Delay(100U);
}
#else
typedef int Tr2RegisterProbeDisabled;
#endif
