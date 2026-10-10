#include "stm32_modbus_application.h"

/* Explicit missing board binding, not a simulated production service. */
__attribute__((weak)) Tr2Result stm32_modbus_application_bind(
    SerialTransport *uart, ModbusSystemServerBinding *binding)
{
    (void)uart;
    (void)binding;
    return TR2_ERROR_NOT_AVAILABLE;
}
