#ifndef TR2_CONTINUOUS_FIFO_H
#define TR2_CONTINUOUS_FIFO_H
#include "stm32_iis3dwb_burst_capture.h"
#define TR2_CONT_WORDS 4096U
#define TR2_CONT_BATCH_WORDS 128U
#define TR2_CONT_BURSTS (TR2_CONT_WORDS/TR2_CONT_BATCH_WORDS)
#define TR2_CONT_POLLS 192U
#define TR2_CONT_TRACES 384U
#define TR2_CONT_BUDGET_MS 250U
#define TR2_CONT_LEVEL_CEILING 384U
#define TR2_CONT_TRIGGER 0x4D31354AU
/* Window0 empty,1 armed,2 filling,3 complete,4 invalid. */
typedef struct {
 uint32_t stage,result,restore_result,core_hz,spi_hz,poll_count,burst_count,word_count;
 uint32_t accepted,errors,loss_flags,tag_errors,parity_errors,gap_events,saturations;
 uint32_t start_cycles,end_cycles,capture_end_cycles,window_state,max_level,budget_violations,first_failure_word;
 uint32_t saved[7],configured[7],restored[7],nvic_before[9],nvic_after[9],nvic_valid_mask;
} Stm32ContinuousInfo;
typedef struct {
 Stm32BurstMeta io;
 uint32_t before_begin,before_end,after_begin,after_end;
 uint32_t produced_lower,produced_upper,produced_observed,valid_mask;
} Stm32ContinuousMeta;
extern volatile uint32_t tr2_cont_trigger,tr2_cont_stop,tr2_cont_trace_count;
extern volatile Stm32ContinuousInfo tr2_cont_info;
extern volatile Stm32ConfigTrace tr2_cont_traces[TR2_CONT_TRACES];
extern volatile Stm32ConfigRefusal tr2_cont_first_refusal;
extern volatile Stm32BurstPoll tr2_cont_polls[TR2_CONT_POLLS];
extern volatile Stm32ContinuousMeta tr2_cont_meta[TR2_CONT_BURSTS];
extern volatile uint8_t tr2_cont_rx[TR2_CONT_BURSTS][TR2_BURST_BYTES];
extern volatile int16_t tr2_cont_xyz[TR2_CONT_WORDS][3];
extern volatile uint8_t tr2_cont_tags[TR2_CONT_WORDS];
__attribute__((noreturn)) void stm32_iis3dwb_continuous_run(SPI_HandleTypeDef*,GPIO_TypeDef*,uint16_t);
#endif
