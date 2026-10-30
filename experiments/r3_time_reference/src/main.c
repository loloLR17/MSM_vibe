/* TR2-002 only. No sensor/SD/FRAM/RTU access. No periodic transmission.
 * USART1 PA9/PA10 is the MB1549 documented default STLINK VCP route. */
#include "stm32u5xx_hal.h"
#include "reference_frame.h"
#include "endurance_clock.h"
void SystemClock_Config(void);
typedef struct {uint32_t stage,error,uid[3],core_nominal,sequence,uart_errors;uint64_t cycles;} Probe;
volatile Probe time_probe;
static uint32_t previous;static uint64_t accumulated;
static uint64_t sample(void){uint32_t c=DWT->CYCCNT;accumulated=durable_elapsed_cycles(accumulated,c,previous);previous=c;return accumulated;}
void HAL_MspInit(void){__HAL_RCC_PWR_CLK_ENABLE();}
void SysTick_Handler(void){HAL_IncTick();}
void Error_Handler(void){if(!time_probe.error)time_probe.error=13;time_probe.stage=99;for(;;)__WFI();}
static void fail(uint32_t e){time_probe.error=e;Error_Handler();}
static void uart_init(void)
{
 __HAL_RCC_GPIOA_CLK_ENABLE();__HAL_RCC_USART1_CLK_ENABLE();
 __HAL_RCC_USART1_FORCE_RESET();__HAL_RCC_USART1_RELEASE_RESET();
 RCC_PeriphCLKInitTypeDef clock={0};clock.PeriphClockSelection=RCC_PERIPHCLK_USART1;clock.Usart1ClockSelection=RCC_USART1CLKSOURCE_PCLK2;
 if(HAL_RCCEx_PeriphCLKConfig(&clock)!=HAL_OK)fail(3);
 GPIO_InitTypeDef g={0};g.Pin=GPIO_PIN_9|GPIO_PIN_10;g.Mode=GPIO_MODE_AF_PP;g.Pull=GPIO_PULLUP;g.Speed=GPIO_SPEED_FREQ_LOW;g.Alternate=GPIO_AF7_USART1;HAL_GPIO_Init(GPIOA,&g);
 USART1->CR1=0;USART1->CR2=0;USART1->CR3=0;USART1->PRESC=0;
 USART1->BRR=(HAL_RCC_GetPCLK2Freq()+57600u)/115200u;
 USART1->CR1=USART_CR1_TE|USART_CR1_RE|USART_CR1_UE;
 uint32_t start=HAL_GetTick();while((USART1->ISR&(USART_ISR_TEACK|USART_ISR_REACK))!=(USART_ISR_TEACK|USART_ISR_REACK)){(void)sample();if(HAL_GetTick()-start>10)fail(4);}
}
static void transmit(const uint8_t b[32])
{
 uint32_t start=HAL_GetTick();
 for(unsigned j=0;j<32;j++){
  while(!(USART1->ISR&USART_ISR_TXE_TXFNF)){(void)sample();if(HAL_GetTick()-start>100)fail(5);}
  USART1->TDR=b[j];
 }
 while(!(USART1->ISR&USART_ISR_TC)){(void)sample();if(HAL_GetTick()-start>100)fail(6);}
}
int main(void)
{
 time_probe.uid[0]=HAL_GetUIDw0();time_probe.uid[1]=HAL_GetUIDw1();time_probe.uid[2]=HAL_GetUIDw2();
 if(time_probe.uid[0]!=0x001b0046||time_probe.uid[1]!=0x38425018||time_probe.uid[2]!=0x20333157)fail(1);
 if(HAL_Init()!=HAL_OK)fail(2);
 HAL_PWREx_DisableUCPDDeadBattery();
 if(HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY)!=HAL_OK)fail(12);
 SystemClock_Config();
 time_probe.core_nominal=HAL_RCC_GetHCLKFreq();if(time_probe.core_nominal!=160000000u)fail(7);
 CoreDebug->DEMCR|=CoreDebug_DEMCR_TRCENA_Msk;if(DWT->CTRL&DWT_CTRL_NOCYCCNT_Msk)fail(8);DWT->CYCCNT=0;DWT->CTRL|=DWT_CTRL_CYCCNTENA_Msk;previous=DWT->CYCCNT;
 uart_init();time_probe.stage=2;
 for(;;){
  time_probe.cycles=sample();
  if(USART1->ISR&(USART_ISR_ORE|USART_ISR_NE|USART_ISR_FE|USART_ISR_PE)){time_probe.uart_errors++;fail(9);}
  if(USART1->ISR&USART_ISR_RXNE_RXFNE){
   uint8_t c=(uint8_t)USART1->RDR;
   if(c=='Q'){
    if(time_probe.sequence==UINT32_MAX)fail(10);
    uint32_t uid[3]={time_probe.uid[0],time_probe.uid[1],time_probe.uid[2]};uint8_t frame[32];
    uint64_t at=sample();reference_frame(frame,uid,++time_probe.sequence,at);transmit(frame);
   }else fail(11);
  }
  __WFI(); /* SysTick samples DWT well within26.84s wrap period. */
 }
}
