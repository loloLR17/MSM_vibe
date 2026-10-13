/* Adapted from ST NUCLEO-U575ZI-Q GPIO_IOToggle, CubeU5 v1.9.0.
 * Copyright (c) 2021 STMicroelectronics; vendor/LICENSE.examples.md.
 * Local changes: only PB15/PB3/PB4, stable debug-controlled finite sequence,
 * no LED toggles or other GPIO changes. SD card MUST be physically removed.
 */
#include "stm32u5xx_hal.h"
#define MASK (GPIO_PIN_15|GPIO_PIN_3|GPIO_PIN_4)
#define COMMAND_BASE 0x47310000U
#define STOP 0x4731FFFFU
#define DWELL_MS 20000U
#define HIGH_TIMEOUT_MS 300000U
volatile uint32_t gpio_stage, gpio_phase, gpio_command, gpio_output_mask;
volatile uint32_t gpio_phase_started_ms, gpio_error, gpio_initialized;
volatile uint32_t gpio_uid[3];
void SystemClock_Config(void);
void SysTick_Handler(void) { HAL_IncTick(); }
static void all_low(void) {
  if(gpio_initialized) HAL_GPIO_WritePin(GPIOB,MASK,GPIO_PIN_RESET);
  gpio_output_mask=0;
}
void Error_Handler(void) { all_low(); gpio_stage=99; while(1) {} }
static void phase_set(uint32_t phase) {
  all_low();
  uint32_t pin=phase==1?GPIO_PIN_15:phase==3?GPIO_PIN_3:phase==5?GPIO_PIN_4:0;
  if(pin) HAL_GPIO_WritePin(GPIOB,(uint16_t)pin,GPIO_PIN_SET);
  gpio_output_mask=pin;
  gpio_phase=phase;
  gpio_phase_started_ms=HAL_GetTick();
}
int main(void) {
  gpio_stage=1;
  for(unsigned i=0;i<3;i++) gpio_uid[i]=*(volatile uint32_t *)(0x0BFA0700U+i*4U);
  if(gpio_uid[0]!=0x001B0046U || gpio_uid[1]!=0x38425018U || gpio_uid[2]!=0x20333157U) { gpio_error=1; Error_Handler(); }
  HAL_Init();
  HAL_PWREx_DisableUCPDDeadBattery();
  if(HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY)!=HAL_OK) { gpio_error=2; Error_Handler(); }
  SystemClock_Config();
  if(DBGMCU->CR&DBGMCU_CR_TRACE_IOEN) { gpio_error=3; Error_Handler(); }
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /* Set output latch low BEFORE selecting output mode. Other pins untouched. */
  HAL_GPIO_WritePin(GPIOB,MASK,GPIO_PIN_RESET);
  GPIO_InitTypeDef config={0};
  config.Pin=MASK; config.Mode=GPIO_MODE_OUTPUT_PP;
  config.Pull=GPIO_NOPULL; config.Speed=GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB,&config);
  gpio_initialized=1; phase_set(0); gpio_stage=2;
  while(gpio_phase<6) {
    uint32_t command=gpio_command;
    uint32_t age=HAL_GetTick()-gpio_phase_started_ms;
    if(command==STOP || (gpio_output_mask && age>=HIGH_TIMEOUT_MS)) {
      gpio_command=0; phase_set(6); gpio_stage=4; break;
    }
    if(command && command!=(COMMAND_BASE| (gpio_phase+1))) {
      gpio_error=4; gpio_command=0; Error_Handler();
    }
    if(command && age>=DWELL_MS) {
      gpio_command=0; phase_set(gpio_phase+1);
    }
    HAL_Delay(10);
  }
  all_low(); gpio_stage=4;
  while(1) {}
}
