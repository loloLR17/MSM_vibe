#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/command_apply_configuration.h"

#define TEST_MAX_TRANSACTION_ID 4u
#define TEST_STORAGE_SIZE \
    (TEST_MAX_TRANSACTION_ID * TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS * \
     TR2_COMMAND_JOURNAL_RECORD_SIZE)

typedef struct {
    uint8_t durable[TEST_STORAGE_SIZE];
    uint8_t staged[TEST_STORAGE_SIZE];
    bool fail_commit;
} TestMediaContext;

typedef struct {
    unsigned calls;
    Tr2Result result;
} ActivationContext;

static Tr2Result media_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestMediaContext *media = (TestMediaContext *)context;
    if ((size_t)offset + size > sizeof(media->staged)) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(buffer, &media->staged[offset], size);
    return TR2_OK;
}

static Tr2Result media_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    TestMediaContext *media = (TestMediaContext *)context;
    if ((size_t)offset + size > sizeof(media->staged)) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(&media->staged[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result media_commit(void *context)
{
    TestMediaContext *media = (TestMediaContext *)context;
    if (media->fail_commit) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(media->durable, media->staged, sizeof(media->durable));
    return TR2_OK;
}

static void media_init(TestMediaContext *media)
{
    memset(media, 0, sizeof(*media));
    memset(media->durable, 0xFF, sizeof(media->durable));
    memcpy(media->staged, media->durable, sizeof(media->staged));
}

static Tr2Result activation_commit(void *context,
                                   const ValidatedConfiguration *validated,
                                   ActiveConfigurationSnapshot *committed)
{
    ActivationContext *activation = (ActivationContext *)context;
    ++activation->calls;
    if (activation->result != TR2_OK) {
        return activation->result;
    }
    memset(committed, 0, sizeof(*committed));
    committed->generation = validated->generation;
    committed->config_id = validated->config_id;
    committed->revision_counter = 1u;
    committed->payload = validated->payload;
    return TR2_OK;
}

static void init_journal(TestMediaContext *media,
                         PersistentMedia *persistent_media,
                         PersistentStorageCore *core,
                         CommandJournalStore *store,
                         CommandEngine *engine)
{
    CommandJournalRecoveryResult recovery;

    persistent_media->context = media;
    persistent_media->read = media_read;
    persistent_media->write = media_write;
    persistent_media->commit = media_commit;
    assert(persistent_storage_core_init(core, persistent_media) == TR2_OK);
    assert(command_journal_store_init(store, core, TEST_MAX_TRANSACTION_ID) == TR2_OK);
    assert(command_journal_store_recover(store, &recovery) == TR2_OK);
    assert(recovery.status == COMMAND_JOURNAL_RECOVERY_EMPTY ||
           recovery.status == COMMAND_JOURNAL_RECOVERY_VALID);
    assert(command_engine_init(engine, command_journal_store_journal(store)) == TR2_OK);
}

static void init_valid_workflow(ConfigurationWorkflow *workflow,
                                ConfigurationStagingService *staging,
                                ActivationContext *activation)
{
    memset(staging, 0, sizeof(*staging));
    staging->has_prepared = true;
    staging->validation_current = true;
    staging->prepared.generation = 7u;
    staging->prepared.config_id = 42u;
    staging->prepared.supplied_crc = UINT32_C(0x11223344);

    memset(workflow, 0, sizeof(*workflow));
    workflow->staging = staging;
    workflow->state = CONFIGURATION_STATE_VALID;
    workflow->has_validated = true;
    workflow->validated.generation = staging->prepared.generation;
    workflow->validated.config_id = staging->prepared.config_id;
    workflow->validated.supplied_crc = staging->prepared.supplied_crc;
    workflow->activation.context = activation;
    workflow->activation.commit = activation_commit;
}

static CommandRequest apply_request(uint16_t transaction_id)
{
    CommandRequest request;
    memset(&request, 0, sizeof(request));
    request.transaction_id = transaction_id;
    request.identity.command_code = COMMAND_CODE_APPLY_CONFIGURATION;
    return request;
}

static void test_apply_config_persists_context_before_effect(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalStore store;
    CommandEngine engine;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    ConfigurationWorkflow workflow;
    ConfigurationStagingService staging;
    ActivationContext activation;
    CommandTerminalTimestamp timestamp;
    CommandRequest request;

    media_init(&media);
    memset(&activation, 0, sizeof(activation));
    activation.result = TR2_OK;
    init_journal(&media, &persistent_media, &core, &store, &engine);
    init_valid_workflow(&workflow, &staging, &activation);
    request = apply_request(1u);
    assert(command_engine_admit(&engine, &request, &admission) == TR2_OK);

    timestamp.available = true;
    timestamp.value = UINT32_C(1234);
    assert(command_apply_configuration_execute(&engine, &store, &workflow, 1u,
                                               &timestamp, &entry) == TR2_OK);
    assert(activation.calls == 1u);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(entry.has_recovery_context);
    assert(entry.recovery_context.kind == COMMAND_RECOVERY_CONTEXT_CONFIGURATION);
    assert(entry.recovery_context.value1 == 7u);
    assert(entry.recovery_context.value2 == 42u);
    assert(entry.recovery_context.value3 == UINT32_C(0x11223344));
    assert(entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(entry.final_result.result_code == COMMAND_RESULT_SUCCESS);
}

static void test_missing_validated_config_is_refused_without_effect(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalStore store;
    CommandEngine engine;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    ConfigurationWorkflow workflow;
    ConfigurationStagingService staging;
    ActivationContext activation;
    CommandTerminalTimestamp timestamp;
    CommandRequest request;

    media_init(&media);
    memset(&activation, 0, sizeof(activation));
    init_journal(&media, &persistent_media, &core, &store, &engine);
    memset(&workflow, 0, sizeof(workflow));
    memset(&staging, 0, sizeof(staging));
    workflow.staging = &staging;
    workflow.activation.context = &activation;
    workflow.activation.commit = activation_commit;
    request = apply_request(2u);
    assert(command_engine_admit(&engine, &request, &admission) == TR2_OK);

    memset(&timestamp, 0, sizeof(timestamp));
    assert(command_apply_configuration_execute(&engine, &store, &workflow, 2u,
                                               &timestamp, &entry) == TR2_OK);
    assert(activation.calls == 0u);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(!entry.has_recovery_context);
    assert(entry.final_result.status == COMMAND_STATUS_REFUSED);
    assert(entry.final_result.result_code == COMMAND_RESULT_PREPARED_CONFIGURATION_INCOMPLETE);
}

static void test_context_commit_failure_prevents_started_and_effect(void)
{
    TestMediaContext media;
    PersistentMedia persistent_media;
    PersistentStorageCore core;
    CommandJournalStore store;
    CommandEngine engine;
    CommandAdmissionResult admission;
    CommandJournalEntry entry;
    ConfigurationWorkflow workflow;
    ConfigurationStagingService staging;
    ActivationContext activation;
    CommandTerminalTimestamp timestamp;
    CommandRequest request;
    CommandJournalStore rebooted_store;
    CommandEngine rebooted_engine;
    PersistentStorageCore rebooted_core;
    PersistentMedia rebooted_media;
    CommandJournalRecoveryResult recovery;

    media_init(&media);
    memset(&activation, 0, sizeof(activation));
    activation.result = TR2_OK;
    init_journal(&media, &persistent_media, &core, &store, &engine);
    init_valid_workflow(&workflow, &staging, &activation);
    request = apply_request(3u);
    assert(command_engine_admit(&engine, &request, &admission) == TR2_OK);

    media.fail_commit = true;
    memset(&timestamp, 0, sizeof(timestamp));
    assert(command_apply_configuration_execute(&engine, &store, &workflow, 3u,
                                               &timestamp, &entry) == TR2_ERROR_STORAGE);
    assert(activation.calls == 0u);

    memcpy(media.staged, media.durable, sizeof(media.staged));
    media.fail_commit = false;
    rebooted_media.context = &media;
    rebooted_media.read = media_read;
    rebooted_media.write = media_write;
    rebooted_media.commit = media_commit;
    assert(persistent_storage_core_init(&rebooted_core, &rebooted_media) == TR2_OK);
    assert(command_journal_store_init(&rebooted_store, &rebooted_core,
                                      TEST_MAX_TRANSACTION_ID) == TR2_OK);
    assert(command_journal_store_recover(&rebooted_store, &recovery) == TR2_OK);
    assert(command_engine_init(&rebooted_engine,
                               command_journal_store_journal(&rebooted_store)) == TR2_OK);
    assert(command_journal_store_journal(&rebooted_store)->find(
               command_journal_store_journal(&rebooted_store)->context, 3u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_RESERVED);
    assert(!entry.has_recovery_context);
}

int main(void)
{
    test_apply_config_persists_context_before_effect();
    test_missing_validated_config_is_refused_without_effect();
    test_context_commit_failure_prevents_started_and_effect();
    return 0;
}
