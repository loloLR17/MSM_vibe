#include <stddef.h>
#include <string.h>

#include "tr2/application/configuration_workflow.h"

static ConfigurationValidationResult validation_result(ConfigurationValidationStatus status)
{
    ConfigurationValidationResult result = { status };
    return result;
}

static bool validated_matches_current(
    const ConfigurationWorkflow *workflow,
    const PreparedConfiguration *current)
{
    return workflow->has_validated &&
           workflow->validated.generation == current->generation &&
           workflow->validated.config_id == current->config_id &&
           workflow->validated.supplied_crc == current->supplied_crc;
}

Tr2Result configuration_workflow_init(
    ConfigurationWorkflow *workflow,
    ConfigurationStagingService *staging,
    ConfigurationIntegrityPort integrity,
    ActivationCommitPort activation)
{
    if (workflow == NULL || staging == NULL || integrity.compute_prepared_crc == NULL ||
        activation.commit == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(workflow, 0, sizeof(*workflow));
    workflow->staging = staging;
    workflow->integrity = integrity;
    workflow->activation = activation;
    workflow->state = configuration_staging_has_prepared(staging)
                          ? CONFIGURATION_STATE_DRAFT
                          : CONFIGURATION_STATE_EMPTY;
    return TR2_OK;
}

void configuration_workflow_note_prepared_payload_modified(ConfigurationWorkflow *workflow)
{
    if (workflow == NULL) {
        return;
    }

    workflow->has_validated = false;
    memset(&workflow->validated, 0, sizeof(workflow->validated));
    workflow->state = configuration_staging_has_prepared(workflow->staging)
                          ? CONFIGURATION_STATE_DRAFT
                          : CONFIGURATION_STATE_EMPTY;
}

ConfigurationValidationResult configuration_workflow_validate(
    ConfigurationWorkflow *workflow,
    const ConfigurationValidationEnvironment *environment)
{
    PreparedConfiguration prepared = { 0 };
    ValidatedConfiguration validated = { 0 };
    ConfigurationValidationResult result;
    uint32_t computed_crc;

    if (workflow == NULL || environment == NULL) {
        return validation_result(CONFIGURATION_VALIDATION_INVALID);
    }

    if (!configuration_staging_snapshot(workflow->staging, &prepared)) {
        workflow->has_validated = false;
        memset(&workflow->validated, 0, sizeof(workflow->validated));
        workflow->state = CONFIGURATION_STATE_EMPTY;
        return validation_result(CONFIGURATION_VALIDATION_INVALID);
    }

    computed_crc = workflow->integrity.compute_prepared_crc(&prepared.payload);
    if (computed_crc != prepared.supplied_crc) {
        workflow->has_validated = false;
        memset(&workflow->validated, 0, sizeof(workflow->validated));
        configuration_staging_mark_validation_current(workflow->staging);
        workflow->state = CONFIGURATION_STATE_VALIDATION_ERROR;
        return validation_result(CONFIGURATION_VALIDATION_INVALID);
    }

    result = configuration_validate(&prepared, environment, &validated);
    if (result.status == CONFIGURATION_VALIDATION_VALID) {
        workflow->validated = validated;
        workflow->has_validated = true;
        configuration_staging_mark_validation_current(workflow->staging);
        workflow->state = CONFIGURATION_STATE_VALID;
    } else if (result.status == CONFIGURATION_VALIDATION_INVALID) {
        workflow->has_validated = false;
        memset(&workflow->validated, 0, sizeof(workflow->validated));
        configuration_staging_mark_validation_current(workflow->staging);
        workflow->state = CONFIGURATION_STATE_VALIDATION_ERROR;
    } else {
        workflow->has_validated = false;
        memset(&workflow->validated, 0, sizeof(workflow->validated));
        workflow->state = CONFIGURATION_STATE_DRAFT;
    }

    return result;
}

Tr2Result configuration_workflow_apply(ConfigurationWorkflow *workflow)
{
    PreparedConfiguration current = { 0 };
    ActiveConfigurationSnapshot committed = { 0 };
    Tr2Result result;

    if (workflow == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    if (workflow->state != CONFIGURATION_STATE_VALID ||
        !configuration_staging_is_validation_current(workflow->staging) ||
        !configuration_staging_snapshot(workflow->staging, &current) ||
        !validated_matches_current(workflow, &current)) {
        return TR2_ERROR_INVALID_STATE;
    }

    result = workflow->activation.commit(
        workflow->activation.context,
        &workflow->validated,
        &committed);
    if (result != TR2_OK) {
        workflow->state = CONFIGURATION_STATE_APPLICATION_ERROR;
        return result;
    }

    workflow->active = committed;
    workflow->has_active = true;
    workflow->state = CONFIGURATION_STATE_ACTIVE;
    return TR2_OK;
}

ConfigurationState configuration_workflow_state(const ConfigurationWorkflow *workflow)
{
    return workflow == NULL ? CONFIGURATION_STATE_EMPTY : workflow->state;
}

bool configuration_workflow_validated_snapshot(
    const ConfigurationWorkflow *workflow,
    ValidatedConfiguration *out_snapshot)
{
    PreparedConfiguration current = { 0 };

    if (workflow == NULL || out_snapshot == NULL || !workflow->has_validated ||
        !configuration_staging_is_validation_current(workflow->staging) ||
        !configuration_staging_snapshot(workflow->staging, &current) ||
        !validated_matches_current(workflow, &current)) {
        return false;
    }

    *out_snapshot = workflow->validated;
    return true;
}

bool configuration_workflow_active_snapshot(
    const ConfigurationWorkflow *workflow,
    ActiveConfigurationSnapshot *out_snapshot)
{
    if (workflow == NULL || out_snapshot == NULL || !workflow->has_active) {
        return false;
    }

    *out_snapshot = workflow->active;
    return true;
}
