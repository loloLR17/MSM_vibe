#ifndef TR2_STM32_REGISTER_PROBE_H
#define TR2_STM32_REGISTER_PROBE_H
#include "stm32u5xx_hal.h"
#define TR2_REGISTER_PROBE_COUNT 512U
typedef struct {
 uint32_t begin_cycles,end_cycles,reg,length,status,error,sr_before,sr_after;
 uint32_t cs_before,cs_during,cs_after,tx_left,rx_left,cfg1,cfg2;
 uint8_t tx[8],rx[8];
} Stm32RegisterProbeRecord;
typedef struct {uint32_t stage,count,errors,core_hz,spi_hz,elapsed_ms;} Stm32RegisterProbeInfo;
extern volatile uint32_t tr2_register_probe_trigger;
extern volatile Stm32RegisterProbeInfo tr2_register_probe_info;
extern volatile Stm32RegisterProbeRecord tr2_register_probe_records[TR2_REGISTER_PROBE_COUNT];
__attribute__((noreturn)) void stm32_iis3dwb_register_probe_run(SPI_HandleTypeDef*,GPIO_TypeDef*,uint16_t);
#endif
