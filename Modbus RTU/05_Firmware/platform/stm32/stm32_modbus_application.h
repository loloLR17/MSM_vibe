#ifndef TR2_STM32_MODBUS_APPLICATION_H
#define TR2_STM32_MODBUS_APPLICATION_H

#include "tr2/application/modbus_system_server.h"

/* Board integration hook. TR2_ERROR_NOT_AVAILABLE is the default and keeps
 * receive-only diagnostics. An override must supply an already booted runtime,
 * explicitly provisioned unit ID, long-lived application authorities, and a
 * SerialTransport implementing the qualified DE//RE and TX/RX policy. The raw
 * UART argument is not by itself a qualified RS-485 transport.
 */
Tr2Result stm32_modbus_application_bind(SerialTransport *uart,
                                       ModbusSystemServerBinding *binding);

#endif
