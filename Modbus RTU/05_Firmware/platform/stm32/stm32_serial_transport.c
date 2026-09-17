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
    SerialTransportEvent events[TR2_SERIAL_EVENT_CAPACITY];
    volatile uint32_t event_head;
    volatile uint32_t event_tail;
    volatile bool event_overflow_pending;
} Stm32SerialContext;

static Stm32SerialContext g_serial;

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
    (void)HAL_TIM_Base_Stop_IT(&g_serial.rtu_timer);
    __HAL_TIM_SET_COUNTER(&g_serial.rtu_timer, 0u);
}

static bool rtu_timer_start_period(uint32_t period_us)
{
    rtu_timer_stop();
    __HAL_TIM_SET_AUTORELOAD(&g_serial.rtu_timer, period_us - 1u);
    __HAL_TIM_SET_COUNTER(&g_serial.rtu_timer, 0u);
    __HAL_TIM_CLEAR_FLAG(&g_serial.rtu_timer, TIM_FLAG_UPDATE);
    return HAL_TIM_Base_Start_IT(&g_serial.rtu_timer) == HAL_OK;
}

static void rtu_timer_restart_from_byte(void)
{
    g_serial.rtu_timer_phase = TR2_RTU_TIMER_WAIT_T1_5;
    if (!rtu_timer_start_period(TR2_RTU_T1_5_US)) {
        SerialTransportEvent error = {
            SERIAL_TRANSPORT_EVENT_ERROR,
            0u,
            SERIAL_TRANSPORT_ERROR_UNSPECIFIED
        };
        g_serial.rtu_timer_phase = TR2_RTU_TIMER_IDLE;
        (void)event_push_from_isr(error);
    }
}

static Tr2Result stm32_start_receive(void *context)
{
    Stm32SerialContext *serial = (Stm32SerialContext *)context;

    if (serial == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    if (HAL_UART_Receive_IT(&serial->uart, &serial->rx_byte, 1u) != HAL_OK) {
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

    memcpy(serial->tx_buffer, data, length);
    serial->tx_busy = true;

    if (HAL_UART_Transmit_IT(&serial->uart, serial->tx_buffer, (uint16_t)length) != HAL_OK) {
        serial->tx_busy = false;
        return TR2_ERROR_UNAVAILABLE;
    }

    return TR2_OK;
}

static bool stm32_poll_event(void *context, SerialTransportEvent *event)
{
    Stm32SerialContext *serial = (Stm32SerialContext *)context;
    uint32_t tail;

    if (serial == NULL || event == NULL) {
        return false;
    }

    tail = serial->event_tail;
    if (tail != serial->event_head) {
        *event = serial->events[tail];
        serial->event_tail = (tail + 1u) % TR2_SERIAL_EVENT_CAPACITY;
        return true;
    }

    if (serial->event_overflow_pending) {
        serial->event_overflow_pending = false;
        event->type = SERIAL_TRANSPORT_EVENT_ERROR;
        event->byte = 0u;
        event->error = SERIAL_TRANSPORT_ERROR_OVERRUN;
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

Tr2Result stm32_serial_transport_init(SerialTransport *transport)
{
    GPIO_InitTypeDef gpio = {0};

    if (transport == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(&g_serial, 0, sizeof(g_serial));

    __HAL_RCC_GPIOG_CLK_ENABLE();
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

    if (HAL_UART_Init(&g_serial.uart) != HAL_OK) {
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

    (void)event_push_from_isr(event);
    rtu_timer_restart_from_byte();
    (void)HAL_UART_Receive_IT(&g_serial.uart, &g_serial.rx_byte, 1u);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &g_serial.uart) {
        g_serial.tx_busy = false;
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    SerialTransportEvent event;

    if (huart != &g_serial.uart) {
        return;
    }

    event.type = SERIAL_TRANSPORT_EVENT_ERROR;
    event.byte = 0u;
    event.error = map_uart_error(HAL_UART_GetError(huart));
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
        if (!rtu_timer_start_period(TR2_RTU_T3_5_REMAINDER_US)) {
            event.type = SERIAL_TRANSPORT_EVENT_ERROR;
            event.error = SERIAL_TRANSPORT_ERROR_UNSPECIFIED;
            g_serial.rtu_timer_phase = TR2_RTU_TIMER_IDLE;
            (void)event_push_from_isr(event);
        }
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
