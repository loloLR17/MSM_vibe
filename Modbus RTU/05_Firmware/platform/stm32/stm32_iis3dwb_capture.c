#include "stm32_iis3dwb_capture.h"
#if TR2_STM32_ACQUISITION_CAPTURE
#include <stddef.h>
volatile uint32_t tr2_iis3dwb_capture_trigger;
volatile Stm32Iis3dwbCaptureInfo tr2_iis3dwb_capture_info;
volatile Stm32Iis3dwbCaptureSample tr2_iis3dwb_capture_samples[TR2_IIS3DWB_CAPTURE_COUNT];
/* Audit FIFO_CTRL1..4, interrupt routes, CTRL4/6/7/8/10 and user offsets.
 * No FIFO, sensor timestamp or axis/filter/offset mutation is needed here. */
static const uint8_t audit_regs[13] = {
    0x07U, 0x08U, 0x09U, 0x0AU, 0x0DU, 0x0EU, 0x13U,
    0x15U, 0x16U, 0x17U, 0x19U, 0x73U, 0x74U
};
static const uint8_t control_regs[3] = {0x10U,0x12U,0x14U};
static Tr2Result checked_write(Stm32Iis3dwbVibrationSource *source,
    uint8_t reg, uint8_t value)
{
    uint8_t actual = 0U;
    Tr2Result result = stm32_iis3dwb_write_control(source,reg,value);
    if (result == TR2_OK) result = stm32_iis3dwb_read_register(source,reg,&actual);
    if (result == TR2_OK && actual != value) result = TR2_ERROR_INTERNAL;
    return result;
}
static Tr2Result take_sample(Stm32Iis3dwbVibrationSource *source,
    uint32_t phase, bool keep)
{
    uint32_t deadline = HAL_GetTick();
    Stm32Iis3dwbRawSample sample;
    Tr2Result result;
    uint32_t begin, end;
    do {
        begin = DWT->CYCCNT;
        result = stm32_iis3dwb_read_raw(source,&sample);
        end = DWT->CYCCNT;
        tr2_iis3dwb_capture_info.polls++;
        if (result == TR2_ERROR_NOT_AVAILABLE) tr2_iis3dwb_capture_info.not_ready++;
        else if (result != TR2_OK) { tr2_iis3dwb_capture_info.errors++; return result; }
        if ((uint32_t)(HAL_GetTick() - deadline) >= 100U) return TR2_ERROR_UNAVAILABLE;
    } while (result == TR2_ERROR_NOT_AVAILABLE);
    if (keep) {
        uint32_t i = tr2_iis3dwb_capture_info.count;
        if (i >= TR2_IIS3DWB_CAPTURE_COUNT) return TR2_ERROR_INVALID_STATE;
        Stm32Iis3dwbCaptureSample record = {
            begin,end,phase,sample.status,(uint32_t)result,
            sample.xyz[0],sample.xyz[1],sample.xyz[2],
            sample.sample.x_mg,sample.sample.y_mg,sample.sample.z_mg,
            sample.sample.valid ? 1U : 0U,sample.sample.saturated ? 1U : 0U
        };
        tr2_iis3dwb_capture_samples[i] = record;
        tr2_iis3dwb_capture_info.count = i + 1U;
        if (sample.sample.saturated) tr2_iis3dwb_capture_info.saturation++;
    }
    return TR2_OK;
}
__attribute__((noreturn)) void stm32_iis3dwb_capture_run(
    SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin)
{
    Stm32Iis3dwbVibrationSource source;
    VibrationSource interface;
    VibrationSourceConfiguration configuration = {26667U,7U,1U};
    Tr2Result result = TR2_OK;
    bool saved = false, changed = false;
    uint8_t value = 0U;
    uint32_t i, phase;
    static const uint8_t stimulus[4] = {0U,1U,2U,0U};
    tr2_iis3dwb_capture_info.stage = 1U;
    tr2_iis3dwb_capture_info.restore_result = (uint32_t)TR2_ERROR_NOT_AVAILABLE;
    while (tr2_iis3dwb_capture_trigger != 0x4D303135U) HAL_Delay(10U);
    tr2_iis3dwb_capture_trigger = 0U;
    tr2_iis3dwb_capture_info.stage = 2U;
    result = stm32_iis3dwb_vibration_source_init(&source,spi,port,pin);
    if (result != TR2_OK) goto finish;
    interface = stm32_iis3dwb_vibration_source_interface(&source);
    result = stm32_iis3dwb_vibration_source_read_who_am_i(&source,&value);
    tr2_iis3dwb_capture_info.who_am_i = value;
    if (result != TR2_OK) goto finish;
    for (i = 0U; i < 3U; i++) {
        result = stm32_iis3dwb_read_register(&source,control_regs[i],&value);
        if (result != TR2_OK) goto finish;
        tr2_iis3dwb_capture_info.original_controls[i] = value;
    }
    saved = true;
    for (i = 0U; i < 13U; i++) {
        result = stm32_iis3dwb_read_register(&source,audit_regs[i],&value);
        if (result != TR2_OK) goto finish;
        tr2_iis3dwb_capture_info.audit_registers[i] = value;
        if (value != 0U) { result = TR2_ERROR_UNSUPPORTED; goto finish; }
    }
    result = stm32_iis3dwb_read_register(&source,0x75U,&value);
    if (result != TR2_OK || value != 0U) { if (result == TR2_OK) result = TR2_ERROR_UNSUPPORTED; goto finish; }
    /* Refuse inherited self-test/reset/reboot settings rather than silently
     * changing an unknown sensor authority. */
    if ((tr2_iis3dwb_capture_info.original_controls[1] & 0x81U) != 0U ||
        tr2_iis3dwb_capture_info.original_controls[2] != 0U) {
        result = TR2_ERROR_UNSUPPORTED; goto finish;
    }
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    if ((DWT->CTRL & DWT_CTRL_NOCYCCNT_Msk) != 0U) { result = TR2_ERROR_UNSUPPORTED; goto finish; }
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    { uint32_t before = DWT->CYCCNT;
      HAL_Delay(2U);
      if (DWT->CYCCNT == before) { result = TR2_ERROR_UNAVAILABLE; goto finish; }
    }
    tr2_iis3dwb_capture_info.clock_verified = 1U;
    tr2_iis3dwb_capture_info.core_hz = HAL_RCC_GetHCLKFreq();
    tr2_iis3dwb_capture_info.spi_kernel_hz = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3);
    tr2_iis3dwb_capture_info.spi_hz = tr2_iis3dwb_capture_info.spi_kernel_hz / 256U;
    changed = true;
    result = interface.configure(interface.context,&configuration);
    if (result != TR2_OK) goto finish;
    result = stm32_iis3dwb_read_register(&source,0x10U,&value);
    tr2_iis3dwb_capture_info.ctrl1_readback = value;
    if (result != TR2_OK || value != 0xA8U) { if (result == TR2_OK) result = TR2_ERROR_INTERNAL; goto finish; }
    result = stm32_iis3dwb_read_register(&source,0x12U,&value);
    tr2_iis3dwb_capture_info.ctrl3_readback = value;
    if (result != TR2_OK || value != 0x44U) { if (result == TR2_OK) result = TR2_ERROR_INTERNAL; goto finish; }
    result = interface.start(interface.context);
    if (result != TR2_OK) goto finish;
    for (phase = 0U; phase < 4U; phase++) {
        tr2_iis3dwb_capture_info.stage = 3U + phase;
        result = checked_write(&source,0x14U,stimulus[phase]);
        tr2_iis3dwb_capture_info.phase_ctrl5[phase] = stimulus[phase];
        if (result != TR2_OK) goto finish;
        HAL_Delay(100U); /* Manufacturer self-test settling interval. */
        result = take_sample(&source,phase,false); /* Discard first ready sample. */
        if (result != TR2_OK) goto finish;
        for (i = 0U; i < TR2_IIS3DWB_CAPTURE_PHASE_SAMPLES; i++) {
            result = take_sample(&source,phase,true);
            if (result != TR2_OK) goto finish;
        }
    }
