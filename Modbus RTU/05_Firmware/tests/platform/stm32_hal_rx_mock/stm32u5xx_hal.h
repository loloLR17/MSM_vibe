#ifndef TR2_TEST_STM32_HAL_RX_MOCK_H
#define TR2_TEST_STM32_HAL_RX_MOCK_H
#include <stdint.h>
/* Only the HAL surface used by the real board RX GPIO initializer. */
typedef struct { uint32_t odr; uint32_t outputs; uint32_t pulldowns; } GPIO_TypeDef;
typedef struct {
    uint32_t Pin;
    uint32_t Mode;
    uint32_t Pull;
    uint32_t Speed;
    uint32_t Alternate;
} GPIO_InitTypeDef;
typedef enum { GPIO_PIN_RESET = 0, GPIO_PIN_SET } GPIO_PinState;
#define GPIO_PIN_4 UINT32_C(0x0010)
#define GPIO_MODE_OUTPUT_PP UINT32_C(0x0001)
#define GPIO_PULLDOWN UINT32_C(0x0002)
#define GPIO_SPEED_FREQ_LOW UINT32_C(0x0000)
extern GPIO_TypeDef test_gpio_g;
#define GPIOG (&test_gpio_g)
void test_enable_pwr_clock(void);
void test_enable_gpio_g_clock(void);
#define __HAL_RCC_PWR_CLK_ENABLE() test_enable_pwr_clock()
#define __HAL_RCC_GPIOG_CLK_ENABLE() test_enable_gpio_g_clock()
void HAL_GPIO_WritePin(GPIO_TypeDef *port, uint16_t pins, GPIO_PinState state);
void HAL_GPIO_Init(GPIO_TypeDef *port, const GPIO_InitTypeDef *init);
void HAL_PWREx_EnableVddIO2(void);
#endif
