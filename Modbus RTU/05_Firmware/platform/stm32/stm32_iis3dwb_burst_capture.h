#ifndef TR2_BURST_CAPTURE_H
#define TR2_BURST_CAPTURE_H
#include "stm32u5xx_hal.h"
#include "tr2/common/result.h"
#define TR2_BURST_BYTES 897U
#define TR2_BURST_POLLS 32U
typedef struct {uint32_t begin_cycles,end_cycles,status,level;} Stm32BurstPoll;
typedef struct {
 uint32_t begin_cycles,end_cycles,status_before,status_after,hal_status,error;
 uint32_t cs_before,cs_during,cs_after,rx_left,tx_left,cfg1,cfg2;
} Stm32BurstMeta;
typedef struct {
 uint32_t stage,result,restore_result,core_hz,spi_hz,poll_count,burst_count;
 uint32_t word_count,accepted,errors,loss_flags,tag_errors,parity_errors,gap_events;
 uint32_t saturations,start_cycles,end_cycles,filled_status,stopped_status,final_status;
 uint32_t saved[7],configured[7],restored[7];
} Stm32BurstInfo;
extern volatile uint32_t tr2_burst_trigger;
extern volatile Stm32BurstInfo tr2_burst_info;
extern volatile Stm32BurstPoll tr2_burst_polls[TR2_BURST_POLLS];
extern volatile Stm32BurstMeta tr2_burst_meta[2];
extern volatile uint8_t tr2_burst_rx[2][TR2_BURST_BYTES];
__attribute__((noreturn)) void stm32_iis3dwb_burst_capture_run(SPI_HandleTypeDef*,GPIO_TypeDef*,uint16_t);
#endif
