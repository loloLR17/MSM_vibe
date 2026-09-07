#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/modbus/read_adapter.h"

int main(void)
{
    ModbusReadSources sources = {0};
    ModbusReadOutcome outcome;
    SupervisionSnapshot supervision;
    uint16_t values[48] = {0u};

    memset(&supervision, 0, sizeof(supervision));
    supervision.values_available = true;
    supervision.window_complete = true;
    supervision.saturation_observed = false;
    supervision.calculation_sequence = UINT32_C(7);
    supervision.window_duration_ms = UINT32_C(125);
    supervision.valid_sample_count = UINT32_C(2);
    supervision.rms_global_mg = UINT32_C(9);
    supervision.peak_global_mg = UINT32_C(12);
    supervision.rms_x_mg = UINT32_C(2);
    supervision.rms_y_mg = UINT32_C(2);
    supervision.rms_z_mg = UINT32_C(8);
    supervision.peak_x_mg = UINT32_C(3);
    supervision.peak_y_mg = UINT32_C(4);
    supervision.peak_z_mg = UINT32_C(12);
    sources.supervision = &supervision;

    outcome = modbus_read_adapter_read(&sources, UINT16_C(3000), UINT16_C(48), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(values[0] == 0u);
    assert(values[1] == UINT16_C(0x0028));
    assert(values[8] == 0u);
    assert(values[9] == UINT16_C(7));
    assert(values[10] == 0u);
    assert(values[11] == UINT16_C(125));
    assert(values[14] == 0u);
    assert(values[15] == UINT16_C(9));
    assert(values[16] == 0u);
    assert(values[17] == UINT16_C(12));
    assert(values[30] == 0u);
    assert(values[39] == 0u);
    assert(values[47] == 0u);

    sources.supervision = NULL;
    values[0] = UINT16_C(0xBEEF);
    outcome = modbus_read_adapter_read(&sources, UINT16_C(3000), UINT16_C(1), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_NOT_AVAILABLE);
    assert(values[0] == UINT16_C(0xBEEF));

    sources.supervision = &supervision;
    supervision.values_available = false;
    outcome = modbus_read_adapter_read(&sources, UINT16_C(3000), UINT16_C(1), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_NOT_AVAILABLE);

    return 0;
}
