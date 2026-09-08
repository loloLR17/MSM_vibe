#include "stm32u5xx_hal.h"

void SysTick_Handler(void)
{
    HAL_IncTick();
}
