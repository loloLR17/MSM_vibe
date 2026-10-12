#include "stm32_iis3dwb_ctrl1_differential.h"
#if TR2_STM32_CTRL1_DIFFERENTIAL
#include <string.h>
#include <limits.h>
volatile uint32_t tr2_ctrl1_trigger;
volatile Stm32Ctrl1Info tr2_ctrl1_info;
volatile Stm32ConfigTrace tr2_ctrl1_traces[TR2_CTRL1_TRACE_COUNT];
volatile Stm32ConfigRefusal tr2_ctrl1_first_divergence;
volatile uint32_t tr2_ctrl1_nvic[7][10];
static SPI_HandleTypeDef *bus;
static GPIO_TypeDef *port;
static uint16_t pin;
static const uint8_t controls[7]={7U,8U,9U,10U,16U,18U,20U};
static void first(uint32_t reason,uint32_t reg,uint32_t expected_valid,uint32_t expected,
 uint32_t observed_valid,uint32_t observed,uint32_t index)
{
 if(tr2_ctrl1_first_divergence.valid)return;
 tr2_ctrl1_first_divergence.reason=reason;tr2_ctrl1_first_divergence.phase=tr2_ctrl1_info.stage;
 tr2_ctrl1_first_divergence.reg=reg;tr2_ctrl1_first_divergence.expected_valid=expected_valid;
 tr2_ctrl1_first_divergence.expected=expected;tr2_ctrl1_first_divergence.observed_valid=observed_valid;
 tr2_ctrl1_first_divergence.observed=observed;tr2_ctrl1_first_divergence.trace_index=index;
 if(index<tr2_ctrl1_info.count)tr2_ctrl1_first_divergence.io=tr2_ctrl1_traces[index];
 tr2_ctrl1_first_divergence.valid=1U;
}
static void mismatch(uint8_t reg,uint8_t expected,uint8_t observed)
{
 tr2_ctrl1_info.mismatches++;
 uint32_t index=UINT32_MAX;
 for(uint32_t i=tr2_ctrl1_info.count;i>0U;i--){volatile Stm32ConfigTrace *v=&tr2_ctrl1_traces[i-1U];if(v->method!=4U&&reg>=v->reg&&reg<v->reg+v->nbytes-1U){index=i-1U;break;}}
 first(2U,reg,1U,expected,1U,observed,index);
}
static int nvic_snapshot(unsigned phase)
{
 int safe=1;
 for(unsigned i=0;i<3U;i++){
  tr2_ctrl1_nvic[phase][i]=NVIC->ISER[i];tr2_ctrl1_nvic[phase][3U+i]=NVIC->ISPR[i];
  if(NVIC->ISER[i]!=0U||NVIC->ISPR[i]!=0U)safe=0; /* Exclusive blocking profile. */
 }
 tr2_ctrl1_nvic[phase][6]=__get_PRIMASK();tr2_ctrl1_nvic[phase][7]=__get_BASEPRI();
 tr2_ctrl1_nvic[phase][8]=__get_FAULTMASK();tr2_ctrl1_nvic[phase][9]=SCB->ICSR;
 if(tr2_ctrl1_nvic[phase][6]||tr2_ctrl1_nvic[phase][7]||tr2_ctrl1_nvic[phase][8])safe=0;
 tr2_ctrl1_info.nvic_valid_phases|=1U<<phase;
 return safe;
}
static Tr2Result io(uint8_t reg,uint8_t *data,uint8_t n,int write)
{
 Stm32ConfigTrace v={0};uint32_t i=tr2_ctrl1_info.count;
 if(i>=TR2_CTRL1_TRACE_COUNT){first(3U,reg,0U,UINT32_MAX,0U,UINT32_MAX,UINT32_MAX);return TR2_ERROR_INTERNAL;}
 /* Only volatile CTRL1/CTRL3 writes; no FIFO enable, reset, self-test or media. */
 if(n==0U||n>6U||(write&&(n!=1U||!((reg==16U&&(*data==0U||*data==0xA8U))||(reg==18U&&(*data==4U||*data==0x44U))))))return TR2_ERROR_UNSUPPORTED;
 v.phase=tr2_ctrl1_info.stage;v.method=write?4U:(n==1U?1U:2U);
 v.reg=reg;v.nbytes=n+1U;v.raw_length=n+1U;v.tx[0]=write?reg:(uint8_t)(reg|128U);
 if(write)v.tx[1]=*data;
 v.cs_before=(port->ODR&pin)!=0U;v.sr_before=bus->Instance->SR;
 v.cfg1=bus->Instance->CFG1;v.cfg2=bus->Instance->CFG2;v.begin_cycles=DWT->CYCCNT;v.valid_mask=1U;
 HAL_GPIO_WritePin(port,pin,GPIO_PIN_RESET);v.cs_during=(port->ODR&pin)!=0U;
 v.hal_status=(uint32_t)HAL_SPI_TransmitReceive(bus,v.tx,v.rx,(uint16_t)(n+1U),10U);
 v.sr_after=bus->Instance->SR;HAL_GPIO_WritePin(port,pin,GPIO_PIN_SET);
 v.end_cycles=DWT->CYCCNT;v.cs_after=(port->ODR&pin)!=0U;v.error=bus->ErrorCode;
 v.rx_left=bus->RxXferCount;v.tx_left=bus->TxXferCount;v.valid_mask|=2U|4U;
 if(v.hal_status==HAL_OK&&!write)v.valid_mask|=8U;
 tr2_ctrl1_traces[i]=v;tr2_ctrl1_info.count=i+1U;
 if(v.hal_status!=HAL_OK||v.error||v.rx_left||v.tx_left||v.cs_before!=1U||v.cs_during!=0U||v.cs_after!=1U){
  first(1U,reg,write?1U:0U,write?v.tx[1]:UINT32_MAX,0U,UINT32_MAX,i);
  tr2_ctrl1_info.errors++;return v.hal_status==HAL_TIMEOUT?TR2_ERROR_UNAVAILABLE:TR2_ERROR_INTERNAL;
 }
 if(!write)memcpy(data,v.rx+1U,n);
 return TR2_OK;
}
static Tr2Result write_reg(uint8_t reg,uint8_t value){return io(reg,&value,1U,1);}
static Tr2Result read_controls(uint32_t *values)
{
 uint8_t data[4];Tr2Result r=io(7U,data,4U,0);if(r!=TR2_OK)return r;
 for(unsigned i=0;i<4U;i++)values[i]=data[i];
 for(unsigned i=4U;i<7U;i++){r=io(controls[i],data,1U,0);if(r!=TR2_OK)return r;values[i]=data[0];}
 return r;
}
/* Predetermined S G G S quartet controls read-order/time confounding.
 * CTRL1 mismatch remains latched; only the declared read-only comparison follows.
 * WHO/CTRL3/CTRL4/CTRL5 or I/O anomaly stops immediately. No fallback/vote. */
