#ifndef TR2_APPLICATION_CONFIGURATION_WORKFLOW_H
#define TR2_APPLICATION_CONFIGURATION_WORKFLOW_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/configuration/configuration.h"
#include "tr2/domain/configuration/configuration_staging.h"
#include "tr2/domain/configuration/configuration_validator.h"

typedef enum {
    CONFIGURATION_STATE_EMPTY = 0,
    CONFIGURATION_STATE_DRAFT = 1,
    CONFIGURATION_STATE_VALID = 2,
    CONFIGURATION_STATE_ACTIVE = 4,
    CONFIGURATION_STATE_VALIDATION_ERROR = 5,
    CONFIGURATION_STATE_APPLICATION_ERROR = 6
} ConfigurationState;

typedef uint32_t (*ConfigurationPreparedCrcCompute)(const ConfigurationPayload *payload);

typedef struct {
    ConfigurationPreparedCrcCompute compute_prepared_crc;
} ConfigurationIntegrityPort;

typedef Tr2Result (*ActivationCommitFunction)(
    void *context,
    const ValidatedConfiguration *validated,
    ActiveConfigurationSnapshot *out_committed_snapshot);

typedef struct {
    void *context;
    ActivationCommitFunction commit;
} ActivationCommitPort;

typedef struct {
    ConfigurationStagingService *staging;
    ConfigurationIntegrityPort integrity;
    ActivationCommitPort activation;
    ConfigurationState state;
    bool has_validated;
    ValidatedConfiguration validated;
    bool has_active;
    ActiveConfigurationSnapshot active;
} ConfigurationWorkflow;

Tr2Result configuration_workflow_init(
    ConfigurationWorkflow *workflow,
    ConfigurationStagingService *staging,
    ConfigurationIntegrityPort integrity,
    ActivationCommitPort activation);

void configuration_workflow_note_prepared_payload_modified(ConfigurationWorkflow *workflow);

ConfigurationValidationResult configuration_workflow_validate(
    ConfigurationWorkflow *workflow,
    const ConfigurationValidationEnvironment *environment);

Tr2Result configuration_workflow_apply(ConfigurationWorkflow *workflow);

ConfigurationState configuration_workflow_state(const ConfigurationWorkflow *workflow);
bool configuration_workflow_validated_snapshot(
    const ConfigurationWorkflow *workflow,
    ValidatedConfiguration *out_snapshot);
bool configuration_workflow_active_snapshot(
    const ConfigurationWorkflow *workflow,
    ActiveConfigurationSnapshot *out_snapshot);

#endif
