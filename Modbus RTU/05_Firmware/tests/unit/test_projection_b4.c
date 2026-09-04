#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/modbus/b4_configuration_codec.h"
#include "tr2/modbus/projection.h"

static PreparedConfiguration make_prepared(void)
{
    PreparedConfiguration prepared = { 0 };

    prepared.generation = UINT32_C(7);
    prepared.config_id = UINT32_C(0x11223344);
    prepared.supplied_crc = UINT32_C(0xA1B2C3D4);
    prepared.payload.sampling_frequency_hz = UINT16_C(26667);
    prepared.payload.axes_enable_mask = UINT16_C(7);
    prepared.payload.full_scale_code = UINT16_C(2);
    prepared.payload.acquisition_mode = UINT16_C(1);
    prepared.payload.window_size_samples = UINT16_C(32768);
    prepared.payload.indicator_period_ms = UINT16_C(5000);
    prepared.payload.campaign_duration_s = UINT32_C(3600);
    prepared.payload.storage_mode = UINT16_C(1);
    prepared.payload.storage_limit_mb = UINT32_C(512);
    prepared.payload.campaign_context_id = UINT32_C(1);
    prepared.payload.mission_id = UINT32_C(2);
    prepared.payload.campaign_label[0] = 'A';
    prepared.payload.campaign_label[1] = 'B';
    prepared.payload.mission_label[0] = 'M';
    prepared.payload.mission_label[1] = 'N';
    prepared.payload.operating_mode_code = UINT16_C(1);
    prepared.payload.navigation_zone_code = UINT16_C(2);
    prepared.payload.load_state_code = UINT16_C(3);
    prepared.payload.sea_state_code = UINT16_C(4);

    return prepared;
}

static ActiveConfigurationSnapshot make_active(void)
{
    ActiveConfigurationSnapshot active = { 0 };

    active.generation = UINT32_C(11);
    active.config_id = UINT32_C(0x55667788);
    active.revision_counter = UINT32_C(0x01020304);
    active.payload = make_prepared().payload;
    active.payload.campaign_label[0] = 'X';
    active.payload.campaign_label[1] = 'Y';

    return active;
}

static void test_no_prepared_no_active_is_explicitly_neutral(void)
{
    ModbusBlock4ProjectionSource source = {
        UINT16_C(0x1234),
        UINT16_C(0x5678),
        UINT16_C(0),
        UINT16_C(0),
        NULL,
        NULL
    };
    ModbusBlock4Image image;
    size_t index;

    assert(modbus_project_b4(&source, &image) == TR2_OK);
    assert(image.registers[0] == UINT16_C(0x1234));
    assert(image.registers[1] == UINT16_C(0x5678));
    assert(image.registers[2] == 0u);
    assert(image.registers[3] == 0u);
    assert(image.registers[4] == 0u);
    assert(image.registers[5] == 0u);
    assert(image.registers[6] == 0u);
    assert(image.registers[7] == 0u);
    assert(image.registers[8] == 0u);
    assert(image.registers[9] == 0u);
    assert(image.registers[10] == UINT16_C(0x177C));
    assert(image.registers[11] == UINT16_C(0x92D9));
    assert(image.registers[12] == 0u);
    assert(image.registers[13] == 0u);
    assert(image.registers[14] == 0u);
    assert(image.registers[15] == 0u);

    for (index = 16u; index < 100u; ++index) {
        assert(image.registers[index] == 0u);
    }
    for (index = 100u; index < TR2_B4_REGISTER_COUNT; ++index) {
        assert(image.registers[index] == 0u);
    }
}

static void test_prepared_projection_uses_staging_values(void)
{
    const PreparedConfiguration prepared = make_prepared();
    ModbusBlock4ProjectionSource source = {
        UINT16_C(9),
        UINT16_C(0x00F0),
        UINT16_C(1),
        UINT16_C(0xBEEF),
        &prepared,
        NULL
    };
    ModbusBlock4Image image;

    assert(modbus_project_b4(&source, &image) == TR2_OK);
    assert(image.registers[2] == UINT16_C(0x1122));
    assert(image.registers[3] == UINT16_C(0x3344));
    assert(image.registers[6] == UINT16_C(1));
    assert(image.registers[7] == UINT16_C(0xBEEF));
    assert(image.registers[8] == UINT16_C(0xA1B2));
    assert(image.registers[9] == UINT16_C(0xC3D4));
    assert(image.registers[16] == UINT16_C(26667));
    assert(image.registers[17] == 0u);
    assert(image.registers[18] == UINT16_C(7));
    assert(image.registers[60] == UINT16_C(0x4142));
    assert(image.registers[76] == UINT16_C(0x4D4E));
    assert(image.registers[96] == 0u);
    assert(image.registers[99] == 0u);
    assert(image.registers[4] == 0u);
    assert(image.registers[5] == 0u);
    assert(image.registers[10] == UINT16_C(0x177C));
    assert(image.registers[11] == UINT16_C(0x92D9));
}

static void test_active_projection_uses_active_snapshot_and_crc(void)
{
    const PreparedConfiguration prepared = make_prepared();
    const ActiveConfigurationSnapshot active = make_active();
    const uint32_t expected_crc = tr2_b4_active_payload_crc(&active.payload);
    ModbusBlock4ProjectionSource source = {
        UINT16_C(1),
        UINT16_C(0),
        UINT16_C(4),
        UINT16_C(0),
        &prepared,
        &active
    };
    ModbusBlock4Image image;

    assert(modbus_project_b4(&source, &image) == TR2_OK);
    assert(image.registers[4] == UINT16_C(0x5566));
    assert(image.registers[5] == UINT16_C(0x7788));
    assert(image.registers[10] == (uint16_t)(expected_crc >> 16));
    assert(image.registers[11] == (uint16_t)(expected_crc & UINT32_C(0xFFFF)));
    assert(image.registers[12] == UINT16_C(0x0102));
    assert(image.registers[13] == UINT16_C(0x0304));
    assert(image.registers[100] == UINT16_C(26667));
    assert(image.registers[111] == 0u);
    assert(image.registers[115] == 0u);
    assert(image.registers[132] == UINT16_C(0x5859));
    assert(image.registers[168] == 0u);
    assert(image.registers[175] == 0u);
}

static void test_reserved_state_is_never_emitted(void)
{
    ModbusBlock4ProjectionSource source = { 0 };
    ModbusBlock4Image image;

    memset(&image, 0xA5, sizeof(image));
    source.config_state = UINT16_C(3);
    assert(modbus_project_b4(&source, &image) == TR2_ERROR_INVALID_STATE);
    assert(image.registers[0] == UINT16_C(0xA5A5));

    source.config_state = UINT16_C(7);
    assert(modbus_project_b4(&source, &image) == TR2_ERROR_INVALID_STATE);
    assert(image.registers[0] == UINT16_C(0xA5A5));
}

static void test_null_arguments_are_rejected(void)
{
    ModbusBlock4ProjectionSource source = { 0 };
    ModbusBlock4Image image;

    assert(modbus_project_b4(NULL, &image) == TR2_ERROR_INVALID_ARGUMENT);
    assert(modbus_project_b4(&source, NULL) == TR2_ERROR_INVALID_ARGUMENT);
}

int main(void)
{
    test_no_prepared_no_active_is_explicitly_neutral();
    test_prepared_projection_uses_staging_values();
    test_active_projection_uses_active_snapshot_and_crc();
    test_reserved_state_is_never_emitted();
    test_null_arguments_are_rejected();
    return 0;
}
