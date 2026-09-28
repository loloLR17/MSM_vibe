#include "stm32_iis3dwb_vibration_source.h"

#include <limits.h>
#include <stddef.h>
#include <string.h>

#define IIS3DWB_SPI_TIMEOUT_MS 10U
#define IIS3DWB_WHO_AM_I_REG 0x0FU
#define IIS3DWB_WHO_AM_I_EXPECTED 0x7BU
#define IIS3DWB_CTRL1_XL_REG 0x10U
#define IIS3DWB_CTRL3_C_REG 0x12U
#define IIS3DWB_STATUS_REG 0x1EU
#define IIS3DWB_OUTX_L_A_REG 0x28U
#define IIS3DWB_STATUS_XLDA 0x01U
#define IIS3DWB_CTRL3_C_BDU_IF_INC 0x44U
#define IIS3DWB_ODR_26K7HZ 0xA0U
#define IIS3DWB_FS_2G 0x00U
#define IIS3DWB_FS_4G 0x08U
#define IIS3DWB_FS_8G 0x0CU
#define IIS3DWB_FS_16G 0x04U
#define IIS3DWB_FS_MASK 0x0CU
#define IIS3DWB_SAMPLING_FREQUENCY_HZ 26667U
#define IIS3DWB_AXES_MASK_ALL 0x0007U

static Tr2Result hal_to_tr2(HAL_StatusTypeDef status)
{
    if (status == HAL_OK) {
        return TR2_OK;
    }
    if (status == HAL_TIMEOUT || status == HAL_BUSY) {
        return TR2_ERROR_UNAVAILABLE;
    }
    return TR2_ERROR_INTERNAL;
}

static HAL_StatusTypeDef read_registers(
    Stm32Iis3dwbVibrationSource *source,
    uint8_t reg,
    uint8_t *data,
    uint16_t length)
{
    uint8_t command = (uint8_t)(reg | 0x80U);
    HAL_StatusTypeDef status;

    if (source == NULL || source->spi == NULL || source->cs_port == NULL ||
        data == NULL || length == 0U) {
        return HAL_ERROR;
    }

    HAL_GPIO_WritePin(source->cs_port, source->cs_pin, GPIO_PIN_RESET);
    status = HAL_SPI_Transmit(source->spi, &command, 1U, IIS3DWB_SPI_TIMEOUT_MS);
    if (status == HAL_OK) {
        status = HAL_SPI_Receive(source->spi, data, length, IIS3DWB_SPI_TIMEOUT_MS);
    }
    HAL_GPIO_WritePin(source->cs_port, source->cs_pin, GPIO_PIN_SET);
    return status;
}

static HAL_StatusTypeDef write_register(
    Stm32Iis3dwbVibrationSource *source,
    uint8_t reg,
    uint8_t value)
{
    uint8_t frame[2] = {(uint8_t)(reg & 0x7FU), value};
    HAL_StatusTypeDef status;

    if (source == NULL || source->spi == NULL || source->cs_port == NULL) {
        return HAL_ERROR;
    }

    HAL_GPIO_WritePin(source->cs_port, source->cs_pin, GPIO_PIN_RESET);
    status = HAL_SPI_Transmit(source->spi, frame, sizeof(frame), IIS3DWB_SPI_TIMEOUT_MS);
    HAL_GPIO_WritePin(source->cs_port, source->cs_pin, GPIO_PIN_SET);
    return status;
}

static bool full_scale_bits(uint16_t code, uint8_t *bits)
{
    if (bits == NULL) {
        return false;
    }

    switch (code) {
    case 0U: *bits = IIS3DWB_FS_2G; return true;
    case 1U: *bits = IIS3DWB_FS_4G; return true;
    case 2U: *bits = IIS3DWB_FS_8G; return true;
    case 3U: *bits = IIS3DWB_FS_16G; return true;
    default: return false;
    }
}

static int32_t raw_to_mg(int16_t raw, uint16_t full_scale_code)
{
    static const int32_t sensitivity_ug_per_lsb[4] = {61, 122, 244, 488};
    int32_t micro_g = (int32_t)raw * sensitivity_ug_per_lsb[full_scale_code];

    if (micro_g >= 0) {
        return (micro_g + 500) / 1000;
    }
    return (micro_g - 500) / 1000;
}

