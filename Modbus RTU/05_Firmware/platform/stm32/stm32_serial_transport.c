#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "stm32u5xx_hal.h"
#include "stm32_serial_transport.h"
#include "stm32_irq_observer.h"

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
/* 256 bytes at 115200/8E1 take <25 ms; bound stuck DE with TIM6. */
#define TR2_RTU_TX_TIMEOUT_US    50000u

typedef enum {
    TR2_RTU_TIMER_IDLE = 0,
    TR2_RTU_TIMER_WAIT_T1_5,
    TR2_RTU_TIMER_WAIT_T3_5,
    TR2_RTU_TIMER_TX_WATCHDOG,
    TR2_RTU_TIMER_TX_GUARD
} Tr2RtuTimerPhase;

typedef struct {
    UART_HandleTypeDef uart;
    TIM_HandleTypeDef rtu_timer;
    volatile Tr2RtuTimerPhase rtu_timer_phase;
    uint8_t rx_byte;
    uint8_t tx_buffer[TR2_SERIAL_TX_CAPACITY];
    volatile bool tx_busy;
    volatile bool tx_fault;
    volatile bool bus_silent;
    SerialTransportEvent events[TR2_SERIAL_EVENT_CAPACITY];
    volatile uint32_t event_head;
    volatile uint32_t event_tail;
    volatile bool event_overflow_pending;
} Stm32SerialContext;

static Stm32SerialContext g_serial;

#if TR2_STM32_IRQ_DIAGNOSTICS
volatile Stm32IrqObserver tr2_irq_observer;

static void sample_core(volatile Stm32IrqCoreSample *sample)
{
#if defined(STM32U575xx)
    /* NVIC is the bank addressed by this build/execution context. ITNS
     * describes attribution only where TrustZone is actually enabled. */
    sample->cpuid = SCB->CPUID;
    sample->vtor = SCB->VTOR;
    sample->aircr = SCB->AIRCR;
    sample->primask = __get_PRIMASK();
    sample->basepri = __get_BASEPRI();
    sample->faultmask = __get_FAULTMASK();
    sample->control = __get_CONTROL();
    for (unsigned i = 0u; i < 3u; ++i) {
        sample->iser[i] = NVIC->ISER[i];
        sample->ispr[i] = NVIC->ISPR[i];
        sample->itns[i] = NVIC->ITNS[i];
    }
    if (g_serial.uart.Instance != NULL) {
        sample->uart_isr = g_serial.uart.Instance->ISR;
        sample->uart_cr1 = g_serial.uart.Instance->CR1;
        sample->uart_cr3 = g_serial.uart.Instance->CR3;
        sample->hal_rx_state = g_serial.uart.RxState;
        sample->hal_error = g_serial.uart.ErrorCode;
    }
    sample->valid = 1u;
#else
    /* Host HAL counters are useful, but never pretend to sample a core. */
    sample->valid = 0u;
#endif
}

void stm32_irq_observer_sample(void)
{
    sample_core(&tr2_irq_observer.live);
}
#endif


static bool event_push_from_isr(SerialTransportEvent event)
{
    const uint32_t head = g_serial.event_head;
    const uint32_t next = (head + 1u) % TR2_SERIAL_EVENT_CAPACITY;

    if (next == g_serial.event_tail) {
        TR2_OBSERVE_INC(events_dropped);
        g_serial.event_overflow_pending = true;
        return false;
    }

    TR2_OBSERVE_INC(events_pushed);
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

    if (HAL_UART_Receive_IT(&serial->uart, &serial->rx_byte, 1u) != HAL_OK) {
        return TR2_ERROR_UNAVAILABLE;
    }

    return TR2_OK;
}

/* HAL aborts are synchronous, bounded register operations here: no DMA. */
static bool restore_receive(void)
{
    if (HAL_UART_AbortReceive(&g_serial.uart) != HAL_OK) return false;
    return HAL_UART_Receive_IT(&g_serial.uart, &g_serial.rx_byte, 1u) == HAL_OK;
}

