#include "stm32u5xx_hal.h"

#include "stm32_serial_transport.h"

#define TR2_BRINGUP_LED_PORT GPIOC
#define TR2_BRINGUP_LED_PIN  GPIO_PIN_7

#define TR2_FRAM_CS_PORT GPIOD
#define TR2_FRAM_CS_PIN  GPIO_PIN_14
#define TR2_FRAM_RDID_COMMAND 0x9FU
#define TR2_FRAM_RDID_SIZE 4U
#define TR2_FRAM_SPI_TIMEOUT_MS 10U

static const uint8_t tr2_fram_expected_device_id[TR2_FRAM_RDID_SIZE] = {
    0x04U, 0x7FU, 0x48U, 0x03U
};

static SPI_HandleTypeDef hspi1;

volatile HAL_StatusTypeDef tr2_fram_rdid_status = HAL_ERROR;
volatile uint8_t tr2_fram_device_id[TR2_FRAM_RDID_SIZE] = {0U};
volatile uint8_t tr2_fram_device_id_matches = 0U;

static void SystemClock_Config(void);
static void SystemPower_Config(void);
static void BringupLed_Init(void);
static void FramSpi_Init(void);
static HAL_StatusTypeDef Fram_ReadDeviceId(uint8_t device_id[TR2_FRAM_RDID_SIZE]);
static void Error_Handler(void);

void HAL_MspInit(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
}

int main(void)
{
    SerialTransport serial_transport;

    HAL_Init();
    SystemClock_Config();
    SystemPower_Config();
    BringupLed_Init();
    FramSpi_Init();

    {
        uint8_t device_id[TR2_FRAM_RDID_SIZE] = {0U};
        uint8_t matches = 1U;

        tr2_fram_rdid_status = Fram_ReadDeviceId(device_id);

        for (uint32_t i = 0U; i < TR2_FRAM_RDID_SIZE; ++i) {
            tr2_fram_device_id[i] = device_id[i];
            if (device_id[i] != tr2_fram_expected_device_id[i]) {
                matches = 0U;
            }
        }

        tr2_fram_device_id_matches =
            (tr2_fram_rdid_status == HAL_OK) ? matches : 0U;
    }

    if (stm32_serial_transport_init(&serial_transport) != TR2_OK) {
        Error_Handler();
    }

    if (serial_transport_start_receive(&serial_transport) != TR2_OK) {
        Error_Handler();
    }

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

static void FramSpi_Init(void)
{
    GPIO_InitTypeDef gpio = {0};
    RCC_PeriphCLKInitTypeDef periph_clk = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    HAL_GPIO_WritePin(TR2_FRAM_CS_PORT, TR2_FRAM_CS_PIN, GPIO_PIN_SET);

    gpio.Pin = TR2_FRAM_CS_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(TR2_FRAM_CS_PORT, &gpio);

    periph_clk.PeriphClockSelection = RCC_PERIPHCLK_SPI1;
    periph_clk.Spi1ClockSelection = RCC_SPI1CLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&periph_clk) != HAL_OK) {
        Error_Handler();
    }

    __HAL_RCC_SPI1_CLK_ENABLE();

    gpio.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &gpio);

    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 0x7U;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_08DATA;
    hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
    hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;
    hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
    hspi1.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
    hspi1.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;

    if (HAL_SPI_Init(&hspi1) != HAL_OK) {
        Error_Handler();
    }
}

static HAL_StatusTypeDef Fram_ReadDeviceId(uint8_t device_id[TR2_FRAM_RDID_SIZE])
{
    uint8_t command = TR2_FRAM_RDID_COMMAND;
    HAL_StatusTypeDef status;

    HAL_GPIO_WritePin(TR2_FRAM_CS_PORT, TR2_FRAM_CS_PIN, GPIO_PIN_RESET);

    status = HAL_SPI_Transmit(&hspi1, &command, 1U, TR2_FRAM_SPI_TIMEOUT_MS);
    if (status == HAL_OK) {
        status = HAL_SPI_Receive(&hspi1,
                                 device_id,
                                 TR2_FRAM_RDID_SIZE,
                                 TR2_FRAM_SPI_TIMEOUT_MS);
    }

    HAL_GPIO_WritePin(TR2_FRAM_CS_PORT, TR2_FRAM_CS_PIN, GPIO_PIN_SET);

    return status;
}

static void Error_Handler(void)
{
    __disable_irq();
    for (;;) {
    }
}
