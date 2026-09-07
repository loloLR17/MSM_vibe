#include <stddef.h>

#include "tr2/modbus/projection.h"
#include "tr2/modbus/read_adapter.h"

#define TR2_B5_BASE_ADDRESS UINT16_C(5000)
#define TR2_B6_BASE_ADDRESS UINT16_C(6000)
#define TR2_B6_LAST_ADDRESS UINT16_C(6063)

static ModbusReadOutcome read_from_b0(const ModbusReadSources *sources,
                                      uint16_t start_address,
                                      uint16_t quantity,
                                      uint16_t *values)
{
    ModbusReadOutcome outcome = { MODBUS_ACCESS_OK, TR2_OK };
    ModbusBlock0Image image;
    uint16_t index;

    if (sources->identity == NULL) {
        outcome.operation_result = TR2_ERROR_NOT_AVAILABLE;
        return outcome;
    }
    outcome.operation_result = modbus_project_b0(sources->identity, &image);
    if (outcome.operation_result != TR2_OK) {
        return outcome;
    }
    for (index = 0u; index < quantity; ++index) {
        values[index] = image.registers[(uint16_t)(start_address + index)];
    }
    return outcome;
}

static ModbusReadOutcome read_from_b1(const ModbusReadSources *sources,
                                      uint16_t start_address,
                                      uint16_t quantity,
                                      uint16_t *values)
{
    ModbusReadOutcome outcome = { MODBUS_ACCESS_OK, TR2_OK };
    ModbusBlock1Image image;
    ModbusBlock1ProjectionSource source;
    uint16_t index;

    if (sources->system_state == NULL) {
        outcome.operation_result = TR2_ERROR_NOT_AVAILABLE;
        return outcome;
    }
    source.system_state = sources->system_state;
    source.time = sources->time;
    outcome.operation_result = modbus_project_b1(&source, &image);
    if (outcome.operation_result != TR2_OK) {
        return outcome;
    }
    for (index = 0u; index < quantity; ++index) {
        values[index] = image.registers[(uint16_t)(start_address - UINT16_C(1000) + index)];
    }
    return outcome;
}

static ModbusReadOutcome read_from_b2(const ModbusReadSources *sources,
                                      uint16_t start_address,
                                      uint16_t quantity,
                                      uint16_t *values)
{
    ModbusReadOutcome outcome = { MODBUS_ACCESS_OK, TR2_OK };
    ModbusBlock2Image image;
    uint16_t index;

    if (sources->time == NULL) {
        outcome.operation_result = TR2_ERROR_NOT_AVAILABLE;
        return outcome;
    }
    outcome.operation_result = modbus_project_b2(sources->time, &image);
    if (outcome.operation_result != TR2_OK) {
        return outcome;
    }
    for (index = 0u; index < quantity; ++index) {
        values[index] = image.registers[(uint16_t)(start_address - UINT16_C(2000) + index)];
    }
    return outcome;
}

static ModbusReadOutcome read_from_b3(const ModbusReadSources *sources,
                                      uint16_t start_address,
                                      uint16_t quantity,
                                      uint16_t *values)
{
    ModbusReadOutcome outcome = { MODBUS_ACCESS_OK, TR2_OK };
    ModbusBlock3Image image;
    uint16_t index;

    if (sources->supervision == NULL) {
        outcome.operation_result = TR2_ERROR_NOT_AVAILABLE;
        return outcome;
    }
    outcome.operation_result = modbus_project_b3(sources->supervision, &image);
    if (outcome.operation_result != TR2_OK) {
        return outcome;
    }
    for (index = 0u; index < quantity; ++index) {
        values[index] = image.registers[(uint16_t)(start_address - UINT16_C(3000) + index)];
    }
    return outcome;
}

