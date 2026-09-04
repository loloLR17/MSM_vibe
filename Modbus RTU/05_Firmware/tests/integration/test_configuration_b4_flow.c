#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/configuration_workflow.h"
#include "tr2/modbus/b4_configuration_codec.h"
#include "tr2/modbus/projection.h"
#include "tr2/modbus/write_adapter.h"

typedef struct {
    bool fail;
    uint32_t calls;
    uint32_t revision;
    uint32_t generation;
} TestCommitContext;

static Tr2Result test_commit(
    void *context,
    const ValidatedConfiguration *validated,
    ActiveConfigurationSnapshot *out_committed_snapshot)
{
    TestCommitContext *commit = (TestCommitContext *)context;

    assert(commit != NULL);
    assert(validated != NULL);
    assert(out_committed_snapshot != NULL);
    commit->calls += 1u;

    if (commit->fail) {
        return TR2_ERROR_STORAGE;
    }

    memset(out_committed_snapshot, 0, sizeof(*out_committed_snapshot));
    out_committed_snapshot->generation = commit->generation;
    out_committed_snapshot->config_id = validated->config_id;
    out_committed_snapshot->revision_counter = commit->revision;
    out_committed_snapshot->payload = validated->payload;
    return TR2_OK;
}

static ConfigurationPayload valid_payload(void)
{
    ConfigurationPayload payload = { 0 };

    payload.sampling_frequency_hz = UINT16_C(26667);
    payload.axes_enable_mask = UINT16_C(7);
    payload.full_scale_code = UINT16_C(2);
    payload.acquisition_mode = UINT16_C(1);
    payload.window_size_samples = UINT16_C(32768);
    payload.indicator_period_ms = UINT16_C(5000);
    payload.campaign_duration_s = UINT32_C(3600);
    payload.storage_mode = UINT16_C(1);
    payload.storage_limit_mb = UINT32_C(512);
    payload.supervision_enable_mask = UINT16_C(1);
    payload.rms_warn_threshold_mg = UINT16_C(100);
    payload.rms_alarm_threshold_mg = UINT16_C(200);
    payload.peak_warn_threshold_mg = UINT16_C(300);
    payload.peak_alarm_threshold_mg = UINT16_C(400);
    payload.threshold_hysteresis_mg = UINT16_C(20);
    payload.alarm_hold_time_ms = UINT16_C(500);
    payload.campaign_context_id = UINT32_C(1);
    payload.mission_id = UINT32_C(2);
    memcpy(payload.campaign_label, "CAMPAIGN-A", 10u);
    memcpy(payload.mission_label, "MISSION-A", 9u);
    payload.operating_mode_code = UINT16_C(1);
    payload.navigation_zone_code = UINT16_C(2);
    payload.load_state_code = UINT16_C(3);
    payload.sea_state_code = UINT16_C(1);
    return payload;
}

static void assert_write_ok(ModbusWriteOutcome outcome)
{
    assert(outcome.access_result == MODBUS_ACCESS_OK);
    assert(outcome.operation_result == TR2_OK);
}

static void write_prepared_configuration(
    ConfigurationStagingService *staging,
    ConfigurationWorkflow *workflow,
    uint32_t config_id,
    const ConfigurationPayload *payload)
{
    uint16_t registers[TR2_B4_PREPARED_REGISTER_COUNT];
    uint16_t words[2];
    uint32_t crc;

    tr2_b4_serialize_prepared_payload(payload, registers);
    crc = tr2_b4_prepared_payload_crc(payload);

    words[0] = (uint16_t)(config_id >> 16u);
    words[1] = (uint16_t)(config_id & UINT32_C(0xFFFF));
    assert_write_ok(modbus_write_adapter_write_b4(staging, UINT16_C(4002), words, UINT16_C(2)));

    words[0] = (uint16_t)(crc >> 16u);
    words[1] = (uint16_t)(crc & UINT32_C(0xFFFF));
    assert_write_ok(modbus_write_adapter_write_b4(staging, UINT16_C(4008), words, UINT16_C(2)));

    assert_write_ok(modbus_write_adapter_write_b4(staging, UINT16_C(4016), &registers[0], UINT16_C(1)));
    assert_write_ok(modbus_write_adapter_write_b4(staging, UINT16_C(4018), &registers[2], UINT16_C(10)));
    assert_write_ok(modbus_write_adapter_write_b4(staging, UINT16_C(4040), &registers[24], UINT16_C(7)));
    assert_write_ok(modbus_write_adapter_write_b4(staging, UINT16_C(4056), &registers[40], UINT16_C(40)));

    configuration_workflow_note_prepared_payload_modified(workflow);
}

static ModbusBlock4Image project_current(
    const ConfigurationStagingService *staging,
    const ConfigurationWorkflow *workflow,
    uint16_t state)
{
    PreparedConfiguration prepared = { 0 };
    ActiveConfigurationSnapshot active = { 0 };
    ModbusBlock4ProjectionSource source = { 0 };
    ModbusBlock4Image image;

    source.config_structure_version = UINT16_C(0);
    source.config_capabilities_mask = UINT16_C(0);
    source.config_state = state;
    source.config_error_code = UINT16_C(0);
    source.prepared = configuration_staging_snapshot(staging, &prepared) ? &prepared : NULL;
    source.active = configuration_workflow_active_snapshot(workflow, &active) ? &active : NULL;

    assert(modbus_project_b4(&source, &image) == TR2_OK);
    return image;
}

