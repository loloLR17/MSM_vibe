#include "tr2/application/configuration_activation_adapter.h"

#include <string.h>

static Tr2Result activation_commit(void *context,
                                   const ValidatedConfiguration *validated,
                                   ActiveConfigurationSnapshot *out_committed_snapshot)
{
    ConfigurationActivationAdapter *adapter = (ConfigurationActivationAdapter *)context;
    ConfigurationActivationMetadata metadata;
    ValidatedConfiguration persistent_candidate;
    Tr2Result result;

    if (adapter == NULL || validated == NULL || out_committed_snapshot == NULL ||
        adapter->service == NULL || adapter->acquire_metadata == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = adapter->acquire_metadata(adapter->metadata_context,
                                       validated,
                                       &metadata);
    if (result != TR2_OK) {
        return result;
    }

    persistent_candidate = *validated;
    persistent_candidate.generation = metadata.persistent_generation;

    return configuration_service_commit_validated(adapter->service,
                                                  &persistent_candidate,
                                                  metadata.revision_counter,
                                                  out_committed_snapshot);
}

Tr2Result configuration_activation_adapter_init(
    ConfigurationActivationAdapter *adapter,
    ConfigurationService *service,
    void *metadata_context,
    ConfigurationActivationMetadataAcquire acquire_metadata)
{
    if (adapter == NULL || service == NULL ||
        !configuration_service_is_initialized(service) ||
        acquire_metadata == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(adapter, 0, sizeof(*adapter));
    adapter->service = service;
    adapter->metadata_context = metadata_context;
    adapter->acquire_metadata = acquire_metadata;
    return TR2_OK;
}

ActivationCommitPort configuration_activation_adapter_port(
    ConfigurationActivationAdapter *adapter)
{
    ActivationCommitPort port = { 0 };

    if (adapter != NULL && adapter->service != NULL &&
        adapter->acquire_metadata != NULL) {
        port.context = adapter;
        port.commit = activation_commit;
    }

    return port;
}