static ModbusReadOutcome read_from_b5(const ModbusReadSources *sources,
                                      uint16_t start_address,
                                      uint16_t quantity,
                                      uint16_t *values)
{
    ModbusReadOutcome outcome = { MODBUS_ACCESS_OK, TR2_OK };
    uint16_t index;

    if (sources->b5_image == NULL) {
        outcome.operation_result = TR2_ERROR_NOT_AVAILABLE;
        return outcome;
    }
    for (index = 0u; index < quantity; ++index) {
        values[index] = sources->b5_image->registers[
            (uint16_t)(start_address - TR2_B5_BASE_ADDRESS + index)];
    }
    return outcome;
}

static ModbusReadOutcome read_from_b6(const ModbusReadSources *sources,
                                      uint16_t start_address,
                                      uint16_t quantity,
                                      uint16_t *values)
{
    ModbusReadOutcome outcome = { MODBUS_ACCESS_OK, TR2_OK };
    uint16_t index;

    if (sources->b6_image == NULL) {
        outcome.operation_result = TR2_ERROR_NOT_AVAILABLE;
        return outcome;
    }
    for (index = 0u; index < quantity; ++index) {
        values[index] = sources->b6_image->registers[
            (uint16_t)(start_address - TR2_B6_BASE_ADDRESS + index)];
    }
    return outcome;
}

static bool range_is_b6(uint16_t start_address, uint16_t quantity)
{
    uint32_t last_address;
    if (quantity == 0u || start_address < TR2_B6_BASE_ADDRESS) {
        return false;
    }
    last_address = (uint32_t)start_address + (uint32_t)quantity - 1u;
    return last_address <= TR2_B6_LAST_ADDRESS;
}

ModbusReadOutcome modbus_read_adapter_read(const ModbusReadSources *sources,
                                           uint16_t start_address,
                                           uint16_t quantity,
                                           uint16_t *values)
{
    ModbusReadOutcome outcome = { MODBUS_ACCESS_OK, TR2_OK };
    const ModbusRegisterDescriptor *first;
    const ModbusRegisterDescriptor *last;
    uint32_t last_address;

    if (sources == NULL || values == NULL) {
        outcome.operation_result = TR2_ERROR_INVALID_ARGUMENT;
        return outcome;
    }
    if (range_is_b6(start_address, quantity)) {
        return read_from_b6(sources, start_address, quantity, values);
    }
    if (start_address >= TR2_B6_BASE_ADDRESS && start_address <= TR2_B6_LAST_ADDRESS) {
        outcome.access_result = MODBUS_ACCESS_ILLEGAL_ADDRESS;
        return outcome;
    }

    outcome.access_result = modbus_register_model_validate_read(start_address, quantity);
    if (outcome.access_result != MODBUS_ACCESS_OK) {
        return outcome;
    }
    last_address = (uint32_t)start_address + (uint32_t)quantity - 1u;
    if (last_address > UINT16_MAX) {
        outcome.access_result = MODBUS_ACCESS_ILLEGAL_ADDRESS;
        return outcome;
    }
    first = modbus_register_model_find(start_address);
    last = modbus_register_model_find((uint16_t)last_address);
    if (first == NULL || last == NULL || first->block != last->block) {
        outcome.access_result = MODBUS_ACCESS_ILLEGAL_ADDRESS;
        return outcome;
    }

    switch (first->block) {
    case MODBUS_BLOCK_0:
        return read_from_b0(sources, start_address, quantity, values);
    case MODBUS_BLOCK_1:
        return read_from_b1(sources, start_address, quantity, values);
    case MODBUS_BLOCK_2:
        return read_from_b2(sources, start_address, quantity, values);
    case MODBUS_BLOCK_3:
        return read_from_b3(sources, start_address, quantity, values);
    case MODBUS_BLOCK_5:
        return read_from_b5(sources, start_address, quantity, values);
    default:
        outcome.operation_result = TR2_ERROR_UNSUPPORTED;
        return outcome;
    }
}

#undef TR2_B5_BASE_ADDRESS
#undef TR2_B6_BASE_ADDRESS
#undef TR2_B6_LAST_ADDRESS
