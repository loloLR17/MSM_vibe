/* Pins: official ST stm32u5xx-nucleo-bsp/USE_NUCLEO_144 (MB1549).
 * PC13 is strictly an input. No UART, FRAM or native-USB stack. */
#include "standalone.h"
static bool standalone_leds_ready;
volatile uint32_t standalone_state,standalone_fs_readback;
static void standalone_led(unsigned state)
{
 if(!standalone_leds_ready)return;
 uint32_t phase=HAL_GetTick();
 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,(state==4||(state==2&&(phase%1000)<200))?GPIO_PIN_SET:GPIO_PIN_RESET);
 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,((state==1&&(phase%250)<125)||(state==3&&(phase%500)<250))?GPIO_PIN_SET:GPIO_PIN_RESET);
 HAL_GPIO_WritePin(GPIOG,GPIO_PIN_2,(state>=98&&(phase%300)<150)?GPIO_PIN_SET:GPIO_PIN_RESET);
}
static void standalone_gpio(void)
{
 __HAL_RCC_GPIOC_CLK_ENABLE();__HAL_RCC_GPIOB_CLK_ENABLE();__HAL_RCC_GPIOG_CLK_ENABLE();HAL_PWREx_EnableVddIO2();
 GPIO_InitTypeDef g={0};g.Pin=GPIO_PIN_7;g.Mode=GPIO_MODE_OUTPUT_PP;g.Speed=GPIO_SPEED_FREQ_LOW;g.Pull=GPIO_NOPULL;
 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET);HAL_GPIO_Init(GPIOC,&g);
 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET);HAL_GPIO_Init(GPIOB,&g);
 g.Pin=GPIO_PIN_2;HAL_GPIO_WritePin(GPIOG,GPIO_PIN_2,GPIO_PIN_RESET);HAL_GPIO_Init(GPIOG,&g);
 g.Pin=GPIO_PIN_13;g.Mode=GPIO_MODE_INPUT;g.Pull=GPIO_PULLDOWN;HAL_GPIO_Init(GPIOC,&g);
 /* Existing FRAM remains deselected; no SPI/FRAM transaction is issued. */
 __HAL_RCC_GPIOD_CLK_ENABLE();g.Pin=GPIO_PIN_14;g.Mode=GPIO_MODE_OUTPUT_PP;g.Pull=GPIO_PULLUP;
 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);HAL_GPIO_Init(GPIOD,&g);
 standalone_leds_ready=true;standalone_state=1;standalone_led(1);
}
static void standalone_heartbeat(void)
{static uint32_t last;uint32_t t=HAL_GetTick();if((uint32_t)(t-last)>=50){last=t;standalone_led(standalone_state);}}
