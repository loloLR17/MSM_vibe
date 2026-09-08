#include "stm32u5xx_hal.h"

#define TR2_BRINGUP_LED_PORT GPIOC
#define TR2_BRINGUP_LED_PIN  GPIO_PIN_7

static void SystemClock_Config(void);
static void SystemPower_Config(void);
static void BringupLed_Init(void);
static void Error_Handler(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    SystemPower_Config();
    BringupLed_Init();

    for (;;) {
        HAL_GPIO_TogglePin(TR2_BRINGUP_LED_PORT, TR2_BRINGUP_LED_PIN);
        HAL_Delay(250U);
    }
}

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
        Error_Handler();
    }

    osc.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    osc.MSIState = RCC_MSI_ON;
    osc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    osc.MSIClockRange = RCC_MSIRANGE_4;
    osc.PLL.PLLState = RCC_PLL_ON;
    osc.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    osc.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
    osc.PLL.PLLM = 1U;
    osc.PLL.PLLN = 80U;
    osc.PLL.PLLP = 2U;
    osc.PLL.PLLQ = 2U;
    osc.PLL.PLLR = 2U;
    osc.PLL.PLLRGE = RCC_PLLVCIRANGE_0;
    osc.PLL.PLLFRACN = 0U;

    if (HAL_RCC_OscConfig(&osc) != HAL_OK) {
        Error_Handler();
    }

    clk.ClockType = RCC_CLOCKTYPE_HCLK |
                    RCC_CLOCKTYPE_SYSCLK |
                    RCC_CLOCKTYPE_PCLK1 |
                    RCC_CLOCKTYPE_PCLK2 |
                    RCC_CLOCKTYPE_PCLK3;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV1;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;
    clk.APB3CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_4) != HAL_OK) {
        Error_Handler();
    }
}

static void SystemPower_Config(void)
{
    HAL_PWREx_DisableUCPDDeadBattery();

    if (HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY) != HAL_OK) {
        Error_Handler();
    }
}

static void BringupLed_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();

    HAL_GPIO_WritePin(TR2_BRINGUP_LED_PORT, TR2_BRINGUP_LED_PIN, GPIO_PIN_RESET);

    gpio.Pin = TR2_BRINGUP_LED_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(TR2_BRINGUP_LED_PORT, &gpio);
}

static void Error_Handler(void)
{
    __disable_irq();
    for (;;) {
    }
}
