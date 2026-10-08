#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "stm32u5xx_hal.h"
#include "stm32_serial_transport.h"

#define TR2_SERIAL_EVENT_CAPACITY 64u
#define TR2_SERIAL_TX_CAPACITY    256u

#define TR2_LPUART_TX_PORT GPIOG
#define TR2_LPUART_TX_PIN  GPIO_PIN_7
#define TR2_LPUART_RX_PORT GPIOG
#define TR2_LPUART_RX_PIN  GPIO_PIN_8
#define TR2_LPUART_AF      GPIO_AF8_LPUART1

/* F3-A: Click J2 couples DE and /RE. One GPIO avoids output contention. */
#define TR2_RS485_DIRECTION_PIN GPIO_PIN_4

#define TR2_RTU_TIMER_HZ        1000000u
#define TR2_RTU_T1_5_US             750u
#define TR2_RTU_T3_5_US            1750u
#define TR2_RTU_T3_5_REMAINDER_US  1000u

typedef enum {
    TR2_RTU_TIMER_IDLE = 0,
    TR2_RTU_TIMER_WAIT_T1_5,
    TR2_RTU_TIMER_WAIT_T3_5
} Tr2RtuTimerPhase;

typedef struct {
    UART_HandleTypeDef uart;
    TIM_HandleTypeDef rtu_timer;
    volatile Tr2RtuTimerPhase rtu_timer_phase;
    uint8_t rx_byte;
    uint8_t tx_buffer[TR2_SERIAL_TX_CAPACITY];
    volatile bool tx_busy;
    bool rs485;
    SerialTransportEvent events[TR2_SERIAL_EVENT_CAPACITY];
    volatile uint32_t event_head;
    volatile uint32_t event_tail;
    volatile bool event_overflow_pending;
} Stm32SerialContext;

static Stm32SerialContext g_serial;

/* STM32-only observations. HAL status values, not portable Tr2Result codes. */
volatile uint32_t tr2_serial_uart_init_result = UINT32_MAX;
volatile uint32_t tr2_serial_rx_start_result = UINT32_MAX;
volatile uint32_t tr2_serial_tx_launch_result = UINT32_MAX;
volatile uint32_t tr2_serial_tx_length = 0u;
volatile uint32_t tr2_serial_tx_launch_count = 0u;
volatile uint32_t tr2_serial_tx_complete_count = 0u;
volatile uint32_t tr2_serial_rx_complete_count = 0u;
volatile uint32_t tr2_serial_uart_error_count = 0u;
volatile uint32_t tr2_serial_uart_error_code = 0u;
volatile uint32_t tr2_serial_gpio_base = 0u;
volatile uint32_t tr2_serial_uart_base = 0u;

__attribute__((noinline)) void Stm32Rs485DeAsserted(void) { __NOP(); }
__attribute__((noinline)) void Stm32Rs485TxLaunched(void) { __NOP(); }
__attribute__((noinline)) void Stm32Rs485TxCompleted(void) { __NOP(); }


static bool event_push_from_isr(SerialTransportEvent event)
{
    const uint32_t head = g_serial.event_head;
    const uint32_t next = (head + 1u) % TR2_SERIAL_EVENT_CAPACITY;

    if (next == g_serial.event_tail) {
        g_serial.event_overflow_pending = true;
        return false;
    }

    g_serial.events[head] = event;
    g_serial.event_head = next;
    return true;
}

static void rtu_timer_stop(void)
{
    __HAL_TIM_DISABLE_IT(&g_serial.rtu_timer, TIM_IT_UPDATE);
    __HAL_TIM_DISABLE(&g_serial.rtu_timer);
    __HAL_TIM_SET_COUNTER(&g_serial.rtu_timer, 0u);
    __HAL_TIM_CLEAR_FLAG(&g_serial.rtu_timer, TIM_FLAG_UPDATE);
}

static void rtu_timer_start_period(uint32_t period_us)
{
    rtu_timer_stop();
    __HAL_TIM_SET_AUTORELOAD(&g_serial.rtu_timer, period_us - 1u);
    __HAL_TIM_SET_COUNTER(&g_serial.rtu_timer, 0u);
    __HAL_TIM_CLEAR_FLAG(&g_serial.rtu_timer, TIM_FLAG_UPDATE);
    __HAL_TIM_ENABLE_IT(&g_serial.rtu_timer, TIM_IT_UPDATE);
    __HAL_TIM_ENABLE(&g_serial.rtu_timer);
}

