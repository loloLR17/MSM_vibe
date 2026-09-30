#include "stm32_sdmmc_bulk_media.h"

#include <string.h>

static Tr2Result wait_transfer(Stm32SdmmcBulkMedia *adapter)
{
    uint32_t start;

    if (adapter == NULL || !adapter->initialized || adapter->hsd == NULL) {
        return TR2_ERROR_INVALID_STATE;
    }

    start = HAL_GetTick();
    for (;;) {
        HAL_SD_CardStateTypeDef state = HAL_SD_GetCardState(adapter->hsd);

        if (state == HAL_SD_CARD_TRANSFER) {
            return TR2_OK;
        }
        if (state == HAL_SD_CARD_ERROR ||
            state == HAL_SD_CARD_DISCONNECTED) {
            return TR2_ERROR_STORAGE;
        }
        if ((HAL_GetTick() - start) >= adapter->ready_timeout_ms) {
            return TR2_ERROR_UNAVAILABLE;
        }
    }
}

static bool range_valid(const Stm32SdmmcBulkMedia *adapter,
                        uint64_t offset,
                        size_t size)
{
    return adapter != NULL &&
           offset <= adapter->capacity_bytes &&
           (uint64_t)size <= adapter->capacity_bytes - offset;
}

static Tr2Result read_blocks(Stm32SdmmcBulkMedia *adapter,
                             uint32_t block,
                             uint8_t *buffer,
                             uint32_t count)
{
    Tr2Result result = wait_transfer(adapter);

    if (result != TR2_OK) {
        return result;
    }
    if (HAL_SD_ReadBlocks(adapter->hsd,
                          buffer,
                          block,
                          count,
                          adapter->io_timeout_ms) != HAL_OK) {
        return TR2_ERROR_STORAGE;
    }
    return wait_transfer(adapter);
}

static Tr2Result write_blocks(Stm32SdmmcBulkMedia *adapter,
                              uint32_t block,
                              const uint8_t *buffer,
                              uint32_t count)
{
    Tr2Result result = wait_transfer(adapter);

    if (result != TR2_OK) {
        return result;
    }
    if (HAL_SD_WriteBlocks(adapter->hsd,
                           buffer,
                           block,
                           count,
                           adapter->io_timeout_ms) != HAL_OK) {
        return TR2_ERROR_STORAGE;
    }
    return wait_transfer(adapter);
}

