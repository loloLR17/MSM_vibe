#include <assert.h>
#include <stdint.h>

#include "tr2/modbus/read_adapter.h"

int main(void)
{
    const IdentitySnapshot identity = {
        .generation = UINT32_C(11),
        .device_id = UINT32_C(0x12345678),
        .hardware_version = UINT16_C(2),
        .firmware_version_major = UINT16_C(1),
        .firmware_version_minor = UINT16_C(4),
        .firmware_version_patch = UINT16_C(7),
        .protocol_version = UINT16_C(1),
        .device_capabilities = UINT16_C(0xFFFF),
        .serial_number = "SERIAL0000000001",
        .manufacturer = "TR2MAKER"
    };
    TimeSnapshot time = {
        .generation = UINT32_C(13),
        .synchronization_facts_available = true,
        .time_status = UINT16_C(4),
        .time_flags = UINT16_C(0x0080),
        .current_time_available = true,
        .current_time = UINT32_C(0x10203040),
        .last_sync_time = UINT32_C(0x01020304),
        .time_since_sync_s = UINT32_C(0x00010002),
        .prepared_time_available = true,
        .prepared_time = UINT32_C(0x55667788),
        .prepared_time_status = UINT16_C(1),
        .time_accuracy_ms = UINT16_C(25),
        .drift_ppm = INT16_C(-20),
        .sync_source = UINT16_C(1),
        .civil_time_usable = true,
        .continuity = TIME_CONTINUITY_PROVEN,
        .last_sync_history = {
            .state = LAST_SYNC_HISTORY_VALID,
            .timestamp = UINT32_C(0x01020304),
            .source = UINT16_C(1)
        }
    };
    ModbusBlock0Image b0 = { { 0u }, 0u };
    ModbusBlock1Image b1 = { { 0u }, UINT32_C(12) };
    ModbusBlock3Image b3 = { { 0u }, UINT32_C(7) };
    ModbusBlock4Image b4 = { { 0u } };
    ModbusBlock7Image b7 = { { 0u }, UINT32_C(77) };
    ModbusReadSources sources = {0};
    ModbusReadOutcome outcome;
    uint16_t values[48] = {0u};
    uint16_t index;

    assert(modbus_project_b0(&identity, &b0) == TR2_OK);
    for (index = 0u; index < TR2_B1_REGISTER_COUNT; ++index) {
        b1.registers[index] = (uint16_t)(UINT16_C(0x1100) + index);
    }
    for (index = 0u; index < TR2_B3_REGISTER_COUNT; ++index) {
        b3.registers[index] = (uint16_t)(UINT16_C(0x3300) + index);
    }
    b4.registers[0] = UINT16_C(0x4100);
    b4.registers[1] = UINT16_C(0x4101);
    b4.registers[106] = UINT16_C(0x416A);
    b4.registers[107] = UINT16_C(0x416B);
    b4.registers[108] = UINT16_C(0x416C);
    b4.registers[164] = UINT16_C(0x41A4);
    for (index = 0u; index < TR2_B7_REGISTER_COUNT; ++index) {
        b7.registers[index] = (uint16_t)(UINT16_C(0x7000) + index);
    }

    sources.b0_image = &b0;
    sources.time = &time;
    sources.b1_image = &b1;
    sources.b3_image = &b3;
    sources.b4_image = &b4;
    sources.b7_image = &b7;

    outcome = modbus_read_adapter_read(&sources, UINT16_C(0), UINT16_C(21), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(values[0] == UINT16_C(0x1234));
    assert(values[1] == UINT16_C(0x5678));
    assert(values[20] == 0u);

    sources.b0_image = NULL;
    values[0] = UINT16_C(0xBEEF);
    outcome = modbus_read_adapter_read(&sources, UINT16_C(0), UINT16_C(1), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_NOT_AVAILABLE);
    assert(values[0] == UINT16_C(0xBEEF));
    sources.b0_image = &b0;

    outcome = modbus_read_adapter_read(&sources, UINT16_C(1000), UINT16_C(20), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    for (index = 0u; index < TR2_B1_REGISTER_COUNT; ++index) {
        assert(values[index] == (uint16_t)(UINT16_C(0x1100) + index));
    }

    outcome = modbus_read_adapter_read(&sources, UINT16_C(1004), UINT16_C(2), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(values[0] == UINT16_C(0x1104));
    assert(values[1] == UINT16_C(0x1105));

    sources.b1_image = NULL;
    values[0] = UINT16_C(0xFACE);
    outcome = modbus_read_adapter_read(&sources, UINT16_C(1000), UINT16_C(1), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_NOT_AVAILABLE);
    assert(values[0] == UINT16_C(0xFACE));
    sources.b1_image = &b1;

    outcome = modbus_read_adapter_read(&sources, UINT16_C(2000), UINT16_C(16), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(values[2] == UINT16_C(0x1020));
    assert(values[3] == UINT16_C(0x3040));

    outcome = modbus_read_adapter_read(&sources, UINT16_C(3006), UINT16_C(3), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(values[0] == UINT16_C(0x3306));
    assert(values[1] == UINT16_C(0x3307));
    assert(values[2] == UINT16_C(0x3308));

    sources.b3_image = NULL;
    values[0] = UINT16_C(0xD00D);
    outcome = modbus_read_adapter_read(&sources, UINT16_C(3000), UINT16_C(1), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_NOT_AVAILABLE);
    assert(values[0] == UINT16_C(0xD00D));
    sources.b3_image = &b3;

    outcome = modbus_read_adapter_read(&sources, UINT16_C(4000), UINT16_C(2), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(values[0] == UINT16_C(0x4100));
    assert(values[1] == UINT16_C(0x4101));

    outcome = modbus_read_adapter_read(&sources, UINT16_C(4106), UINT16_C(3), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(values[0] == UINT16_C(0x416A));
    assert(values[1] == UINT16_C(0x416B));
    assert(values[2] == UINT16_C(0x416C));

    outcome = modbus_read_adapter_read(&sources, UINT16_C(4175), UINT16_C(1), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(values[0] == 0u);

    sources.b4_image = NULL;
    values[0] = UINT16_C(0xD00D);
    outcome = modbus_read_adapter_read(&sources, UINT16_C(4000), UINT16_C(1), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_NOT_AVAILABLE);
    assert(values[0] == UINT16_C(0xD00D));
    sources.b4_image = &b4;

    values[0] = UINT16_C(0xBEEF);
    outcome = modbus_read_adapter_read(&sources, UINT16_C(4175), UINT16_C(2), values);
    assert(outcome.access_result == MODBUS_ACCESS_ILLEGAL_ADDRESS);
    assert(values[0] == UINT16_C(0xBEEF));

    outcome = modbus_read_adapter_read(&sources, UINT16_C(7000), UINT16_C(16), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(values[0] == UINT16_C(0x7000));
    assert(values[15] == UINT16_C(0x700F));

    sources.b7_image = NULL;
    values[0] = UINT16_C(0xD00D);
    outcome = modbus_read_adapter_read(&sources, UINT16_C(7000), UINT16_C(1), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_NOT_AVAILABLE);
    assert(values[0] == UINT16_C(0xD00D));

    time.synchronization_facts_available = false;
    values[0] = UINT16_C(0xD00D);
    outcome = modbus_read_adapter_read(&sources, UINT16_C(2000), UINT16_C(1), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_NOT_AVAILABLE);
    assert(values[0] == UINT16_C(0xD00D));

    values[0] = UINT16_C(0xCAFE);
    outcome = modbus_read_adapter_read(&sources, UINT16_C(1019), UINT16_C(2), values);
    assert(outcome.access_result == MODBUS_ACCESS_ILLEGAL_ADDRESS);
    assert(values[0] == UINT16_C(0xCAFE));

    return 0;
}
