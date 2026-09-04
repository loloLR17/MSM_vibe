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
    const SystemStateSnapshot system_state = {
        .generation = UINT32_C(12),
        .system_status = UINT16_C(2),
        .system_flags = UINT16_C(0x0015),
        .fault_flags = UINT16_C(0x0002),
        .warning_flags = UINT16_C(0x0004),
        .uptime_s = UINT32_C(0x01020304),
        .last_reset_cause = UINT16_C(3),
        .internal_temp_dC = INT16_C(-50),
        .cpu_load_percent = UINT16_C(37),
        .memory_usage_percent = UINT16_C(48),
        .storage_status = UINT16_C(1),
        .storage_usage_percent = UINT16_C(52),
        .acquisition_state = UINT16_C(1),
        .active_campaign_id = UINT32_C(0xA1B2C3D4),
        .error_code = UINT16_C(9),
        .warning_code = UINT16_C(10)
    };
    TimeSnapshot time = {
        .generation = UINT32_C(13),
        .synchronization_facts_available = true,
        .time_status = UINT16_C(3),
        .time_flags = UINT16_C(0x008B),
        .current_time_available = true,
        .current_time = UINT32_C(0x10203040),
        .last_sync_time = UINT32_C(0x01020304),
        .time_since_sync_s = UINT32_C(0x00010002),
        .prepared_time_available = true,
        .prepared_time = UINT32_C(0x55667788),
        .prepared_time_status = UINT16_C(1),
        .time_accuracy_ms = UINT16_C(25),
        .drift_ppm = INT16_C(-20),
        .sync_source = UINT16_C(1)
    };
    ModbusReadSources sources = { &identity, &system_state, &time };
    ModbusReadOutcome outcome;
    uint16_t values[21] = { 0u };

    outcome = modbus_read_adapter_read(&sources, UINT16_C(0), UINT16_C(21), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(values[0] == UINT16_C(0x1234));
    assert(values[1] == UINT16_C(0x5678));
    assert(values[7] == UINT16_C(0x000F));
    assert(values[8] == UINT16_C(0x5345));
    assert(values[20] == 0u);

    values[0] = UINT16_C(0xAAAA);
    values[1] = UINT16_C(0xBBBB);
    outcome = modbus_read_adapter_read(&sources, UINT16_C(1004), UINT16_C(2), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(values[0] == UINT16_C(0x0102));
    assert(values[1] == UINT16_C(0x0304));

    outcome = modbus_read_adapter_read(&sources, UINT16_C(2000), UINT16_C(16), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
    assert(values[0] == UINT16_C(3));
    assert(values[1] == UINT16_C(0x008B));
    assert(values[2] == UINT16_C(0x1020));
    assert(values[3] == UINT16_C(0x3040));
    assert(values[8] == UINT16_C(0x5566));
    assert(values[9] == UINT16_C(0x7788));
    assert(values[12] == UINT16_C(0xFFEC));
    assert(values[14] == 0u);
    assert(values[15] == 0u);

    values[0] = UINT16_C(0xCAFE);
    outcome = modbus_read_adapter_read(&sources, UINT16_C(21), UINT16_C(1), values);
    assert(outcome.access_result == MODBUS_ACCESS_ILLEGAL_ADDRESS);
    assert(outcome.operation_result == TR2_OK);
    assert(values[0] == UINT16_C(0xCAFE));

    values[0] = UINT16_C(0xBEEF);
    outcome = modbus_read_adapter_read(&sources, UINT16_C(1019), UINT16_C(2), values);
    assert(outcome.access_result == MODBUS_ACCESS_ILLEGAL_ADDRESS);
    assert(outcome.operation_result == TR2_OK);
    assert(values[0] == UINT16_C(0xBEEF));

    time.synchronization_facts_available = false;
    values[0] = UINT16_C(0xD00D);
    outcome = modbus_read_adapter_read(&sources, UINT16_C(2000), UINT16_C(1), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_NOT_AVAILABLE);
    assert(values[0] == UINT16_C(0xD00D));

    sources.system_state = NULL;
    values[0] = UINT16_C(0xFACE);
    outcome = modbus_read_adapter_read(&sources, UINT16_C(1000), UINT16_C(1), values);
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_ERROR_NOT_AVAILABLE);
    assert(values[0] == UINT16_C(0xFACE));

    return 0;
}
