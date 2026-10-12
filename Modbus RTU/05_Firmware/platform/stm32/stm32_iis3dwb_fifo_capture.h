#ifndef TR2_STM32_IIS3DWB_FIFO_CAPTURE_H
#define TR2_STM32_IIS3DWB_FIFO_CAPTURE_H
#include "stm32_iis3dwb_vibration_source.h"
#define TR2_FIFO_WORDS 256U
#define TR2_FIFO_POLLS 512U
typedef struct {
    uint32_t begin_cycles, end_cycles, tag, level_before, status_before;
    uint32_t level_after, status_after;
    int32_t raw_x, raw_y, raw_z;
    uint32_t sequence_ok, parity_ok, saturated, result;
} Stm32Iis3dwbFifoWord;
typedef struct { uint32_t begin_cycles, end_cycles, status, level; } Stm32Iis3dwbFifoPoll;
typedef struct {
    uint32_t stage, result, restore_result, who_am_i, core_hz, spi_hz;
    uint32_t count, poll_count, loss_flags, bus_errors, tag_errors;
    uint32_t parity_errors, gap_events, holes_mod4, saturations;
    uint32_t start_low, start_high, last_not_wm_low, wm_high, drain_low, drain_high;
    uint32_t initial_status, filled_status, empty_status, negative_status, negative_rejected;
    uint32_t saved[7], configured[7], restored[7];
} Stm32Iis3dwbFifoInfo;
extern volatile uint32_t tr2_fifo_trigger;
extern volatile Stm32Iis3dwbFifoInfo tr2_fifo_info;
extern volatile Stm32Iis3dwbFifoWord tr2_fifo_words[TR2_FIFO_WORDS];
extern volatile Stm32Iis3dwbFifoPoll tr2_fifo_polls[TR2_FIFO_POLLS];
__attribute__((noreturn)) void stm32_iis3dwb_fifo_capture_run(
    SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin);
#endif
