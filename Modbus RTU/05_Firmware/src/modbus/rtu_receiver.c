#include <stddef.h>

#include "tr2/modbus/rtu_receiver.h"

static ModbusRtuReceiverEvent no_frame(void)
{
    ModbusRtuReceiverEvent event = { false, NULL, 0u };
    return event;
}

void modbus_rtu_receiver_init(ModbusRtuReceiver *receiver)
{
    if (receiver == NULL) {
        return;
    }
    receiver->state = MODBUS_RTU_RECEIVER_IDLE;
    receiver->length = 0u;
}

ModbusRtuReceiverEvent modbus_rtu_receiver_push_byte(ModbusRtuReceiver *receiver,
                                                     uint8_t byte)
{
    if (receiver == NULL) {
        return no_frame();
    }

    if (receiver->state == MODBUS_RTU_RECEIVER_INVALID) {
        return no_frame();
    }

    if (receiver->state == MODBUS_RTU_RECEIVER_IDLE) {
        receiver->state = MODBUS_RTU_RECEIVER_RECEIVING;
        receiver->length = 0u;
    }

    if (receiver->length >= MODBUS_RTU_ADU_MAX_SIZE) {
        receiver->state = MODBUS_RTU_RECEIVER_INVALID;
        return no_frame();
    }

    receiver->buffer[receiver->length] = byte;
    receiver->length += 1u;
    return no_frame();
}

ModbusRtuReceiverEvent modbus_rtu_receiver_on_silence_t1_5(ModbusRtuReceiver *receiver)
{
    if (receiver == NULL) {
        return no_frame();
    }

    if (receiver->state == MODBUS_RTU_RECEIVER_RECEIVING) {
        receiver->state = MODBUS_RTU_RECEIVER_INVALID;
    }
    return no_frame();
}

ModbusRtuReceiverEvent modbus_rtu_receiver_on_silence_t3_5(ModbusRtuReceiver *receiver)
{
    ModbusRtuReceiverEvent event = no_frame();

    if (receiver == NULL) {
        return event;
    }

    if (receiver->state == MODBUS_RTU_RECEIVER_RECEIVING) {
        event.frame_available = true;
        event.frame = receiver->buffer;
        event.frame_length = receiver->length;
    }

    receiver->state = MODBUS_RTU_RECEIVER_IDLE;
    receiver->length = 0u;
    return event;
}
