#ifndef TR2_STM32_IIS3DWB_CAPTURE_H
#define TR2_STM32_IIS3DWB_CAPTURE_H
#include "stm32_iis3dwb_vibration_source.h"
#define TR2_IIS3DWB_CAPTURE_PHASE_SAMPLES 512U
#define TR2_IIS3DWB_CAPTURE_PHASES 4U
#define TR2_IIS3DWB_CAPTURE_COUNT (TR2_IIS3DWB_CAPTURE_PHASES * TR2_IIS3DWB_CAPTURE_PHASE_SAMPLES)
typedef struct {
    uint32_t begin_cycles, end_cycles, phase, status, result;
    int32_t raw_x, raw_y, raw_z, x_mg, y_mg, z_mg;
    uint32_t valid, saturated;
} Stm32Iis3dwbCaptureSample;
typedef struct {
    uint32_t stage, result, count, polls, not_ready, errors, saturation;
    uint32_t restore_result, core_hz, spi_kernel_hz, spi_hz, who_am_i;
    uint32_t clock_verified, ctrl1_readback, ctrl3_readback;
    uint32_t phase_ctrl5[4], original_controls[3], final_controls[3];
    uint32_t audit_registers[13];
} Stm32Iis3dwbCaptureInfo;
extern volatile uint32_t tr2_iis3dwb_capture_trigger;
extern volatile Stm32Iis3dwbCaptureInfo tr2_iis3dwb_capture_info;
extern volatile Stm32Iis3dwbCaptureSample tr2_iis3dwb_capture_samples[TR2_IIS3DWB_CAPTURE_COUNT];
/* Explicit one-shot debugger trigger; idle both before and after. */
__attribute__((noreturn)) void stm32_iis3dwb_capture_run(
    SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin);
#endif