static Tr2Result media_capacity(void *context, uint64_t *capacity_bytes)
{
    Stm32SdmmcBulkMedia *adapter = context;

    if (!stm32_sdmmc_bulk_media_is_initialized(adapter) ||
        capacity_bytes == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    *capacity_bytes = adapter->capacity_bytes;
    return TR2_OK;
}

static Tr2Result media_read(void *context,
                            uint64_t offset,
                            void *buffer,
                            size_t size)
{
    Stm32SdmmcBulkMedia *adapter = context;
    uint8_t *out = buffer;

    if (!stm32_sdmmc_bulk_media_is_initialized(adapter) ||
        (buffer == NULL && size != 0u)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!range_valid(adapter, offset, size)) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    while (size != 0u) {
        uint32_t block = (uint32_t)(offset / TR2_STM32_SDMMC_LOGICAL_BLOCK_SIZE);
        size_t in_block =
            (size_t)(offset % TR2_STM32_SDMMC_LOGICAL_BLOCK_SIZE);

        if (in_block != 0u || size < TR2_STM32_SDMMC_LOGICAL_BLOCK_SIZE) {
            size_t take = TR2_STM32_SDMMC_LOGICAL_BLOCK_SIZE - in_block;
            Tr2Result result;

            if (take > size) {
                take = size;
            }
            result = read_blocks(adapter, block, adapter->sector_scratch, 1u);
            if (result != TR2_OK) {
                return result;
            }
            memcpy(out, &adapter->sector_scratch[in_block], take);
            out += take;
            offset += (uint64_t)take;
            size -= take;
        } else {
            uint64_t full_blocks =
                (uint64_t)(size / TR2_STM32_SDMMC_LOGICAL_BLOCK_SIZE);
            uint32_t count = full_blocks > UINT32_MAX
                                 ? UINT32_MAX
                                 : (uint32_t)full_blocks;
            size_t bytes =
                (size_t)count * TR2_STM32_SDMMC_LOGICAL_BLOCK_SIZE;
            Tr2Result result = read_blocks(adapter, block, out, count);

            if (result != TR2_OK) {
                return result;
            }
            out += bytes;
            offset += (uint64_t)bytes;
            size -= bytes;
        }
    }
    return TR2_OK;
}

static Tr2Result media_write(void *context,
                             uint64_t offset,
                             const void *buffer,
                             size_t size)
{
    Stm32SdmmcBulkMedia *adapter = context;
    const uint8_t *in = buffer;

    if (!stm32_sdmmc_bulk_media_is_initialized(adapter) ||
        (buffer == NULL && size != 0u)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!range_valid(adapter, offset, size)) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    while (size != 0u) {
        uint32_t block = (uint32_t)(offset / TR2_STM32_SDMMC_LOGICAL_BLOCK_SIZE);
        size_t in_block =
            (size_t)(offset % TR2_STM32_SDMMC_LOGICAL_BLOCK_SIZE);

        if (in_block != 0u || size < TR2_STM32_SDMMC_LOGICAL_BLOCK_SIZE) {
            size_t take = TR2_STM32_SDMMC_LOGICAL_BLOCK_SIZE - in_block;
            Tr2Result result;

            if (take > size) {
                take = size;
            }
            result = read_blocks(adapter, block, adapter->sector_scratch, 1u);
            if (result != TR2_OK) {
                return result;
            }
            memcpy(&adapter->sector_scratch[in_block], in, take);
            result = write_blocks(adapter, block, adapter->sector_scratch, 1u);
            if (result != TR2_OK) {
                return result;
            }
            in += take;
            offset += (uint64_t)take;
            size -= take;
        } else {
            uint64_t full_blocks =
                (uint64_t)(size / TR2_STM32_SDMMC_LOGICAL_BLOCK_SIZE);
            uint32_t count = full_blocks > UINT32_MAX
                                 ? UINT32_MAX
                                 : (uint32_t)full_blocks;
            size_t bytes =
                (size_t)count * TR2_STM32_SDMMC_LOGICAL_BLOCK_SIZE;
            Tr2Result result = write_blocks(adapter, block, in, count);

            if (result != TR2_OK) {
                return result;
            }
            in += bytes;
            offset += (uint64_t)bytes;
            size -= bytes;
        }
    }
    return TR2_OK;
}

static Tr2Result media_sync(void *context)
{
    Stm32SdmmcBulkMedia *adapter = context;

    if (!stm32_sdmmc_bulk_media_is_initialized(adapter)) {
        return TR2_ERROR_INVALID_STATE;
    }
    return wait_transfer(adapter);
}

Tr2Result stm32_sdmmc_bulk_media_init(
    Stm32SdmmcBulkMedia *adapter,
    SD_HandleTypeDef *hsd,
    uint8_t *sector_scratch,
    size_t sector_scratch_size,
    uint32_t io_timeout_ms,
    uint32_t ready_timeout_ms)
{
    HAL_SD_CardInfoTypeDef info = {0};
    uint64_t capacity;

    if (adapter == NULL || hsd == NULL || sector_scratch == NULL ||
        sector_scratch_size < TR2_STM32_SDMMC_LOGICAL_BLOCK_SIZE ||
        io_timeout_ms == 0u || ready_timeout_ms == 0u) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (HAL_SD_GetCardInfo(hsd, &info) != HAL_OK) {
        return TR2_ERROR_UNAVAILABLE;
    }
    if (info.LogBlockSize != TR2_STM32_SDMMC_LOGICAL_BLOCK_SIZE ||
        info.LogBlockNbr == 0u) {
        return TR2_ERROR_UNSUPPORTED;
    }

    capacity = (uint64_t)info.LogBlockNbr *
               (uint64_t)info.LogBlockSize;

    memset(adapter, 0, sizeof(*adapter));
    adapter->hsd = hsd;
    adapter->sector_scratch = sector_scratch;
    adapter->sector_scratch_size = sector_scratch_size;
    adapter->io_timeout_ms = io_timeout_ms;
    adapter->ready_timeout_ms = ready_timeout_ms;
    adapter->capacity_bytes = capacity;
    adapter->initialized = true;

    return wait_transfer(adapter);
}

CampaignBulkMedia stm32_sdmmc_bulk_media_interface(
    Stm32SdmmcBulkMedia *adapter)
{
    CampaignBulkMedia media = {
        adapter,
        media_capacity,
        media_read,
        media_write,
        media_sync
    };
    return media;
}

bool stm32_sdmmc_bulk_media_is_initialized(
    const Stm32SdmmcBulkMedia *adapter)
{
    return adapter != NULL && adapter->initialized &&
           adapter->hsd != NULL &&
           adapter->sector_scratch != NULL &&
           adapter->sector_scratch_size >=
               TR2_STM32_SDMMC_LOGICAL_BLOCK_SIZE;
}
