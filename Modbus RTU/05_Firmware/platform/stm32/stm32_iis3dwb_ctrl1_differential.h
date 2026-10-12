#ifndef TR2_CTRL1_DIFFERENTIAL_H
#define TR2_CTRL1_DIFFERENTIAL_H
#include "stm32_iis3dwb_burst_capture.h" /* Existing raw trace/validity types only. */
#define TR2_CTRL1_TRACE_COUNT 64U
#define TR2_CTRL1_DELAY_MS 20U
#define TR2_CTRL1_TRIGGER 0x4D313548U
/* Stages1 baseline,2 BDU prepared,3 immediate active,4 delayed active,
 * 5 disabled,6 restoration,10 finished. No data acquisition authority. */
typedef struct {
 uint32_t stage,result,restore_result,core_hz,spi_hz,count,errors,mismatches;
 uint32_t start_cycles,end_cycles,delay_begin_cycles,delay_end_cycles,nvic_valid_phases;
 uint32_t saved[7],restored[7];
} Stm32Ctrl1Info;
extern volatile uint32_t tr2_ctrl1_trigger;
extern volatile Stm32Ctrl1Info tr2_ctrl1_info;
extern volatile Stm32ConfigTrace tr2_ctrl1_traces[TR2_CTRL1_TRACE_COUNT];
extern volatile Stm32ConfigRefusal tr2_ctrl1_first_divergence;
/* ISER0..2, ISPR0..2, PRIMASK,BASEPRI,FAULTMASK,ICSR; validity in info. */
extern volatile uint32_t tr2_ctrl1_nvic[7][10];
__attribute__((noreturn)) void stm32_iis3dwb_ctrl1_differential_run(SPI_HandleTypeDef*,GPIO_TypeDef*,uint16_t);
#endif