static void tx_fail_safe(void)
{
    TR2_OBSERVE_INC(tx_fail_safe);
    SerialTransportEvent event = { SERIAL_TRANSPORT_EVENT_ERROR, 0u,
                                   SERIAL_TRANSPORT_ERROR_UNSPECIFIED };
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_RESET); /* Release FIRST. */
    (void)HAL_UART_AbortTransmit(&g_serial.uart);
    g_serial.tx_busy = false;
    g_serial.tx_fault = true; /* No automatic retry/re-enable after a fault. */
    g_serial.bus_silent = false;
    rtu_timer_stop();
    g_serial.rtu_timer_phase = TR2_RTU_TIMER_IDLE;
    (void)restore_receive();
    (void)event_push_from_isr(event);
}

static Tr2Result stm32_transmit(void *context, const uint8_t *data, size_t length)
{
    Stm32SerialContext *serial = (Stm32SerialContext *)context;

    if (serial == NULL || data == NULL || length == 0u || length > TR2_SERIAL_TX_CAPACITY) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

#if TR2_STM32_IRQ_DIAGNOSTICS
    TR2_OBSERVE_INC(tx_attempt);
    tr2_irq_observer.last_tx_length = (uint32_t)length;
    for (size_t i = 0u; i < length; ++i) tr2_irq_observer.last_tx[i] = data[i];
#endif
    const uint32_t primask = __get_PRIMASK();
    Tr2Result result = TR2_ERROR_UNAVAILABLE;
    __disable_irq();
    /* A response is allowed only after the consumed T3.5, with no newer RX.
     * The build gate remains OFF until the physical board is qualified.
     */
    if (TR2_STM32_RS485_TX_QUALIFIED && !serial->tx_busy && !serial->tx_fault &&
        serial->bus_silent && serial->event_head == serial->event_tail &&
        serial->rtu_timer_phase == TR2_RTU_TIMER_IDLE &&
        !__HAL_UART_GET_FLAG(&serial->uart, UART_FLAG_RXNE) &&
        !__HAL_UART_GET_FLAG(&serial->uart, UART_FLAG_BUSY)) {
        memcpy(serial->tx_buffer, data, length);
        serial->tx_busy = true;
        serial->bus_silent = false;
        if (HAL_UART_AbortReceive(&serial->uart) == HAL_OK) {
            /* J2 common DE-/RE: DE high disables receiver, no PG5 control. */
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_SET);
            serial->rtu_timer_phase = TR2_RTU_TIMER_TX_WATCHDOG;
            rtu_timer_start_period(TR2_RTU_TX_TIMEOUT_US);
            if (HAL_UART_Transmit_IT(&serial->uart, serial->tx_buffer,
                                     (uint16_t)length) == HAL_OK) {
                TR2_OBSERVE_INC(tx_started);
                result = TR2_OK;
            } else {
                tx_fail_safe();
            }
        } else {
            tx_fail_safe();
        }
    }
    if (primask == 0u) __enable_irq();
    return result;
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
            if (serial->tx_busy) tx_fail_safe();
            serial->bus_silent = false;
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
#if TR2_STM32_IRQ_DIAGNOSTICS
        TR2_OBSERVE_INC(events_polled);
        tr2_irq_observer.last_event = (uint32_t)event->type;
        if (event->type == SERIAL_TRANSPORT_EVENT_BYTE) TR2_OBSERVE_INC(bytes_polled);
        if (event->type == SERIAL_TRANSPORT_EVENT_ERROR) TR2_OBSERVE_INC(errors_polled);
        if (event->type == SERIAL_TRANSPORT_EVENT_SILENCE_T1_5) TR2_OBSERVE_INC(t1_5_polled);
        if (event->type == SERIAL_TRANSPORT_EVENT_SILENCE_T3_5) TR2_OBSERVE_INC(t3_5_polled);
#endif
        serial->event_tail = (tail + 1u) % TR2_SERIAL_EVENT_CAPACITY;
        /* Guard checked again atomically by transmit against ISR activity. */
        if (event->type == SERIAL_TRANSPORT_EVENT_SILENCE_T3_5) {
            uint32_t primask = __get_PRIMASK();
            __disable_irq();
            serial->bus_silent = serial->event_head == serial->event_tail &&
                serial->rtu_timer_phase == TR2_RTU_TIMER_IDLE && !serial->tx_busy;
            if (primask == 0u) __enable_irq();
        }
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
#if TR2_STM32_IRQ_DIAGNOSTICS
    sample_core(&tr2_irq_observer.after_timer_enable);
