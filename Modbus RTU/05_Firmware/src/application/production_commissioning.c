#include "tr2/application/production_commissioning.h"
#include "tr2/persistence/system_persistent_layout.h"
#include "tr2/persistence/campaign_data_store_persistent_composition.h"
#include "tr2/persistence/time_history_store.h"
#include "tr2/persistence/diagnostic_history_store.h"
#include "tr2/persistence/boot_intent_store.h"
#include <string.h>

typedef struct { const uint8_t *bytes; size_t size; } ReadOnlyPrefix;
static Tr2Result read_prefix(void *context, uint32_t offset, void *buffer, size_t size)
{
    ReadOnlyPrefix *prefix = context;
    if (buffer == NULL || offset > prefix->size || size > prefix->size-offset)
        return TR2_ERROR_INVALID_ARGUMENT;
    memcpy(buffer, prefix->bytes+offset, size);
    return TR2_OK;
}
static Tr2Result deny_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    (void)context; (void)offset; (void)buffer; (void)size;
    return TR2_ERROR_NOT_AVAILABLE;
}
static Tr2Result deny_commit(void *context)
{
    (void)context; return TR2_ERROR_NOT_AVAILABLE;
}
/* The recovery enums inspected below share these five explicit states. */
_Static_assert(CONFIGURATION_RECOVERY_VALID == 0 && TIME_HISTORY_RECOVERY_EMPTY == 1 &&
    DIAGNOSTIC_HISTORY_RECOVERY_CORRUPTED == 2 && BOOT_INTENT_RECOVERY_UNAVAILABLE == 3 &&
    CAMPAIGN_REPOSITORY_RECOVERY_UNSUPPORTED == 4, "recovery status mapping");
