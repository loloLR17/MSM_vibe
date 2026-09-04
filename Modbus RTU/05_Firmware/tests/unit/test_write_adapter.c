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

int main(void)
{
    TestWallClockContext clock_context = { UINT32_C(0x01020304), 0u };
    WallClock wall_clock = { &clock_context, test_wall_read, test_wall_set };
    TimeService service = { 0 };
    TimeService second_service = { 0 };
    ModbusWriteOutcome outcome;
    bool available = false;
    uint16_t values[2];
    uint32_t generation_before;

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

    return 0;
}
