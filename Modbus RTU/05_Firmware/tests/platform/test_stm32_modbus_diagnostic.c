#include <assert.h>
#include <string.h>

#include "stm32_modbus_diagnostic.h"

typedef struct {
    SerialTransportEvent events[MODBUS_RTU_ADU_MAX_SIZE + 2u];
    size_t count;
    size_t index;
    unsigned starts;
    unsigned transmissions;
    uint8_t reply[MODBUS_RTU_ADU_MAX_SIZE];
    size_t reply_length;
    Tr2Result tx_result;
} Bench;

static Tr2Result start(void *context)
{
    ((Bench *)context)->starts++;
    return TR2_OK;
}

static Tr2Result transmit(void *context, const uint8_t *data, size_t length)
{
    Bench *bench = context;
    assert(length <= sizeof(bench->reply));
    bench->transmissions++;
    memcpy(bench->reply, data, length);
    bench->reply_length = length;
    return bench->tx_result;
}

static bool poll(void *context, SerialTransportEvent *event)
{
    Bench *bench = context;
    *event = bench->index < bench->count ? bench->events[bench->index++]
        : (SerialTransportEvent){SERIAL_TRANSPORT_EVENT_NONE, 0u, 0};
    return true;
}

static Tr2Result request(Stm32ModbusDiagnostic *diagnostic, Bench *bench,
    uint8_t unit, const uint8_t *pdu, size_t length, bool bad_crc)
{
    uint8_t adu[MODBUS_RTU_ADU_MAX_SIZE];
    size_t adu_length;
    Tr2Result result = TR2_OK;
    assert(modbus_rtu_adu_encode(unit, pdu, length, adu, sizeof(adu),
        &adu_length) == MODBUS_RTU_CODEC_OK);
    if (bad_crc) adu[adu_length - 1u] ^= 1u;
    bench->index = 0u;
    bench->count = 0u;
    for (size_t i = 0u; i < adu_length; ++i)
        bench->events[bench->count++] =
            (SerialTransportEvent){SERIAL_TRANSPORT_EVENT_BYTE, adu[i], 0};
    bench->events[bench->count++] =
        (SerialTransportEvent){SERIAL_TRANSPORT_EVENT_SILENCE_T1_5, 0u, 0};
    bench->events[bench->count++] =
        (SerialTransportEvent){SERIAL_TRANSPORT_EVENT_SILENCE_T3_5, 0u, 0};
    const unsigned before = bench->transmissions;
    while (bench->index < bench->count) {
        result = modbus_rtu_server_runtime_poll_once(&diagnostic->rtu);
        if (bench->index < bench->count) assert(bench->transmissions == before);
    }
    return result;
}

static void expect_reply(const Bench *bench, const uint8_t *pdu, size_t length)
{
    ModbusRtuAduView decoded;
    assert(modbus_rtu_adu_decode(bench->reply, bench->reply_length, &decoded)
        == MODBUS_RTU_CODEC_OK);
    assert(decoded.unit_id == 17u);
    assert(decoded.pdu_length == length);
    assert(memcmp(decoded.pdu, pdu, length) == 0);
}

