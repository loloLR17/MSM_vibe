#ifndef TR2_STM32_PRODUCTION_BOARD_H
#define TR2_STM32_PRODUCTION_BOARD_H
#include "stm32_production_runtime.h"
void stm32_production_board_initialize(SPI_HandleTypeDef *fram, SPI_HandleTypeDef *sensor);
__attribute__((noreturn)) void stm32_production_manufacturing_run(void);
__attribute__((noreturn)) void stm32_production_server_run(void);
#endif
