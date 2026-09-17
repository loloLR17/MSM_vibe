#include <stddef.h>

#include "tr2/modbus/rtu_server_runtime.h"

static void reset_receiver(ModbusRtuServerRuntime *runtime)
{
    modbus_rtu_receiver_init(&runtime->receiver);
}

static Tr2Result process_frame(ModbusRtuServerRuntime *runtime,
                               const uint8_t *frame,
                               size_t frame_length)
{
    ModbusRtuAduView request;
    ModbusPduServerOutcome pdu_outcome;
    ModbusRtuCodecResult codec_result;
    size_t response_adu_length = 0u;
    bool broadcast;

    codec_result = modbus_rtu_adu_decode(frame, frame_length, &request);
    if (codec_result != MODBUS_RTU_CODEC_OK) {
        return TR2_OK;
    }

    broadcast = request.unit_id == MODBUS_RTU_BROADCAST_UNIT_ID;
    if (!broadcast && request.unit_id != runtime->local_unit_id) {
        return TR2_OK;
    }

    if (broadcast &&
        (request.pdu_length == 0u ||
         request.pdu[0] != MODBUS_PDU_FC_WRITE_MULTIPLE_REGISTERS)) {
        return TR2_OK;
    }

    pdu_outcome = modbus_pdu_server_process(&runtime->pdu_context,
                                            request.pdu,
                                            request.pdu_length,
                                            runtime->response_pdu,
                                            sizeof(runtime->response_pdu));
    if (pdu_outcome.operation_result != TR2_OK) {
        return pdu_outcome.operation_result;
    }

    if (broadcast) {
        return TR2_OK;
    }

    codec_result = modbus_rtu_adu_encode(runtime->local_unit_id,
                                        runtime->response_pdu,
                                        pdu_outcome.response_length,
                                        runtime->response_adu,
                                        sizeof(runtime->response_adu),
                                        &response_adu_length);
    if (codec_result != MODBUS_RTU_CODEC_OK) {
        return TR2_ERROR_INTERNAL;
    }

    return serial_transport_transmit(runtime->transport,
                                     runtime->response_adu,
                                     response_adu_length);
}

bool modbus_rtu_server_runtime_unit_id_is_valid(uint8_t unit_id)
{
    return unit_id >= MODBUS_RTU_UNIT_ID_MIN && unit_id <= MODBUS_RTU_UNIT_ID_MAX;
}

Tr2Result modbus_rtu_server_runtime_init(ModbusRtuServerRuntime *runtime,
                                         SerialTransport *transport,
                                         uint8_t local_unit_id,
                                         const ModbusPduServerContext *pdu_context)
{
    if (runtime == NULL || transport == NULL || pdu_context == NULL ||
        !serial_transport_is_valid(transport) ||
        !modbus_rtu_server_runtime_unit_id_is_valid(local_unit_id)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    runtime->transport = transport;
    runtime->pdu_context = *pdu_context;
    runtime->local_unit_id = local_unit_id;
    modbus_rtu_receiver_init(&runtime->receiver);
    return TR2_OK;
}

Tr2Result modbus_rtu_server_runtime_start(ModbusRtuServerRuntime *runtime)
{
    if (runtime == NULL || runtime->transport == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    reset_receiver(runtime);
    return serial_transport_start_receive(runtime->transport);
}

Tr2Result modbus_rtu_server_runtime_poll_once(ModbusRtuServerRuntime *runtime)
{
    SerialTransportEvent serial_event;
    ModbusRtuReceiverEvent receiver_event = { false, NULL, 0u };

    if (runtime == NULL || runtime->transport == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    if (!serial_transport_poll_event(runtime->transport, &serial_event)) {
        reset_receiver(runtime);
        return TR2_ERROR_UNAVAILABLE;
    }

    switch (serial_event.type) {
    case SERIAL_TRANSPORT_EVENT_NONE:
        return TR2_OK;
    case SERIAL_TRANSPORT_EVENT_BYTE:
        receiver_event = modbus_rtu_receiver_push_byte(&runtime->receiver,
                                                       serial_event.byte);
        break;
    case SERIAL_TRANSPORT_EVENT_SILENCE_T1_5:
        receiver_event = modbus_rtu_receiver_on_silence_t1_5(&runtime->receiver);
        break;
    case SERIAL_TRANSPORT_EVENT_SILENCE_T3_5:
        receiver_event = modbus_rtu_receiver_on_silence_t3_5(&runtime->receiver);
        break;
    case SERIAL_TRANSPORT_EVENT_ERROR:
        reset_receiver(runtime);
        return TR2_OK;
    default:
        reset_receiver(runtime);
        return TR2_ERROR_UNSUPPORTED;
    }

    if (!receiver_event.frame_available) {
        return TR2_OK;
    }

    return process_frame(runtime, receiver_event.frame, receiver_event.frame_length);
}