int main(void)
{
    ConfigurationStagingService staging;
    ConfigurationWorkflow workflow;
    ConfigurationIntegrityPort integrity = { tr2_b4_prepared_payload_crc };
    TestCommitContext commit_context = { false, 0u, UINT32_C(17), UINT32_C(1001) };
    ActivationCommitPort activation = { &commit_context, test_commit };
    ConfigurationValidationEnvironment environment = { true, UINT32_C(1024) };
    ConfigurationPayload payload = valid_payload();
    ConfigurationValidationResult validation;
    ModbusBlock4Image image;
    ActiveConfigurationSnapshot active_before;
    PreparedConfiguration prepared_before;
    ModbusWriteOutcome write_outcome;
    uint16_t invalid_frequency = UINT16_C(0xFFFF);
    uint16_t attempted_ro = UINT16_C(0x1234);
    uint32_t prepared_crc = tr2_b4_prepared_payload_crc(&payload);
    uint32_t active_crc;

    configuration_staging_init(&staging);
    assert(configuration_workflow_init(&workflow, &staging, integrity, activation) == TR2_OK);

    image = project_current(&staging, &workflow, CONFIGURATION_STATE_EMPTY);
    assert(image.registers[4] == UINT16_C(0));
    assert(image.registers[5] == UINT16_C(0));
    assert(image.registers[10] == UINT16_C(0x177C));
    assert(image.registers[11] == UINT16_C(0x92D9));
    assert(image.registers[12] == UINT16_C(0));
    assert(image.registers[13] == UINT16_C(0));

    write_prepared_configuration(&staging, &workflow, UINT32_C(42), &payload);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_DRAFT);

    image = project_current(&staging, &workflow, CONFIGURATION_STATE_DRAFT);
    assert(image.registers[2] == UINT16_C(0));
    assert(image.registers[3] == UINT16_C(42));
    assert(image.registers[8] == (uint16_t)(prepared_crc >> 16u));
    assert(image.registers[9] == (uint16_t)(prepared_crc & UINT32_C(0xFFFF)));
    assert(image.registers[16] == UINT16_C(26667));
    assert(image.registers[18] == UINT16_C(7));
    assert(image.registers[60] == UINT16_C(0x4341));
    assert(image.registers[61] == UINT16_C(0x4D50));
    assert(image.registers[4] == UINT16_C(0));
    assert(image.registers[10] == UINT16_C(0x177C));
    assert(image.registers[11] == UINT16_C(0x92D9));

    validation = configuration_workflow_validate(&workflow, &environment);
    assert(validation.status == CONFIGURATION_VALIDATION_VALID);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_VALID);
    assert(configuration_workflow_apply(&workflow) == TR2_OK);
    assert(commit_context.calls == UINT32_C(1));
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_ACTIVE);

    image = project_current(&staging, &workflow, CONFIGURATION_STATE_ACTIVE);
    active_crc = tr2_b4_active_payload_crc(&payload);
    assert(image.registers[4] == UINT16_C(0));
    assert(image.registers[5] == UINT16_C(42));
    assert(image.registers[10] == (uint16_t)(active_crc >> 16u));
    assert(image.registers[11] == (uint16_t)(active_crc & UINT32_C(0xFFFF)));
    assert(image.registers[12] == UINT16_C(0));
    assert(image.registers[13] == UINT16_C(17));
    assert(image.registers[100] == UINT16_C(26667));
    assert(image.registers[101] == UINT16_C(7));
    assert(image.registers[132] == UINT16_C(0x4341));
    assert(image.registers[133] == UINT16_C(0x4D50));

    assert(configuration_workflow_active_snapshot(&workflow, &active_before));
    assert(configuration_staging_snapshot(&staging, &prepared_before));

    write_outcome = modbus_write_adapter_write_b4(
        &staging,
        UINT16_C(4100),
        &attempted_ro,
        UINT16_C(1));
    assert(write_outcome.access_result == MODBUS_ACCESS_READ_ONLY);
    assert(write_outcome.operation_result == TR2_OK);
    {
        PreparedConfiguration prepared_after;
        assert(configuration_staging_snapshot(&staging, &prepared_after));
        assert(prepared_after.generation == prepared_before.generation);
        assert(memcmp(&prepared_after.payload, &prepared_before.payload, sizeof(prepared_after.payload)) == 0);
    }

    assert_write_ok(modbus_write_adapter_write_b4(
        &staging,
        UINT16_C(4016),
        &invalid_frequency,
        UINT16_C(1)));
    configuration_workflow_note_prepared_payload_modified(&workflow);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_DRAFT);

    image = project_current(&staging, &workflow, CONFIGURATION_STATE_DRAFT);
    assert(image.registers[16] == UINT16_C(0xFFFF));
    assert(image.registers[100] == UINT16_C(26667));

    validation = configuration_workflow_validate(&workflow, &environment);
    assert(validation.status == CONFIGURATION_VALIDATION_INVALID);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_VALIDATION_ERROR);
    assert(configuration_workflow_apply(&workflow) == TR2_ERROR_INVALID_STATE);
    assert(commit_context.calls == UINT32_C(1));
    {
        ActiveConfigurationSnapshot active_after;
        assert(configuration_workflow_active_snapshot(&workflow, &active_after));
        assert(active_after.generation == active_before.generation);
        assert(active_after.config_id == active_before.config_id);
        assert(active_after.revision_counter == active_before.revision_counter);
        assert(active_after.payload.sampling_frequency_hz == UINT16_C(26667));
    }

    image = project_current(&staging, &workflow, CONFIGURATION_STATE_VALIDATION_ERROR);
    assert(image.registers[16] == UINT16_C(0xFFFF));
    assert(image.registers[100] == UINT16_C(26667));
    assert(image.registers[5] == UINT16_C(42));

    return 0;
}
