#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/command_request_mailbox.h"
#include "tr2/modbus/rtu_server_runtime.h"

typedef struct {
    unsigned start_calls;
    unsigned transmit_calls;
    uint8_t transmitted[MODBUS_RTU_ADU_MAX_SIZE];
    size_t transmitted_length;
} FakeSerial;

static Tr2Result fake_start(void *context)
{
    FakeSerial *fake = (FakeSerial *)context;
    fake->start_calls += 1u;
    return TR2_OK;
}

static Tr2Result fake_transmit(void *context, const uint8_t *data, size_t length)
{
    FakeSerial *fake = (FakeSerial *)context;
    assert(length <= sizeof(fake->transmitted));
    memcpy(fake->transmitted, data, length);
    fake->transmitted_length = length;
    fake->transmit_calls += 1u;
    return TR2_OK;
}

static bool fake_poll_none(void *context, SerialTransportEvent *event)
{
    (void)context;
    event->type = SERIAL_TRANSPORT_EVENT_NONE;
    event->byte = 0u;
    event->error = SERIAL_TRANSPORT_ERROR_UNSPECIFIED;
    return true;
}

static void feed_adu(ModbusRtuServerRuntime *runtime,
                     const uint8_t *adu,
                     size_t adu_length)
{
    size_t index;
    for (index = 0u; index < adu_length; ++index) {
        ModbusRtuReceiverEvent event =
            modbus_rtu_receiver_push_byte(&runtime->receiver, adu[index]);
        assert(!event.frame_available);
    }
}

static Tr2Result finish_frame(ModbusRtuServerRuntime *runtime)
{
    SerialTransportEvent event;
    ModbusRtuReceiverEvent frame;

    frame = modbus_rtu_receiver_on_silence_t1_5(&runtime->receiver);
    assert(!frame.frame_available);
    frame = modbus_rtu_receiver_on_silence_t3_5(&runtime->receiver);
    assert(frame.frame_available);

    event.type = SERIAL_TRANSPORT_EVENT_NONE;
    event.byte = 0u;
    event.error = SERIAL_TRANSPORT_ERROR_UNSPECIFIED;

    /* The public poll path is exercised separately; frame processing is reached
       by replaying the complete event stream through a dedicated fake below. */
    (void)event;
    return TR2_OK;
}

typedef struct {
    FakeSerial io;
    SerialTransportEvent events[MODBUS_RTU_ADU_MAX_SIZE + 2u];
    size_t event_count;
    size_t event_index;
} EventSerial;

static bool event_poll(void *context, SerialTransportEvent *event)
{
    EventSerial *fake = (EventSerial *)context;
    if (fake->event_index >= fake->event_count) {
        event->type = SERIAL_TRANSPORT_EVENT_NONE;
        event->byte = 0u;
        event->error = SERIAL_TRANSPORT_ERROR_UNSPECIFIED;
        return true;
    }
    *event = fake->events[fake->event_index++];
    return true;
}

static Tr2Result event_start(void *context)
{
    EventSerial *fake = (EventSerial *)context;
    fake->io.start_calls += 1u;
    return TR2_OK;
}

static Tr2Result event_transmit(void *context, const uint8_t *data, size_t length)
{
    EventSerial *fake = (EventSerial *)context;
    assert(length <= sizeof(fake->io.transmitted));
    memcpy(fake->io.transmitted, data, length);
    fake->io.transmitted_length = length;
    fake->io.transmit_calls += 1u;
    return TR2_OK;
}

static void load_frame_events(EventSerial *fake, const uint8_t *adu, size_t adu_length)
{
    size_t index;
    memset(fake->events, 0, sizeof(fake->events));
    fake->event_count = 0u;
    fake->event_index = 0u;
    for (index = 0u; index < adu_length; ++index) {
        fake->events[fake->event_count].type = SERIAL_TRANSPORT_EVENT_BYTE;
        fake->events[fake->event_count].byte = adu[index];
        fake->event_count += 1u;
    }
    fake->events[fake->event_count++].type = SERIAL_TRANSPORT_EVENT_SILENCE_T1_5;
    fake->events[fake->event_count++].type = SERIAL_TRANSPORT_EVENT_SILENCE_T3_5;
}

static void run_loaded_events(ModbusRtuServerRuntime *runtime, EventSerial *fake)
{
    size_t index;
    for (index = 0u; index < fake->event_count; ++index) {
        assert(modbus_rtu_server_runtime_poll_once(runtime) == TR2_OK);
    }
}

