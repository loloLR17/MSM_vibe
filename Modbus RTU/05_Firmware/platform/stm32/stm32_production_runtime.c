#include "stm32_production_runtime.h"
#include "stm32_runtime_platform.h"
#include "tr2/application/production_provisioning.h"

Tr2Result stm32_production_runtime_prepare(Stm32ProductionRuntime *runtime,
    const Stm32ProductionDependencies *dependencies)
{
    SystemRuntimeDependencies deps = {0};
    MonotonicClock monotonic;
    WallClock wall;
    ResetCauseProvider reset;
    TimeContinuityEvidenceProvider continuity;
    VibrationSource vibration;
    TransactionalImagePhysicalStorage physical;
    Tr2Result result;
    uint32_t uid[3];
    if (runtime == NULL || dependencies == NULL ||
        dependencies->campaign_data_store == NULL ||
        !dependencies->exclusive_media_ownership ||
        !dependencies->protocol_authority_available ||
        dependencies->geometry.physical_size != TR2_STM32_FRAM_CAPACITY) return TR2_ERROR_INVALID_ARGUMENT;
    if (runtime->prepare_attempted) return TR2_ERROR_INVALID_STATE;
    result = transactional_image_geometry_validate(&dependencies->geometry);
    if (result != TR2_OK) return result;
    runtime->prepare_attempted = true;
    result = stm32_fram_storage_init(&runtime->fram, dependencies->fram_spi,
        dependencies->fram_cs_port, dependencies->fram_cs_pin, 100u);
    if (result != TR2_OK) return result;
    physical = stm32_fram_storage_physical(&runtime->fram);
    result = transactional_image_media_init_v2(&runtime->transactional_media, &physical,
        &dependencies->geometry, runtime->candidate, sizeof(runtime->candidate));
    if (result != TR2_OK) return result;
    result = production_provisioning_probe(&runtime->transactional_media, &runtime->recovery);
    if (result != TR2_OK) return result;
    if (runtime->transactional_media.logical_version != 2u) return TR2_ERROR_NOT_AVAILABLE;
    uid[0] = HAL_GetUIDw0(); uid[1] = HAL_GetUIDw1(); uid[2] = HAL_GetUIDw2();
    result = persistent_identity_from_image(runtime->candidate, sizeof(runtime->candidate),
        uid, dependencies->supported_protocol_version, &runtime->persistent_identity,
        &runtime->identity_b0);
    if (result != TR2_OK) return result;
    /* The read-only probe denies EMPTY and every unavailable/invalid state.
     * It never provisions, formats, or manufactures production identity. */
    result = stm32_iis3dwb_vibration_source_init(&runtime->vibration,
        dependencies->vibration_spi, dependencies->vibration_cs_port, dependencies->vibration_cs_pin);
    if (result != TR2_OK) return result;
    result = stm32_runtime_platform_init();
    if (result != TR2_OK) return result;
    monotonic = stm32_runtime_monotonic_clock();
    wall = stm32_runtime_wall_clock();
    reset = stm32_runtime_reset_cause_provider();
    continuity = stm32_runtime_time_continuity_evidence_provider();
    vibration = stm32_iis3dwb_vibration_source_interface(&runtime->vibration);
    deps.monotonic_clock = &monotonic;
    deps.wall_clock = &wall;
    deps.reset_cause_provider = &reset;
    deps.time_continuity_evidence_provider = &continuity;
    deps.persistent_media = transactional_image_media_interface(&runtime->transactional_media);
    deps.configuration_validation_environment = &dependencies->environment;
    deps.vibration_source = &vibration;
    deps.campaign_data_store = dependencies->campaign_data_store;
    deps.selftest_executor = dependencies->selftest_executor;
    deps.reset_trigger = dependencies->reset_trigger;
    result = production_application_boot(&runtime->application, &deps, NULL);
    if (result == TR2_OK && dependencies->configuration_metadata_acquire != NULL) {
        result = production_application_enable_configuration(&runtime->application,
            dependencies->configuration_metadata_context, dependencies->configuration_metadata_acquire);
    }
    return result;
}
