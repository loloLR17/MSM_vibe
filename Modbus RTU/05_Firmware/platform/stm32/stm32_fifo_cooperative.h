#ifndef STM32_FIFO_COOPERATIVE_H
#define STM32_FIFO_COOPERATIVE_H
#include "stm32u5xx_hal.h"
#include "tr2/modbus/codec.h"
void stm32_fifo_cooperative_prepare(SPI_HandleTypeDef *,SPI_HandleTypeDef *);
void stm32_fifo_cooperative_poll(void);
void stm32_fifo_cooperative_observe(const ModbusRtuAduView *);
#endif
