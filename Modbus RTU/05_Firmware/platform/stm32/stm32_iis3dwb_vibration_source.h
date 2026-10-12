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

/* Additive board diagnostic surface: coherent raw bytes and the same conversion
 * used by production. No timestamp is asserted at the sensor sampling instant. */
typedef struct {
    int16_t xyz[3];
    uint8_t status;
    VibrationSample sample;
} Stm32Iis3dwbRawSample;
Tr2Result stm32_iis3dwb_read_raw(Stm32Iis3dwbVibrationSource *source,
    Stm32Iis3dwbRawSample *sample);
Tr2Result stm32_iis3dwb_read_register(Stm32Iis3dwbVibrationSource *source,
    uint8_t reg, uint8_t *value);
/* Only volatile CTRL1_XL, CTRL3_C and CTRL5_C may be written here.
 * Caller owns the isolated bus and must restore saved values. */
Tr2Result stm32_iis3dwb_write_control(Stm32Iis3dwbVibrationSource *source,
    uint8_t reg, uint8_t value);
#endif
