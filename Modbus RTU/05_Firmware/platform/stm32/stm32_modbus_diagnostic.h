#ifndef TR2_STM32_MODBUS_DIAGNOSTIC_H
#define TR2_STM32_MODBUS_DIAGNOSTIC_H

#include "tr2/modbus/rtu_server_runtime.h"

/* Isolated, unbooted production application: only B1/1000 UNKNOWN is known.
 * No production identity, readiness, command authority or media is supplied. */
typedef struct {
    ModbusRtuServerRuntime rtu;
    ModbusBlock1Image incomplete_system;
} Stm32ModbusDiagnostic;

Tr2Result stm32_modbus_diagnostic_init(Stm32ModbusDiagnostic *diagnostic,
    SerialTransport *transport, uint8_t diagnostic_unit_id);

#endif
