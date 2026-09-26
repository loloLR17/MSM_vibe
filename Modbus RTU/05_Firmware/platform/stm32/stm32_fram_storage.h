#ifndef TR2_STM32_FRAM_STORAGE_H
#define TR2_STM32_FRAM_STORAGE_H

#include <stddef.h>
#include <stdint.h>

#include "stm32u5xx_hal.h"

#include "tr2/common/result.h"
#include "tr2/persistence/transactional_image_media.h"

#define TR2_STM32_FRAM_CAPACITY ((size_t)262144U)

typedef struct {
    SPI_HandleTypeDef *spi;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
    uint32_t timeout_ms;
} Stm32FramStorage;

Tr2Result stm32_fram_storage_init(
    Stm32FramStorage *storage,
    SPI_HandleTypeDef *spi,
    GPIO_TypeDef *cs_port,
    uint16_t cs_pin,
    uint32_t timeout_ms);

TransactionalImagePhysicalStorage stm32_fram_storage_physical(
    Stm32FramStorage *storage);

Tr2Result stm32_fram_storage_read(
    void *context,
    uint32_t offset,
    void *buffer,
    size_t size);

Tr2Result stm32_fram_storage_write(
    void *context,
    uint32_t offset,
    const void *buffer,
    size_t size);

#endif