int main(void)
{
    Bench bench = {0};
    SerialTransport transport = {&bench, start, transmit, poll};
    Stm32ModbusDiagnostic diagnostic;
    const uint8_t read_status[] = {3u, 3u, 0xE8u, 0u, 1u};
    const uint8_t unknown[] = {3u, 2u, 0u, 0u};
    const uint8_t read_all[] = {3u, 3u, 0xE8u, 0u, 20u};
    const uint8_t read_temperature[] = {3u, 3u, 0xEFu, 0u, 1u};
    const uint8_t read_identity[] = {3u, 0u, 0u, 0u, 1u};
    const uint8_t read_failure[] = {0x83u, 4u};
    const uint8_t write_b1[] = {16u, 3u, 0xE8u, 0u, 1u, 2u, 0u, 1u};
    const uint8_t write_b2[] = {16u, 7u, 0xD8u, 0u, 2u, 4u, 0u, 0u, 0u, 1u};
    const uint8_t write_b5[] = {16u, 0x13u, 0x8Au, 0u, 1u, 2u, 0u, 1u};
    const uint8_t write_ro_failure[] = {0x90u, 2u};
    const uint8_t write_no_authority[] = {0x90u, 4u};
    const uint8_t fc06[] = {6u, 0u, 0u, 0u, 1u};
    const uint8_t unsupported[] = {0x86u, 1u};

    assert(stm32_modbus_diagnostic_init(NULL, &transport, 17u) == TR2_ERROR_INVALID_ARGUMENT);
    assert(stm32_modbus_diagnostic_init(&diagnostic, &transport, 0u) == TR2_ERROR_INVALID_ARGUMENT);
    assert(stm32_modbus_diagnostic_init(&diagnostic, &transport, 248u) == TR2_ERROR_INVALID_ARGUMENT);
    assert(stm32_modbus_diagnostic_init(&diagnostic, &transport, 17u) == TR2_OK);
    assert(modbus_rtu_server_runtime_start(&diagnostic.rtu) == TR2_OK);
    assert(bench.starts == 1u);
    assert(modbus_rtu_server_runtime_poll_once(&diagnostic.rtu) == TR2_OK);
    assert(bench.transmissions == 0u);
    assert(request(&diagnostic, &bench, 17u, read_status, sizeof(read_status), false) == TR2_OK);
    expect_reply(&bench, unknown, sizeof(unknown));
    assert(request(&diagnostic, &bench, 17u, read_all, sizeof(read_all), false) == TR2_OK);
    expect_reply(&bench, read_failure, sizeof(read_failure));
    assert(request(&diagnostic, &bench, 17u, read_temperature, sizeof(read_temperature), false) == TR2_OK);
    expect_reply(&bench, read_failure, sizeof(read_failure));
    assert(request(&diagnostic, &bench, 17u, read_identity, sizeof(read_identity), false) == TR2_OK);
    expect_reply(&bench, read_failure, sizeof(read_failure));
    assert(request(&diagnostic, &bench, 17u, write_b1, sizeof(write_b1), false) == TR2_OK);
    expect_reply(&bench, write_ro_failure, sizeof(write_ro_failure));
    assert(request(&diagnostic, &bench, 17u, write_b2, sizeof(write_b2), false) == TR2_OK);
    expect_reply(&bench, write_no_authority, sizeof(write_no_authority));
    assert(request(&diagnostic, &bench, 17u, write_b5, sizeof(write_b5), false) == TR2_OK);
    expect_reply(&bench, write_no_authority, sizeof(write_no_authority));
    assert(request(&diagnostic, &bench, 17u, fc06, sizeof(fc06), false) == TR2_OK);
    expect_reply(&bench, unsupported, sizeof(unsupported));
    const unsigned before = bench.transmissions;
    assert(request(&diagnostic, &bench, 18u, read_status, sizeof(read_status), false) == TR2_OK);
    assert(request(&diagnostic, &bench, 0u, read_status, sizeof(read_status), false) == TR2_OK);
    assert(request(&diagnostic, &bench, 0u, write_b5, sizeof(write_b5), false) == TR2_OK);
    assert(request(&diagnostic, &bench, 17u, read_status, sizeof(read_status), true) == TR2_OK);
    assert(bench.transmissions == before);
    assert(diagnostic.rtu.pdu_context.command_mailbox == NULL);
    assert(diagnostic.rtu.pdu_context.time_service == NULL);
    assert(diagnostic.rtu.pdu_context.configuration_staging == NULL);
    assert(diagnostic.rtu.pdu_context.campaign_inventory == NULL);
    assert(diagnostic.incomplete_system.registers[0] == 0u);
    bench.tx_result = TR2_ERROR_UNAVAILABLE;
    assert(request(&diagnostic, &bench, 17u, read_status, sizeof(read_status), false)
        == TR2_ERROR_UNAVAILABLE);
    const unsigned failed = bench.transmissions;
    assert(modbus_rtu_server_runtime_poll_once(&diagnostic.rtu) == TR2_OK);
    assert(bench.transmissions == failed); /* No automatic replay. */
    return 0;
}
