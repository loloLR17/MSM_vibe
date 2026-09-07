#ifndef TR2_APPLICATION_SYSTEM_RUNTIME_H
#define TR2_APPLICATION_SYSTEM_RUNTIME_H

#include <stdbool.h>

#include "tr2/application/campaign_inventory_service.h"
#include "tr2/application/configuration_service.h"
#include "tr2/common/result.h"
#include "tr2/domain/configuration/configuration_validator.h"
#include "tr2/domain/time/time_service.h"
#include "tr2/modbus/projection.h"
#include "tr2/persistence/campaign_data_store_persistent.h"
#include "tr2/persistence/campaign_repository_store.h"
#include "tr2/persistence/configuration_store.h"
#include "tr2/persistence/persistent_media_region.h"
#include "tr2/persistence/persistent_storage_core.h"
#include "tr2/persistence/time_history_store.h"
#include "tr2/platform/monotonic_clock.h"
#include "tr2/platform/persistent_media.h"
#include "tr2/platform/reset_cause_provider.h"
#include "tr2/platform/time_continuity_evidence.h"
#include "tr2/platform/wall_clock.h"

typedef struct {
    ResetCause reset_cause;
} BootContext;

typedef struct {
    CampaignMetadata metadata;
    CampaignDataRecoveryResult data_recovery;
} CampaignBootRecoveryEntry;

typedef struct {
    CampaignRepositoryRecoveryStatus repository_status;
    CampaignInventorySummary inventory;
    size_t recovered_campaign_count;
    CampaignBootRecoveryEntry campaigns[TR2_CAMPAIGN_REPOSITORY_CAPACITY];
} CampaignBootRecoverySnapshot;

typedef struct {
    const MonotonicClock *monotonic_clock;
    const WallClock *wall_clock;
    const ResetCauseProvider *reset_cause_provider;
    const TimeContinuityEvidenceProvider *time_continuity_evidence_provider;
    const PersistentMedia *persistent_media;
    const ConfigurationValidationEnvironment *configuration_validation_environment;
} SystemRuntimeDependencies;

typedef struct {
    SystemRuntimeDependencies deps;
    BootContext boot_context;
    PersistentStorageCore persistent_storage_core;
    ConfigurationStore configuration_store;
    ConfigurationService configuration_service;
    TimeHistoryStore time_history_store;
    TimeHistoryRecoveryStatus time_history_recovery_status;
    TimeService time_service;
    TimeSnapshot time_snapshot;
    bool time_snapshot_available;
    PersistentMediaRegion campaign_repository_media_region;
    PersistentMediaRegion campaign_data_media_region;
    PersistentStorageCore campaign_repository_storage_core;
    PersistentStorageCore campaign_data_storage_core;
    CampaignRepositoryStore campaign_repository_store;
    CampaignDataStorePersistent campaign_data_store;
    CampaignBootRecoverySnapshot campaign_recovery_snapshot;
    bool campaign_recovery_available;
    CampaignInventoryService campaign_inventory_service;
    CampaignInventoryViewSnapshot campaign_inventory_snapshot;
    bool campaign_inventory_snapshot_available;
    ModbusBlock4Image b4_image;
    bool b4_image_available;
    ModbusBlock6Image b6_image;
    bool b6_image_available;
    bool initialized;
    bool system_ready_for_modbus;
} SystemRuntime;

Tr2Result system_runtime_init(SystemRuntime *runtime, const SystemRuntimeDependencies *deps);
Tr2Result system_runtime_boot(SystemRuntime *runtime);
const BootContext *system_runtime_boot_context(const SystemRuntime *runtime);
bool system_runtime_is_ready_for_modbus(const SystemRuntime *runtime);
bool system_runtime_time_snapshot(const SystemRuntime *runtime, TimeSnapshot *out_snapshot);
bool system_runtime_time_history_recovery_status(const SystemRuntime *runtime,
                                                 TimeHistoryRecoveryStatus *out_status);
bool system_runtime_campaign_recovery_snapshot(
    const SystemRuntime *runtime,
    CampaignBootRecoverySnapshot *out_snapshot);
bool system_runtime_campaign_inventory_snapshot(
    const SystemRuntime *runtime,
    CampaignInventoryViewSnapshot *out_snapshot);
bool system_runtime_b4_image(const SystemRuntime *runtime, ModbusBlock4Image *out_image);
bool system_runtime_b6_image(const SystemRuntime *runtime, ModbusBlock6Image *out_image);

#endif
