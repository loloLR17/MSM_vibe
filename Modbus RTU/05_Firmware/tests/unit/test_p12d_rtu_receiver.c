#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/modbus/rtu_receiver.h"

static void push_bytes(ModbusRtuReceiver *receiver,
                       const uint8_t *bytes,
                       size_t length)
{
    size_t index;
    for (index = 0u; index < length; ++index) {
        ModbusRtuReceiverEvent event = modbus_rtu_receiver_push_byte(receiver, bytes[index]);
        assert(!event.frame_available);
    }
}

int main(void)
{
    ModbusRtuReceiver receiver;
    ModbusRtuReceiverEvent event;
    const uint8_t frame1[] = { 0x11u, 0x03u, 0x00u, 0x00u, 0x00u, 0x02u, 0xC6u, 0x9Bu };
    const uint8_t frame2[] = { 0x22u, 0x10u, 0x00u, 0x01u };
    size_t index;

    modbus_rtu_receiver_init(&receiver);
    assert(receiver.state == MODBUS_RTU_RECEIVER_IDLE);
    assert(receiver.length == 0u);

    event = modbus_rtu_receiver_on_silence_t3_5(&receiver);
    assert(!event.frame_available);
    assert(receiver.state == MODBUS_RTU_RECEIVER_IDLE);

    push_bytes(&receiver, frame1, sizeof(frame1));
    assert(receiver.state == MODBUS_RTU_RECEIVER_RECEIVING);
    assert(receiver.length == sizeof(frame1));

    event = modbus_rtu_receiver_on_silence_t1_5(&receiver);
    assert(!event.frame_available);
    assert(receiver.state == MODBUS_RTU_RECEIVER_WAITING_T3_5);
    assert(receiver.length == sizeof(frame1));

    event = modbus_rtu_receiver_on_silence_t3_5(&receiver);
    assert(event.frame_available);
    assert(event.frame == receiver.buffer);
    assert(event.frame_length == sizeof(frame1));
    for (index = 0u; index < sizeof(frame1); ++index) {
        assert(event.frame[index] == frame1[index]);
    }
    assert(receiver.state == MODBUS_RTU_RECEIVER_IDLE);
    assert(receiver.length == 0u);

    push_bytes(&receiver, frame2, sizeof(frame2));
    event = modbus_rtu_receiver_on_silence_t1_5(&receiver);
    assert(!event.frame_available);
    assert(receiver.state == MODBUS_RTU_RECEIVER_WAITING_T3_5);
    assert(receiver.length == sizeof(frame2));

    event = modbus_rtu_receiver_push_byte(&receiver, 0xAAu);
    assert(!event.frame_available);
    assert(receiver.state == MODBUS_RTU_RECEIVER_INVALID);
    assert(receiver.length == sizeof(frame2));

    event = modbus_rtu_receiver_on_silence_t3_5(&receiver);
    assert(!event.frame_available);
    assert(receiver.state == MODBUS_RTU_RECEIVER_IDLE);
    assert(receiver.length == 0u);

    for (index = 0u; index < MODBUS_RTU_ADU_MAX_SIZE; ++index) {
        event = modbus_rtu_receiver_push_byte(&receiver, (uint8_t)index);
        assert(!event.frame_available);
    }
    assert(receiver.state == MODBUS_RTU_RECEIVER_RECEIVING);
    assert(receiver.length == MODBUS_RTU_ADU_MAX_SIZE);

    event = modbus_rtu_receiver_push_byte(&receiver, 0x55u);
    assert(!event.frame_available);
    assert(receiver.state == MODBUS_RTU_RECEIVER_INVALID);
    assert(receiver.length == MODBUS_RTU_ADU_MAX_SIZE);

    event = modbus_rtu_receiver_on_silence_t3_5(&receiver);
    assert(!event.frame_available);
    assert(receiver.state == MODBUS_RTU_RECEIVER_IDLE);
    assert(receiver.length == 0u);

    event = modbus_rtu_receiver_push_byte(&receiver, 0x42u);
    assert(!event.frame_available);
    event = modbus_rtu_receiver_on_silence_t3_5(&receiver);
    assert(event.frame_available);
    assert(event.frame_length == 1u);
    assert(event.frame[0] == 0x42u);

    modbus_rtu_receiver_init(NULL);
    event = modbus_rtu_receiver_push_byte(NULL, 0u);
    assert(!event.frame_available);
    event = modbus_rtu_receiver_on_silence_t1_5(NULL);
    assert(!event.frame_available);
    event = modbus_rtu_receiver_on_silence_t3_5(NULL);
    assert(!event.frame_available);

    return 0;
}
