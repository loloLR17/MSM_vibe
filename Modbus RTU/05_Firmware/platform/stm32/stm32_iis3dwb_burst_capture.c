#include "stm32_iis3dwb_burst_capture.h"
#if TR2_STM32_BURST_CAPTURE
#include <string.h>
#include <limits.h>
volatile uint32_t tr2_burst_trigger;
volatile Stm32BurstInfo tr2_burst_info;
volatile Stm32BurstPoll tr2_burst_polls[TR2_BURST_POLLS];
volatile Stm32BurstMeta tr2_burst_meta[2];
volatile uint8_t tr2_burst_rx[2][TR2_BURST_BYTES];
volatile uint32_t tr2_config_trace_count;
volatile Stm32ConfigTrace tr2_config_traces[TR2_CONFIG_TRACE_COUNT];
volatile Stm32ConfigRefusal tr2_config_first_refusal;
/* Immutable first observable failure: later restoration cannot overwrite it.
 * reason1 HAL I/O,2 readback mismatch,3 telemetry bound. */
static void refuse(uint32_t reason,uint32_t reg,uint32_t ev,uint32_t expected,
 uint32_t ov,uint32_t observed,uint32_t trace_index)
{
 if(tr2_config_first_refusal.valid)return;
 tr2_config_first_refusal.reason=reason;tr2_config_first_refusal.phase=tr2_burst_info.stage;
 tr2_config_first_refusal.reg=reg;tr2_config_first_refusal.expected_valid=ev;
 tr2_config_first_refusal.expected=expected;tr2_config_first_refusal.observed_valid=ov;
 tr2_config_first_refusal.observed=observed;tr2_config_first_refusal.trace_index=trace_index;
 if(trace_index<tr2_config_trace_count)tr2_config_first_refusal.io=tr2_config_traces[trace_index];
 tr2_config_first_refusal.valid=1U;
}
static void mismatch(uint32_t reg,uint32_t expected,uint32_t observed)
{
 uint32_t index=UINT32_MAX;
 for(uint32_t i=tr2_config_trace_count;i>0U;i--){
  volatile Stm32ConfigTrace *v=&tr2_config_traces[i-1U];
  if(v->method!=4U&&reg>=v->reg&&reg<v->reg+v->nbytes-1U){index=i-1U;break;}
 }
 refuse(2U,reg,1U,expected,1U,observed,index);
}
static const uint8_t regs[7]={7U,8U,9U,10U,16U,18U,20U};
static SPI_HandleTypeDef *bus;
static GPIO_TypeDef *port;
static uint16_t pin;
static uint8_t tx[TR2_BURST_BYTES],rx[TR2_BURST_BYTES];
static Tr2Result io(uint8_t reg,uint8_t *data,uint16_t n,int write,Stm32BurstMeta *meta)
{
 HAL_StatusTypeDef status;
 Stm32ConfigTrace trace={0};
 uint32_t index=tr2_config_trace_count;
 if(index>=TR2_CONFIG_TRACE_COUNT){refuse(3U,reg,0U,UINT32_MAX,0U,UINT32_MAX,UINT32_MAX);return TR2_ERROR_INTERNAL;}
 if(n==0U||n+1U>TR2_BURST_BYTES||(write&&n!=1U))return TR2_ERROR_INVALID_ARGUMENT;
 memset(tx,0,sizeof(tx));memset(rx,0,sizeof(rx));tx[0]=write?reg:(uint8_t)(reg|128U);
 if(write)tx[1]=data[0];
 trace.phase=tr2_burst_info.stage;trace.method=write?4U:(reg==0x78U?3U:(n==1U?1U:2U));
 trace.reg=reg;trace.nbytes=n+1U;trace.raw_length=n+1U<8U?n+1U:8U;
 trace.cs_before=(port->ODR&pin)!=0U;trace.sr_before=bus->Instance->SR;
 trace.cfg1=bus->Instance->CFG1;trace.cfg2=bus->Instance->CFG2;
 trace.begin_cycles=DWT->CYCCNT;trace.valid_mask=1U;
 if(meta){meta->cs_before=(port->ODR&pin)!=0U;meta->cfg1=bus->Instance->CFG1;meta->cfg2=bus->Instance->CFG2;meta->begin_cycles=DWT->CYCCNT;}
 HAL_GPIO_WritePin(port,pin,GPIO_PIN_RESET);
 trace.cs_during=(port->ODR&pin)!=0U;
 if(meta)meta->cs_during=(port->ODR&pin)!=0U;
 status=HAL_SPI_TransmitReceive(bus,tx,rx,(uint16_t)(n+1U),10U);
 trace.sr_after=bus->Instance->SR;
 HAL_GPIO_WritePin(port,pin,GPIO_PIN_SET);
 trace.end_cycles=DWT->CYCCNT;trace.cs_after=(port->ODR&pin)!=0U;
 trace.hal_status=(uint32_t)status;trace.error=bus->ErrorCode;
 trace.rx_left=bus->RxXferCount;trace.tx_left=bus->TxXferCount;
 for(uint32_t j=0;j<trace.raw_length;j++){trace.tx[j]=tx[j];trace.rx[j]=rx[j];}
 trace.valid_mask|=2U|4U;
 int transfer_ok=status==HAL_OK&&trace.error==0U&&trace.rx_left==0U&&trace.tx_left==0U&&trace.cs_before==1U&&trace.cs_during==0U&&trace.cs_after==1U;
 if(transfer_ok&&!write)trace.valid_mask|=8U;
 tr2_config_traces[index]=trace;tr2_config_trace_count=index+1U;
 if(meta){meta->cs_after=(port->ODR&pin)!=0U;meta->end_cycles=DWT->CYCCNT;meta->hal_status=(uint32_t)status;meta->error=bus->ErrorCode;meta->rx_left=bus->RxXferCount;meta->tx_left=bus->TxXferCount;}
 if(!transfer_ok){refuse(1U,reg,write?1U:0U,write?tx[1]:UINT32_MAX,0U,UINT32_MAX,index);tr2_burst_info.errors++;return status==HAL_TIMEOUT?TR2_ERROR_UNAVAILABLE:TR2_ERROR_INTERNAL;}
 if(!write&&data!=NULL)memcpy(data,rx+1U,n);
 return TR2_OK;
}
static Tr2Result write_reg(uint8_t reg,uint8_t value)
{
 int allowed=0;for(unsigned i=0;i<7U;i++)if(reg==regs[i])allowed=1;
 return allowed?io(reg,&value,1U,1,NULL):TR2_ERROR_UNSUPPORTED;
}
/* Two grouped readbacks; no single control read on admission or restoration.
 * WHO/IF_INC/CTRL4 anchor coherence; reserved0x11 is captured, not interpreted. */
