#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/configuration_workflow.h"
#include "tr2/modbus/b4_configuration_codec.h"

typedef struct {
    bool fail;
    uint32_t calls;
    uint32_t revision_to_return;
    uint32_t generation_to_return;
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
    out_committed_snapshot->generation = commit->generation_to_return;
    out_committed_snapshot->config_id = validated->config_id;
    out_committed_snapshot->revision_counter = commit->revision_to_return;
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
    payload.operating_mode_code = UINT16_C(1);
    payload.navigation_zone_code = UINT16_C(2);
    payload.load_state_code = UINT16_C(3);
    payload.sea_state_code = UINT16_C(1);
    return payload;
}

static void prepare(
    ConfigurationStagingService *staging,
    ConfigurationWorkflow *workflow,
    uint32_t config_id,
    const ConfigurationPayload *payload)
{
    configuration_staging_set_config_id(staging, config_id);
    configuration_staging_replace_payload(staging, payload);
    configuration_staging_set_supplied_crc(
        staging,
        tr2_b4_prepared_payload_crc(payload));
    configuration_workflow_note_prepared_payload_modified(workflow);
}

int main(void)
{
    ConfigurationStagingService staging;
    ConfigurationWorkflow workflow;
    ConfigurationIntegrityPort integrity = { tr2_b4_prepared_payload_crc };
    TestCommitContext commit_context = { false, 0u, UINT32_C(77), UINT32_C(900) };
    ActivationCommitPort activation = { &commit_context, test_commit };
    ConfigurationValidationEnvironment environment = { true, UINT32_C(1024) };
    ConfigurationValidationEnvironment unknown_environment = { false, 0u };
    ConfigurationPayload payload = valid_payload();
    ConfigurationValidationResult validation;
    ValidatedConfiguration validated;
    ActiveConfigurationSnapshot active;
    uint32_t previous_active_generation;

    configuration_staging_init(&staging);
    assert(configuration_workflow_init(&workflow, &staging, integrity, activation) == TR2_OK);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_EMPTY);
    assert(!configuration_workflow_validated_snapshot(&workflow, &validated));
    assert(!configuration_workflow_active_snapshot(&workflow, &active));
    assert(configuration_workflow_apply(&workflow) == TR2_ERROR_INVALID_STATE);
    assert(commit_context.calls == 0u);

    validation = configuration_workflow_validate(&workflow, &environment);
    assert(validation.status == CONFIGURATION_VALIDATION_INVALID);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_EMPTY);

    prepare(&staging, &workflow, UINT32_C(42), &payload);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_DRAFT);
    assert(!configuration_staging_is_validation_current(&staging));

    validation = configuration_workflow_validate(&workflow, &environment);
    assert(validation.status == CONFIGURATION_VALIDATION_VALID);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_VALID);
    assert(configuration_staging_is_validation_current(&staging));
    assert(configuration_workflow_validated_snapshot(&workflow, &validated));
    assert(validated.config_id == UINT32_C(42));
    assert(validated.supplied_crc == UINT32_C(0x5207CCFC));

    assert(configuration_workflow_apply(&workflow) == TR2_OK);
    assert(commit_context.calls == 1u);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_ACTIVE);
    assert(configuration_workflow_active_snapshot(&workflow, &active));
    assert(active.config_id == UINT32_C(42));
    assert(active.revision_counter == UINT32_C(77));
    assert(active.generation == UINT32_C(900));
    assert(active.payload.storage_limit_mb == UINT32_C(512));
    previous_active_generation = active.generation;

    payload.storage_limit_mb = UINT32_C(2048);
    configuration_staging_replace_payload(&staging, &payload);
    configuration_staging_set_supplied_crc(&staging, tr2_b4_prepared_payload_crc(&payload));
    configuration_workflow_note_prepared_payload_modified(&workflow);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_DRAFT);
    assert(!configuration_workflow_validated_snapshot(&workflow, &validated));
    assert(configuration_workflow_active_snapshot(&workflow, &active));
    assert(active.generation == previous_active_generation);

    validation = configuration_workflow_validate(&workflow, &environment);
    assert(validation.status == CONFIGURATION_VALIDATION_INVALID);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_VALIDATION_ERROR);
    assert(configuration_staging_is_validation_current(&staging));
    assert(configuration_workflow_active_snapshot(&workflow, &active));
    assert(active.generation == previous_active_generation);

    payload = valid_payload();
    configuration_staging_replace_payload(&staging, &payload);
    configuration_staging_set_supplied_crc(&staging, tr2_b4_prepared_payload_crc(&payload));
    configuration_workflow_note_prepared_payload_modified(&workflow);
    validation = configuration_workflow_validate(&workflow, &unknown_environment);
    assert(validation.status == CONFIGURATION_VALIDATION_ENVIRONMENT_NOT_CHARACTERIZED);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_DRAFT);
    assert(!configuration_staging_is_validation_current(&staging));

    validation = configuration_workflow_validate(&workflow, &environment);
    assert(validation.status == CONFIGURATION_VALIDATION_VALID);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_VALID);

    configuration_staging_set_supplied_crc(&staging, UINT32_C(0));
    assert(!configuration_staging_is_validation_current(&staging));
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_VALID);
    assert(!configuration_workflow_validated_snapshot(&workflow, &validated));
    assert(configuration_workflow_apply(&workflow) == TR2_ERROR_INVALID_STATE);
    assert(commit_context.calls == 1u);
    assert(configuration_workflow_active_snapshot(&workflow, &active));
    assert(active.generation == previous_active_generation);

    configuration_staging_set_supplied_crc(&staging, tr2_b4_prepared_payload_crc(&payload));
    validation = configuration_workflow_validate(&workflow, &environment);
    assert(validation.status == CONFIGURATION_VALIDATION_VALID);

    commit_context.fail = true;
    assert(configuration_workflow_apply(&workflow) == TR2_ERROR_STORAGE);
    assert(commit_context.calls == 2u);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_APPLICATION_ERROR);
    assert(configuration_workflow_active_snapshot(&workflow, &active));
    assert(active.generation == previous_active_generation);

    payload.indicator_period_ms = UINT16_C(10000);
    configuration_staging_replace_payload(&staging, &payload);
    configuration_staging_set_supplied_crc(&staging, tr2_b4_prepared_payload_crc(&payload));
    configuration_workflow_note_prepared_payload_modified(&workflow);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_DRAFT);

    configuration_staging_set_supplied_crc(&staging, UINT32_C(0xDEADBEEF));
    validation = configuration_workflow_validate(&workflow, &environment);
    assert(validation.status == CONFIGURATION_VALIDATION_INVALID);
    assert(configuration_workflow_state(&workflow) == CONFIGURATION_STATE_VALIDATION_ERROR);
    assert(configuration_staging_is_validation_current(&staging));
    assert(!configuration_workflow_validated_snapshot(&workflow, &validated));
    assert(configuration_workflow_active_snapshot(&workflow, &active));
    assert(active.generation == previous_active_generation);

    return 0;
}
