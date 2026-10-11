#include "stm32_modbus_application.h"
#include "stm32_production_runtime.h"

static Stm32ProductionRuntime production_runtime;

__attribute__((weak)) Tr2Result stm32_production_binding_acquire(
    SerialTransport *uart, Stm32ProductionDependencies *dependencies,
    SerialTransport **qualified_transport)
{
    (void)uart;
    (void)dependencies;
    (void)qualified_transport;
    return TR2_ERROR_NOT_AVAILABLE;
}

Tr2Result stm32_modbus_application_bind(
    SerialTransport *uart, ModbusSystemServerBinding *binding)
{
    Stm32ProductionDependencies dependencies = {0};
    SerialTransport *transport = NULL;
    Tr2Result result;
    if (binding == NULL) return TR2_ERROR_INVALID_ARGUMENT;
    result = stm32_production_binding_acquire(uart, &dependencies, &transport);
    if (result != TR2_OK) return result;
    /* Transport authority first; identity/address come exclusively from V2. */
    if (!serial_transport_is_valid(transport)) return TR2_ERROR_INVALID_ARGUMENT;
    result = stm32_production_runtime_prepare(&production_runtime, &dependencies);
    if (result != TR2_OK) return result;
    return production_application_binding(&production_runtime.application,
                                           transport, production_runtime.persistent_identity.identity.unit_id,
                                           &production_runtime.identity_b0, binding);
}
