#ifndef TR2_MODBUS_RTU_SERVER_RUNTIME_H
#define TR2_MODBUS_RTU_SERVER_RUNTIME_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/modbus/codec.h"
#include "tr2/modbus/pdu_server.h"
#include "tr2/modbus/rtu_receiver.h"
#include "tr2/platform/serial_transport.h"

#define MODBUS_RTU_UNIT_ID_MIN UINT8_C(1)
#define MODBUS_RTU_UNIT_ID_MAX UINT8_C(247)
#define MODBUS_RTU_BROADCAST_UNIT_ID UINT8_C(0)

typedef struct {
    SerialTransport *transport;
    ModbusPduServerContext pdu_context;
    uint8_t local_unit_id;
    ModbusRtuReceiver receiver;
    uint8_t response_pdu[MODBUS_RTU_PDU_MAX_SIZE];
    uint8_t response_adu[MODBUS_RTU_ADU_MAX_SIZE];
} ModbusRtuServerRuntime;

bool modbus_rtu_server_runtime_unit_id_is_valid(uint8_t unit_id);
Tr2Result modbus_rtu_server_runtime_init(ModbusRtuServerRuntime *runtime,
                                         SerialTransport *transport,
                                         uint8_t local_unit_id,
                                         const ModbusPduServerContext *pdu_context);
Tr2Result modbus_rtu_server_runtime_start(ModbusRtuServerRuntime *runtime);
Tr2Result modbus_rtu_server_runtime_poll_once(ModbusRtuServerRuntime *runtime);

#endif
