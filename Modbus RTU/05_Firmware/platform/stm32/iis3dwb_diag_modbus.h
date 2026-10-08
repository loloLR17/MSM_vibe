#ifndef TR2_IIS3DWB_DIAG_MODBUS_H
#define TR2_IIS3DWB_DIAG_MODBUS_H

#include "iis3dwb_diag_window.h"
#include "tr2/modbus/rtu_server_runtime.h"

/* Diagnostic composition only; identity and backend lifetime belong to caller. */
typedef struct {
    ModbusBlock0Image identity_image;
    ModbusRtuServerRuntime server;
} Iis3dwbDiagModbus;

Tr2Result iis3dwb_diag_modbus_init(Iis3dwbDiagModbus *diag,
                                  SerialTransport *transport, uint8_t unit_id,
                                  const IdentitySnapshot *identity,
                                  const Iis3dwbDiagWindow *window);
/* One explicitly requested unsolicited B0 response; diagnostic bus test only.
   Caller must serialize it with normal server polling/transmission. */
Tr2Result iis3dwb_diag_modbus_transmit_probe(Iis3dwbDiagModbus *diag);
#endif
