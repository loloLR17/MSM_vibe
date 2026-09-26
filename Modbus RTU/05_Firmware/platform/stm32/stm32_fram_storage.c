#include "stm32_fram_storage.h"

#define FRAM_COMMAND_WREN  UINT8_C(0x06)
#define FRAM_COMMAND_WRDI  UINT8_C(0x04)
#define FRAM_COMMAND_RDSR  UINT8_C(0x05)
#define FRAM_COMMAND_READ  UINT8_C(0x03)
#define FRAM_COMMAND_WRITE UINT8_C(0x02)

#define FRAM_STATUS_WEL UINT8_C(0x02)

#define FRAM_TRANSFER_CHUNK_SIZE ((size_t)256U)

static bool range_is_valid(uint32_t offset, size_t size)
{
    if (size > TR2_STM32_FRAM_CAPACITY) {
        return false;
    }

    return ((size_t)offset <= (TR2_STM32_FRAM_CAPACITY - size));
}

static void cs_high(const Stm32FramStorage *storage)
{
    HAL_GPIO_WritePin(storage->cs_port, storage->cs_pin, GPIO_PIN_SET);
}

static void cs_low(const Stm32FramStorage *storage)
{
    HAL_GPIO_WritePin(storage->cs_port, storage->cs_pin, GPIO_PIN_RESET);
}

static HAL_StatusTypeDef transmit(
    Stm32FramStorage *storage,
    const uint8_t *data,
    size_t size)
{
    if (size > UINT16_MAX) {
        return HAL_ERROR;
    }

    return HAL_SPI_Transmit(
        storage->spi,
        (uint8_t *)(uintptr_t)data,
        (uint16_t)size,
        storage->timeout_ms);
}

static HAL_StatusTypeDef receive(
    Stm32FramStorage *storage,
    uint8_t *data,
    size_t size)
{
    if (size > UINT16_MAX) {
        return HAL_ERROR;
    }

    return HAL_SPI_Receive(
        storage->spi,
        data,
        (uint16_t)size,
        storage->timeout_ms);
}

static HAL_StatusTypeDef transmit_chunked(
    Stm32FramStorage *storage,
    const uint8_t *data,
    size_t size)
{
    while (size != 0U) {
        size_t chunk = size;
        HAL_StatusTypeDef status;

        if (chunk > FRAM_TRANSFER_CHUNK_SIZE) {
            chunk = FRAM_TRANSFER_CHUNK_SIZE;
        }

        status = transmit(storage, data, chunk);
        if (status != HAL_OK) {
            return status;
        }

        data += chunk;
        size -= chunk;
    }

    return HAL_OK;
}

static HAL_StatusTypeDef receive_chunked(
    Stm32FramStorage *storage,
    uint8_t *data,
    size_t size)
{
    while (size != 0U) {
        size_t chunk = size;
        HAL_StatusTypeDef status;

        if (chunk > FRAM_TRANSFER_CHUNK_SIZE) {
            chunk = FRAM_TRANSFER_CHUNK_SIZE;
        }

        status = receive(storage, data, chunk);
        if (status != HAL_OK) {
            return status;
        }

        data += chunk;
        size -= chunk;
    }

    return HAL_OK;
}

static void build_command_address(
    uint8_t frame[4],
    uint8_t command,
    uint32_t offset)
{
    frame[0] = command;
    frame[1] = (uint8_t)((offset >> 16U) & UINT32_C(0xFF));
    frame[2] = (uint8_t)((offset >> 8U) & UINT32_C(0xFF));
    frame[3] = (uint8_t)(offset & UINT32_C(0xFF));
}

static HAL_StatusTypeDef send_simple_command(
    Stm32FramStorage *storage,
    uint8_t command)
{
    HAL_StatusTypeDef status;

    cs_low(storage);
    status = transmit(storage, &command, 1U);
    cs_high(storage);

    return status;
}

static HAL_StatusTypeDef read_status_register(
    Stm32FramStorage *storage,
    uint8_t *status_register)
{
    uint8_t command = FRAM_COMMAND_RDSR;
    HAL_StatusTypeDef status;

    cs_low(storage);
    status = transmit(storage, &command, 1U);
    if (status == HAL_OK) {
        status = receive(storage, status_register, 1U);
    }
    cs_high(storage);

    return status;
}

Tr2Result stm32_fram_storage_init(
    Stm32FramStorage *storage,
    SPI_HandleTypeDef *spi,
    GPIO_TypeDef *cs_port,
    uint16_t cs_pin,
    uint32_t timeout_ms)
{
    if ((storage == NULL) ||
        (spi == NULL) ||
        (cs_port == NULL) ||
        (cs_pin == 0U) ||
        (timeout_ms == 0U)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    storage->spi = spi;
    storage->cs_port = cs_port;
    storage->cs_pin = cs_pin;
    storage->timeout_ms = timeout_ms;
    cs_high(storage);

    return TR2_OK;
}

TransactionalImagePhysicalStorage stm32_fram_storage_physical(
    Stm32FramStorage *storage)
{
    TransactionalImagePhysicalStorage physical = {
        .context = storage,
        .read = stm32_fram_storage_read,
        .write = stm32_fram_storage_write
    };

    return physical;
}

Tr2Result stm32_fram_storage_read(
    void *context,
    uint32_t offset,
    void *buffer,
    size_t size)
{
    Stm32FramStorage *storage = (Stm32FramStorage *)context;
    uint8_t frame[4];

    if (storage == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!range_is_valid(offset, size)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (size == 0U) {
        return TR2_OK;
    }
    if (buffer == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    build_command_address(frame, FRAM_COMMAND_READ, offset);

    cs_low(storage);
    HAL_StatusTypeDef status = transmit(storage, frame, sizeof(frame));
    if (status == HAL_OK) {
        status = receive_chunked(storage, (uint8_t *)buffer, size);
    }
    cs_high(storage);

    return (status == HAL_OK) ? TR2_OK : TR2_ERROR_STORAGE;
}

Tr2Result stm32_fram_storage_write(
    void *context,
    uint32_t offset,
    const void *buffer,
    size_t size)
{
    Stm32FramStorage *storage = (Stm32FramStorage *)context;
    uint8_t frame[4];
    uint8_t status_register = 0U;
    HAL_StatusTypeDef status;
    HAL_StatusTypeDef close_status;

    if (storage == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!range_is_valid(offset, size)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (size == 0U) {
        return TR2_OK;
    }
    if (buffer == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    status = send_simple_command(storage, FRAM_COMMAND_WREN);
    if (status == HAL_OK) {
        status = read_status_register(storage, &status_register);
    }
    if ((status == HAL_OK) && ((status_register & FRAM_STATUS_WEL) == 0U)) {
        status = HAL_ERROR;
    }

    if (status == HAL_OK) {
        build_command_address(frame, FRAM_COMMAND_WRITE, offset);

        cs_low(storage);
        status = transmit(storage, frame, sizeof(frame));
        if (status == HAL_OK) {
            status = transmit_chunked(storage, (const uint8_t *)buffer, size);
        }
        cs_high(storage);
    }

    close_status = send_simple_command(storage, FRAM_COMMAND_WRDI);
    if ((status == HAL_OK) && (close_status != HAL_OK)) {
        status = close_status;
    }

    return (status == HAL_OK) ? TR2_OK : TR2_ERROR_STORAGE;
}
