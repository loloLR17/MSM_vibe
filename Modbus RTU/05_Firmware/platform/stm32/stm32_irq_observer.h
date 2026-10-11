#ifndef TR2_STM32_IRQ_OBSERVER_H
#define TR2_STM32_IRQ_OBSERVER_H

#include <stdint.h>

#ifndef TR2_STM32_IRQ_DIAGNOSTICS
#define TR2_STM32_IRQ_DIAGNOSTICS 0
#endif

/* RAM only, OFF by default. Counters wrap modulo 2^32 within one boot.
 * No RDR/ICR access, interrupt writes, UART logging or synthetic events. */
typedef struct {
    uint32_t valid, cpuid, vtor, aircr;
    uint32_t primask, basepri, faultmask, control;
    uint32_t iser[3], ispr[3], itns[3];
    uint32_t uart_isr, uart_cr1, uart_cr3, hal_rx_state, hal_error;
} Stm32IrqCoreSample;

typedef struct {
    uint32_t uart_irq, timer_irq, rx_callback, error_callback, tx_callback;
    uint32_t timer_callback, events_pushed, events_dropped, events_polled;
    uint32_t bytes_polled, errors_polled, t1_5_polled, t3_5_polled;
    uint32_t poll_count, frame_count, crc_valid, crc_bad, frame_invalid;
    uint32_t foreign_unit, broadcast, accepted_request, available_status;
    uint32_t tx_attempt, tx_started, tx_fail_safe;
    uint32_t last_irq_isr, last_hal_error, last_rx_byte, last_event;
    uint32_t last_codec_result, last_frame_length, last_tx_length;
    uint8_t last_frame[256], last_tx[256];
    Stm32IrqCoreSample before_enable, after_timer_enable, after_uart_enable, live;
} Stm32IrqObserver;

#if TR2_STM32_IRQ_DIAGNOSTICS
extern volatile Stm32IrqObserver tr2_irq_observer;
void stm32_irq_observer_sample(void);
#define TR2_OBSERVE_INC(member) (++tr2_irq_observer.member)
#else
static inline void stm32_irq_observer_sample(void) { }
#define TR2_OBSERVE_INC(member) ((void)0)
#endif

#endif
