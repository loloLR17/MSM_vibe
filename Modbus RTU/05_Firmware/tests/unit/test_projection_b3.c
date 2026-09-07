#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/modbus/projection.h"

int main(void)
{
    SupervisionSnapshot snapshot;
    ModbusBlock3Image image;
    uint16_t index;

    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.values_available = true;
    snapshot.window_complete = true;
    snapshot.saturation_observed = false;
    snapshot.calculation_error = false;
    snapshot.civil_timestamp_available = true;
    snapshot.civil_timestamp = UINT32_C(0x11223344);
    snapshot.value_age_available = true;
    snapshot.value_age_ms = UINT32_C(0x55667788);
    snapshot.calculation_sequence = UINT32_C(0x01020304);
    snapshot.window_duration_ms = UINT32_C(5000);
    snapshot.valid_sample_count = UINT32_C(4096);
    snapshot.rms_global_mg = UINT32_C(101);
    snapshot.peak_global_mg = UINT32_C(202);
    snapshot.rms_x_mg = UINT32_C(11);
    snapshot.rms_y_mg = UINT32_C(22);
    snapshot.rms_z_mg = UINT32_C(33);
    snapshot.peak_x_mg = UINT32_C(44);
    snapshot.peak_y_mg = UINT32_C(55);
    snapshot.peak_z_mg = UINT32_C(66);
    snapshot.threshold_facts_available = true;
    snapshot.threshold_facts.global.rms_warning = SUPERVISION_THRESHOLD_ABOVE;
    snapshot.threshold_facts.global.rms_alarm = SUPERVISION_THRESHOLD_EQUAL;

    assert(modbus_project_b3(&snapshot, &image) == TR2_OK);
    assert(image.source_calculation_sequence == UINT32_C(0x01020304));

    /* Only factual validity bits are asserted in P5-G. */
    assert(image.registers[0] == 0u);
    assert(image.registers[1] == UINT16_C(0x0028));
    assert(image.registers[2] == 0u);
    assert(image.registers[3] == 0u);

    assert(image.registers[4] == UINT16_C(0x1122));
    assert(image.registers[5] == UINT16_C(0x3344));
    assert(image.registers[6] == UINT16_C(0x5566));
    assert(image.registers[7] == UINT16_C(0x7788));
    assert(image.registers[8] == UINT16_C(0x0102));
    assert(image.registers[9] == UINT16_C(0x0304));
    assert(image.registers[10] == 0u);
    assert(image.registers[11] == UINT16_C(5000));
    assert(image.registers[12] == 0u);
    assert(image.registers[13] == UINT16_C(4096));
    assert(image.registers[14] == 0u);
    assert(image.registers[15] == UINT16_C(101));
    assert(image.registers[16] == 0u);
    assert(image.registers[17] == UINT16_C(202));
    assert(image.registers[18] == 0u);
    assert(image.registers[19] == UINT16_C(11));
    assert(image.registers[20] == 0u);
    assert(image.registers[21] == UINT16_C(22));
    assert(image.registers[22] == 0u);
    assert(image.registers[23] == UINT16_C(33));
    assert(image.registers[24] == 0u);
    assert(image.registers[25] == UINT16_C(44));
    assert(image.registers[26] == 0u);
    assert(image.registers[27] == UINT16_C(55));
    assert(image.registers[28] == 0u);
    assert(image.registers[29] == UINT16_C(66));

    /* P5-F comparison facts must not be silently converted into B3 decisions. */
    for (index = 30u; index < TR2_B3_REGISTER_COUNT; ++index) {
        assert(image.registers[index] == 0u);
    }

    snapshot.window_complete = false;
    snapshot.saturation_observed = true;
    snapshot.calculation_error = true;
    snapshot.civil_timestamp_available = false;
    snapshot.value_age_available = false;
    assert(modbus_project_b3(&snapshot, &image) == TR2_OK);
    assert(image.registers[1] == UINT16_C(0x0800));
    assert(image.registers[4] == 0u);
    assert(image.registers[5] == 0u);
    assert(image.registers[6] == 0u);
    assert(image.registers[7] == 0u);

    snapshot.values_available = false;
    assert(modbus_project_b3(&snapshot, &image) == TR2_ERROR_NOT_AVAILABLE);

    return 0;
}
