#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tr2/application/system_runtime.h"
#include "tr2/persistence/campaign_data_store_bulk.h"
#include "tr2/persistence/system_persistent_layout.h"
#include "tr2/platform_host/host_platform.h"

/* D5-C memory fixture sizes, not a production geometry or capacity claim. */
#define MEDIA_SIZE 16384u
#define PAYLOAD_BUFFER_SIZE 64u
#define BLOCK_SCRATCH_SIZE 128u
#define WINDOW_SAMPLES 4u

_Static_assert(TR2_CAMPAIGN_BULK_SLOT_COUNT == 8u, "D4-C slots unchanged");
_Static_assert(TR2_CAMPAIGN_SAMPLE_RECORD_SIZE == 16u, "P8-B record");

typedef struct {
    uint8_t working[MEDIA_SIZE];
    uint8_t durable[MEDIA_SIZE];
    Tr2Result write_result;
    Tr2Result sync_result;
    unsigned failed_writes;
    unsigned failed_syncs;
    unsigned reads;
} FakeMedia;

typedef struct {
    CampaignBulkMedia media;
    CampaignDataStoreBulk store;
    uint8_t buffer[PAYLOAD_BUFFER_SIZE];
    uint8_t scratch[BLOCK_SCRATCH_SIZE];
} BulkComposition;

typedef struct {
    HostPlatform platform;
    MonotonicClock monotonic;
    WallClock wall;
    ResetCauseProvider reset;
    TimeContinuityEvidenceProvider continuity;
    PersistentMedia fram;
    VibrationSource vibration;
    ConfigurationValidationEnvironment environment;
    SystemRuntimeDependencies deps;
    uint8_t historical_before[TR2_CAMPAIGN_DATA_STORAGE_SIZE];
} RuntimeComposition;

static Tr2Result fake_capacity(void *context, uint64_t *capacity)
{
    (void)context;
    *capacity = MEDIA_SIZE;
    return TR2_OK;
}

static Tr2Result fake_read(void *context, uint64_t offset, void *buffer, size_t size)
{
    FakeMedia *fake = context;
    fake->reads++;
    if (offset > MEDIA_SIZE || size > MEDIA_SIZE - (size_t)offset)
        return TR2_ERROR_STORAGE;
    memcpy(buffer, &fake->working[(size_t)offset], size);
    return TR2_OK;
}

static Tr2Result fake_write(void *context, uint64_t offset,
                           const void *buffer, size_t size)
{
    FakeMedia *fake = context;
    if (fake->write_result != TR2_OK) {
        fake->failed_writes++;
        return fake->write_result;
    }
    if (offset > MEDIA_SIZE || size > MEDIA_SIZE - (size_t)offset)
        return TR2_ERROR_STORAGE;
    memcpy(&fake->working[(size_t)offset], buffer, size);
    return TR2_OK;
}

static Tr2Result fake_sync(void *context)
{
    FakeMedia *fake = context;
    if (fake->sync_result != TR2_OK) {
        fake->failed_syncs++;
        return fake->sync_result;
    }
    memcpy(fake->durable, fake->working, sizeof(fake->durable));
    return TR2_OK;
}

static void init_bulk(BulkComposition *bulk, FakeMedia *fake)
{
    bulk->media = (CampaignBulkMedia){fake, fake_capacity, fake_read,
                                     fake_write, fake_sync};
    assert(campaign_data_store_bulk_init(&bulk->store, &bulk->media,
        bulk->buffer, sizeof(bulk->buffer), bulk->scratch,
        sizeof(bulk->scratch)) == TR2_OK);
}

/* Keep the existing host source lifecycle/counters, with explicit P8-B values. */
static Tr2Result read_sample(void *context, VibrationSample *sample)
{
    HostPlatform *platform = context;
    VibrationSource base = host_platform_vibration_source(platform);
    Tr2Result result = base.read_sample(base.context, sample);
    if (result == TR2_OK) {
        *sample = (VibrationSample){-1, 2, -3,
            platform->vibration_read_calls % 2u != 0u, true};
    }
    return result;
}

