#include "stm32u5xx_hal.h"
#include "stm32_serial_transport.h"

void SysTick_Handler(void)
{
    HAL_IncTick();
}

void LPUART1_IRQHandler(void)
{
    stm32_serial_transport_irq_handler();
}

void TIM6_IRQHandler(void)
{
    stm32_serial_transport_tim6_irq_handler();
}