static Tr2Result status_result(unsigned status, bool allow_empty)
{
    switch (status) {
    case 0u: return TR2_OK;
    case 1u: return allow_empty ? TR2_OK : TR2_ERROR_NOT_AVAILABLE;
    case 2u: return TR2_ERROR_CORRUPTED;
    case 3u: return TR2_ERROR_UNAVAILABLE;
    case 4u: return TR2_ERROR_UNSUPPORTED;
    default: return TR2_ERROR_INTERNAL;
    }
}
Tr2Result production_commissioning_check_attribution(void *context,
    const uint32_t uid[3], uint32_t device_id)
{
    ProductionCommissioningAuthorities *authority = context;
    if (authority == NULL) return TR2_ERROR_NOT_AVAILABLE;
    return persistent_identity_attribution_check(authority->assignments,
        authority->assignment_count, uid, device_id);
}
Tr2Result production_commissioning_inspect_prefix(void *context,
    const uint8_t *image, size_t size)
{
    ProductionCommissioningAuthorities *authority = context;
    ReadOnlyPrefix prefix = {image, size};
    PersistentMedia media = {&prefix, read_prefix, deny_write, deny_commit};
    PersistentStorageCore storage = {0}, repository_core = {0}, journal_core = {0};
    PersistentMediaRegion repository_region = {0}, journal_region = {0};
    ConfigurationStore configuration = {0};
    ConfigurationRecoveryResult config;
    TimeHistoryStore time = {0}; TimeHistoryRecoveryResult history;
    CampaignRepositoryStore repository = {0}; CampaignRepositoryRecoveryResult inventory;
    CampaignDataStorePersistentComposition historical = {0};
    CommandJournalBoundedRecoveryResult journal;
    DiagnosticHistoryStore diagnostic = {0}; DiagnosticHistoryRecoveryResult fault;
    DiagnosticSelfTestRecoveryResult selftest;
    BootIntentStore boot = {0}; BootIntentRecoveryResult intent;
    Tr2Result result;
    if (authority == NULL || image == NULL || size != TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)
        return TR2_ERROR_INVALID_ARGUMENT;
    result = persistent_storage_core_init(&storage, &media);
    if (result != TR2_OK) return result;
    result = configuration_store_init(&configuration, &storage);
    if (result != TR2_OK) return result;
    result = configuration_store_recover(&configuration, &authority->environment, &config);
    if (result != TR2_OK) return result;
    result = status_result((unsigned)config.status, false);
    if (result != TR2_OK) return result;
    result = time_history_store_init(&time, &storage, TR2_TIME_HISTORY_STORAGE_OFFSET);
    if (result != TR2_OK) return result;
    result = time_history_store_recover(&time, &history);
    if (result != TR2_OK) return result;
    result = status_result((unsigned)history.status, true);
    if (result != TR2_OK) return result;
    result = persistent_media_region_init(&repository_region, &media,
        TR2_CAMPAIGN_REPOSITORY_STORAGE_OFFSET, TR2_CAMPAIGN_REPOSITORY_STORAGE_SIZE);
    if (result != TR2_OK) return result;
    result = persistent_storage_core_init(&repository_core,
        persistent_media_region_interface(&repository_region));
    if (result != TR2_OK) return result;
    result = campaign_repository_store_init(&repository, &repository_core);
    if (result != TR2_OK) return result;
    result = campaign_repository_store_inspect(&repository);
    if (result != TR2_OK) return result;
    { CampaignRepository *port = campaign_repository_store_interface(&repository);
      result = port->recover(port->context, &inventory); }
    if (result != TR2_OK) return result;
    result = status_result((unsigned)inventory.status, true);
    if (result != TR2_OK) return result;
    result = campaign_data_store_persistent_composition_init(&historical, &media);
    if (result != TR2_OK) return result;
    result = campaign_data_store_persistent_inspect(&historical.store);
    if (result != TR2_OK) return result;
    result = persistent_media_region_init(&journal_region, &media,
        TR2_COMMAND_JOURNAL_STORAGE_OFFSET, TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE);
    if (result != TR2_OK) return result;
    result = persistent_storage_core_init(&journal_core,
        persistent_media_region_interface(&journal_region));
    if (result != TR2_OK) return result;
    result = command_journal_bounded_recovery_scan(&journal_core, &journal);
    if (result != TR2_OK) return result;
    result = status_result((unsigned)journal.status, true);
    if (result != TR2_OK) return result;
    result = diagnostic_history_store_init(&diagnostic, &storage, TR2_DIAGNOSTIC_HISTORY_STORAGE_OFFSET);
    if (result != TR2_OK) return result;
    result = diagnostic_history_store_recover(&diagnostic, &fault);
    if (result != TR2_OK) return result;
    result = status_result((unsigned)fault.status, true);
    if (result != TR2_OK) return result;
    result = diagnostic_history_store_recover_selftest(&diagnostic, &selftest);
    if (result != TR2_OK) return result;
    result = status_result((unsigned)selftest.status, true);
    if (result != TR2_OK) return result;
    result = boot_intent_store_init(&boot, &storage, TR2_BOOT_INTENT_STORAGE_OFFSET);
    if (result != TR2_OK) return result;
    result = boot_intent_store_recover(&boot, &intent);
    if (result != TR2_OK) return result;
    result = status_result((unsigned)intent.status, true);
    if (result != TR2_OK) return result;
    if (intent.status == BOOT_INTENT_RECOVERY_VALID && intent.intent.kind != BOOT_INTENT_NONE)
        return TR2_ERROR_INVALID_STATE;
    return TR2_OK;
}

