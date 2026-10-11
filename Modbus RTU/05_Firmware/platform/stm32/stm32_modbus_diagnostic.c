#include <string.h>

#include "stm32_modbus_diagnostic.h"

static Tr2Result prepare_request(void *context, const ModbusRtuAduView *request)
{
    Stm32ModbusDiagnostic *diagnostic = context;
    ModbusPduServerContext *pdu = &diagnostic->rtu.pdu_context;

    /* Never expose placeholder temperature/load/storage or a stale source.
     * Valid but unavailable reads use the existing V1 exception 04 path. */
    memset(pdu, 0, sizeof(*pdu));
    if (request->pdu_length == 5u &&
        request->pdu[0] == MODBUS_PDU_FC_READ_HOLDING_REGISTERS &&
        request->pdu[1] == 0x03u && request->pdu[2] == 0xE8u &&
        request->pdu[3] == 0u && request->pdu[4] == 1u) {
        pdu->read_sources.b1_image = &diagnostic->incomplete_system;
    }
    return TR2_OK;
}

Tr2Result stm32_modbus_diagnostic_init(Stm32ModbusDiagnostic *diagnostic,
    SerialTransport *transport, uint8_t diagnostic_unit_id)
{
    const ModbusPduServerContext pdu = {0};
    Tr2Result result;
    if (diagnostic == NULL) return TR2_ERROR_INVALID_ARGUMENT;
    memset(diagnostic, 0, sizeof(*diagnostic));
    result = modbus_rtu_server_runtime_init(&diagnostic->rtu, transport,
        diagnostic_unit_id, &pdu);
    if (result != TR2_OK) return result;
    /* V1 B1: 0 = UNKNOWN / initialisation incomplete. No runtime is booted. */
    diagnostic->incomplete_system.registers[0] = 0u;
    diagnostic->rtu.prepare_context = diagnostic;
    diagnostic->rtu.prepare_request = prepare_request;
    return TR2_OK;
}
