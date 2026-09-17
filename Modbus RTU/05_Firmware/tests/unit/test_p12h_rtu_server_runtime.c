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
    Tr2Result transmit_result;
    uint8_t transmitted[MODBUS_RTU_ADU_MAX_SIZE];
    size_t transmitted_length;
    SerialTransportEvent events[MODBUS_RTU_ADU_MAX_SIZE + 2u];
    size_t event_count;
    size_t event_index;
} EventSerial;

static Tr2Result event_start(void *context)
{
    EventSerial *fake = (EventSerial *)context;
    fake->start_calls += 1u;
    return TR2_OK;
}

static Tr2Result event_transmit(void *context, const uint8_t *data, size_t length)
{
    EventSerial *fake = (EventSerial *)context;

    fake->transmit_calls += 1u;
    if (fake->transmit_result != TR2_OK) {
        return fake->transmit_result;
    }

    assert(length <= sizeof(fake->transmitted));
    memcpy(fake->transmitted, data, length);
    fake->transmitted_length = length;
    return TR2_OK;
}

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

static void clear_events(EventSerial *fake)
{
    memset(fake->events, 0, sizeof(fake->events));
    fake->event_count = 0u;
    fake->event_index = 0u;
}

static void load_frame_events(EventSerial *fake, const uint8_t *adu, size_t adu_length)
{
    size_t index;

    clear_events(fake);
    for (index = 0u; index < adu_length; ++index) {
        fake->events[fake->event_count].type = SERIAL_TRANSPORT_EVENT_BYTE;
        fake->events[fake->event_count].byte = adu[index];
        fake->event_count += 1u;
    }
    fake->events[fake->event_count++].type = SERIAL_TRANSPORT_EVENT_SILENCE_T1_5;
    fake->events[fake->event_count++].type = SERIAL_TRANSPORT_EVENT_SILENCE_T3_5;
}

static void load_interrupted_frame_events(EventSerial *fake,
                                          const uint8_t *adu,
                                          size_t adu_length)
{
    size_t index;

    clear_events(fake);
    for (index = 0u; index < adu_length; ++index) {
        fake->events[fake->event_count].type = SERIAL_TRANSPORT_EVENT_BYTE;
        fake->events[fake->event_count].byte = adu[index];
        fake->event_count += 1u;
    }
    fake->events[fake->event_count++].type = SERIAL_TRANSPORT_EVENT_SILENCE_T1_5;
    fake->events[fake->event_count].type = SERIAL_TRANSPORT_EVENT_BYTE;
    fake->events[fake->event_count].byte = 0xAAu;
    fake->event_count += 1u;
    fake->events[fake->event_count++].type = SERIAL_TRANSPORT_EVENT_SILENCE_T3_5;
}

static Tr2Result run_loaded_events(ModbusRtuServerRuntime *runtime, EventSerial *fake)
{
    Tr2Result result = TR2_OK;
    size_t index;

    for (index = 0u; index < fake->event_count; ++index) {
        result = modbus_rtu_server_runtime_poll_once(runtime);
        if (result != TR2_OK) {
            return result;
        }
    }
    return result;
}

