#ifndef TR2_APPLICATION_CONFIGURATION_ACTIVATION_ADAPTER_H
#define TR2_APPLICATION_CONFIGURATION_ACTIVATION_ADAPTER_H

#include <stdint.h>

#include "tr2/application/configuration_service.h"
#include "tr2/application/configuration_workflow.h"
#include "tr2/common/result.h"

typedef struct {
    uint32_t persistent_generation;
    uint32_t revision_counter;
} ConfigurationActivationMetadata;

typedef Tr2Result (*ConfigurationActivationMetadataAcquire)(
    void *context,
    const ValidatedConfiguration *validated,
    ConfigurationActivationMetadata *out_metadata);

typedef struct {
    ConfigurationService *service;
    void *metadata_context;
    ConfigurationActivationMetadataAcquire acquire_metadata;
} ConfigurationActivationAdapter;

Tr2Result configuration_activation_adapter_init(
    ConfigurationActivationAdapter *adapter,
    ConfigurationService *service,
    void *metadata_context,
    ConfigurationActivationMetadataAcquire acquire_metadata);

ActivationCommitPort configuration_activation_adapter_port(
    ConfigurationActivationAdapter *adapter);

#endif
