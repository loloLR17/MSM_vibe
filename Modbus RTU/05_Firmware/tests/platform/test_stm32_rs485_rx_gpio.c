#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32u5xx_hal.h"
#include "stm32_rs485_rx_gpio.h"

GPIO_TypeDef test_gpio_g;
static unsigned step;
static bool io2sv;

void test_enable_pwr_clock(void) { assert(step++ == 0U); }
void test_enable_gpio_g_clock(void) { assert(step++ == 1U); }

void HAL_GPIO_WritePin(GPIO_TypeDef *port, uint16_t pins, GPIO_PinState state)
{
    assert(step++ == 2U);
    assert(port == GPIOG);
    assert(pins == GPIO_PIN_4);
    assert(state == GPIO_PIN_RESET); /* Never command DE high. */
    port->odr &= ~(uint32_t)pins;
}

void HAL_GPIO_Init(GPIO_TypeDef *port, const GPIO_InitTypeDef *init)
{
    assert(step++ == 3U);
    assert(port == GPIOG);
    assert(init->Pin == GPIO_PIN_4); /* No PG5 or UART pins configured here. */
    assert(init->Mode == GPIO_MODE_OUTPUT_PP);
    assert(init->Pull == GPIO_PULLDOWN);
    assert(init->Speed == GPIO_SPEED_FREQ_LOW);
    assert((port->odr & init->Pin) == 0U); /* Latch low BEFORE output mode. */
    port->outputs |= init->Pin;
    port->pulldowns |= init->Pin;
}

void HAL_PWREx_EnableVddIO2(void)
{
    assert(step++ == 4U);
    assert((test_gpio_g.odr & GPIO_PIN_4) == 0U);
    assert((test_gpio_g.outputs & GPIO_PIN_4) != 0U);
    io2sv = true;
}

static void check_init(bool previously_enabled)
{
    /* Start with stale high latch, to catch mode-before-preload regressions.
     * Sentinel bits represent all unrelated pins, including unconnected PG5.
     */
    const uint32_t others = UINT32_C(0xffff) & ~GPIO_PIN_4;
    test_gpio_g.odr = UINT32_C(0xffff);
    test_gpio_g.outputs = UINT32_C(0x0020);
    test_gpio_g.pulldowns = UINT32_C(0x0080);
    step = 0U;
    io2sv = previously_enabled;
    stm32_rs485_rx_gpio_init(); /* Actual production source, not a test copy. */
    assert(step == 5U && io2sv);
    assert(test_gpio_g.odr == others);
    assert(test_gpio_g.outputs == (UINT32_C(0x0020) | GPIO_PIN_4));
    assert(test_gpio_g.pulldowns == (UINT32_C(0x0080) | GPIO_PIN_4));
}

int main(void)
{
    check_init(false);
    check_init(true);
    puts("RX GPIO sequencing passed; no electrical timing claim");
    return 0;
}
