#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/platform/serial_transport.h"

typedef struct {
    unsigned start_calls;
    unsigned transmit_calls;
    uint8_t transmitted[16];
    size_t transmitted_length;
    SerialTransportEvent events[4];
    size_t event_count;
    size_t event_index;
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
    if (length > sizeof(fake->transmitted)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memcpy(fake->transmitted, data, length);
    fake->transmitted_length = length;
    fake->transmit_calls += 1u;
    return TR2_OK;
}

static bool fake_poll(void *context, SerialTransportEvent *event)
{
    FakeSerial *fake = (FakeSerial *)context;
    if (fake->event_index >= fake->event_count) {
        event->type = SERIAL_TRANSPORT_EVENT_NONE;
        event->byte = 0u;
        event->error = SERIAL_TRANSPORT_ERROR_UNSPECIFIED;
        return true;
    }
    *event = fake->events[fake->event_index];
    fake->event_index += 1u;
    return true;
}

int main(void)
{
    FakeSerial fake = {0};
    SerialTransport transport = { &fake, fake_start, fake_transmit, fake_poll };
    SerialTransport invalid = {0};
    SerialTransportEvent event;
    const uint8_t adu[] = { 0x01u, 0x03u, 0x00u, 0x00u };

    assert(serial_transport_is_valid(&transport));
    assert(!serial_transport_is_valid(NULL));
    assert(!serial_transport_is_valid(&invalid));

    assert(serial_transport_start_receive(&transport) == TR2_OK);
    assert(fake.start_calls == 1u);

    assert(serial_transport_transmit(&transport, adu, sizeof(adu)) == TR2_OK);
    assert(fake.transmit_calls == 1u);
    assert(fake.transmitted_length == sizeof(adu));
    assert(memcmp(fake.transmitted, adu, sizeof(adu)) == 0);

    fake.events[0].type = SERIAL_TRANSPORT_EVENT_BYTE;
    fake.events[0].byte = 0x42u;
    fake.events[1].type = SERIAL_TRANSPORT_EVENT_SILENCE_T1_5;
    fake.events[2].type = SERIAL_TRANSPORT_EVENT_SILENCE_T3_5;
    fake.events[3].type = SERIAL_TRANSPORT_EVENT_ERROR;
    fake.events[3].error = SERIAL_TRANSPORT_ERROR_FRAMING;
    fake.event_count = 4u;

    assert(serial_transport_poll_event(&transport, &event));
    assert(event.type == SERIAL_TRANSPORT_EVENT_BYTE);
    assert(event.byte == 0x42u);
    assert(serial_transport_poll_event(&transport, &event));
    assert(event.type == SERIAL_TRANSPORT_EVENT_SILENCE_T1_5);
    assert(serial_transport_poll_event(&transport, &event));
    assert(event.type == SERIAL_TRANSPORT_EVENT_SILENCE_T3_5);
    assert(serial_transport_poll_event(&transport, &event));
    assert(event.type == SERIAL_TRANSPORT_EVENT_ERROR);
    assert(event.error == SERIAL_TRANSPORT_ERROR_FRAMING);
    assert(serial_transport_poll_event(&transport, &event));
    assert(event.type == SERIAL_TRANSPORT_EVENT_NONE);

    assert(serial_transport_start_receive(NULL) == TR2_ERROR_INVALID_ARGUMENT);
    assert(serial_transport_transmit(&transport, NULL, sizeof(adu)) == TR2_ERROR_INVALID_ARGUMENT);
    assert(serial_transport_transmit(&transport, adu, 0u) == TR2_ERROR_INVALID_ARGUMENT);
    assert(!serial_transport_poll_event(NULL, &event));
    assert(!serial_transport_poll_event(&transport, NULL));

    return 0;
}