int main(void)
{
    EventSerial fake = {0};
    SerialTransport transport = { &fake, event_start, event_transmit, event_poll };
    ModbusBlock0Image b0 = { {0u}, 0u };
    CommandRequestMailbox mailbox;
    ModbusPduServerContext pdu = {0};
    ModbusRtuServerRuntime runtime;
    ModbusRtuAduView response;
    uint8_t request[MODBUS_RTU_ADU_MAX_SIZE];
    size_t request_length = 0u;
    const uint8_t fc03[] = { 0x03u, 0x00u, 0x00u, 0x00u, 0x02u };
    const uint8_t fc10_b5[] = {
        0x10u, 0x13u, 0x88u, 0x00u, 0x02u, 0x04u, 0x00u, 0x03u, 0x12u, 0x34u
    };

    (void)fake_start;
    (void)fake_transmit;
    (void)fake_poll_none;
    (void)feed_adu;
    (void)finish_frame;

    assert(modbus_rtu_server_runtime_unit_id_is_valid(1u));
    assert(modbus_rtu_server_runtime_unit_id_is_valid(247u));
    assert(!modbus_rtu_server_runtime_unit_id_is_valid(0u));
    assert(!modbus_rtu_server_runtime_unit_id_is_valid(248u));

    b0.registers[0] = UINT16_C(0x1234);
    b0.registers[1] = UINT16_C(0x5678);
    pdu.read_sources.b0_image = &b0;
    command_request_mailbox_init(&mailbox);
    pdu.command_mailbox = &mailbox;

    assert(modbus_rtu_server_runtime_init(&runtime, &transport, 17u, &pdu) == TR2_OK);
    assert(modbus_rtu_server_runtime_start(&runtime) == TR2_OK);
    assert(fake.io.start_calls == 1u);

    assert(modbus_rtu_adu_encode(17u, fc03, sizeof(fc03),
                                 request, sizeof(request), &request_length) == MODBUS_RTU_CODEC_OK);
    load_frame_events(&fake, request, request_length);
    run_loaded_events(&runtime, &fake);
    assert(fake.io.transmit_calls == 1u);
    assert(modbus_rtu_adu_decode(fake.io.transmitted,
                                 fake.io.transmitted_length,
                                 &response) == MODBUS_RTU_CODEC_OK);
    assert(response.unit_id == 17u);
    assert(response.pdu_length == 6u);
    assert(response.pdu[0] == 0x03u);
    assert(response.pdu[1] == 0x04u);
    assert(response.pdu[2] == 0x12u);
    assert(response.pdu[3] == 0x34u);
    assert(response.pdu[4] == 0x56u);
    assert(response.pdu[5] == 0x78u);

    assert(modbus_rtu_adu_encode(18u, fc03, sizeof(fc03),
                                 request, sizeof(request), &request_length) == MODBUS_RTU_CODEC_OK);
    load_frame_events(&fake, request, request_length);
    run_loaded_events(&runtime, &fake);
    assert(fake.io.transmit_calls == 1u);

    assert(modbus_rtu_adu_encode(0u, fc03, sizeof(fc03),
                                 request, sizeof(request), &request_length) == MODBUS_RTU_CODEC_OK);
    load_frame_events(&fake, request, request_length);
    run_loaded_events(&runtime, &fake);
    assert(fake.io.transmit_calls == 1u);

    assert(modbus_rtu_adu_encode(0u, fc10_b5, sizeof(fc10_b5),
                                 request, sizeof(request), &request_length) == MODBUS_RTU_CODEC_OK);
    load_frame_events(&fake, request, request_length);
    run_loaded_events(&runtime, &fake);
    assert(fake.io.transmit_calls == 1u);
    assert(mailbox.command_code == UINT16_C(3));
    assert(mailbox.transaction_id == UINT16_C(0x1234));

    request[request_length - 1u] ^= 0x01u;
    load_frame_events(&fake, request, request_length);
    run_loaded_events(&runtime, &fake);
    assert(fake.io.transmit_calls == 1u);

    fake.events[0].type = SERIAL_TRANSPORT_EVENT_BYTE;
    fake.events[0].byte = 0x11u;
    fake.events[1].type = SERIAL_TRANSPORT_EVENT_ERROR;
    fake.events[1].error = SERIAL_TRANSPORT_ERROR_OVERRUN;
    fake.event_count = 2u;
    fake.event_index = 0u;
    run_loaded_events(&runtime, &fake);
    assert(runtime.receiver.state == MODBUS_RTU_RECEIVER_IDLE);
    assert(runtime.receiver.length == 0u);

    assert(modbus_rtu_server_runtime_init(NULL, &transport, 17u, &pdu) == TR2_ERROR_INVALID_ARGUMENT);
    assert(modbus_rtu_server_runtime_init(&runtime, &transport, 0u, &pdu) == TR2_ERROR_INVALID_ARGUMENT);
    assert(modbus_rtu_server_runtime_init(&runtime, &transport, 248u, &pdu) == TR2_ERROR_INVALID_ARGUMENT);

    return 0;
}
