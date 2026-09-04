#ifndef TR2_DOMAIN_CONFIGURATION_VALIDATOR_H
#define TR2_DOMAIN_CONFIGURATION_VALIDATOR_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/domain/configuration/configuration.h"

typedef struct {
    bool storage_capacity_known;
    uint32_t usable_storage_capacity_mb;
} ConfigurationValidationEnvironment;

typedef enum {
    CONFIGURATION_VALIDATION_VALID = 0,
    CONFIGURATION_VALIDATION_INVALID,
    CONFIGURATION_VALIDATION_ENVIRONMENT_NOT_CHARACTERIZED
} ConfigurationValidationStatus;

typedef struct {
    ConfigurationValidationStatus status;
} ConfigurationValidationResult;

ConfigurationValidationResult configuration_validate(
    const PreparedConfiguration *prepared,
    const ConfigurationValidationEnvironment *environment,
    ValidatedConfiguration *out_validated);

#endif
