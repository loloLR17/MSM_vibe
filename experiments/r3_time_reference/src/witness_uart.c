/* One byte transmitted per service call, no wait during capture, no IRQ. */
#include "stm32u5xx_hal.h"
#include "witness_uart.h"
uint32_t witness_sequence,witness_frames,witness_errors;
static uint8_t packet[48];static uint32_t position=48;
int witness_uart_init(void)
{
 __HAL_RCC_GPIOA_CLK_ENABLE();__HAL_RCC_USART1_CLK_ENABLE();__HAL_RCC_USART1_FORCE_RESET();__HAL_RCC_USART1_RELEASE_RESET();
 RCC_PeriphCLKInitTypeDef c={0};c.PeriphClockSelection=RCC_PERIPHCLK_USART1;c.Usart1ClockSelection=RCC_USART1CLKSOURCE_PCLK2;
 if(HAL_RCCEx_PeriphCLKConfig(&c)!=HAL_OK)return -1;
 GPIO_InitTypeDef g={0};g.Pin=GPIO_PIN_9|GPIO_PIN_10;g.Mode=GPIO_MODE_AF_PP;g.Pull=GPIO_PULLUP;g.Speed=GPIO_SPEED_FREQ_LOW;g.Alternate=GPIO_AF7_USART1;HAL_GPIO_Init(GPIOA,&g);
 USART1->CR1=0;USART1->CR2=0;USART1->CR3=0;USART1->PRESC=0;USART1->BRR=(HAL_RCC_GetPCLK2Freq()+57600u)/115200u;USART1->CR1=USART_CR1_TE|USART_CR1_RE|USART_CR1_UE;
 uint32_t start=HAL_GetTick();while((USART1->ISR&(USART_ISR_TEACK|USART_ISR_REACK))!=(USART_ISR_TEACK|USART_ISR_REACK))if(HAL_GetTick()-start>10)return -2;
 return 0;
}
int witness_uart_step(const uint32_t uid[3],uint32_t flags,uint64_t us,uint64_t session,uint32_t accepted)
{
 if(USART1->ISR&(USART_ISR_ORE|USART_ISR_NE|USART_ISR_FE|USART_ISR_PE)){witness_errors++;return -1;}
 if(USART1->ISR&USART_ISR_RXNE_RXFNE){
  uint8_t c=(uint8_t)USART1->RDR;
  if(c!='Q'||position!=48||witness_sequence==UINT32_MAX){witness_errors++;return -2;}
  witness_frame(packet,uid,++witness_sequence,us,flags,session,accepted);position=0;
 }
 if(position<48&&(USART1->ISR&USART_ISR_TXE_TXFNF)){USART1->TDR=packet[position++];if(position==48)witness_frames++;}
 return 0;
}
