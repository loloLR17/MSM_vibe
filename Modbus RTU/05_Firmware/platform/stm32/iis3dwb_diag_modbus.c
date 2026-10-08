#include "iis3dwb_diag_modbus.h"

Tr2Result iis3dwb_diag_modbus_init(Iis3dwbDiagModbus *diag,
                                  SerialTransport *transport, uint8_t unit_id,
                                  const IdentitySnapshot *identity,
                                  const Iis3dwbDiagWindow *window)
{
    ModbusPduServerContext context = {0};
    Tr2Result result;
    if (diag == NULL || identity == NULL || window == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!window->image_available) return TR2_ERROR_NOT_AVAILABLE;
    result = modbus_project_b0(identity, &diag->identity_image);
    if (result != TR2_OK) return result;
    context.read_sources.b0_image = &diag->identity_image;
    context.read_sources.b3_image = &window->image;
    /* No production runtime, persistence, write service or synthetic B1/B2. */
    return modbus_rtu_server_runtime_init(&diag->server, transport, unit_id, &context);
}

Tr2Result iis3dwb_diag_modbus_transmit_probe(Iis3dwbDiagModbus *diag)
{
    const uint8_t request[] = { MODBUS_PDU_FC_READ_HOLDING_REGISTERS,
                               0u, 0u, 0u, TR2_B0_REGISTER_COUNT };
    ModbusPduServerOutcome outcome;
    size_t length = 0u;
    if (diag == NULL || diag->server.transport == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    outcome = modbus_pdu_server_process(&diag->server.pdu_context,
                                        request, sizeof(request),
                                        diag->server.response_pdu,
                                        sizeof(diag->server.response_pdu));
    if (outcome.operation_result != TR2_OK) return outcome.operation_result;
    if (modbus_rtu_adu_encode(diag->server.local_unit_id,
                              diag->server.response_pdu, outcome.response_length,
                              diag->server.response_adu,
                              sizeof(diag->server.response_adu), &length)
        != MODBUS_RTU_CODEC_OK) {
        return TR2_ERROR_INTERNAL;
    }
    return serial_transport_transmit(diag->server.transport,
                                     diag->server.response_adu, length);
}
