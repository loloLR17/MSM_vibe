#include "tr2/application/system_runtime.h"

#include <string.h>

#include "tr2/application/configuration_workflow.h"
#include "tr2/persistence/time_history_record.h"

#define TR2_TIME_HISTORY_STORAGE_OFFSET ((uint32_t)TR2_CONFIGURATION_STORE_STORAGE_SIZE)
#define TR2_CAMPAIGN_REPOSITORY_STORAGE_OFFSET \
    (TR2_TIME_HISTORY_STORAGE_OFFSET + (uint32_t)TR2_TIME_HISTORY_RECORD_SIZE)
#define TR2_CAMPAIGN_DATA_STORAGE_OFFSET \
    (TR2_CAMPAIGN_REPOSITORY_STORAGE_OFFSET + \
     (uint32_t)TR2_CAMPAIGN_REPOSITORY_STORAGE_SIZE)

static bool dependencies_are_valid(const SystemRuntimeDependencies *deps)
{
    return deps != NULL &&
           deps->monotonic_clock != NULL &&
           deps->monotonic_clock->now_ms != NULL &&
           deps->wall_clock != NULL &&
           deps->wall_clock->read != NULL &&
           deps->wall_clock->set != NULL &&
           deps->reset_cause_provider != NULL &&
           deps->reset_cause_provider->get != NULL &&
           deps->time_continuity_evidence_provider != NULL &&
           deps->time_continuity_evidence_provider->get != NULL &&
           deps->persistent_media != NULL &&
           deps->persistent_media->read != NULL &&
           deps->persistent_media->write != NULL &&
           deps->persistent_media->commit != NULL &&
           deps->configuration_validation_environment != NULL;
}

static TimeContinuity continuity_from_platform(TimeContinuityEvidence evidence,
                                               WallClockReadResult wall_result)
{
    if (wall_result == WALL_CLOCK_INVALID) {
        return TIME_CONTINUITY_BROKEN;
    }

    switch (evidence) {
    case TIME_CONTINUITY_EVIDENCE_PROVEN:
        return TIME_CONTINUITY_PROVEN;
    case TIME_CONTINUITY_EVIDENCE_BROKEN:
        return TIME_CONTINUITY_BROKEN;
    case TIME_CONTINUITY_EVIDENCE_INDETERMINATE:
    default:
        return TIME_CONTINUITY_INDETERMINATE;
    }
}

static Tr2Result recover_time(SystemRuntime *runtime)
{
    TimeHistoryRecoveryResult history_recovery;
    TimeRecoveryContext recovery_context;
    Tr2CivilTimestamp current_time = 0u;
    WallClockReadResult wall_result;
    TimeContinuityEvidence evidence;
    Tr2Result result;

    result = time_history_store_init(&runtime->time_history_store,
                                     &runtime->persistent_storage_core,
                                     TR2_TIME_HISTORY_STORAGE_OFFSET);
    if (result != TR2_OK) {
        return result;
    }

    result = time_service_init(&runtime->time_service, runtime->deps.wall_clock);
    if (result != TR2_OK) {
        return result;
    }

    result = time_service_bind_synchronization_dependencies(&runtime->time_service,
                                                            runtime->deps.monotonic_clock,
                                                            &runtime->time_history_store);
    if (result != TR2_OK) {
        return result;
    }

    result = time_history_store_recover(&runtime->time_history_store, &history_recovery);
    if (result != TR2_OK) {
        return result;
    }
    runtime->time_history_recovery_status = history_recovery.status;

    wall_result = runtime->deps.wall_clock->read(runtime->deps.wall_clock->context,
                                                &current_time);
    (void)current_time;
    evidence = runtime->deps.time_continuity_evidence_provider->get(
        runtime->deps.time_continuity_evidence_provider->context);

    recovery_context.civil_time_usable = (wall_result == WALL_CLOCK_OK);
    recovery_context.continuity = continuity_from_platform(evidence, wall_result);
    recovery_context.last_sync_history = history_recovery.history;

    result = time_service_apply_recovery_context(&runtime->time_service, &recovery_context);
    if (result != TR2_OK) {
        return result;
    }

    result = time_service_get_snapshot(&runtime->time_service, &runtime->time_snapshot);
    if (result != TR2_OK) {
        return result;
    }

    runtime->time_snapshot_available = true;
    return TR2_OK;
}