static Tr2Result controls(uint32_t *values)
{
 uint8_t data[6];Tr2Result r=io(7U,data,4U,0,NULL);
 if(r!=TR2_OK)return r;
 for(unsigned i=0;i<4U;i++)values[i]=data[i];
 r=io(15U,data,6U,0,NULL);if(r!=TR2_OK)return r;
 if(data[0]!=0x7BU){mismatch(15U,0x7BU,data[0]);tr2_burst_info.loss_flags|=4U;return TR2_ERROR_INTERNAL;}
 if((data[3]&4U)==0U){mismatch(18U,tr2_burst_info.stage==1U||tr2_burst_info.stage==5U?4U:0x44U,data[3]);tr2_burst_info.loss_flags|=4U;return TR2_ERROR_INTERNAL;}
 if(data[4]!=0U){mismatch(19U,0U,data[4]);tr2_burst_info.loss_flags|=4U;return TR2_ERROR_INTERNAL;}
 values[4]=data[1];values[5]=data[3];values[6]=data[5];
 return TR2_OK;
}
static Tr2Result audit_grouped(void)
{
 static const uint8_t starts[4]={0x0D,0x15,0x18,0x73};
 static const uint8_t lengths[4]={3U,3U,2U,3U};
 for(unsigned i=0;i<4U;i++){
  uint8_t data[3];Tr2Result r=io(starts[i],data,lengths[i],0,NULL);if(r!=TR2_OK)return r;
  for(unsigned j=0;j<lengths[i];j++){
   if(i==2U&&j==0U)continue; /* Reserved0x18, no invented expected value. */
   uint8_t expected=i==0U&&j==2U?0x7BU:0U;
   if(data[j]!=expected){mismatch((uint8_t)(starts[i]+j),expected,data[j]);return TR2_ERROR_UNSUPPORTED;}
  }
 }
 return TR2_OK;
}
static Tr2Result status_read(uint32_t *status)
{
 uint8_t bytes[2];Tr2Result r=io(0x3AU,bytes,2U,0,NULL);
 if(r==TR2_OK){*status=bytes[0]|((uint32_t)bytes[1]<<8U);if(*status&0x4800U)tr2_burst_info.loss_flags|=1U;}
 return r;
}
static unsigned parity(uint8_t tag){unsigned p=0;for(unsigned i=0;i<8;i++)p^=(tag>>i)&1U;return p;}
__attribute__((noreturn)) void stm32_iis3dwb_burst_capture_run(SPI_HandleTypeDef *spi,GPIO_TypeDef *gpio,uint16_t cs)
{
 uint32_t saved[7]={0},actual[7]={0},status=0U,previous_level=0U,last_counter=0U;
 int have_saved=0,changed=0;
 Tr2Result result=TR2_OK,restored=TR2_ERROR_NOT_AVAILABLE;
 bus=spi;port=gpio;pin=cs;tr2_burst_info.stage=1U;
 while(tr2_burst_trigger!=0x4D313546U)HAL_Delay(10U);
 tr2_burst_trigger=0U;
 CoreDebug->DEMCR|=CoreDebug_DEMCR_TRCENA_Msk;
 if(DWT->CTRL&(1UL<<25)){result=TR2_ERROR_UNSUPPORTED;goto finish;}
 DWT->CYCCNT=0U;DWT->CTRL|=DWT_CTRL_CYCCNTENA_Msk;
 {uint32_t before=DWT->CYCCNT;HAL_Delay(2U);if(before==DWT->CYCCNT){result=TR2_ERROR_UNAVAILABLE;goto finish;}}
 tr2_burst_info.core_hz=HAL_RCC_GetHCLKFreq();tr2_burst_info.spi_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3)/64U;
 if(tr2_burst_info.core_hz!=160000000U||tr2_burst_info.spi_hz!=2500000U){result=TR2_ERROR_UNSUPPORTED;goto finish;}
 result=controls(saved);if(result!=TR2_OK)goto finish;
 for(unsigned i=0;i<7;i++){tr2_burst_info.saved[i]=saved[i];if(saved[i]!=(i==5U?4U:0U)){mismatch(regs[i],i==5U?4U:0U,saved[i]);result=TR2_ERROR_UNSUPPORTED;goto finish;}}
 have_saved=1;
 result=audit_grouped();if(result!=TR2_OK)goto finish;
 result=status_read(&status);if(result!=TR2_OK||status!=0U){result=TR2_ERROR_INVALID_STATE;goto finish;}
 changed=1;tr2_burst_info.stage=2U;
 {const uint8_t values[7]={0U,0x81U,0x0AU,0U,0xA8U,0x44U,0U};const uint8_t order[7]={5U,4U,0U,1U,2U,3U,6U};
  for(unsigned j=0;j<7;j++){unsigned i=order[j];result=write_reg(regs[i],values[i]);if(result!=TR2_OK)goto finish;}
  result=controls(actual);if(result!=TR2_OK)goto finish;
  for(unsigned i=0;i<7;i++)if(actual[i]!=values[i]){mismatch(regs[i],values[i],actual[i]);tr2_burst_info.loss_flags|=4U;result=TR2_ERROR_INTERNAL;goto finish;}
 }
 HAL_Delay(100U);
 tr2_burst_info.start_cycles=DWT->CYCCNT;result=write_reg(10U,1U);if(result!=TR2_OK)goto finish;
 tr2_burst_info.stage=3U;
 {uint32_t start=HAL_GetTick();do{
   Stm32BurstPoll record={DWT->CYCCNT,0U,0U,0U};result=status_read(&status);record.end_cycles=DWT->CYCCNT;record.status=status;record.level=status&0x03FFU;
   unsigned i=tr2_burst_info.poll_count;tr2_burst_polls[i]=record;tr2_burst_info.poll_count=i+1U;
   if(result!=TR2_OK)goto finish;
   if(record.level<previous_level)tr2_burst_info.loss_flags|=2U;
   previous_level=record.level;
   if(tr2_burst_info.loss_flags){result=TR2_ERROR_INVALID_STATE;goto finish;}
   if(status&0x8000U)break;
   if(i+1U>=TR2_BURST_POLLS||(uint32_t)(HAL_GetTick()-start)>=20U){result=TR2_ERROR_UNAVAILABLE;goto finish;}
   HAL_Delay(1U);
  }while(1);
 }
 tr2_burst_info.filled_status=status;
 if((status&0x03FFU)!=256U){tr2_burst_info.loss_flags|=32U;result=TR2_ERROR_INVALID_STATE;goto finish;}
 /* Freeze input before draining; no continuous acquisition claim. */
 result=write_reg(16U,0U);if(result!=TR2_OK)goto finish;HAL_Delay(2U);
 result=controls(actual);if(result!=TR2_OK)goto finish;
 {const uint32_t expected[7]={0U,0x81U,0x0AU,1U,0U,0x44U,0U};
  for(unsigned i=0;i<7;i++){tr2_burst_info.configured[i]=actual[i];if(actual[i]!=expected[i]){mismatch(regs[i],expected[i],actual[i]);tr2_burst_info.loss_flags|=4U;result=TR2_ERROR_INTERNAL;goto finish;}}
 }
 result=status_read(&status);tr2_burst_info.stopped_status=status;
 if(result!=TR2_OK||(status&0x03FFU)!=256U||tr2_burst_info.loss_flags){result=TR2_ERROR_INVALID_STATE;goto finish;}
 tr2_burst_info.stage=4U;
 for(unsigned burst=0U;burst<2U;burst++){
  Stm32BurstMeta meta={0};meta.status_before=status;
  result=io(0x78U,NULL,896U,0,&meta);
  /* io's internal rx also contains command byte and partial data on HAL error. */
  for(unsigned j=0;j<TR2_BURST_BYTES;j++)tr2_burst_rx[burst][j]=rx[j];
  tr2_burst_meta[burst]=meta;tr2_burst_info.burst_count=burst+1U;
  if(result!=TR2_OK)goto finish;
  if(meta.error||meta.rx_left||meta.tx_left||meta.cs_before!=1U||meta.cs_during!=0U||meta.cs_after!=1U){tr2_burst_info.loss_flags|=128U;result=TR2_ERROR_INTERNAL;goto finish;}
  for(unsigned word=0U;word<128U;word++){
   unsigned pos=1U+word*7U;uint8_t tag=rx[pos];uint32_t counter=(tag>>1U)&3U;
   if((tag>>3U)!=2U){tr2_burst_info.tag_errors++;tr2_burst_info.loss_flags|=8U;}
   if(parity(tag)){tr2_burst_info.parity_errors++;tr2_burst_info.loss_flags|=16U;}
   if(tr2_burst_info.word_count>0U&&counter!=((last_counter+1U)&3U)){tr2_burst_info.gap_events++;tr2_burst_info.loss_flags|=64U;}
   last_counter=counter;
   for(unsigned axis=0;axis<3;axis++){int16_t v=(int16_t)(rx[pos+1U+2U*axis]|((uint16_t)rx[pos+2U+2U*axis]<<8U));if(v==INT16_MIN||v==INT16_MAX)tr2_burst_info.saturations++;}
   tr2_burst_info.word_count++;
  }
  result=status_read(&status);meta.status_after=status;tr2_burst_meta[burst]=meta;
  if(result!=TR2_OK||(status&0x03FFU)!=(burst==0U?128U:0U)){tr2_burst_info.loss_flags|=32U;result=TR2_ERROR_INVALID_STATE;goto finish;}
  if(tr2_burst_info.loss_flags){result=TR2_ERROR_INVALID_STATE;goto finish;}
 }
 tr2_burst_info.final_status=status;tr2_burst_info.end_cycles=DWT->CYCCNT;
 if(tr2_burst_info.word_count==256U&&tr2_burst_info.loss_flags==0U)tr2_burst_info.accepted=1U;
finish:
 tr2_burst_info.result=(uint32_t)result;tr2_burst_info.stage=5U;
 if(have_saved){restored=TR2_OK;
  if(changed){const uint8_t order[7]={4U,3U,2U,1U,0U,6U,5U};for(unsigned j=0;j<7;j++){unsigned i=order[j];if(write_reg(regs[i],(uint8_t)saved[i])!=TR2_OK)restored=TR2_ERROR_INTERNAL;}}
  if(controls(actual)!=TR2_OK)restored=TR2_ERROR_INTERNAL;
  for(unsigned i=0;i<7;i++){tr2_burst_info.restored[i]=actual[i];if(actual[i]!=saved[i])restored=TR2_ERROR_INTERNAL;}
 }
 tr2_burst_info.restore_result=(uint32_t)restored;if(restored!=TR2_OK)tr2_burst_info.accepted=0U;
 tr2_burst_info.stage=10U;for(;;)HAL_Delay(100U);
}
#else
typedef int Tr2BurstCaptureDisabled;
#endif
