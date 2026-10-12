#ifndef TR2_ACTIVE_READBACK_H
#define TR2_ACTIVE_READBACK_H
#include "stm32_iis3dwb_register_probe.h"
#define TR2_ACTIVE_READBACK_COUNT 1024U
typedef struct {uint32_t phase;Stm32RegisterProbeRecord io;} Stm32ActiveReadbackRecord;
typedef struct {
 uint32_t stage,result,restore_result,count,errors,core_hz,spi_hz,elapsed_ms;
 uint32_t saved[7],restored[7];
} Stm32ActiveReadbackInfo;
extern volatile uint32_t tr2_active_readback_trigger;
extern volatile Stm32ActiveReadbackInfo tr2_active_readback_info;
extern volatile Stm32ActiveReadbackRecord tr2_active_readback_records[TR2_ACTIVE_READBACK_COUNT];
__attribute__((noreturn)) void stm32_iis3dwb_active_readback_run(SPI_HandleTypeDef*,GPIO_TypeDef*,uint16_t);
#endif