static Tr2Result recover_configuration(SystemRuntime *runtime)
{
    Tr2Result result;

    result = configuration_store_init(&runtime->configuration_store,
                                      &runtime->persistent_storage_core);
    if (result != TR2_OK) {
        return result;
    }

    result = configuration_service_init(&runtime->configuration_service,
                                        &runtime->configuration_store);
    if (result != TR2_OK) {
        return result;
    }

    return configuration_service_recover(
        &runtime->configuration_service,
        runtime->deps.configuration_validation_environment);
}

static Tr2Result recover_campaigns(SystemRuntime *runtime)
{
    CampaignRepositoryRecoveryResult repository_recovery;
    CampaignRepository *repository;
    CampaignDataStore *data_store;
    size_t index;
    Tr2Result result;

    result = persistent_media_region_init(
        &runtime->campaign_repository_media_region,
        runtime->deps.persistent_media,
        TR2_CAMPAIGN_REPOSITORY_STORAGE_OFFSET,
        (uint32_t)TR2_CAMPAIGN_REPOSITORY_STORAGE_SIZE);
    if (result != TR2_OK) {
        return result;
    }

    result = persistent_storage_core_init(
        &runtime->campaign_repository_storage_core,
        persistent_media_region_interface(&runtime->campaign_repository_media_region));
    if (result != TR2_OK) {
        return result;
    }

    result = campaign_repository_store_init(
        &runtime->campaign_repository_store,
        &runtime->campaign_repository_storage_core);
    if (result != TR2_OK) {
        return result;
    }

    result = persistent_media_region_init(
        &runtime->campaign_data_media_region,
        runtime->deps.persistent_media,
        TR2_CAMPAIGN_DATA_STORAGE_OFFSET,
        (uint32_t)TR2_CAMPAIGN_DATA_STORAGE_SIZE);
    if (result != TR2_OK) {
        return result;
    }

    result = persistent_storage_core_init(
        &runtime->campaign_data_storage_core,
        persistent_media_region_interface(&runtime->campaign_data_media_region));
    if (result != TR2_OK) {
        return result;
    }

    result = campaign_data_store_persistent_init(
        &runtime->campaign_data_store,
        &runtime->campaign_data_storage_core);
    if (result != TR2_OK) {
        return result;
    }

    repository = campaign_repository_store_interface(
        &runtime->campaign_repository_store);
    data_store = campaign_data_store_persistent_interface(
        &runtime->campaign_data_store);
    if (repository == NULL || data_store == NULL) {
        return TR2_ERROR_INTERNAL;
    }

    memset(&repository_recovery, 0, sizeof(repository_recovery));
    result = repository->recover(repository->context, &repository_recovery);
    if (result != TR2_OK) {
        return result;
    }

    memset(&runtime->campaign_recovery_snapshot,
           0,
           sizeof(runtime->campaign_recovery_snapshot));
    runtime->campaign_recovery_snapshot.repository_status =
        repository_recovery.status;
    runtime->campaign_recovery_snapshot.inventory = repository_recovery.inventory;

    if (repository_recovery.status == CAMPAIGN_REPOSITORY_RECOVERY_VALID) {
        size_t recover_count = repository_recovery.inventory.valid_campaign_count;

        if (recover_count > TR2_CAMPAIGN_REPOSITORY_CAPACITY) {
            return TR2_ERROR_CORRUPTED;
        }

        for (index = 0u; index < recover_count; ++index) {
            CampaignBootRecoveryEntry *entry =
                &runtime->campaign_recovery_snapshot.campaigns[index];

            result = repository->get_campaign_by_index(repository->context,
                                                       index,
                                                       &entry->metadata);
            if (result != TR2_OK) {
                return result;
            }

            result = data_store->recover_campaign(data_store->context,
                                                  entry->metadata.campaign_id,
                                                  &entry->data_recovery);
            if (result != TR2_OK) {
                return result;
            }

            runtime->campaign_recovery_snapshot.recovered_campaign_count++;
        }
    }

    runtime->campaign_recovery_available = true;
    return TR2_OK;
}

static Tr2Result rebuild_b4(SystemRuntime *runtime)
{
    ActiveConfigurationSnapshot active;
    ModbusBlock4ProjectionSource source;
    const bool has_active = configuration_service_active_snapshot(
        &runtime->configuration_service,
        &active);
    Tr2Result result;

    memset(&source, 0, sizeof(source));
    source.config_state = has_active
                              ? (uint16_t)CONFIGURATION_STATE_ACTIVE
                              : (uint16_t)CONFIGURATION_STATE_EMPTY;
    source.config_error_code = 0u;
    source.prepared = NULL;
    source.active = has_active ? &active : NULL;

    result = modbus_project_b4(&source, &runtime->b4_image);
    if (result != TR2_OK) {
        runtime->b4_image_available = false;
        return result;
    }

    runtime->b4_image_available = true;
    return TR2_OK;
}

