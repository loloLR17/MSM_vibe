#ifndef TR2_STM32_SDMMC_BULK_MEDIA_H
#define TR2_STM32_SDMMC_BULK_MEDIA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "stm32u5xx_hal.h"

#include "tr2/common/result.h"
#include "tr2/persistence/campaign_bulk_media.h"

#define TR2_STM32_SDMMC_LOGICAL_BLOCK_SIZE ((size_t)512u)

typedef struct {
    SD_HandleTypeDef *hsd;
    uint8_t *sector_scratch;
    size_t sector_scratch_size;
    uint32_t io_timeout_ms;
    uint32_t ready_timeout_ms;
    uint64_t capacity_bytes;
    bool initialized;
} Stm32SdmmcBulkMedia;

Tr2Result stm32_sdmmc_bulk_media_init(
    Stm32SdmmcBulkMedia *adapter,
    SD_HandleTypeDef *hsd,
    uint8_t *sector_scratch,
    size_t sector_scratch_size,
    uint32_t io_timeout_ms,
    uint32_t ready_timeout_ms);

CampaignBulkMedia stm32_sdmmc_bulk_media_interface(
    Stm32SdmmcBulkMedia *adapter);

bool stm32_sdmmc_bulk_media_is_initialized(
    const Stm32SdmmcBulkMedia *adapter);

#endif
