#include "stm32_iis3dwb_fifo_capture.h"
#if TR2_STM32_FIFO_CAPTURE
#include <limits.h>
#include <stddef.h>
#include <string.h>
volatile uint32_t tr2_fifo_trigger;
volatile Stm32Iis3dwbFifoInfo tr2_fifo_info;
volatile Stm32Iis3dwbFifoWord tr2_fifo_words[TR2_FIFO_WORDS];
volatile Stm32Iis3dwbFifoPoll tr2_fifo_polls[TR2_FIFO_POLLS];
static const uint8_t saved_regs[7] = {0x07U,0x08U,0x09U,0x0AU,0x10U,0x12U,0x14U};
static const uint8_t audit_regs[] = {0x0DU,0x0EU,0x13U,0x15U,0x16U,0x17U,0x19U,0x73U,0x74U,0x75U};
/* Isolated FIFO register I/O only. Production driver and its API stay unchanged. */
static Tr2Result read_bytes(Stm32Iis3dwbVibrationSource *source,
    uint8_t reg, uint8_t *data, uint16_t length)
{
    uint8_t tx[8] = {0U}, rx[8] = {0U};
    HAL_StatusTypeDef status;
    if (length == 0U || length > 7U) return TR2_ERROR_INVALID_ARGUMENT;
    tx[0] = reg | 0x80U;
    HAL_GPIO_WritePin(source->cs_port,source->cs_pin,GPIO_PIN_RESET);
    status = HAL_SPI_TransmitReceive(source->spi,tx,rx,(uint16_t)(length+1U),10U);
    HAL_GPIO_WritePin(source->cs_port,source->cs_pin,GPIO_PIN_SET);
    if (status != HAL_OK) {
        tr2_fifo_info.bus_errors++;
        return status == HAL_TIMEOUT || status == HAL_BUSY ? TR2_ERROR_UNAVAILABLE : TR2_ERROR_INTERNAL;
    }
    memcpy(data,rx+1U,length);
    return TR2_OK;
}
static Tr2Result write_checked(Stm32Iis3dwbVibrationSource *source,uint8_t reg,uint8_t value)
{
    uint8_t actual = 0U;
    Tr2Result result;
    if (reg >= 0x07U && reg <= 0x0AU) {
        uint8_t tx[2] = {reg,value};
        HAL_StatusTypeDef status;
        HAL_GPIO_WritePin(source->cs_port,source->cs_pin,GPIO_PIN_RESET);
        status = HAL_SPI_Transmit(source->spi,tx,2U,10U);
        HAL_GPIO_WritePin(source->cs_port,source->cs_pin,GPIO_PIN_SET);
        result = status == HAL_OK ? TR2_OK : TR2_ERROR_UNAVAILABLE;
        if (status != HAL_OK) tr2_fifo_info.bus_errors++;
    } else result = stm32_iis3dwb_write_control(source,reg,value);
    if (result == TR2_OK) result = read_bytes(source,reg,&actual,1U);
    if (result == TR2_OK && actual != value) result = TR2_ERROR_INTERNAL;
    return result;
}
static Tr2Result fifo_status(Stm32Iis3dwbVibrationSource *source,uint32_t *raw,uint32_t *level)
{
    uint8_t bytes[2];
    /* BDU requires STATUS1 before STATUS2. A single burst preserves that order.
     * STATUS2 clears OVR_LATCHED: every observed flag is retained in RAM. */
    Tr2Result result = read_bytes(source,0x3AU,bytes,2U);
    if (result != TR2_OK) return result;
    *raw = bytes[0] | ((uint32_t)bytes[1]<<8U);
    *level = *raw & 0x03FFU;
    return TR2_OK;
}
static bool parity_even(uint8_t tag)
{
    unsigned count = 0U;
    for (unsigned i = 0U;i < 8U;i++) count += (tag>>i)&1U;
    return (count&1U) == 0U;
}
__attribute__((noreturn)) void stm32_iis3dwb_fifo_capture_run(
    SPI_HandleTypeDef *spi,GPIO_TypeDef *port,uint16_t pin)
{
    Stm32Iis3dwbVibrationSource source;
    VibrationSource interface;
    VibrationSourceConfiguration configuration = {26667U,7U,1U};
    Tr2Result result = TR2_OK;
    uint8_t value = 0U, frame[7];
    uint32_t level = 0U, status = 0U, previous_counter = 0U;
    bool saved = false, changed = false;
    tr2_fifo_info.stage = 1U;
    tr2_fifo_info.restore_result = (uint32_t)TR2_ERROR_NOT_AVAILABLE;
    while (tr2_fifo_trigger != 0x4D313542U) HAL_Delay(10U);
    tr2_fifo_trigger = 0U;
    tr2_fifo_info.stage = 2U;
    result = stm32_iis3dwb_vibration_source_init(&source,spi,port,pin);
    if (result != TR2_OK) goto finish;
    result = stm32_iis3dwb_vibration_source_read_who_am_i(&source,&value);
    tr2_fifo_info.who_am_i = value;
    if (result != TR2_OK) goto finish;
    for (unsigned i = 0U;i < 7U;i++) {
        result = read_bytes(&source,saved_regs[i],&value,1U);
        if (result != TR2_OK) goto finish;
        tr2_fifo_info.saved[i] = value;
    }
    saved = true;
    /* Never clear a pre-existing FIFO or overwrite an active sensor authority. */
    for (unsigned i = 0U;i < 7U;i++) {
        if (tr2_fifo_info.saved[i] != (i == 5U ? 4U : 0U)) { result = TR2_ERROR_UNSUPPORTED; goto finish; }
    }
    for (unsigned i = 0U;i < sizeof(audit_regs);i++) {
        result = read_bytes(&source,audit_regs[i],&value,1U);
        if (result != TR2_OK) goto finish;
        if (value != 0U) { result = TR2_ERROR_UNSUPPORTED; goto finish; }
    }
    result = fifo_status(&source,&status,&level);
    tr2_fifo_info.initial_status = status;
    if (result != TR2_OK || level != 0U || (status&0x4800U) != 0U) { if (result == TR2_OK) result = TR2_ERROR_INVALID_STATE; goto finish; }
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    if ((DWT->CTRL&DWT_CTRL_NOCYCCNT_Msk) != 0U) { result = TR2_ERROR_UNSUPPORTED; goto finish; }
    DWT->CYCCNT = 0U; DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    { uint32_t before = DWT->CYCCNT; HAL_Delay(2U);
      if (DWT->CYCCNT == before) { result = TR2_ERROR_UNAVAILABLE; goto finish; }
    }
    tr2_fifo_info.core_hz = HAL_RCC_GetHCLKFreq();
    tr2_fifo_info.spi_hz = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3)/256U;
    interface = stm32_iis3dwb_vibration_source_interface(&source);
    changed = true;
    result = interface.configure(interface.context,&configuration);
    if (result != TR2_OK) goto finish;
    /* FIFO mode (not continuous), 256-word capacity, XYZ batch 26667 Hz.
     * Temperature and sensor timestamp batches are disabled. */
    { const uint8_t values[4] = {0U,0x81U,0x0AU,0U};
      for (unsigned i = 0U;i < 4U;i++) {
        result = write_checked(&source,saved_regs[i],values[i]);
        if (result != TR2_OK) goto finish;
      }
    }
    HAL_Delay(100U);
    tr2_fifo_info.stage = 3U;
    tr2_fifo_info.start_low = DWT->CYCCNT;
    result = write_checked(&source,0x0AU,1U);
    tr2_fifo_info.start_high = DWT->CYCCNT;
    if (result != TR2_OK) goto finish;
    for (unsigned i = 0U;i < 7U;i++) {
        result = read_bytes(&source,saved_regs[i],&value,1U);
        if (result != TR2_OK) goto finish;
        tr2_fifo_info.configured[i] = value;
    }
    if (tr2_fifo_info.configured[4] != 0xA8U || tr2_fifo_info.configured[5] != 0x44U) { result = TR2_ERROR_INTERNAL; goto finish; }
    { uint32_t deadline = HAL_GetTick();
      do {
        uint32_t begin = DWT->CYCCNT;
        result = fifo_status(&source,&status,&level);
        uint32_t end = DWT->CYCCNT;
        if (result != TR2_OK) goto finish;
        uint32_t index = tr2_fifo_info.poll_count;
        if (index >= TR2_FIFO_POLLS || (uint32_t)(HAL_GetTick()-deadline) >= 100U) { tr2_fifo_info.loss_flags |= 0x40U; result = TR2_ERROR_UNAVAILABLE; goto finish; }
        Stm32Iis3dwbFifoPoll record = {begin,end,status,level};
        tr2_fifo_polls[index] = record; tr2_fifo_info.poll_count = index+1U;
        if ((status&0x4800U) != 0U) tr2_fifo_info.loss_flags |= 1U;
        if ((status&0x8000U) == 0U) tr2_fifo_info.last_not_wm_low = begin;
        else tr2_fifo_info.wm_high = end;
      } while ((status&0x8000U) == 0U);
    }
    tr2_fifo_info.filled_status = status;
    if (level != TR2_FIFO_WORDS || tr2_fifo_info.loss_flags != 0U) { tr2_fifo_info.loss_flags |= 0x20U; result = TR2_ERROR_INVALID_STATE; goto finish; }
    tr2_fifo_info.stage = 4U;
    tr2_fifo_info.drain_low = DWT->CYCCNT;
    for (unsigned i = 0U;i < TR2_FIFO_WORDS;i++) {
        uint32_t before_level, before_status, begin, end;
        result = fifo_status(&source,&before_status,&before_level);
        if (result != TR2_OK) goto finish;
        if (before_level == 0U) { tr2_fifo_info.loss_flags |= 0x20U; result = TR2_ERROR_INVALID_STATE; goto finish; }
        begin = DWT->CYCCNT; result = read_bytes(&source,0x78U,frame,7U); end = DWT->CYCCNT;
        if (result != TR2_OK) goto finish;
        result = fifo_status(&source,&status,&level);
        if (result != TR2_OK) goto finish;
        uint32_t counter = (frame[0]>>1U)&3U;
        bool parity = parity_even(frame[0]);
        bool sequence = i == 0U || counter == ((previous_counter+1U)&3U);
        int32_t x = (int16_t)(frame[1]|((uint16_t)frame[2]<<8U));
        int32_t y = (int16_t)(frame[3]|((uint16_t)frame[4]<<8U));
        int32_t z = (int16_t)(frame[5]|((uint16_t)frame[6]<<8U));
        bool saturation = x == INT16_MIN || x == INT16_MAX || y == INT16_MIN || y == INT16_MAX || z == INT16_MIN || z == INT16_MAX;
        if ((before_status&0x4800U) != 0U || (status&0x4800U) != 0U) tr2_fifo_info.loss_flags |= 1U;
        if ((frame[0]>>3U) != 2U) { tr2_fifo_info.tag_errors++; tr2_fifo_info.loss_flags |= 2U; }
        if (!parity) { tr2_fifo_info.parity_errors++; tr2_fifo_info.loss_flags |= 4U; }
        if (!sequence) { tr2_fifo_info.gap_events++; tr2_fifo_info.holes_mod4 += (counter-previous_counter-1U)&3U; tr2_fifo_info.loss_flags |= 8U; }
        if (level+1U != before_level || before_level != TR2_FIFO_WORDS-i) tr2_fifo_info.loss_flags |= 0x20U;
        if (saturation) tr2_fifo_info.saturations++;
        Stm32Iis3dwbFifoWord record = {begin,end,frame[0],before_level,before_status,level,status,x,y,z,sequence?1U:0U,parity?1U:0U,saturation?1U:0U,(uint32_t)result};
        tr2_fifo_words[i] = record; tr2_fifo_info.count = i+1U; previous_counter = counter;
    }
    tr2_fifo_info.drain_high = DWT->CYCCNT;
    tr2_fifo_info.empty_status = status;
    if (level != 0U || tr2_fifo_info.loss_flags != 0U) { result = TR2_ERROR_INVALID_STATE; goto finish; }
    /* One bounded negative control in the same campaign: continuous FIFO,
     * no draining for 32ms (>512/26667). Never accept its overwritten words. */
    tr2_fifo_info.stage = 5U;
    result = write_checked(&source,0x0AU,0U);
    if (result == TR2_OK) result = write_checked(&source,0x08U,1U);
    if (result == TR2_OK) result = write_checked(&source,0x0AU,6U);
    if (result != TR2_OK) goto finish;
    HAL_Delay(32U);
    result = fifo_status(&source,&status,&level); tr2_fifo_info.negative_status = status;
    if (result != TR2_OK) goto finish;
    tr2_fifo_info.negative_rejected = (status&0x4800U) != 0U ? 1U : 0U;
    if (tr2_fifo_info.negative_rejected == 0U) result = TR2_ERROR_INTERNAL;
finish:
    tr2_fifo_info.result = (uint32_t)result;
    if (saved) {
        Tr2Result restoration = TR2_OK;
        if (changed) {
            /* Stop sensor first, clear only our FIFO, then restore all values. */
            const uint8_t order[7] = {4U,3U,2U,1U,0U,6U,5U};
            for (unsigned i = 0U;i < 7U;i++) {
                unsigned index = order[i];
                Tr2Result step = write_checked(&source,saved_regs[index],(uint8_t)tr2_fifo_info.saved[index]);
                if (step != TR2_OK) restoration = step;
            }
        }
        for (unsigned i = 0U;i < 7U;i++) {
            Tr2Result step = read_bytes(&source,saved_regs[i],&value,1U); tr2_fifo_info.restored[i] = value;
            if (step != TR2_OK || value != tr2_fifo_info.saved[i]) restoration = step != TR2_OK ? step : TR2_ERROR_INTERNAL;
        }
        tr2_fifo_info.restore_result = (uint32_t)restoration;
    }
    tr2_fifo_info.stage = 10U;
    for (;;) HAL_Delay(100U);
}
#else
typedef int Tr2Iis3dwbFifoCaptureDisabled;
#endif
