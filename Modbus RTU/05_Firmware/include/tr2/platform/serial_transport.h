#ifndef TR2_PLATFORM_SERIAL_TRANSPORT_H
#define TR2_PLATFORM_SERIAL_TRANSPORT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"

typedef enum {
    SERIAL_TRANSPORT_EVENT_NONE = 0,
    SERIAL_TRANSPORT_EVENT_BYTE,
    SERIAL_TRANSPORT_EVENT_SILENCE_T1_5,
    SERIAL_TRANSPORT_EVENT_SILENCE_T3_5,
    SERIAL_TRANSPORT_EVENT_ERROR
} SerialTransportEventType;

typedef enum {
    SERIAL_TRANSPORT_ERROR_UNSPECIFIED = 0,
    SERIAL_TRANSPORT_ERROR_OVERRUN,
    SERIAL_TRANSPORT_ERROR_FRAMING,
    SERIAL_TRANSPORT_ERROR_PARITY,
    SERIAL_TRANSPORT_ERROR_NOISE
} SerialTransportError;

typedef struct {
    SerialTransportEventType type;
    uint8_t byte;
    SerialTransportError error;
} SerialTransportEvent;

typedef struct {
    void *context;
    Tr2Result (*start_receive)(void *context);
    Tr2Result (*transmit)(void *context, const uint8_t *data, size_t length);
    bool (*poll_event)(void *context, SerialTransportEvent *event);
} SerialTransport;

bool serial_transport_is_valid(const SerialTransport *transport);
Tr2Result serial_transport_start_receive(const SerialTransport *transport);
Tr2Result serial_transport_transmit(const SerialTransport *transport,
                                    const uint8_t *data,
                                    size_t length);
bool serial_transport_poll_event(const SerialTransport *transport,
                                 SerialTransportEvent *event);

#endif