static void rtu_timer_restart_from_byte(void)
{
    g_serial.rtu_timer_phase = TR2_RTU_TIMER_WAIT_T1_5;
    rtu_timer_start_period(TR2_RTU_T1_5_US);
}

static Tr2Result stm32_start_receive(void *context)
{
    Stm32SerialContext *serial = (Stm32SerialContext *)context;

    if (serial == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    HAL_StatusTypeDef status = HAL_UART_Receive_IT(&serial->uart, &serial->rx_byte, 1u);
    tr2_serial_rx_start_result = (uint32_t)status;
    if (status != HAL_OK) {
        return TR2_ERROR_UNAVAILABLE;
    }

    return TR2_OK;
}

static Tr2Result stm32_transmit(void *context, const uint8_t *data, size_t length)
{
    Stm32SerialContext *serial = (Stm32SerialContext *)context;

    if (serial == NULL || data == NULL || length == 0u || length > TR2_SERIAL_TX_CAPACITY) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    if (serial->tx_busy) {
        return TR2_ERROR_UNAVAILABLE;
    }

    tr2_serial_tx_length = (uint32_t)length;
    memcpy(serial->tx_buffer, data, length);
    serial->tx_busy = true;
    if (serial->rs485) {
        /* Disable receiver and enable driver before the first UART start bit.
           Conservative bring-up guard; no throughput/latency guarantee. */
        HAL_GPIO_WritePin(GPIOG, TR2_RS485_DIRECTION_PIN, GPIO_PIN_SET);
        Stm32Rs485DeAsserted();
        HAL_Delay(1u);
    }

    HAL_StatusTypeDef status = HAL_UART_Transmit_IT(&serial->uart, serial->tx_buffer,
                                                  (uint16_t)length);
    tr2_serial_tx_launch_result = (uint32_t)status;
    if (status != HAL_OK) {
        if (serial->rs485) {
            HAL_GPIO_WritePin(GPIOG, TR2_RS485_DIRECTION_PIN, GPIO_PIN_RESET);
        }
        serial->tx_busy = false;
        return TR2_ERROR_UNAVAILABLE;
    }

    if (tr2_serial_tx_launch_count != UINT32_MAX) ++tr2_serial_tx_launch_count;
    if (serial->rs485) Stm32Rs485TxLaunched();
    return TR2_OK;
}

static bool stm32_poll_event(void *context, SerialTransportEvent *event)
{
    Stm32SerialContext *serial = (Stm32SerialContext *)context;
    uint32_t tail;

    if (serial == NULL || event == NULL) {
        return false;
    }

    if (serial->event_overflow_pending) {
        uint32_t primask = __get_PRIMASK();

        __disable_irq();
        if (serial->event_overflow_pending) {
            serial->event_tail = serial->event_head;
            serial->event_overflow_pending = false;
        }
        if (primask == 0u) {
            __enable_irq();
        }

        event->type = SERIAL_TRANSPORT_EVENT_ERROR;
        event->byte = 0u;
        event->error = SERIAL_TRANSPORT_ERROR_OVERRUN;
        return true;
    }

    tail = serial->event_tail;
    if (tail != serial->event_head) {
        *event = serial->events[tail];
        serial->event_tail = (tail + 1u) % TR2_SERIAL_EVENT_CAPACITY;
        return true;
    }

    event->type = SERIAL_TRANSPORT_EVENT_NONE;
    event->byte = 0u;
    event->error = SERIAL_TRANSPORT_ERROR_UNSPECIFIED;
    return true;
}

static SerialTransportError map_uart_error(uint32_t error)
{
    if ((error & HAL_UART_ERROR_ORE) != 0u) {
        return SERIAL_TRANSPORT_ERROR_OVERRUN;
    }
    if ((error & HAL_UART_ERROR_FE) != 0u) {
        return SERIAL_TRANSPORT_ERROR_FRAMING;
    }
    if ((error & HAL_UART_ERROR_PE) != 0u) {
        return SERIAL_TRANSPORT_ERROR_PARITY;
    }
    if ((error & HAL_UART_ERROR_NE) != 0u) {
        return SERIAL_TRANSPORT_ERROR_NOISE;
    }
    return SERIAL_TRANSPORT_ERROR_UNSPECIFIED;
}

static Tr2Result rtu_timer_init(void)
{
    RCC_ClkInitTypeDef clock_config = {0};
    uint32_t flash_latency = 0u;
    uint32_t pclk1_hz;
    uint32_t timer_clock_hz;
    uint32_t prescaler;

    __HAL_RCC_TIM6_CLK_ENABLE();

    HAL_RCC_GetClockConfig(&clock_config, &flash_latency);
    pclk1_hz = HAL_RCC_GetPCLK1Freq();
    timer_clock_hz = (clock_config.APB1CLKDivider == RCC_HCLK_DIV1) ? pclk1_hz : (2u * pclk1_hz);

    if (timer_clock_hz < TR2_RTU_TIMER_HZ || (timer_clock_hz % TR2_RTU_TIMER_HZ) != 0u) {
        return TR2_ERROR_UNAVAILABLE;
    }

    prescaler = (timer_clock_hz / TR2_RTU_TIMER_HZ) - 1u;
    if (prescaler > 0xFFFFu) {
        return TR2_ERROR_UNAVAILABLE;
    }

    g_serial.rtu_timer.Instance = TIM6;
    g_serial.rtu_timer.Init.Prescaler = prescaler;
    g_serial.rtu_timer.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_serial.rtu_timer.Init.Period = TR2_RTU_T1_5_US - 1u;
    g_serial.rtu_timer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&g_serial.rtu_timer) != HAL_OK) {
        return TR2_ERROR_UNAVAILABLE;
    }

    g_serial.rtu_timer_phase = TR2_RTU_TIMER_IDLE;
    HAL_NVIC_SetPriority(TIM6_IRQn, 5u, 0u);
    HAL_NVIC_EnableIRQ(TIM6_IRQn);
    return TR2_OK;
}