static void init_composition(RuntimeComposition *composition)
{
    memset(composition, 0, sizeof(*composition));
    host_platform_init(&composition->platform);
    assert(composition->platform.persistent_committed != NULL);
    assert(composition->platform.persistent_candidate != NULL);
    composition->monotonic = host_platform_monotonic_clock(&composition->platform);
    composition->wall = host_platform_wall_clock(&composition->platform);
    composition->reset = host_platform_reset_cause_provider(&composition->platform);
    composition->continuity = host_platform_time_continuity_evidence_provider(&composition->platform);
    composition->fram = host_platform_persistent_media(&composition->platform);
    composition->vibration = host_platform_vibration_source(&composition->platform);
    composition->vibration.read_sample = read_sample;
    /* Existing runtime test environment: no production capacity publication. */
    composition->environment = (ConfigurationValidationEnvironment){true, 4096u};
    composition->deps = (SystemRuntimeDependencies){
        .monotonic_clock = &composition->monotonic,
        .wall_clock = &composition->wall,
        .reset_cause_provider = &composition->reset,
        .time_continuity_evidence_provider = &composition->continuity,
        .persistent_media = &composition->fram,
        .configuration_validation_environment = &composition->environment,
        .vibration_source = &composition->vibration
    };
    /* Non-empty sentinel in the historical reservation detects any fallback. */
    memset(&composition->platform.persistent_committed[TR2_CAMPAIGN_DATA_STORAGE_OFFSET],
           0xA5, TR2_CAMPAIGN_DATA_STORAGE_SIZE);
    memcpy(composition->platform.persistent_candidate,
           composition->platform.persistent_committed, HOST_PLATFORM_PERSISTENT_BYTES);
    memcpy(composition->historical_before,
           &composition->platform.persistent_committed[TR2_CAMPAIGN_DATA_STORAGE_OFFSET],
           sizeof(composition->historical_before));
}

static void assert_no_historical_write(const RuntimeComposition *composition)
{
    assert(memcmp(composition->historical_before,
        &composition->platform.persistent_committed[TR2_CAMPAIGN_DATA_STORAGE_OFFSET],
        sizeof(composition->historical_before)) == 0);
    assert(memcmp(composition->historical_before,
        &composition->platform.persistent_candidate[TR2_CAMPAIGN_DATA_STORAGE_OFFSET],
        sizeof(composition->historical_before)) == 0);
}

static void boot_runtime(RuntimeComposition *composition, BulkComposition *bulk,
                         SystemRuntime *runtime)
{
    CampaignDataStore *iface = campaign_data_store_bulk_interface(&bulk->store);
    composition->deps.campaign_data_store = iface;
    assert(system_runtime_init(runtime, &composition->deps) == TR2_OK);
    assert(system_runtime_boot(runtime) == TR2_OK);
    assert(system_runtime_is_ready_for_modbus(runtime));
    assert(runtime->deps.campaign_data_store == iface);
    assert(system_runtime_campaign_service(runtime)->data_store == iface);
    assert(iface->context == &bulk->store);
    assert(!bulk->store.campaign_active);
    assert(!campaign_service_campaign_open(&runtime->campaign_service));
    assert(!campaign_service_acquisition_running(&runtime->campaign_service));
}

static void commit_configuration(SystemRuntime *runtime)
{
    ValidatedConfiguration validated = {0};
    ActiveConfigurationSnapshot committed;
    validated.generation = 1u;
    validated.config_id = 1u;
    validated.payload = (ConfigurationPayload){
        .sampling_frequency_hz = 1000u, .axes_enable_mask = 7u,
        .full_scale_code = 2u, .acquisition_mode = 1u,
        .window_size_samples = WINDOW_SAMPLES, .indicator_period_ms = 2000u,
        .campaign_duration_s = 60u, .storage_mode = 1u,
        .storage_limit_mb = 8u, .campaign_context_id = 1u,
        .mission_id = 2u, .operating_mode_code = 1u
    };
    assert(configuration_service_commit_validated(&runtime->configuration_service,
        &validated, 1u, &committed) == TR2_OK);
}

