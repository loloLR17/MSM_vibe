#include <assert.h>
#include <stdint.h>

#include "tr2/modbus/read_adapter.h"

int main(void)
{
    ModbusBlock3Image b3 = { { 0u }, UINT32_C(7) };
    ModbusReadSources sources = {0};
    ModbusReadOutcome outcome;
    uint16_t values[48] = {0u};
    uint16_t index;

    for (index = 0u; index < TR2_B3_REGISTER_COUNT; ++index) {
        b3.registers[index] = (uint16_t)(UINT16_C(0x3000) + index);
    }
    b3.registers[40] = 0u;
    b3.registers[47] = 0u;
    sources.b3_image = &b3;

    outcome = modbus_read_adapter_read(&sources, UINT16_C(3000), UINT16_C(48), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(values[0] == UINT16_C(0x3000));
    assert(values[9] == UINT16_C(0x3009));
    assert(values[39] == UINT16_C(0x3027));
    assert(values[40] == 0u);
    assert(values[47] == 0u);

    outcome = modbus_read_adapter_read(&sources, UINT16_C(3014), UINT16_C(4), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(values[0] == UINT16_C(0x300E));
    assert(values[3] == UINT16_C(0x3011));

    sources.b3_image = NULL;
    values[0] = UINT16_C(0xBEEF);
    outcome = modbus_read_adapter_read(&sources, UINT16_C(3000), UINT16_C(1), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_NOT_AVAILABLE);
    assert(values[0] == UINT16_C(0xBEEF));

    values[0] = UINT16_C(0xCAFE);
    outcome = modbus_read_adapter_read(&sources, UINT16_C(3047), UINT16_C(2), values);
    assert(outcome.access_result == MODBUS_ACCESS_ILLEGAL_ADDRESS);
    assert(outcome.operation_result == TR2_OK);
    assert(values[0] == UINT16_C(0xCAFE));

    return 0;
}