static Tr2Result serial_init(SerialTransport *transport, bool rs485)
{
    GPIO_InitTypeDef gpio = {0};

    if (transport == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(&g_serial, 0, sizeof(g_serial));
    tr2_serial_uart_init_result = UINT32_MAX;
    tr2_serial_rx_start_result = UINT32_MAX;
    tr2_serial_tx_launch_result = UINT32_MAX;
    tr2_serial_tx_length = 0u;
    tr2_serial_tx_launch_count = 0u;
    tr2_serial_tx_complete_count = 0u;
    tr2_serial_rx_complete_count = 0u;
    tr2_serial_uart_error_count = 0u;
    tr2_serial_uart_error_code = 0u;
    tr2_serial_gpio_base = (uint32_t)(uintptr_t)GPIOG;
    tr2_serial_uart_base = (uint32_t)(uintptr_t)LPUART1;

    g_serial.rs485 = rs485;
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_EnableVddIO2();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    if (rs485) {
        HAL_GPIO_WritePin(GPIOG, TR2_RS485_DIRECTION_PIN, GPIO_PIN_RESET);
        gpio.Pin = TR2_RS485_DIRECTION_PIN;
        gpio.Mode = GPIO_MODE_OUTPUT_PP;
        gpio.Pull = GPIO_NOPULL;
        gpio.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOG, &gpio);
    }
    __HAL_RCC_LPUART1_CLK_ENABLE();

    gpio.Pin = TR2_LPUART_TX_PIN | TR2_LPUART_RX_PIN;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = TR2_LPUART_AF;
    HAL_GPIO_Init(GPIOG, &gpio);

    g_serial.uart.Instance = LPUART1;
    g_serial.uart.Init.BaudRate = 115200u;
    g_serial.uart.Init.WordLength = UART_WORDLENGTH_9B;
    g_serial.uart.Init.StopBits = UART_STOPBITS_1;
    g_serial.uart.Init.Parity = UART_PARITY_EVEN;
    g_serial.uart.Init.Mode = UART_MODE_TX_RX;
    g_serial.uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    g_serial.uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    g_serial.uart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    g_serial.uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    HAL_StatusTypeDef status = HAL_UART_Init(&g_serial.uart);
    tr2_serial_uart_init_result = (uint32_t)status;
    if (status != HAL_OK) {
        return TR2_ERROR_UNAVAILABLE;
    }

    if (HAL_UARTEx_SetTxFifoThreshold(&g_serial.uart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK ||
        HAL_UARTEx_SetRxFifoThreshold(&g_serial.uart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK ||
        HAL_UARTEx_DisableFifoMode(&g_serial.uart) != HAL_OK) {
        return TR2_ERROR_UNAVAILABLE;
    }

    if (rtu_timer_init() != TR2_OK) {
        return TR2_ERROR_UNAVAILABLE;
    }

    HAL_NVIC_SetPriority(LPUART1_IRQn, 5u, 0u);
    HAL_NVIC_EnableIRQ(LPUART1_IRQn);

    transport->context = &g_serial;
    transport->start_receive = stm32_start_receive;
    transport->transmit = stm32_transmit;
    transport->poll_event = stm32_poll_event;

    return TR2_OK;
}

Tr2Result stm32_serial_transport_init(SerialTransport *transport)
{
    return serial_init(transport, false);
}

Tr2Result stm32_serial_transport_init_rs485(SerialTransport *transport)
{
    return serial_init(transport, true);
}

void stm32_serial_transport_irq_handler(void)
{
    HAL_UART_IRQHandler(&g_serial.uart);
}

void stm32_serial_transport_tim6_irq_handler(void)
{
    HAL_TIM_IRQHandler(&g_serial.rtu_timer);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    SerialTransportEvent event = {
        SERIAL_TRANSPORT_EVENT_BYTE,
        g_serial.rx_byte,
        SERIAL_TRANSPORT_ERROR_UNSPECIFIED
    };

    if (huart != &g_serial.uart) {
        return;
    }

    if (tr2_serial_rx_complete_count != UINT32_MAX) ++tr2_serial_rx_complete_count;
    (void)event_push_from_isr(event);
    rtu_timer_restart_from_byte();
    (void)HAL_UART_Receive_IT(&g_serial.uart, &g_serial.rx_byte, 1u);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &g_serial.uart) {
        /* HAL calls this only on TC, after the final stop bit (not TXE). */
        if (g_serial.rs485) {
            HAL_GPIO_WritePin(GPIOG, TR2_RS485_DIRECTION_PIN, GPIO_PIN_RESET);
        }
        g_serial.tx_busy = false;
        if (tr2_serial_tx_complete_count != UINT32_MAX) ++tr2_serial_tx_complete_count;
        if (g_serial.rs485) Stm32Rs485TxCompleted();
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    SerialTransportEvent event;

    if (huart != &g_serial.uart) {
        return;
    }

    tr2_serial_uart_error_code = HAL_UART_GetError(huart);
    if (tr2_serial_uart_error_count != UINT32_MAX) ++tr2_serial_uart_error_count;
    event.type = SERIAL_TRANSPORT_EVENT_ERROR;
    event.byte = 0u;
    event.error = map_uart_error(tr2_serial_uart_error_code);
    (void)event_push_from_isr(event);

    rtu_timer_stop();
    g_serial.rtu_timer_phase = TR2_RTU_TIMER_IDLE;
    (void)HAL_UART_Receive_IT(&g_serial.uart, &g_serial.rx_byte, 1u);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    SerialTransportEvent event;

    if (htim != &g_serial.rtu_timer) {
        return;
    }

    if (g_serial.rtu_timer_phase == TR2_RTU_TIMER_WAIT_T1_5) {
        event.type = SERIAL_TRANSPORT_EVENT_SILENCE_T1_5;
        event.byte = 0u;
        event.error = SERIAL_TRANSPORT_ERROR_UNSPECIFIED;
        (void)event_push_from_isr(event);

        g_serial.rtu_timer_phase = TR2_RTU_TIMER_WAIT_T3_5;
        rtu_timer_start_period(TR2_RTU_T3_5_REMAINDER_US);
        return;
    }

    if (g_serial.rtu_timer_phase == TR2_RTU_TIMER_WAIT_T3_5) {
        event.type = SERIAL_TRANSPORT_EVENT_SILENCE_T3_5;
        event.byte = 0u;
        event.error = SERIAL_TRANSPORT_ERROR_UNSPECIFIED;
        (void)event_push_from_isr(event);

        rtu_timer_stop();
        g_serial.rtu_timer_phase = TR2_RTU_TIMER_IDLE;
    }
}