static Tr2Result command(SystemRuntime *runtime, uint16_t code, uint16_t transaction,
                          CommandJournalEntry *entry)
{
    CommandRequest request = {0};
    CommandAdmissionResult admission;
    CommandTerminalTimestamp timestamp = {false, 0u};
    request.transaction_id = transaction;
    request.identity.command_code = code;
    Tr2Result result = system_runtime_execute_acquisition_command(runtime, &request,
        &timestamp, &admission, entry);
    assert(admission.kind == COMMAND_ADMISSION_NEW);
    return result;
}

static CampaignId start(SystemRuntime *runtime, BulkComposition *bulk)
{
    CommandJournalEntry entry;
    assert(command(runtime, COMMAND_CODE_START_ACQUISITION, 101u, &entry) == TR2_OK);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
    assert(entry.has_final_result && entry.final_result.status == COMMAND_STATUS_SUCCESS);
    assert(campaign_service_campaign_open(&runtime->campaign_service));
    assert(campaign_service_acquisition_running(&runtime->campaign_service));
    CampaignId id = runtime->campaign_service.active_metadata.campaign_id;
    assert(bulk->store.campaign_active && bulk->store.active_campaign_id == id);
    return id;
}

static void acquire_window(SystemRuntime *runtime)
{
    CampaignAcquisitionStep step;
    for (unsigned i = 0u; i < WINDOW_SAMPLES; i++) {
        assert(system_runtime_drive_acquisition_step(runtime, &step) == TR2_OK);
        assert(step.kind == CAMPAIGN_ACQUISITION_STEP_SAMPLE_READ);
        assert(step.storage_result == TR2_OK);
    }
}

static void assert_records(BulkComposition *bulk, CampaignId id)
{
    CampaignBulkBlockReader reader;
    CampaignBulkBlockInfo info;
    const uint8_t *payload;
    uint8_t scratch[BLOCK_SCRATCH_SIZE];
    /* Independent, explicit expected encoding (including invalid-read samples). */
    static const uint8_t expected[16] = {
        0xFF, 0xFF, 0xFF, 0xFF, 2, 0, 0, 0,
        0xFD, 0xFF, 0xFF, 0xFF, 3, 0, 0, 0
    };
    assert(campaign_bulk_block_reader_init(&reader, &bulk->media, id,
        TR2_CAMPAIGN_BULK_METADATA_BYTES, 0u) == TR2_OK);
    assert(campaign_bulk_block_reader_next(&reader, scratch, sizeof(scratch),
        &info, &payload) == TR2_OK);
    assert(info.campaign_id == id && info.block_index == 0u);
    assert(info.payload_size == WINDOW_SAMPLES * 16u);
    for (unsigned i = 0u; i < WINDOW_SAMPLES; i++) {
        uint8_t record[16];
        memcpy(record, expected, sizeof(record));
        record[12] = i % 2u == 0u ? 3u : 2u;
        assert(memcmp(&payload[i * 16u], record, sizeof(record)) == 0);
    }
}

static void free_composition(RuntimeComposition *composition)
{
    free(composition->platform.persistent_candidate);
    free(composition->platform.persistent_committed);
}

