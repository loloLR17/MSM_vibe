#include <assert.h>
#include <stdint.h>

#include "tr2/modbus/projection.h"

int main(void)
{
    TimeSnapshot snapshot = { 0 };
    ModbusBlock2Image image = { { UINT16_C(0xAAAA) }, UINT32_C(0x55555555) };

    assert(modbus_project_b2(&snapshot, &image) == TR2_ERROR_NOT_AVAILABLE);
    assert(image.registers[0] == UINT16_C(0xAAAA));
    assert(image.source_generation == UINT32_C(0x55555555));

    snapshot.generation = UINT32_C(9);
    snapshot.synchronization_facts_available = true;
    snapshot.time_status = UINT16_C(3);
    snapshot.time_flags = UINT16_C(0xFFFF);
    snapshot.current_time_available = true;
    snapshot.current_time = UINT32_C(0x12345678);
    snapshot.last_sync_time = UINT32_C(0x11223344);
    snapshot.time_since_sync_s = UINT32_C(0x00010002);
    snapshot.prepared_time_available = true;
    snapshot.prepared_time = UINT32_C(0x89ABCDEF);
    snapshot.prepared_time_status = UINT16_C(1);
    snapshot.time_accuracy_ms = UINT16_C(25);
    snapshot.drift_ppm = INT16_C(-20);
    snapshot.sync_source = UINT16_C(2);

    assert(modbus_project_b2(&snapshot, &image) == TR2_OK);
    assert(image.source_generation == UINT32_C(9));
    assert(image.registers[0] == UINT16_C(3));
    assert(image.registers[1] == UINT16_C(0x00FF));
    assert(image.registers[2] == UINT16_C(0x1234));
    assert(image.registers[3] == UINT16_C(0x5678));
    assert(image.registers[4] == UINT16_C(0x1122));
    assert(image.registers[5] == UINT16_C(0x3344));
    assert(image.registers[6] == UINT16_C(0x0001));
    assert(image.registers[7] == UINT16_C(0x0002));
    assert(image.registers[8] == UINT16_C(0x89AB));
    assert(image.registers[9] == UINT16_C(0xCDEF));
    assert(image.registers[10] == UINT16_C(1));
    assert(image.registers[11] == UINT16_C(25));
    assert(image.registers[12] == UINT16_C(0xFFEC));
    assert(image.registers[13] == UINT16_C(2));
    assert(image.registers[14] == 0u);
    assert(image.registers[15] == 0u);

    snapshot.current_time_available = false;
    image.registers[0] = UINT16_C(0xBEEF);
    assert(modbus_project_b2(&snapshot, &image) == TR2_ERROR_NOT_AVAILABLE);
    assert(image.registers[0] == UINT16_C(0xBEEF));

    return 0;
}
