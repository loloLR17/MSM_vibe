#include <stddef.h>

#include "tr2/platform/serial_transport.h"

bool serial_transport_is_valid(const SerialTransport *transport)
{
    return transport != NULL &&
           transport->start_receive != NULL &&
           transport->transmit != NULL &&
           transport->poll_event != NULL;
}

Tr2Result serial_transport_start_receive(const SerialTransport *transport)
{
    if (!serial_transport_is_valid(transport)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    return transport->start_receive(transport->context);
}

Tr2Result serial_transport_transmit(const SerialTransport *transport,
                                    const uint8_t *data,
                                    size_t length)
{
    if (!serial_transport_is_valid(transport) || data == NULL || length == 0u) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    return transport->transmit(transport->context, data, length);
}

bool serial_transport_poll_event(const SerialTransport *transport,
                                 SerialTransportEvent *event)
{
    if (!serial_transport_is_valid(transport) || event == NULL) {
        return false;
    }
    return transport->poll_event(transport->context, event);
}