/* A finished campaign, a checkpointed OPEN, and a flushed/unpublished OPEN. */
static void test_reboot(bool checkpoint, bool finish)
{
    FakeMedia fake = {0};
    RuntimeComposition composition;
    BulkComposition bulk_a, bulk_b;
    SystemRuntime runtime_a, runtime_b;
    CampaignAcquisitionStep step;
    CampaignBootRecoverySnapshot recovery;
    CampaignInventoryViewSnapshot inventory;
    CampaignBulkMetadata metadata;
    CampaignBulkMetadataRecoveryResult bulk_recovery;
    SupervisionSnapshot supervision;
    ModbusBlock3Image b3;
    CommandJournalEntry entry;
    uint8_t durable_before[MEDIA_SIZE];

    init_composition(&composition);
    init_bulk(&bulk_a, &fake);
    boot_runtime(&composition, &bulk_a, &runtime_a);
    commit_configuration(&runtime_a);
    CampaignId id = start(&runtime_a, &bulk_a);
    acquire_window(&runtime_a);
    assert(bulk_a.store.active_logical_bytes == WINDOW_SAMPLES * 16u);
    assert(bulk_a.store.active_durable_bytes == 0u);
    assert(!system_runtime_b3_image(&runtime_a, &b3));
    assert(!supervision_service_snapshot(&runtime_a.supervision_service, &supervision));
    if (checkpoint) {
        assert(system_runtime_drive_acquisition_step(&runtime_a, &step) == TR2_OK);
        assert(step.kind == CAMPAIGN_ACQUISITION_STEP_WINDOW_COMPLETED);
        assert(step.storage_result == TR2_OK && step.window.complete);
        assert(bulk_a.store.active_durable_bytes == WINDOW_SAMPLES * 16u);
        assert(supervision_service_snapshot(&runtime_a.supervision_service, &supervision));
        assert(supervision.configuration.config_id == 1u);
        assert(system_runtime_b3_image(&runtime_a, &b3));
        assert(b3.source_calculation_sequence == supervision.calculation_sequence);
        assert_records(&bulk_a, id);
    }
    if (finish) {
        assert(command(&runtime_a, COMMAND_CODE_STOP_ACQUISITION, 102u, &entry) == TR2_OK);
        assert(entry.lifecycle == COMMAND_LIFECYCLE_COMPLETED);
        assert(entry.has_final_result && entry.final_result.status == COMMAND_STATUS_SUCCESS);
        assert(!bulk_a.store.campaign_active);
        assert(!campaign_service_campaign_open(&runtime_a.campaign_service));
        assert(system_runtime_campaign_inventory_snapshot(&runtime_a, &inventory));
        assert(inventory.selected_campaign.lifecycle_state == CAMPAIGN_LIFECYCLE_CLOSED);
        assert(inventory.selected_campaign.durable_data_size_bytes == WINDOW_SAMPLES * 16u);
    }
    assert_no_historical_write(&composition);

    /* Logical power loss: discard volatile backend/runtime and unsynced media. */
    memcpy(fake.working, fake.durable, sizeof(fake.working));
    memcpy(composition.platform.persistent_candidate,
           composition.platform.persistent_committed, HOST_PLATFORM_PERSISTENT_BYTES);
    composition.platform.vibration_started = false;
    host_platform_set_reset_cause(&composition.platform, RESET_CAUSE_POWER_ON);
    memcpy(durable_before, fake.durable, sizeof(durable_before));
    init_bulk(&bulk_b, &fake);
    unsigned reads_before = fake.reads;
    boot_runtime(&composition, &bulk_b, &runtime_b);
    assert(fake.reads > reads_before); /* Boot actually read the new bulk media. */
    assert(runtime_b.deps.campaign_data_store != runtime_a.deps.campaign_data_store);
    assert(system_runtime_campaign_recovery_snapshot(&runtime_b, &recovery));
    assert(recovery.repository_status == CAMPAIGN_REPOSITORY_RECOVERY_VALID);
    assert(recovery.recovered_campaign_count == 1u);
    assert(recovery.campaigns[0].metadata.campaign_id == id);
    assert(recovery.campaigns[0].metadata.lifecycle_state ==
        (finish ? CAMPAIGN_LIFECYCLE_CLOSED : CAMPAIGN_LIFECYCLE_OPEN));
    assert(recovery.campaigns[0].data_recovery.status == CAMPAIGN_DATA_RECOVERY_VALID);
    assert(recovery.campaigns[0].data_recovery.durable_prefix_bytes ==
        (checkpoint ? WINDOW_SAMPLES * 16u : 0u));
    /* Inspect the existing A/B authority, including finish's persistent state. */
    assert(campaign_bulk_metadata_init(&metadata, &bulk_b.media, 0u,
        TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE) == TR2_OK);
    assert(campaign_bulk_metadata_recover(&metadata, &bulk_recovery) == TR2_OK);
    assert(bulk_recovery.status == CAMPAIGN_BULK_METADATA_RECOVERY_VALID);
    assert(bulk_recovery.descriptor.campaign_id == id);
    assert(bulk_recovery.descriptor.state ==
        (finish ? CAMPAIGN_BULK_METADATA_STATE_FINISHED : CAMPAIGN_BULK_METADATA_STATE_OPEN));
    assert(bulk_recovery.descriptor.durable_prefix_bytes ==
        recovery.campaigns[0].data_recovery.durable_prefix_bytes);
    if (!finish) {
        assert(!recovery.campaigns[0].metadata.end_timestamp.available);
        assert(!recovery.campaigns[0].metadata.duration.available);
    }
    assert(composition.platform.vibration_start_calls == 1u);
    assert(composition.platform.vibration_read_calls == WINDOW_SAMPLES);
    assert(!system_runtime_b3_image(&runtime_b, &b3));
    assert(system_runtime_drive_acquisition_step(&runtime_b, &step) == TR2_ERROR_INVALID_STATE);
    assert(memcmp(fake.durable, durable_before, sizeof(durable_before)) == 0);
    if (checkpoint) assert_records(&bulk_b, id);
    assert_no_historical_write(&composition);
    free_composition(&composition);
}

