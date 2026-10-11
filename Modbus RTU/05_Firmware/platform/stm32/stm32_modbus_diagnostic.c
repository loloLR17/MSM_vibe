#include <string.h>

#include "stm32_modbus_diagnostic.h"
#include "stm32_irq_observer.h"

static Tr2Result prepare_request(void *context, const ModbusRtuAduView *request)
{
    Stm32ModbusDiagnostic *diagnostic = context;
    ModbusPduServerContext *pdu = &diagnostic->rtu.pdu_context;

    TR2_OBSERVE_INC(accepted_request);
    /* Never expose placeholder temperature/load/storage or a stale source.
     * Valid but unavailable reads use the existing V1 exception 04 path. */
    memset(pdu, 0, sizeof(*pdu));
    if (request->pdu_length == 5u &&
        request->pdu[0] == MODBUS_PDU_FC_READ_HOLDING_REGISTERS &&
        request->pdu[1] == 0x03u && request->pdu[2] == 0xE8u &&
        request->pdu[3] == 0u && request->pdu[4] == 1u) {
        TR2_OBSERVE_INC(available_status);
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

Tr2Result stm32_modbus_diagnostic_poll_once(Stm32ModbusDiagnostic *diagnostic)
{
#if TR2_STM32_IRQ_DIAGNOSTICS
    if (diagnostic == NULL) return TR2_ERROR_INVALID_ARGUMENT;
    const uint32_t before = tr2_irq_observer.t3_5_polled;
    const size_t length = diagnostic->rtu.receiver.length;
    const ModbusRtuReceiverState state = diagnostic->rtu.receiver.state;
    const Tr2Result result = modbus_rtu_server_runtime_poll_once(&diagnostic->rtu);
    TR2_OBSERVE_INC(poll_count);
    if (before != tr2_irq_observer.t3_5_polled &&
        (state == MODBUS_RTU_RECEIVER_RECEIVING || state == MODBUS_RTU_RECEIVER_WAITING_T3_5)) {
        ModbusRtuAduView request;
        const uint8_t *frame = diagnostic->rtu.receiver.buffer;
        const ModbusRtuCodecResult decoded = modbus_rtu_adu_decode(frame, length, &request);
        TR2_OBSERVE_INC(frame_count);
        tr2_irq_observer.last_frame_length = (uint32_t)length;
        tr2_irq_observer.last_codec_result = (uint32_t)decoded;
        for (size_t i = 0u; i < length && i < sizeof(tr2_irq_observer.last_frame); ++i)
            tr2_irq_observer.last_frame[i] = frame[i];
        if (decoded == MODBUS_RTU_CODEC_OK) {
            TR2_OBSERVE_INC(crc_valid);
            if (request.unit_id == 0u) TR2_OBSERVE_INC(broadcast);
            else if (request.unit_id != diagnostic->rtu.local_unit_id) TR2_OBSERVE_INC(foreign_unit);
        } else if (decoded == MODBUS_RTU_CODEC_CRC_MISMATCH) TR2_OBSERVE_INC(crc_bad);
        else TR2_OBSERVE_INC(frame_invalid);
    }
    return result;
#else
    if (diagnostic == NULL) return TR2_ERROR_INVALID_ARGUMENT;
    return modbus_rtu_server_runtime_poll_once(&diagnostic->rtu);
#endif
}
