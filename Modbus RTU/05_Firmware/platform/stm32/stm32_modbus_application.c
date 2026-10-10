#include "stm32_modbus_application.h"
#include "stm32_production_runtime.h"

static Stm32ProductionRuntime production_runtime;

__attribute__((weak)) Tr2Result stm32_production_binding_acquire(
    SerialTransport *uart, Stm32ProductionDependencies *dependencies,
    SerialTransport **qualified_transport, uint8_t *unit_id,
    const ModbusBlock0Image **identity)
{
    (void)uart;
    (void)dependencies;
    (void)qualified_transport;
    (void)unit_id;
    (void)identity;
    return TR2_ERROR_NOT_AVAILABLE;
}

Tr2Result stm32_modbus_application_bind(
    SerialTransport *uart, ModbusSystemServerBinding *binding)
{
    Stm32ProductionDependencies dependencies = {0};
    SerialTransport *transport = NULL;
    const ModbusBlock0Image *identity = NULL;
    uint8_t unit_id = 0u;
    Tr2Result result;
    if (binding == NULL) return TR2_ERROR_INVALID_ARGUMENT;
    result = stm32_production_binding_acquire(uart, &dependencies, &transport, &unit_id, &identity);
    if (result != TR2_OK) return result;
    /* Validate transport/address before any boot or media access. */
    if (!serial_transport_is_valid(transport) ||
        !modbus_rtu_server_runtime_unit_id_is_valid(unit_id)) return TR2_ERROR_INVALID_ARGUMENT;
    result = stm32_production_runtime_prepare(&production_runtime, &dependencies);
    if (result != TR2_OK) return result;
    return production_application_binding(&production_runtime.application,
                                           transport, unit_id, identity, binding);
}