finish:
    tr2_iis3dwb_capture_info.result = (uint32_t)result;
    if (saved) {
        Tr2Result restoration = TR2_OK;
        /* Always attempt to remove internal excitation, even after a failure.
         * Restore all saved control values after a partial configure failure. */
        const uint32_t order[3] = {2U,0U,1U};
        for (i = 0U; i < 3U; i++) {
            uint32_t index = order[i];
            Tr2Result step;
            if (changed) step = checked_write(&source,control_regs[index],
                (uint8_t)tr2_iis3dwb_capture_info.original_controls[index]);
            else step = TR2_OK;
            if (step != TR2_OK) restoration = step;
            step = stm32_iis3dwb_read_register(&source,control_regs[index],&value);
            tr2_iis3dwb_capture_info.final_controls[index] = value;
            if (step != TR2_OK || value != tr2_iis3dwb_capture_info.original_controls[index])
                restoration = step != TR2_OK ? step : TR2_ERROR_INTERNAL;
        }
        tr2_iis3dwb_capture_info.restore_result = (uint32_t)restoration;
    }
    tr2_iis3dwb_capture_info.stage = 10U;
    for (;;) HAL_Delay(100U); /* Never repeat, never fall into the harness. */
}
#endif

#if !TR2_STM32_ACQUISITION_CAPTURE
/* Keep the default translation unit nonempty under -Wpedantic. */
typedef int Tr2Iis3dwbCaptureDisabled;
#endif
