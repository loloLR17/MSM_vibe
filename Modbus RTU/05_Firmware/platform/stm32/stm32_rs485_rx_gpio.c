#include "stm32u5xx_hal.h"
#include "stm32_rs485_rx_gpio.h"

void stm32_rs485_rx_gpio_init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    /* Preload the latch before selecting output mode, while PG4 is isolated
     * after reset. PG5 is unconnected and must remain untouched.
     */
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_RESET);
    gpio.Pin = GPIO_PIN_4;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_PULLDOWN;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOG, &gpio);

    /* Board rail presence is confirmed by mission 008, not measured by HAL.
     * Release the entire PG2..PG15 domain only after DE's low configuration.
     */
    HAL_PWREx_EnableVddIO2();
}