static bool empty_configuration_slot(const uint8_t *bytes)
{
    size_t i; bool zero = true, ff = true;
    for (i = 0u; i < TR2_CONFIGURATION_RECORD_SIZE; ++i) {
        zero = zero && bytes[i] == 0u;
        ff = ff && bytes[i] == UINT8_C(0xFF);
    }
    return zero || ff;
}
Tr2Result production_commissioning_install_configuration(TransactionalImageMedia *media,
    const ProductionCommissioningConfigurationRequest *q, bool authorized,
    ProductionProvisioningResult *out)
{
    ConfigurationStore store={0}; PersistentStorageCore core={0};
    ConfigurationRecoveryResult current; PreparedConfiguration prepared={0};
    uint8_t record[TR2_CONFIGURATION_RECORD_SIZE], before[TR2_CONFIGURATION_RECORD_SIZE];
    uint32_t offset; Tr2Result result;
    if (out == NULL) return TR2_ERROR_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out)); out->recovery.status=TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;
    if (q == NULL || q->authorities == NULL || q->configuration == NULL || media == NULL)
        return TR2_ERROR_INVALID_ARGUMENT;
    if (!authorized || !q->exclusive_ownership) return TR2_ERROR_NOT_AVAILABLE;
    if (memcmp(q->observed_uid,q->target_uid,sizeof(q->target_uid)) != 0)
        return TR2_ERROR_INVALID_STATE;
    result=production_commissioning_check_attribution(q->authorities,q->observed_uid,q->device_id);
    if (result != TR2_OK) return result;
    result=production_provisioning_probe(media,&out->recovery);
    if (result != TR2_OK) return result;
    if (out->recovery.generation != q->expected_generation) return TR2_ERROR_INVALID_STATE;
    if (media->logical_version != 1u) return TR2_ERROR_NOT_AVAILABLE;
    result=persistent_storage_core_init(&core,transactional_image_media_interface(media));
    if (result != TR2_OK) return result;
    result=configuration_store_init(&store,&core); if(result != TR2_OK) return result;
    result=configuration_store_recover(&store,&q->authorities->environment,&current);
    if(result != TR2_OK) return result;
    if (current.status == CONFIGURATION_RECOVERY_VALID) {
        result=production_commissioning_inspect_prefix(q->authorities,media->candidate,media->logical_size);
        if(result == TR2_OK) out->unchanged=true;
        return result;
    }
    if (current.status != CONFIGURATION_RECOVERY_EMPTY && current.status != CONFIGURATION_RECOVERY_CORRUPTED)
        return status_result((unsigned)current.status,false);
    prepared.generation=q->configuration->generation;
    prepared.config_id=q->configuration->config_id;prepared.payload=q->configuration->payload;
    { ConfigurationValidationResult valid=configuration_validate(&prepared,&q->authorities->environment,NULL);
      if(valid.status != CONFIGURATION_VALIDATION_VALID)
          return valid.status==CONFIGURATION_VALIDATION_ENVIRONMENT_NOT_CHARACTERIZED
              ? TR2_ERROR_UNAVAILABLE : TR2_ERROR_INVALID_ARGUMENT; }
    for(offset=0u;offset<TR2_CONFIGURATION_STORE_STORAGE_SIZE;offset+=TR2_CONFIGURATION_RECORD_SIZE)
        if(empty_configuration_slot(media->candidate+offset)) break;
    if(offset==TR2_CONFIGURATION_STORE_STORAGE_SIZE) return TR2_ERROR_NOT_AVAILABLE;
    result=tr2_configuration_record_encode(q->configuration,record,sizeof(record));
    if(result != TR2_OK) return result;
    memcpy(before,media->candidate+offset,sizeof(before));
    memcpy(media->candidate+offset,record,sizeof(record));
    result=production_commissioning_inspect_prefix(q->authorities,media->candidate,media->logical_size);
    memcpy(media->candidate+offset,before,sizeof(before));
    if(result != TR2_OK) return result;
    out->write_attempted=true;
    result=persistent_storage_core_write(&core,offset,record,sizeof(record));
    if(result==TR2_OK) result=persistent_storage_core_commit(&core);
    if(result!=TR2_OK) return result;
    result=production_provisioning_probe(media,&out->recovery);
    if(result!=TR2_OK) return result;
    if(memcmp(media->candidate+offset,record,sizeof(record))!=0) return TR2_ERROR_STORAGE;
    return production_commissioning_inspect_prefix(q->authorities,media->candidate,media->logical_size);
}
