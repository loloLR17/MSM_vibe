#ifndef TR2_APPLICATION_PRODUCTION_APPLICATION_H
#define TR2_APPLICATION_PRODUCTION_APPLICATION_H

#include "tr2/application/system_command_dispatcher.h"
#include "tr2/application/configuration_activation_adapter.h"

/* Owns service/interface descriptors in persistent RAM. Backend contexts, their
 * buffers, optional capabilities and workflow remain explicitly borrowed.
 * Zero-initialize; never copy/move after boot; no automatic format or retry. */
typedef struct {
    MonotonicClock monotonic;
    WallClock wall;
    ResetCauseProvider reset;
    TimeContinuityEvidenceProvider continuity;
    PersistentMedia media;
    VibrationSource vibration;
    CampaignDataStore campaign_data;
    ConfigurationValidationEnvironment environment;
    SystemRuntime runtime;
    SystemCommandDispatcher commands;
    ConfigurationStagingService staging;
    ConfigurationActivationAdapter activation;
    ConfigurationWorkflow configuration;
    bool boot_attempted;
    bool booted;
} ProductionApplication;

Tr2Result production_application_boot(ProductionApplication *application,
    const SystemRuntimeDependencies *dependencies, ConfigurationWorkflow *workflow);
/* Optional authority: metadata allocation remains explicitly supplied by board.
 * Initializes owned B4 staging/workflow against this runtime's real config store. */
Tr2Result production_application_enable_configuration(ProductionApplication *application,
    void *metadata_context, ConfigurationActivationMetadataAcquire acquire_metadata);
Tr2Result production_application_binding(ProductionApplication *application,
    SerialTransport *qualified_transport, uint8_t unit_id,
    const ModbusBlock0Image *identity, ModbusSystemServerBinding *binding);

#endif