static Tr2Result quartet(uint8_t expected,uint8_t ctrl3)
{
 for(unsigned i=0;i<4U;i++){
  uint8_t data[6]={0};int grouped=(i==1U||i==2U);
  Tr2Result r=io(grouped?15U:16U,data,grouped?6U:1U,0);if(r!=TR2_OK)return r;
  uint8_t actual=data[grouped?1U:0U];if(actual!=expected)mismatch(16U,expected,actual);
  if(grouped){
   const uint8_t positions[4]={0U,3U,4U,5U};const uint8_t values[4]={0x7BU,ctrl3,0U,0U};
   for(unsigned j=0;j<4U;j++)if(data[positions[j]]!=values[j]){mismatch((uint8_t)(15U+positions[j]),values[j],data[positions[j]]);return TR2_ERROR_INTERNAL;}
   /* Reserved byte0x11 captured, never interpreted as a guaranteed value. */
  }
 }
 return TR2_OK;
}
__attribute__((noreturn)) void stm32_iis3dwb_ctrl1_differential_run(SPI_HandleTypeDef *spi,GPIO_TypeDef *gpio,uint16_t cs)
{
 uint32_t saved[7]={0},actual[7]={0};uint8_t value=0U;int have_saved=0,changed=0;
 Tr2Result result=TR2_OK,restored=TR2_ERROR_NOT_AVAILABLE;
 bus=spi;port=gpio;pin=cs;tr2_ctrl1_info.stage=1U;
 while(tr2_ctrl1_trigger!=TR2_CTRL1_TRIGGER)HAL_Delay(10U);
 tr2_ctrl1_trigger=0U;
 if(!nvic_snapshot(1U)){result=TR2_ERROR_INVALID_STATE;goto finish;}
 CoreDebug->DEMCR|=CoreDebug_DEMCR_TRCENA_Msk;
 if(DWT->CTRL&(1UL<<25)){result=TR2_ERROR_UNSUPPORTED;goto finish;}
 DWT->CYCCNT=0U;DWT->CTRL|=DWT_CTRL_CYCCNTENA_Msk;
 {uint32_t before=DWT->CYCCNT;HAL_Delay(2U);if(before==DWT->CYCCNT){result=TR2_ERROR_UNAVAILABLE;goto finish;}}
 tr2_ctrl1_info.core_hz=HAL_RCC_GetHCLKFreq();tr2_ctrl1_info.spi_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3)/64U;
 if(tr2_ctrl1_info.core_hz!=160000000U||tr2_ctrl1_info.spi_hz!=2500000U){result=TR2_ERROR_UNSUPPORTED;goto finish;}
 tr2_ctrl1_info.start_cycles=DWT->CYCCNT;
 result=read_controls(saved);if(result!=TR2_OK)goto finish;
 for(unsigned i=0;i<7U;i++){tr2_ctrl1_info.saved[i]=saved[i];if(saved[i]!=(i==5U?4U:0U)){mismatch(controls[i],i==5U?4U:0U,(uint8_t)saved[i]);result=TR2_ERROR_UNSUPPORTED;goto finish;}}
 have_saved=1;
 {static const uint8_t audit[]={0x0D,0x0E,0x13,0x15,0x16,0x17,0x19,0x73,0x74,0x75};
  for(unsigned i=0;i<sizeof(audit);i++){result=io(audit[i],&value,1U,0);if(result!=TR2_OK||value!=0U){if(result==TR2_OK)mismatch(audit[i],0U,value);result=TR2_ERROR_UNSUPPORTED;goto finish;}}
 }
 result=quartet(0U,4U);if(result!=TR2_OK||tr2_ctrl1_info.mismatches){result=TR2_ERROR_INTERNAL;goto finish;}
 tr2_ctrl1_info.stage=2U;if(!nvic_snapshot(2U)){result=TR2_ERROR_INVALID_STATE;goto finish;}
 changed=1;result=write_reg(18U,0x44U);if(result!=TR2_OK)goto finish;
 result=quartet(0U,0x44U);if(result!=TR2_OK||tr2_ctrl1_info.mismatches){result=TR2_ERROR_INTERNAL;goto finish;}
 tr2_ctrl1_info.stage=3U;if(!nvic_snapshot(3U)){result=TR2_ERROR_INVALID_STATE;goto finish;}
 result=write_reg(16U,0xA8U);if(result!=TR2_OK)goto finish;
 result=quartet(0xA8U,0x44U);if(result!=TR2_OK)goto finish;
 tr2_ctrl1_info.stage=4U;tr2_ctrl1_info.delay_begin_cycles=DWT->CYCCNT;
 HAL_Delay(TR2_CTRL1_DELAY_MS);tr2_ctrl1_info.delay_end_cycles=DWT->CYCCNT;
 if(!nvic_snapshot(4U)){result=TR2_ERROR_INVALID_STATE;goto finish;}
 result=quartet(0xA8U,0x44U);if(result!=TR2_OK)goto finish;
 tr2_ctrl1_info.stage=5U;result=write_reg(16U,0U);if(result!=TR2_OK)goto finish;
 if(!nvic_snapshot(5U)){result=TR2_ERROR_INVALID_STATE;goto finish;}
 result=quartet(0U,0x44U);
finish:
 if(result==TR2_OK&&tr2_ctrl1_info.mismatches)result=TR2_ERROR_INTERNAL;
 tr2_ctrl1_info.result=(uint32_t)result;tr2_ctrl1_info.stage=6U;
 if(have_saved){restored=TR2_OK;
  if(changed){if(write_reg(16U,0U)!=TR2_OK)restored=TR2_ERROR_INTERNAL;if(write_reg(18U,4U)!=TR2_OK)restored=TR2_ERROR_INTERNAL;}
  if(read_controls(actual)!=TR2_OK)restored=TR2_ERROR_INTERNAL;
  for(unsigned i=0;i<7U;i++){tr2_ctrl1_info.restored[i]=actual[i];if(actual[i]!=saved[i])restored=TR2_ERROR_INTERNAL;}
 }
 if(!nvic_snapshot(6U))restored=TR2_ERROR_INTERNAL;
 tr2_ctrl1_info.restore_result=(uint32_t)restored;tr2_ctrl1_info.end_cycles=DWT->CYCCNT;
 tr2_ctrl1_info.stage=10U;for(;;)HAL_Delay(100U);
}
#else
typedef int Tr2Ctrl1DifferentialDisabled;
#endif