#endif
    return TR2_OK;
}

Tr2Result stm32_serial_transport_init(SerialTransport *transport)
{
    GPIO_InitTypeDef gpio = {0};

    if (transport == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(&g_serial, 0, sizeof(g_serial));
#if TR2_STM32_IRQ_DIAGNOSTICS
    memset((void *)&tr2_irq_observer, 0, sizeof(tr2_irq_observer));
    sample_core(&tr2_irq_observer.before_enable);
#endif
    /* HAL_MspInit already configured PG4 before releasing VDDIO2. */
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_RESET);

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
#if TR2_STM32_IRQ_DIAGNOSTICS
    sample_core(&tr2_irq_observer.after_uart_enable);
#endif

    transport->context = &g_serial;
    transport->start_receive = stm32_start_receive;
    transport->transmit = stm32_transmit;
    transport->poll_event = stm32_poll_event;

    return TR2_OK;
}

void stm32_serial_transport_irq_handler(void)
{
    TR2_OBSERVE_INC(uart_irq);
#if TR2_STM32_IRQ_DIAGNOSTICS && defined(STM32U575xx)
    tr2_irq_observer.last_irq_isr = g_serial.uart.Instance->ISR;
#endif
    HAL_UART_IRQHandler(&g_serial.uart);
}

void stm32_serial_transport_tim6_irq_handler(void)
{
    TR2_OBSERVE_INC(timer_irq);
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

    TR2_OBSERVE_INC(rx_callback);
#if TR2_STM32_IRQ_DIAGNOSTICS
    tr2_irq_observer.last_rx_byte = g_serial.rx_byte;
#endif
    if (g_serial.tx_busy) return; /* Suppress late callback/echo during TX. */
    g_serial.bus_silent = false;
    (void)event_push_from_isr(event);
    rtu_timer_restart_from_byte();
    if (HAL_UART_Receive_IT(&g_serial.uart, &g_serial.rx_byte, 1u) != HAL_OK) {
        tx_fail_safe();
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &g_serial.uart) {
        TR2_OBSERVE_INC(tx_callback);
        if (!g_serial.tx_busy) return;
        /* Official HAL invokes this on TC, not TXE/buffer exhaustion. */
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_RESET);
        g_serial.tx_busy = false;
        rtu_timer_stop();
        if (!restore_receive()) {
            tx_fail_safe();
            return;
        }
        g_serial.rtu_timer_phase = TR2_RTU_TIMER_TX_GUARD;
        rtu_timer_start_period(TR2_RTU_T3_5_US);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    SerialTransportEvent event;

    if (huart != &g_serial.uart) {
        return;
    }

    TR2_OBSERVE_INC(error_callback);
#if TR2_STM32_IRQ_DIAGNOSTICS
    tr2_irq_observer.last_hal_error = HAL_UART_GetError(huart);
#endif
    if (g_serial.tx_busy) {
        tx_fail_safe();
        return;
    }
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_RESET);
    g_serial.bus_silent = false;
    event.type = SERIAL_TRANSPORT_EVENT_ERROR;
    event.byte = 0u;
    event.error = map_uart_error(HAL_UART_GetError(huart));
    (void)event_push_from_isr(event);

    rtu_timer_stop();
    g_serial.rtu_timer_phase = TR2_RTU_TIMER_IDLE;
    if (!restore_receive()) g_serial.tx_fault = true;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    SerialTransportEvent event;

    if (htim != &g_serial.rtu_timer) {
        return;
    }

    TR2_OBSERVE_INC(timer_callback);
    if (g_serial.rtu_timer_phase == TR2_RTU_TIMER_TX_WATCHDOG) {
        tx_fail_safe();
        return;
    }
    if (g_serial.rtu_timer_phase == TR2_RTU_TIMER_TX_GUARD) {
        rtu_timer_stop();
        g_serial.rtu_timer_phase = TR2_RTU_TIMER_IDLE;
        return; /* No synthetic request or new transmit grant. */
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