static Tr2Result adapter_configure(
    void *context,
    const VibrationSourceConfiguration *configuration)
{
    Stm32Iis3dwbVibrationSource *source = (Stm32Iis3dwbVibrationSource *)context;
    uint8_t fs_bits = 0U;
    HAL_StatusTypeDef status;

    if (source == NULL || configuration == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (source->started) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (configuration->sampling_frequency_hz != IIS3DWB_SAMPLING_FREQUENCY_HZ ||
        configuration->axes_enable_mask == 0U ||
        (configuration->axes_enable_mask & (uint16_t)~IIS3DWB_AXES_MASK_ALL) != 0U ||
        !full_scale_bits(configuration->full_scale_code, &fs_bits)) {
        return TR2_ERROR_UNSUPPORTED;
    }

    status = write_register(source, IIS3DWB_CTRL3_C_REG, IIS3DWB_CTRL3_C_BDU_IF_INC);
    if (status != HAL_OK) {
        return hal_to_tr2(status);
    }

    status = write_register(source, IIS3DWB_CTRL1_XL_REG,
                            (uint8_t)(IIS3DWB_ODR_26K7HZ | (fs_bits & IIS3DWB_FS_MASK)));
    if (status != HAL_OK) {
        return hal_to_tr2(status);
    }

    source->axes_enable_mask = configuration->axes_enable_mask;
    source->full_scale_code = configuration->full_scale_code;
    source->configured = true;
    return TR2_OK;
}

static Tr2Result adapter_start(void *context)
{
    Stm32Iis3dwbVibrationSource *source = (Stm32Iis3dwbVibrationSource *)context;

    if (source == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!source->configured || source->started) {
        return TR2_ERROR_INVALID_STATE;
    }
    source->started = true;
    return TR2_OK;
}

static Tr2Result adapter_read(void *context, VibrationSample *sample)
{
    Stm32Iis3dwbVibrationSource *source = (Stm32Iis3dwbVibrationSource *)context;
    uint8_t status_reg = 0U;
    uint8_t raw_bytes[6] = {0U};
    int16_t raw[3];
    HAL_StatusTypeDef status;

    if (source == NULL || sample == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!source->started) {
        return TR2_ERROR_INVALID_STATE;
    }

    status = read_registers(source, IIS3DWB_STATUS_REG, &status_reg, 1U);
    if (status != HAL_OK) {
        return hal_to_tr2(status);
    }
    if ((status_reg & IIS3DWB_STATUS_XLDA) == 0U) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    status = read_registers(source, IIS3DWB_OUTX_L_A_REG, raw_bytes, sizeof(raw_bytes));
    if (status != HAL_OK) {
        return hal_to_tr2(status);
    }

    raw[0] = (int16_t)((uint16_t)raw_bytes[0] | ((uint16_t)raw_bytes[1] << 8));
    raw[1] = (int16_t)((uint16_t)raw_bytes[2] | ((uint16_t)raw_bytes[3] << 8));
    raw[2] = (int16_t)((uint16_t)raw_bytes[4] | ((uint16_t)raw_bytes[5] << 8));

    memset(sample, 0, sizeof(*sample));
    if ((source->axes_enable_mask & 0x0001U) != 0U) {
        sample->x_mg = raw_to_mg(raw[0], source->full_scale_code);
    }
    if ((source->axes_enable_mask & 0x0002U) != 0U) {
        sample->y_mg = raw_to_mg(raw[1], source->full_scale_code);
    }
    if ((source->axes_enable_mask & 0x0004U) != 0U) {
        sample->z_mg = raw_to_mg(raw[2], source->full_scale_code);
    }

    sample->valid = true;
    sample->saturated =
        raw[0] == INT16_MIN || raw[0] == INT16_MAX ||
        raw[1] == INT16_MIN || raw[1] == INT16_MAX ||
        raw[2] == INT16_MIN || raw[2] == INT16_MAX;
    return TR2_OK;
}

static Tr2Result adapter_stop(void *context)
{
    Stm32Iis3dwbVibrationSource *source = (Stm32Iis3dwbVibrationSource *)context;

    if (source == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!source->started) {
        return TR2_ERROR_INVALID_STATE;
    }
    source->started = false;
    return TR2_OK;
}

Tr2Result stm32_iis3dwb_vibration_source_init(
    Stm32Iis3dwbVibrationSource *source,
    SPI_HandleTypeDef *spi,
    GPIO_TypeDef *cs_port,
    uint16_t cs_pin)
{
    uint8_t who_am_i = 0U;
    HAL_StatusTypeDef status;

    if (source == NULL || spi == NULL || cs_port == NULL || cs_pin == 0U) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(source, 0, sizeof(*source));
    source->spi = spi;
    source->cs_port = cs_port;
    source->cs_pin = cs_pin;

    status = read_registers(source, IIS3DWB_WHO_AM_I_REG, &who_am_i, 1U);
    if (status != HAL_OK) {
        return hal_to_tr2(status);
    }
    if (who_am_i != IIS3DWB_WHO_AM_I_EXPECTED) {
        return TR2_ERROR_NOT_FOUND;
    }
    return TR2_OK;
}

VibrationSource stm32_iis3dwb_vibration_source_interface(
    Stm32Iis3dwbVibrationSource *source)
{
    VibrationSource interface = {
        source,
        adapter_configure,
        adapter_start,
        adapter_read,
        adapter_stop
    };
    return interface;
}

Tr2Result stm32_iis3dwb_vibration_source_read_who_am_i(
    Stm32Iis3dwbVibrationSource *source,
    uint8_t *who_am_i)
{
    if (source == NULL || who_am_i == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    return hal_to_tr2(read_registers(source, IIS3DWB_WHO_AM_I_REG, who_am_i, 1U));
}
