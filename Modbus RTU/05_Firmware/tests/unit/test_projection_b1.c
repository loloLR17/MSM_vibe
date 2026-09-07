#include <assert.h>
#include <stdint.h>

#include "tr2/domain/diagnostic/diagnostic.h"
#include "tr2/domain/system_state/system_state.h"
#include "tr2/modbus/projection.h"

int main(void)
{
    const SystemStateSnapshot source = {
        UINT32_C(23),
        UINT16_C(3),
        UINT16_C(0xFFFF),
        UINT16_C(0xFFFF),
        UINT16_C(0xFFFF),
        UINT32_C(0x12345678),
        UINT16_C(3),
        INT16_C(-50),
        UINT16_C(73),
        UINT16_C(42),
        UINT16_C(1),
        UINT16_C(64),
        UINT16_C(1),
        UINT32_C(0x89ABCDEF),
        UINT16_C(0x1357),
        UINT16_C(0x2468)
    };
    TimeSnapshot time = { 0 };
    ModbusBlock1ProjectionSource projection_source = { &source, &time };
    SystemStateService service = { 0 };
    SystemStateSnapshot snapshot = { 0 };
    ModbusBlock1Image image = { { 0u }, 0u };
    DiagnosticSnapshot diagnostic = { 0 };
    ModbusBlock7ProjectionSource b7_source = { 0 };
    ModbusBlock7Image b7 = { { 0u }, 0u };

    assert(system_state_service_get_snapshot(&service, &snapshot) == TR2_ERROR_INVALID_STATE);
    assert(system_state_service_init(&service, &source) == TR2_OK);
    assert(system_state_service_get_snapshot(&service, &snapshot) == TR2_OK);
    assert(snapshot.generation == UINT32_C(23));
    assert(snapshot.cpu_load_percent == UINT16_C(73));

    projection_source.system_state = &snapshot;
    assert(modbus_project_b1(&projection_source, &image) == TR2_OK);
    assert(image.source_generation == UINT32_C(23));
    assert(image.registers[0] == UINT16_C(3));
    assert(image.registers[1] == UINT16_C(0x0017));
    assert(image.registers[2] == UINT16_C(0x003F));
    assert(image.registers[3] == UINT16_C(0x0007));
    assert(image.registers[4] == UINT16_C(0x1234));
    assert(image.registers[5] == UINT16_C(0x5678));
    assert(image.registers[6] == UINT16_C(3));
    assert(image.registers[7] == UINT16_C(0xFFCE));
    assert(image.registers[8] == UINT16_C(73));
    assert(image.registers[9] == UINT16_C(42));
    assert(image.registers[10] == UINT16_C(1));
    assert(image.registers[11] == UINT16_C(64));
    assert(image.registers[12] == UINT16_C(1));
    assert(image.registers[13] == UINT16_C(0x89AB));
    assert(image.registers[14] == UINT16_C(0xCDEF));
    assert(image.registers[15] == UINT16_C(0x1357));
    assert(image.registers[16] == UINT16_C(0x2468));
    assert(image.registers[17] == 0u);
    assert(image.registers[18] == 0u);
    assert(image.registers[19] == 0u);

    time.civil_time_usable = true;
    assert(modbus_project_b1(&projection_source, &image) == TR2_OK);
    assert(image.registers[1] == UINT16_C(0x001F));

    time.civil_time_usable = false;
    assert(modbus_project_b1(&projection_source, &image) == TR2_OK);
    assert((image.registers[1] & UINT16_C(0x0008)) == 0u);

    projection_source.time = NULL;
    assert(modbus_project_b1(&projection_source, &image) == TR2_OK);
    assert((image.registers[1] & UINT16_C(0x0008)) == 0u);

    diagnostic.generation = UINT32_C(42);
    diagnostic.facts.health = DIAGNOSTIC_HEALTH_DEGRADED;
    diagnostic.facts.active_conditions.sensor_fault = true;
    diagnostic.facts.active_conditions.memory_fault = true;
    diagnostic.facts.active_conditions.storage_fault = true;
    diagnostic.facts.active_conditions.firmware_fault = true;
    diagnostic.facts.active_conditions.temperature_out_of_range = true;
    diagnostic.facts.last_fault.present = true;
    diagnostic.facts.last_fault.code = UINT16_C(0x1234);
    diagnostic.facts.last_fault.timestamp_available = true;
    diagnostic.facts.last_fault.timestamp = UINT32_C(0x89ABCDEF);
    diagnostic.facts.selftest.state = DIAGNOSTIC_SELFTEST_FAILED;
    diagnostic.facts.selftest.result_code = UINT16_C(0x4567);
    diagnostic.facts.selftest.detail = UINT16_C(0x89AB);
    diagnostic.facts.internal_temperature_available = true;
    diagnostic.facts.internal_temp_dC = INT16_C(-50);
    diagnostic.facts.supply_voltage_available = true;
    diagnostic.facts.supply_voltage_mV = UINT16_C(3300);

    b7_source.diagnostic = &diagnostic;
    b7_source.uptime_s = UINT32_C(0x12345678);
    b7_source.reset_cause = UINT16_C(4);
    assert(modbus_project_b7(&b7_source, &b7) == TR2_OK);
    assert(b7.source_generation == UINT32_C(42));
    assert(b7.registers[0] == UINT16_C(1));
    assert(b7.registers[1] == UINT16_C(2));
    assert(b7.registers[2] == UINT16_C(0x014D));
    assert(b7.registers[3] == UINT16_C(0x1234));
    assert(b7.registers[4] == UINT16_C(0x89AB));
    assert(b7.registers[5] == UINT16_C(0xCDEF));
    assert(b7.registers[6] == UINT16_C(3));
    assert(b7.registers[7] == UINT16_C(0x4567));
    assert(b7.registers[8] == UINT16_C(0x89AB));
    assert(b7.registers[9] == UINT16_C(0x1234));
    assert(b7.registers[10] == UINT16_C(0x5678));
    assert(b7.registers[11] == UINT16_C(4));
    assert(b7.registers[12] == UINT16_C(0xFFCE));
    assert(b7.registers[13] == UINT16_C(3300));
    assert(b7.registers[14] == 0u);
    assert(b7.registers[15] == 0u);

    diagnostic.facts.last_fault.timestamp_available = false;
    diagnostic.facts.internal_temperature_available = false;
    diagnostic.facts.supply_voltage_available = false;
    assert(modbus_project_b7(&b7_source, &b7) == TR2_OK);
    assert(b7.registers[4] == 0u);
    assert(b7.registers[5] == 0u);
    assert(b7.registers[12] == 0u);
    assert(b7.registers[13] == 0u);

    return 0;
}
