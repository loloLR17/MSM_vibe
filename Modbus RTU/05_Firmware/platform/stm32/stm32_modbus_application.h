#ifndef TR2_STM32_MODBUS_APPLICATION_H
#define TR2_STM32_MODBUS_APPLICATION_H

#include "tr2/application/modbus_system_server.h"

/* Composes the owned production runtime after stm32_production_binding_acquire
 * supplies explicit qualified board dependencies. Default NOT_AVAILABLE keeps
 * receive-only diagnostics, without preparing media or programming DE//RE.
 */
Tr2Result stm32_modbus_application_bind(SerialTransport *uart,
                                       ModbusSystemServerBinding *binding);

#endif
