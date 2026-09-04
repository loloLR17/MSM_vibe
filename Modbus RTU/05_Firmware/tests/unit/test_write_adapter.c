#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/modbus/write_adapter.h"

typedef struct {
    Tr2CivilTimestamp current_time;
    uint32_t set_calls;
} TestWallClockContext;

static WallClockReadResult test_wall_read(void *context, Tr2CivilTimestamp *timestamp)
{
    TestWallClockContext *clock = (TestWallClockContext *)context;
    if (clock == NULL || timestamp == NULL) {
        return WALL_CLOCK_INVALID;
    }
    *timestamp = clock->current_time;
    return WALL_CLOCK_OK;
}

static Tr2Result test_wall_set(void *context, Tr2CivilTimestamp timestamp)
{
    TestWallClockContext *clock = (TestWallClockContext *)context;
    if (clock == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    clock->current_time = timestamp;
    clock->set_calls++;
    return TR2_OK;
}

static Tr2CivilTimestamp prepared_value(const TimeService *service, bool *available)
{
    Tr2CivilTimestamp value = UINT32_C(0xFFFFFFFF);
    assert(time_service_get_prepared_time(service, available, &value) == TR2_OK);
    return value;
}

static PreparedConfiguration prepared_configuration(const ConfigurationStagingService *service)
{
    PreparedConfiguration prepared = { 0 };
    assert(configuration_staging_snapshot(service, &prepared));
    return prepared;
}

int main(void)
{
    TestWallClockContext clock_context = { UINT32_C(0x01020304), 0u };
    WallClock wall_clock = { &clock_context, test_wall_read, test_wall_set };
    TimeService service = { 0 };
    TimeService second_service = { 0 };
    ConfigurationStagingService staging = { 0 };
    ModbusWriteOutcome outcome;
    bool available = false;
    uint16_t values[7];
    uint32_t generation_before;
    PreparedConfiguration prepared;

    assert(time_service_init(&service, &wall_clock) == TR2_OK);

    values[0] = UINT16_C(0x1234);
    outcome = modbus_write_adapter_write_b2(&service, UINT16_C(2008), values, UINT16_C(1));
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(prepared_value(&service, &available) == UINT32_C(0x12340000));
    assert(available);
    assert(service.generation == UINT32_C(1));
    assert(clock_context.set_calls == 0u);

    values[0] = UINT16_C(0x5678);
    outcome = modbus_write_adapter_write_b2(&service, UINT16_C(2009), values, UINT16_C(1));
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(prepared_value(&service, &available) == UINT32_C(0x12345678));
    assert(service.generation == UINT32_C(2));
    assert(clock_context.set_calls == 0u);

    assert(time_service_init(&second_service, &wall_clock) == TR2_OK);
    values[0] = UINT16_C(0x5678);
    outcome = modbus_write_adapter_write_b2(&second_service, UINT16_C(2009), values, UINT16_C(1));
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(prepared_value(&second_service, &available) == UINT32_C(0x00005678));

    values[0] = UINT16_C(0xABCD);
    values[1] = UINT16_C(0xEF01);
    generation_before = service.generation;
    outcome = modbus_write_adapter_write_b2(&service, UINT16_C(2008), values, UINT16_C(2));
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(prepared_value(&service, &available) == UINT32_C(0xABCDEF01));
    assert(service.generation == generation_before + UINT32_C(1));
    assert(clock_context.set_calls == 0u);

    generation_before = service.generation;
    values[0] = UINT16_C(0x9999);
    values[1] = UINT16_C(0x8888);
    outcome = modbus_write_adapter_write_b2(&service, UINT16_C(2007), values, UINT16_C(2));
    assert(outcome.access_result == MODBUS_ACCESS_READ_ONLY);
    assert(outcome.operation_result == TR2_OK);
    assert(prepared_value(&service, &available) == UINT32_C(0xABCDEF01));
    assert(service.generation == generation_before);

    outcome = modbus_write_adapter_write_b2(&service, UINT16_C(2009), values, UINT16_C(2));
    assert(outcome.access_result == MODBUS_ACCESS_READ_ONLY);
    assert(prepared_value(&service, &available) == UINT32_C(0xABCDEF01));
    assert(service.generation == generation_before);

    outcome = modbus_write_adapter_write_b2(&service, UINT16_C(0), values, UINT16_C(1));
    assert(outcome.access_result == MODBUS_ACCESS_READ_ONLY);
    assert(service.generation == generation_before);

    outcome = modbus_write_adapter_write_b2(&service, UINT16_C(2014), values, UINT16_C(1));
    assert(outcome.access_result == MODBUS_ACCESS_RESERVED);
    assert(service.generation == generation_before);

    outcome = modbus_write_adapter_write_b2(&service, UINT16_C(1500), values, UINT16_C(1));
    assert(outcome.access_result == MODBUS_ACCESS_ILLEGAL_ADDRESS);
    assert(service.generation == generation_before);

    outcome = modbus_write_adapter_write_b2(&service, UINT16_C(2008), NULL, UINT16_C(1));
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_INVALID_ARGUMENT);
    assert(service.generation == generation_before);
    assert(clock_context.set_calls == 0u);

    configuration_staging_init(&staging);
    assert(!configuration_staging_has_prepared(&staging));

    values[0] = UINT16_C(0x1234);
    outcome = modbus_write_adapter_write_b4(&staging, UINT16_C(4002), values, UINT16_C(1));
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    prepared = prepared_configuration(&staging);
    assert(prepared.config_id == UINT32_C(0x12340000));
    assert(prepared.generation == UINT32_C(1));

    values[0] = UINT16_C(0x5678);
    outcome = modbus_write_adapter_write_b4(&staging, UINT16_C(4003), values, UINT16_C(1));
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    prepared = prepared_configuration(&staging);
    assert(prepared.config_id == UINT32_C(0x12345678));
    assert(prepared.generation == UINT32_C(2));

    values[0] = UINT16_C(0x89AB);
    values[1] = UINT16_C(0xCDEF);
    outcome = modbus_write_adapter_write_b4(&staging, UINT16_C(4008), values, UINT16_C(2));
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    prepared = prepared_configuration(&staging);
    assert(prepared.supplied_crc == UINT32_C(0x89ABCDEF));
    assert(prepared.config_id == UINT32_C(0x12345678));

    configuration_staging_mark_validation_current(&staging);
    assert(configuration_staging_is_validation_current(&staging));
    generation_before = prepared.generation;

    values[0] = UINT16_C(0xFFFF);
    outcome = modbus_write_adapter_write_b4(&staging, UINT16_C(4016), values, UINT16_C(1));
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    prepared = prepared_configuration(&staging);
    assert(prepared.payload.sampling_frequency_hz == UINT16_C(0xFFFF));
    assert(prepared.config_id == UINT32_C(0x12345678));
    assert(prepared.supplied_crc == UINT32_C(0x89ABCDEF));
    assert(prepared.generation == generation_before + UINT32_C(1));
    assert(!configuration_staging_is_validation_current(&staging));

    generation_before = prepared.generation;
    values[0] = UINT16_C(0x0001);
    values[1] = UINT16_C(0x0002);
    values[2] = UINT16_C(0x0003);
    values[3] = UINT16_C(0x0004);
    values[4] = UINT16_C(0x0005);
    values[5] = UINT16_C(0x0006);
    values[6] = UINT16_C(0x0007);
    outcome = modbus_write_adapter_write_b4(&staging, UINT16_C(4040), values, UINT16_C(7));
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    prepared = prepared_configuration(&staging);
    assert(prepared.payload.supervision_enable_mask == UINT16_C(1));
    assert(prepared.payload.rms_warn_threshold_mg == UINT16_C(2));
    assert(prepared.payload.rms_alarm_threshold_mg == UINT16_C(3));
    assert(prepared.payload.peak_warn_threshold_mg == UINT16_C(4));
    assert(prepared.payload.peak_alarm_threshold_mg == UINT16_C(5));
    assert(prepared.payload.threshold_hysteresis_mg == UINT16_C(6));
    assert(prepared.payload.alarm_hold_time_ms == UINT16_C(7));
    assert(prepared.generation == generation_before + UINT32_C(1));

    values[0] = UINT16_C(0x4142);
    values[1] = UINT16_C(0x0043);
    outcome = modbus_write_adapter_write_b4(&staging, UINT16_C(4060), values, UINT16_C(2));
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    prepared = prepared_configuration(&staging);
    assert((uint8_t)prepared.payload.campaign_label[0] == UINT8_C(0x41));
    assert((uint8_t)prepared.payload.campaign_label[1] == UINT8_C(0x42));
    assert((uint8_t)prepared.payload.campaign_label[2] == UINT8_C(0x00));
    assert((uint8_t)prepared.payload.campaign_label[3] == UINT8_C(0x43));

    values[0] = UINT16_C(0x80FF);
    outcome = modbus_write_adapter_write_b4(&staging, UINT16_C(4076), values, UINT16_C(1));
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    prepared = prepared_configuration(&staging);
    assert((uint8_t)prepared.payload.mission_label[0] == UINT8_C(0x80));
    assert((uint8_t)prepared.payload.mission_label[1] == UINT8_C(0xFF));

    generation_before = prepared.generation;
    values[0] = UINT16_C(0x9999);
    outcome = modbus_write_adapter_write_b4(&staging, UINT16_C(4017), values, UINT16_C(1));
    assert(outcome.access_result == MODBUS_ACCESS_RESERVED);
    assert(prepared_configuration(&staging).generation == generation_before);

    outcome = modbus_write_adapter_write_b4(&staging, UINT16_C(4100), values, UINT16_C(1));
    assert(outcome.access_result == MODBUS_ACCESS_READ_ONLY);
    assert(prepared_configuration(&staging).generation == generation_before);

    values[0] = UINT16_C(0x1111);
    values[1] = UINT16_C(0x2222);
    outcome = modbus_write_adapter_write_b4(&staging, UINT16_C(4016), values, UINT16_C(2));
    assert(outcome.access_result == MODBUS_ACCESS_RESERVED);
    assert(prepared_configuration(&staging).generation == generation_before);

    outcome = modbus_write_adapter_write_b4(&staging, UINT16_C(4176), values, UINT16_C(1));
    assert(outcome.access_result == MODBUS_ACCESS_ILLEGAL_ADDRESS);
    assert(prepared_configuration(&staging).generation == generation_before);

    outcome = modbus_write_adapter_write_b4(&staging, UINT16_C(4016), NULL, UINT16_C(1));
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_INVALID_ARGUMENT);
    assert(prepared_configuration(&staging).generation == generation_before);

    return 0;
}
