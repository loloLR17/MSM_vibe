#include "tr2/application/production_application.h"
#include "tr2/modbus/b4_configuration_codec.h"

Tr2Result production_application_boot(ProductionApplication *application,
    const SystemRuntimeDependencies *dependencies, ConfigurationWorkflow *workflow)
{
    SystemRuntimeDependencies owned;
    Tr2Result result;
    if (application == NULL || dependencies == NULL ||
        dependencies->monotonic_clock == NULL || dependencies->wall_clock == NULL ||
        dependencies->reset_cause_provider == NULL ||
        dependencies->time_continuity_evidence_provider == NULL ||
        dependencies->persistent_media == NULL || dependencies->vibration_source == NULL ||
        dependencies->campaign_data_store == NULL ||
        dependencies->configuration_validation_environment == NULL)
        return TR2_ERROR_INVALID_ARGUMENT;
    if (application->boot_attempted) return TR2_ERROR_INVALID_STATE;
    application->boot_attempted = true;
    application->monotonic = *dependencies->monotonic_clock;
    application->wall = *dependencies->wall_clock;
    application->reset = *dependencies->reset_cause_provider;
    application->continuity = *dependencies->time_continuity_evidence_provider;
    application->media = *dependencies->persistent_media;
    application->vibration = *dependencies->vibration_source;
    application->campaign_data = *dependencies->campaign_data_store;
    application->environment = *dependencies->configuration_validation_environment;
    owned = *dependencies;
    owned.monotonic_clock = &application->monotonic;
    owned.wall_clock = &application->wall;
    owned.reset_cause_provider = &application->reset;
    owned.time_continuity_evidence_provider = &application->continuity;
    owned.persistent_media = &application->media;
    owned.vibration_source = &application->vibration;
    owned.campaign_data_store = &application->campaign_data;
    owned.configuration_validation_environment = &application->environment;
    result = system_runtime_init(&application->runtime, &owned);
    if (result == TR2_OK) result = system_runtime_boot(&application->runtime);
    if (result == TR2_OK) result = system_command_dispatcher_init(&application->commands,
                                                               &application->runtime, workflow);
    application->booted = result == TR2_OK;
    return result;
}

Tr2Result production_application_binding(ProductionApplication *application,
    SerialTransport *qualified_transport, uint8_t unit_id,
    const ModbusBlock0Image *identity, ModbusSystemServerBinding *binding)
{
    if (application == NULL || binding == NULL ||
        !serial_transport_is_valid(qualified_transport) ||
        !modbus_rtu_server_runtime_unit_id_is_valid(unit_id)) return TR2_ERROR_INVALID_ARGUMENT;
    if (!application->booted || !system_runtime_is_ready_for_modbus(&application->runtime))
        return TR2_ERROR_NOT_AVAILABLE;
    *binding = (ModbusSystemServerBinding){0};
    binding->transport = qualified_transport;
    binding->unit_id = unit_id;
    binding->identity = identity;
    system_command_dispatcher_bind(&application->commands, binding);
    return TR2_OK;
}

Tr2Result production_application_enable_configuration(ProductionApplication *application,
    void *metadata_context, ConfigurationActivationMetadataAcquire acquire_metadata)
{
    Tr2Result result;
    if (application == NULL || acquire_metadata == NULL) return TR2_ERROR_INVALID_ARGUMENT;
    if (!application->booted || !system_runtime_is_ready_for_modbus(&application->runtime) ||
        application->commands.configuration_workflow != NULL) return TR2_ERROR_INVALID_STATE;
    result = configuration_activation_adapter_init(&application->activation,
        &application->runtime.configuration_service, metadata_context, acquire_metadata);
    if (result != TR2_OK) return result;
    configuration_staging_init(&application->staging);
    result = configuration_workflow_init(&application->configuration, &application->staging,
        (ConfigurationIntegrityPort){tr2_b4_prepared_payload_crc},
        configuration_activation_adapter_port(&application->activation));
    if (result == TR2_OK) application->commands.configuration_workflow = &application->configuration;
    return result;
}
