#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "iis3dwb_diag_modbus.h"

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


static void request(ModbusRtuServerRuntime *runtime, EventSerial *fake,
                    uint8_t id, const uint8_t *pdu, size_t size)
{
    uint8_t adu[MODBUS_RTU_ADU_MAX_SIZE];
    size_t length;
    encode_request(id, pdu, size, adu, &length);
    load_frame_events(fake, adu, length);
    assert(run_loaded_events(runtime, fake) == TR2_OK);
}

static void assert_exception(EventSerial *fake, uint8_t fc, uint8_t exception)
{
    ModbusRtuAduView response;
    assert(modbus_rtu_adu_decode(fake->transmitted, fake->transmitted_length,
                                &response) == MODBUS_RTU_CODEC_OK);
    assert(response.pdu_length == 2u);
    assert(response.pdu[0] == (fc | 0x80u));
    assert(response.pdu[1] == exception);
}

int main(void)
{
    EventSerial fake = {0};
    SerialTransport transport = { &fake, event_start, event_transmit, event_poll };
    Iis3dwbDiagWindow window;
    Iis3dwbDiagModbus diag;
    /* Explicit host fixture, never a physical acquisition or factory identity. */
    IdentitySnapshot identity = {0};
    VibrationSample sample = {0};
    const uint8_t b0_request[] = {3u, 0u, 0u, 0u, TR2_B0_REGISTER_COUNT};
    const uint8_t b3_request[] = {3u, 0x0Bu, 0xB8u, 0u, TR2_B3_REGISTER_COUNT};
    uint8_t adu[MODBUS_RTU_ADU_MAX_SIZE];
    size_t length;
    ModbusRtuAduView response;
    unsigned calls;
    ModbusBlock3Image original;

    identity.device_id = UINT32_C(0x12345678);
    identity.protocol_version = 1u;
    memcpy(identity.serial_number, "HOST-F3", 7u);
    assert(iis3dwb_diag_window_begin(&window, 10u) == TR2_OK);
    assert(iis3dwb_diag_modbus_init(&diag, &transport, 1u, &identity, &window)
           == TR2_ERROR_NOT_AVAILABLE);
    sample.valid = true;
    sample.x_mg = 300;
    sample.y_mg = 400;
    for (unsigned i=0u; i<TR2_IIS3DWB_DIAG_WINDOW_SAMPLES; ++i) {
        assert(iis3dwb_diag_window_append(&window, &sample) == TR2_OK);
    }
    assert(iis3dwb_diag_window_publish(&window, 260u) == TR2_OK);
    original = window.image;
    assert(iis3dwb_diag_modbus_init(&diag, &transport, 0u, &identity, &window)
           == TR2_ERROR_INVALID_ARGUMENT);
    assert(iis3dwb_diag_modbus_init(&diag, &transport, 1u, &identity, &window) == TR2_OK);
    assert(diag.server.pdu_context.read_sources.b3_image == &window.image);
    assert(diag.server.pdu_context.command_mailbox == NULL);
    assert(diag.server.pdu_context.configuration_staging == NULL);
    assert(diag.server.pdu_context.time_service == NULL);
    assert(modbus_rtu_server_runtime_start(&diag.server) == TR2_OK);
    assert(fake.start_calls == 1u);

    request(&diag.server, &fake, 1u, b0_request, sizeof(b0_request));
    assert(modbus_rtu_adu_decode(fake.transmitted, fake.transmitted_length,
                                &response) == MODBUS_RTU_CODEC_OK);
    assert(response.pdu[0] == 3u && response.pdu[1] == 42u);
    assert(response.pdu[2] == 0x12u && response.pdu[3] == 0x34u);
    assert(response.pdu[4] == 0x56u && response.pdu[5] == 0x78u);
    assert(response.pdu[42] == 0u && response.pdu[43] == 0u);
    request(&diag.server, &fake, 1u, b3_request, sizeof(b3_request));
    assert(modbus_rtu_adu_decode(fake.transmitted, fake.transmitted_length,
                                &response) == MODBUS_RTU_CODEC_OK);
    assert(response.pdu_length == 98u && response.pdu[1] == 96u);
    for (unsigned i=0u; i<TR2_B3_REGISTER_COUNT; ++i) {
        assert(response.pdu[2u+2u*i] == (original.registers[i] >> 8u));
        assert(response.pdu[3u+2u*i] == (original.registers[i] & 255u));
    }
    assert(modbus_codec_u32_from_msw_lsw(original.registers[14], original.registers[15]) == 500u);
    assert(modbus_codec_u32_from_msw_lsw(original.registers[12], original.registers[13]) == 4096u);
    calls = fake.transmit_calls;
    request(&diag.server, &fake, 2u, b3_request, sizeof(b3_request));
    request(&diag.server, &fake, 0u, b3_request, sizeof(b3_request));
    assert(fake.transmit_calls == calls);
    encode_request(1u, b3_request, sizeof(b3_request), adu, &length);
    adu[length-1u] ^= 1u;
    load_frame_events(&fake, adu, length);
    assert(run_loaded_events(&diag.server, &fake) == TR2_OK);
    assert(fake.transmit_calls == calls);
    encode_request(1u, b3_request, sizeof(b3_request), adu, &length);
    load_interrupted_frame_events(&fake, adu, length);
    assert(run_loaded_events(&diag.server, &fake) == TR2_OK);
    assert(fake.transmit_calls == calls);

    const uint8_t b1_request[] = {3u, 3u, 0xE8u, 0u, 1u};
    request(&diag.server, &fake, 1u, b1_request, sizeof(b1_request));
    assert_exception(&fake, 3u, MODBUS_PDU_EXCEPTION_SLAVE_DEVICE_FAILURE);
    const uint8_t b2_request[] = {3u, 7u, 0xD0u, 0u, 1u};
    request(&diag.server, &fake, 1u, b2_request, sizeof(b2_request));
    assert_exception(&fake, 3u, MODBUS_PDU_EXCEPTION_SLAVE_DEVICE_FAILURE);
    const uint8_t write_b3[] = {16u, 0x0Bu, 0xB8u, 0u, 1u, 2u, 0u, 1u};
    request(&diag.server, &fake, 1u, write_b3, sizeof(write_b3));
    assert_exception(&fake, 16u, MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_ADDRESS);
    const uint8_t write_b0[] = {16u, 0u, 0u, 0u, 1u, 2u, 0u, 1u};
    request(&diag.server, &fake, 1u, write_b0, sizeof(write_b0));
    assert_exception(&fake, 16u, MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_ADDRESS);
    assert(memcmp(&window.image, &original, sizeof(original)) == 0);
    /* Probe reuses the existing B0 PDU/CRC/transport chain. No RX is claimed. */
    calls = fake.transmit_calls;
    assert(iis3dwb_diag_modbus_transmit_probe(NULL) == TR2_ERROR_INVALID_ARGUMENT);
    assert(iis3dwb_diag_modbus_transmit_probe(&diag) == TR2_OK);
    assert(fake.transmit_calls == calls+1u);
    assert(fake.transmitted_length == 47u);
    assert(modbus_rtu_adu_decode(fake.transmitted, fake.transmitted_length,
                                &response) == MODBUS_RTU_CODEC_OK);
    assert(response.unit_id == 1u && response.pdu_length == 44u);
    for (unsigned i=0u; i<TR2_B0_REGISTER_COUNT; ++i) {
        assert(response.pdu[2u+2u*i] == (diag.identity_image.registers[i] >> 8u));
        assert(response.pdu[3u+2u*i] == (diag.identity_image.registers[i] & 255u));
    }
    assert(memcmp(&window.image, &original, sizeof(original)) == 0);
    fake.transmit_result = TR2_ERROR_UNAVAILABLE;
    calls = fake.transmit_calls;
    assert(iis3dwb_diag_modbus_transmit_probe(&diag) == TR2_ERROR_UNAVAILABLE);
    assert(fake.transmit_calls == calls+1u); /* exactly one failed probe, no retry */
    encode_request(1u, b3_request, sizeof(b3_request), adu, &length);
    load_frame_events(&fake, adu, length);
    calls = fake.transmit_calls;
    assert(run_loaded_events(&diag.server, &fake) == TR2_ERROR_UNAVAILABLE);
    assert(fake.transmit_calls == calls+1u); /* no retry */
    return 0;
}