static void encode_request(uint8_t unit_id,
                           const uint8_t *pdu,
                           size_t pdu_length,
                           uint8_t *adu,
                           size_t *adu_length)
{
    assert(modbus_rtu_adu_encode(unit_id, pdu, pdu_length,
                                 adu, MODBUS_RTU_ADU_MAX_SIZE,
                                 adu_length) == MODBUS_RTU_CODEC_OK);
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
    unsigned tx_before;
    const uint8_t fc03[] = { 0x03u, 0x00u, 0x00u, 0x00u, 0x02u };
    const uint8_t fc06[] = { 0x06u, 0x00u, 0x00u, 0x00u, 0x01u };
    const uint8_t fc10_b5[] = {
        0x10u, 0x13u, 0x88u, 0x00u, 0x02u, 0x04u, 0x00u, 0x03u, 0x12u, 0x34u
    };

    assert(modbus_rtu_server_runtime_unit_id_is_valid(1u));
    assert(modbus_rtu_server_runtime_unit_id_is_valid(247u));
    assert(!modbus_rtu_server_runtime_unit_id_is_valid(0u));
    assert(!modbus_rtu_server_runtime_unit_id_is_valid(248u));

    fake.transmit_result = TR2_OK;
    b0.registers[0] = UINT16_C(0x1234);
    b0.registers[1] = UINT16_C(0x5678);
    pdu.read_sources.b0_image = &b0;
    command_request_mailbox_init(&mailbox);
    pdu.command_mailbox = &mailbox;

    assert(modbus_rtu_server_runtime_init(&runtime, &transport, 17u, &pdu) == TR2_OK);
    assert(modbus_rtu_server_runtime_start(&runtime) == TR2_OK);
    assert(fake.start_calls == 1u);

    encode_request(17u, fc03, sizeof(fc03), request, &request_length);
    load_frame_events(&fake, request, request_length);
    assert(run_loaded_events(&runtime, &fake) == TR2_OK);
    assert(fake.transmit_calls == 1u);
    assert(modbus_rtu_adu_decode(fake.transmitted, fake.transmitted_length,
                                 &response) == MODBUS_RTU_CODEC_OK);
    assert(response.unit_id == 17u);
    assert(response.pdu_length == 6u);
    assert(response.pdu[0] == 0x03u);
    assert(response.pdu[1] == 0x04u);
    assert(response.pdu[2] == 0x12u);
    assert(response.pdu[3] == 0x34u);
    assert(response.pdu[4] == 0x56u);
    assert(response.pdu[5] == 0x78u);

    encode_request(17u, fc06, sizeof(fc06), request, &request_length);
    load_frame_events(&fake, request, request_length);
    assert(run_loaded_events(&runtime, &fake) == TR2_OK);
    assert(fake.transmit_calls == 2u);
    assert(modbus_rtu_adu_decode(fake.transmitted, fake.transmitted_length,
                                 &response) == MODBUS_RTU_CODEC_OK);
    assert(response.unit_id == 17u);
    assert(response.pdu_length == 2u);
    assert(response.pdu[0] == 0x86u);
    assert(response.pdu[1] == MODBUS_PDU_EXCEPTION_ILLEGAL_FUNCTION);

    command_request_mailbox_init(&mailbox);
    encode_request(17u, fc10_b5, sizeof(fc10_b5), request, &request_length);
    load_frame_events(&fake, request, request_length);
    assert(run_loaded_events(&runtime, &fake) == TR2_OK);
    assert(fake.transmit_calls == 3u);
    assert(mailbox.command_code == UINT16_C(3));
    assert(mailbox.transaction_id == UINT16_C(0x1234));
    assert(modbus_rtu_adu_decode(fake.transmitted, fake.transmitted_length,
                                 &response) == MODBUS_RTU_CODEC_OK);
    assert(response.unit_id == 17u);
    assert(response.pdu_length == 5u);
    assert(response.pdu[0] == MODBUS_PDU_FC_WRITE_MULTIPLE_REGISTERS);
    assert(response.pdu[1] == 0x13u);
    assert(response.pdu[2] == 0x88u);
    assert(response.pdu[3] == 0x00u);
    assert(response.pdu[4] == 0x02u);

    tx_before = fake.transmit_calls;
    encode_request(18u, fc03, sizeof(fc03), request, &request_length);
    load_frame_events(&fake, request, request_length);
    assert(run_loaded_events(&runtime, &fake) == TR2_OK);
    assert(fake.transmit_calls == tx_before);

    encode_request(0u, fc03, sizeof(fc03), request, &request_length);
    load_frame_events(&fake, request, request_length);
    assert(run_loaded_events(&runtime, &fake) == TR2_OK);
    assert(fake.transmit_calls == tx_before);

    command_request_mailbox_init(&mailbox);
    encode_request(0u, fc10_b5, sizeof(fc10_b5), request, &request_length);
    load_frame_events(&fake, request, request_length);
    assert(run_loaded_events(&runtime, &fake) == TR2_OK);
    assert(fake.transmit_calls == tx_before);
    assert(mailbox.command_code == UINT16_C(3));
    assert(mailbox.transaction_id == UINT16_C(0x1234));

    request[request_length - 1u] ^= 0x01u;
    load_frame_events(&fake, request, request_length);
    assert(run_loaded_events(&runtime, &fake) == TR2_OK);
    assert(fake.transmit_calls == tx_before);

    encode_request(17u, fc03, sizeof(fc03), request, &request_length);
    load_interrupted_frame_events(&fake, request, request_length);
    assert(run_loaded_events(&runtime, &fake) == TR2_OK);
    assert(fake.transmit_calls == tx_before);

    clear_events(&fake);
    fake.events[0].type = SERIAL_TRANSPORT_EVENT_BYTE;
    fake.events[0].byte = 0x11u;
    fake.events[1].type = SERIAL_TRANSPORT_EVENT_ERROR;
    fake.events[1].error = SERIAL_TRANSPORT_ERROR_OVERRUN;
    fake.event_count = 2u;
    assert(run_loaded_events(&runtime, &fake) == TR2_OK);
    assert(runtime.receiver.state == MODBUS_RTU_RECEIVER_IDLE);
    assert(runtime.receiver.length == 0u);
    assert(fake.transmit_calls == tx_before);

    fake.transmit_result = TR2_ERROR_UNAVAILABLE;
    encode_request(17u, fc03, sizeof(fc03), request, &request_length);
    load_frame_events(&fake, request, request_length);
    assert(run_loaded_events(&runtime, &fake) == TR2_ERROR_UNAVAILABLE);
    assert(fake.transmit_calls == tx_before + 1u);
    assert(modbus_rtu_server_runtime_poll_once(&runtime) == TR2_OK);
    assert(fake.transmit_calls == tx_before + 1u);
    fake.transmit_result = TR2_OK;

    assert(modbus_rtu_server_runtime_init(NULL, &transport, 17u, &pdu) == TR2_ERROR_INVALID_ARGUMENT);
    assert(modbus_rtu_server_runtime_init(&runtime, &transport, 0u, &pdu) == TR2_ERROR_INVALID_ARGUMENT);
    assert(modbus_rtu_server_runtime_init(&runtime, &transport, 248u, &pdu) == TR2_ERROR_INVALID_ARGUMENT);

    return 0;
}
