#ifndef TR2_STM32_IIS3DWB_VIBRATION_SOURCE_H
#define TR2_STM32_IIS3DWB_VIBRATION_SOURCE_H

#include <stdbool.h>
#include <stdint.h>

#include "stm32u5xx_hal.h"
#include "tr2/platform/vibration_source.h"

typedef struct {
    SPI_HandleTypeDef *spi;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
    uint16_t axes_enable_mask;
    uint16_t full_scale_code;
    bool configured;
    bool started;
} Stm32Iis3dwbVibrationSource;

Tr2Result stm32_iis3dwb_vibration_source_init(
    Stm32Iis3dwbVibrationSource *source,
    SPI_HandleTypeDef *spi,
    GPIO_TypeDef *cs_port,
    uint16_t cs_pin);

VibrationSource stm32_iis3dwb_vibration_source_interface(
    Stm32Iis3dwbVibrationSource *source);

Tr2Result stm32_iis3dwb_vibration_source_read_who_am_i(
    Stm32Iis3dwbVibrationSource *source,
    uint8_t *who_am_i);

#endif