static void test_bulk_error(bool append_error)
{
    FakeMedia fake = {0};
    RuntimeComposition composition;
    BulkComposition bulk;
    SystemRuntime runtime;
    CampaignAcquisitionStep step;
    CampaignInventoryViewSnapshot inventory;
    CommandJournalEntry entry;
    ModbusBlock3Image b3;

    init_composition(&composition);
    init_bulk(&bulk, &fake);
    boot_runtime(&composition, &bulk, &runtime);
    commit_configuration(&runtime);
    start(&runtime, &bulk);
    if (append_error) {
        fake.write_result = TR2_ERROR_STORAGE;
        for (unsigned i = 0u; i < WINDOW_SAMPLES - 1u; i++)
            assert(system_runtime_drive_acquisition_step(&runtime, &step) == TR2_OK);
        assert(system_runtime_drive_acquisition_step(&runtime, &step) == TR2_ERROR_STORAGE);
        assert(step.kind == CAMPAIGN_ACQUISITION_STEP_SAMPLE_READ);
        assert(fake.failed_writes == 1u);
    } else {
        acquire_window(&runtime);
        fake.sync_result = TR2_ERROR_STORAGE;
        assert(system_runtime_drive_acquisition_step(&runtime, &step) == TR2_ERROR_STORAGE);
        assert(step.kind == CAMPAIGN_ACQUISITION_STEP_WINDOW_COMPLETED);
        assert(fake.failed_syncs == 1u);
    }
    assert(step.storage_result == TR2_ERROR_STORAGE);
    assert(bulk.store.active_durable_bytes == 0u);
    assert(!system_runtime_b3_image(&runtime, &b3));
    assert(campaign_data_store_bulk_recovery_required(&bulk.store));
    assert(runtime.campaign_service.data_store == campaign_data_store_bulk_interface(&bulk.store));
    /* No hidden retry/fallback can close the campaign or complete STOP. */
    assert(command(&runtime, COMMAND_CODE_STOP_ACQUISITION, 102u, &entry) == TR2_ERROR_INVALID_STATE);
    assert(entry.lifecycle == COMMAND_LIFECYCLE_STARTED && !entry.has_final_result);
    assert(campaign_service_campaign_open(&runtime.campaign_service));
    assert(system_runtime_campaign_inventory_snapshot(&runtime, &inventory));
    assert(inventory.selected_campaign.lifecycle_state == CAMPAIGN_LIFECYCLE_OPEN);
    assert(fake.failed_writes == (append_error ? 1u : 0u));
    assert(fake.failed_syncs == (append_error ? 0u : 1u));
    assert_no_historical_write(&composition);
    free_composition(&composition);
}

int main(void)
{
    test_reboot(true, true);
    test_reboot(true, false);
    test_reboot(false, false);
    test_bulk_error(true);
    test_bulk_error(false);
    return 0;
}