Tr2Result system_runtime_init(SystemRuntime *runtime, const SystemRuntimeDependencies *deps)
{
    if (runtime == NULL || !dependencies_are_valid(deps)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(runtime, 0, sizeof(*runtime));
    runtime->deps = *deps;
    runtime->boot_context.reset_cause = RESET_CAUSE_UNKNOWN;
    runtime->time_history_recovery_status = TIME_HISTORY_RECOVERY_EMPTY;
    runtime->campaign_recovery_snapshot.repository_status =
        CAMPAIGN_REPOSITORY_RECOVERY_EMPTY;
    runtime->initialized = true;
    return TR2_OK;
}

Tr2Result system_runtime_boot(SystemRuntime *runtime)
{
    Tr2Result result;

    if (runtime == NULL || !runtime->initialized) {
        return TR2_ERROR_INVALID_STATE;
    }

    runtime->system_ready_for_modbus = false;
    runtime->time_snapshot_available = false;
    runtime->campaign_recovery_available = false;
    runtime->b4_image_available = false;

    /* G0/G1: establish minimal platform facts before domain recovery. */
    (void)runtime->deps.monotonic_clock->now_ms(runtime->deps.monotonic_clock->context);
    runtime->boot_context.reset_cause =
        runtime->deps.reset_cause_provider->get(runtime->deps.reset_cause_provider->context);

    /* G2: initialize generic persistence before domain stores. */
    result = persistent_storage_core_init(&runtime->persistent_storage_core,
                                          runtime->deps.persistent_media);
    if (result != TR2_OK) {
        return result;
    }

    /* G4: establish temporal recovery facts and initial TimeSnapshot before
       any projection or business consumer depends on temporal quality. */
    result = recover_time(runtime);
    if (result != TR2_OK) {
        return result;
    }

    /* G5: recover authoritative ActiveConfiguration. */
    result = recover_configuration(runtime);
    if (result != TR2_OK) {
        return result;
    }

    /* G6: recover campaign metadata and durable data prefixes. Recovery is
       observational only: no campaign or acquisition is automatically resumed. */
    result = recover_campaigns(runtime);
    if (result != TR2_OK) {
        return result;
    }

    result = rebuild_b4(runtime);
    if (result != TR2_OK) {
        return result;
    }

    runtime->system_ready_for_modbus = true;
    return TR2_OK;
}

const BootContext *system_runtime_boot_context(const SystemRuntime *runtime)
{
    if (runtime == NULL || !runtime->initialized) {
        return NULL;
    }

    return &runtime->boot_context;
}

bool system_runtime_is_ready_for_modbus(const SystemRuntime *runtime)
{
    return runtime != NULL && runtime->initialized && runtime->system_ready_for_modbus;
}

bool system_runtime_time_snapshot(const SystemRuntime *runtime, TimeSnapshot *out_snapshot)
{
    if (runtime == NULL || out_snapshot == NULL || !runtime->initialized ||
        !runtime->system_ready_for_modbus || !runtime->time_snapshot_available) {
        return false;
    }

    *out_snapshot = runtime->time_snapshot;
    return true;
}

bool system_runtime_time_history_recovery_status(const SystemRuntime *runtime,
                                                 TimeHistoryRecoveryStatus *out_status)
{
    if (runtime == NULL || out_status == NULL || !runtime->initialized ||
        !runtime->system_ready_for_modbus || !runtime->time_snapshot_available) {
        return false;
    }

    *out_status = runtime->time_history_recovery_status;
    return true;
}

bool system_runtime_campaign_recovery_snapshot(
    const SystemRuntime *runtime,
    CampaignBootRecoverySnapshot *out_snapshot)
{
    if (runtime == NULL || out_snapshot == NULL || !runtime->initialized ||
        !runtime->system_ready_for_modbus || !runtime->campaign_recovery_available) {
        return false;
    }

    *out_snapshot = runtime->campaign_recovery_snapshot;
    return true;
}

bool system_runtime_b4_image(const SystemRuntime *runtime, ModbusBlock4Image *out_image)
{
    if (runtime == NULL || out_image == NULL || !runtime->initialized ||
        !runtime->system_ready_for_modbus || !runtime->b4_image_available) {
        return false;
    }

    *out_image = runtime->b4_image;
    return true;
}

#undef TR2_CAMPAIGN_DATA_STORAGE_OFFSET
#undef TR2_CAMPAIGN_REPOSITORY_STORAGE_OFFSET
#undef TR2_TIME_HISTORY_STORAGE_OFFSET
