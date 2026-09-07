#include "tr2/application/system_runtime.h"

#include <string.h>

#include "tr2/application/command_acknowledge_fault.h"
#include "tr2/application/command_maintenance.h"
#include "tr2/application/command_policy.h"
#include "tr2/application/command_refresh_indicators.h"
#include "tr2/application/command_selftest.h"
#include "tr2/application/command_start_acquisition.h"
#include "tr2/application/command_stop_acquisition.h"

#define TR2_B6_INVENTORY_STRUCTURE_VERSION UINT16_C(1)
#define TR2_B1_STORAGE_AVAILABLE UINT16_C(1)
#define TR2_B1_ACQUISITION_STOPPED UINT16_C(0)
#define TR2_B1_ACQUISITION_RUNNING UINT16_C(1)

static uint16_t b1_reset_cause_from_platform(ResetCause cause)
{
    switch (cause) {
    case RESET_CAUSE_POWER_ON: return UINT16_C(1);
    case RESET_CAUSE_SOFTWARE: return UINT16_C(2);
    case RESET_CAUSE_WATCHDOG: return UINT16_C(3);
    case RESET_CAUSE_BROWNOUT: return UINT16_C(4);
    case RESET_CAUSE_EXTERNAL: return UINT16_C(5);
    case RESET_CAUSE_UNKNOWN:
    default: return UINT16_C(0);
    }
}

static Tr2Result collect_runtime_system_state(
    void *context,
    DiagnosticFacts *diagnostic_facts,
    SystemStateAggregationInput *aggregation_input)
{
    SystemRuntime *runtime = (SystemRuntime *)context;
    DiagnosticSnapshot diagnostic_snapshot;
    ActiveConfigurationSnapshot active_configuration;
    TimeSnapshot time_snapshot;
    const bool acquisition_active = runtime != NULL &&
        campaign_service_acquisition_running(&runtime->campaign_service);

    if (runtime == NULL || diagnostic_facts == NULL || aggregation_input == NULL ||
        !runtime->initialized || !runtime->system_ready_for_modbus ||
        !runtime->p9_authorities_available || !runtime->fg_runtime_available) {
        return TR2_ERROR_INVALID_STATE;
    }

    memset(diagnostic_facts, 0, sizeof(*diagnostic_facts));
    if (diagnostic_service_snapshot(&runtime->diagnostic_service, &diagnostic_snapshot)) {
        *diagnostic_facts = diagnostic_snapshot.facts;
    }

    memset(aggregation_input, 0, sizeof(*aggregation_input));
    aggregation_input->ready = true;
    aggregation_input->acquisition_active = acquisition_active;
    aggregation_input->active_configuration_valid =
        configuration_service_active_snapshot(&runtime->configuration_service,
                                              &active_configuration);

    if (time_service_get_snapshot(&runtime->time_service, &time_snapshot) == TR2_OK) {
        runtime->time_snapshot = time_snapshot;
        runtime->time_snapshot_available = true;
        aggregation_input->time_valid = time_snapshot.civil_time_usable;
    }

    aggregation_input->storage_available = true;
    aggregation_input->uptime_s = (uint32_t)(
        runtime->deps.monotonic_clock->now_ms(runtime->deps.monotonic_clock->context) /
        UINT64_C(1000));
    aggregation_input->last_reset_cause =
        b1_reset_cause_from_platform(runtime->boot_context.reset_cause);
    aggregation_input->storage_status = TR2_B1_STORAGE_AVAILABLE;
    aggregation_input->acquisition_state = acquisition_active
                                               ? TR2_B1_ACQUISITION_RUNNING
                                               : TR2_B1_ACQUISITION_STOPPED;
    if (campaign_service_campaign_open(&runtime->campaign_service)) {
        aggregation_input->active_campaign_id = runtime->campaign_service.active_metadata.campaign_id;
    }

    /* CPU, memory, storage usage and primary error/warning codes have no
       authoritative runtime producer yet. Keep their V1 projection neutral. */
    return TR2_OK;
}

static Tr2Result project_runtime_b1(SystemRuntime *runtime)
{
    ModbusBlock1ProjectionSource source;
    Tr2Result result;

    memset(&source, 0, sizeof(source));
    source.system_state = &runtime->system_state_snapshot;
    source.time = runtime->time_snapshot_available ? &runtime->time_snapshot : NULL;
    result = modbus_project_b1(&source, &runtime->b1_image);
    if (result != TR2_OK) {
        runtime->b1_image_available = false;
        return result;
    }

    runtime->system_state_snapshot_available = true;
    runtime->b1_image_available = true;
    return TR2_OK;
}

static Tr2Result project_runtime_b7(SystemRuntime *runtime)
{
    ModbusBlock7ProjectionSource source;
    Tr2Result result;

    memset(&source, 0, sizeof(source));
    source.diagnostic = &runtime->diagnostic_snapshot;
    source.uptime_s = runtime->system_state_snapshot.uptime_s;
    source.reset_cause = runtime->system_state_snapshot.last_reset_cause;
    result = modbus_project_b7(&source, &runtime->b7_image);
    if (result != TR2_OK) {
        runtime->b7_image_available = false;
        return result;
    }

    runtime->b7_image_available = true;
    return TR2_OK;
}

static Tr2Result project_runtime_status_images(SystemRuntime *runtime)
{
    Tr2Result result;

    result = project_runtime_b1(runtime);
    if (result != TR2_OK) {
        return result;
    }
    return project_runtime_b7(runtime);
}

static Tr2Result refresh_runtime_status(SystemRuntime *runtime)
{
    SystemStateRefreshSource refresh_source;
    Tr2Result result;

    refresh_source.context = runtime;
    refresh_source.collect = collect_runtime_system_state;
    result = system_state_aggregator_refresh(
        &runtime->system_state_aggregator,
        &runtime->diagnostic_service,
        &refresh_source,
        &runtime->diagnostic_snapshot,
        &runtime->system_state_snapshot);
    if (result != TR2_OK) {
        return result;
    }
    return project_runtime_status_images(runtime);
}

static Tr2Result refresh_b3(SystemRuntime *runtime)
{
    SupervisionSnapshot snapshot;
    Tr2Result result;

    if (!supervision_service_snapshot(&runtime->supervision_service, &snapshot)) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    result = modbus_project_b3(&snapshot, &runtime->b3_image);
    if (result != TR2_OK) {
        return result;
    }

    runtime->b3_image_available = true;
    return TR2_OK;
}

static Tr2Result refresh_b5(SystemRuntime *runtime)
{
    ModbusBlock5ProjectionSource source;
    CommandEngineFlagsSource flags_source;
    Tr2Result result;

    result = command_engine_snapshot(&runtime->command_engine,
                                     &runtime->command_snapshot);
    if (result != TR2_OK) {
        runtime->b5_image_available = false;
        return result;
    }

    memset(&flags_source, 0, sizeof(flags_source));
    flags_source.maintenance_active = maintenance_service_active(&runtime->maintenance_service);
    runtime->command_snapshot.engine_flags = command_engine_flags_project(&flags_source);

    memset(&source, 0, sizeof(source));
    source.mailbox = &runtime->command_mailbox;
    source.snapshot = &runtime->command_snapshot;
    result = modbus_project_b5(&source, &runtime->b5_image);
    if (result != TR2_OK) {
        runtime->b5_image_available = false;
        return result;
    }

    runtime->b5_image_available = true;
    return TR2_OK;
}

static Tr2Result refresh_b6(SystemRuntime *runtime)
{
    ModbusBlock6ProjectionSource source;
    Tr2Result result;

    runtime->campaign_inventory_snapshot_available = false;
    runtime->b6_image_available = false;

    if (!campaign_inventory_service_is_initialized(&runtime->campaign_inventory_service)) {
        return TR2_ERROR_INVALID_STATE;
    }

    result = campaign_inventory_service_snapshot(&runtime->campaign_inventory_service,
                                                 &runtime->campaign_inventory_snapshot);
    if (result != TR2_OK) {
        return result;
    }
    runtime->campaign_inventory_snapshot_available = true;

    memset(&source, 0, sizeof(source));
    source.inventory_snapshot = &runtime->campaign_inventory_snapshot;
    source.inventory_structure_version = TR2_B6_INVENTORY_STRUCTURE_VERSION;
    result = modbus_project_b6(&source, &runtime->b6_image);
    if (result != TR2_OK) {
        return result;
    }

    runtime->b6_image_available = true;
    return TR2_OK;
}

Tr2Result system_runtime_execute_acquisition_command(
    SystemRuntime *runtime,
    const CommandRequest *request,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandAdmissionResult *out_admission,
    CommandJournalEntry *out_entry)
{
    Tr2Result operation_result;
    Tr2Result b3_result = TR2_OK;
    Tr2Result b5_result;
    Tr2Result b6_result = TR2_OK;

    if (runtime == NULL || request == NULL || terminal_timestamp == NULL ||
        out_admission == NULL || out_entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!runtime->initialized || !runtime->system_ready_for_modbus ||
        !runtime->command_runtime_available || !runtime->fg_runtime_available) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (request->identity.command_code != COMMAND_CODE_START_ACQUISITION &&
        request->identity.command_code != COMMAND_CODE_STOP_ACQUISITION) {
        return TR2_ERROR_UNSUPPORTED;
    }

    memset(out_admission, 0, sizeof(*out_admission));
    memset(out_entry, 0, sizeof(*out_entry));

    operation_result = command_engine_admit(&runtime->command_engine,
                                            request,
                                            out_admission);
    if (operation_result != TR2_OK) {
        return operation_result;
    }
    *out_entry = out_admission->entry;

    if (out_admission->kind != COMMAND_ADMISSION_NEW) {
        return refresh_b5(runtime);
    }

    if (request->identity.command_code == COMMAND_CODE_START_ACQUISITION) {
        operation_result = command_start_acquisition_execute(
            &runtime->command_engine,
            &runtime->campaign_service,
            request->transaction_id,
            terminal_timestamp,
            out_entry);
    } else {
        operation_result = command_stop_acquisition_execute(
            &runtime->command_engine,
            &runtime->campaign_service,
            &runtime->supervision_service,
            request->transaction_id,
            terminal_timestamp,
            out_entry);
        if (operation_result == TR2_OK) {
            b3_result = refresh_b3(runtime);
            if (b3_result == TR2_ERROR_NOT_AVAILABLE) {
                b3_result = TR2_OK;
            }
        }
    }

    b5_result = refresh_b5(runtime);
    b6_result = refresh_b6(runtime);

    if (operation_result != TR2_OK) return operation_result;
    if (b3_result != TR2_OK) return b3_result;
    if (b5_result != TR2_OK) return b5_result;
    return b6_result;
}

Tr2Result system_runtime_execute_p9_command(
    SystemRuntime *runtime,
    const CommandRequest *request,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandAdmissionResult *out_admission,
    CommandJournalEntry *out_entry)
{
    SystemStateRefreshSource refresh_source;
    Tr2Result operation_result;
    Tr2Result status_result = TR2_OK;
    Tr2Result b5_result;

    if (runtime == NULL || request == NULL || terminal_timestamp == NULL ||
        out_admission == NULL || out_entry == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!runtime->initialized || !runtime->system_ready_for_modbus ||
        !runtime->command_runtime_available || !runtime->fg_runtime_available ||
        !runtime->p9_authorities_available) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (request->identity.command_code != COMMAND_CODE_SELFTEST &&
        request->identity.command_code != COMMAND_CODE_ACKNOWLEDGE_FAULT &&
        request->identity.command_code != COMMAND_CODE_REFRESH_INDICATORS &&
        request->identity.command_code != COMMAND_CODE_ENTER_MAINTENANCE &&
        request->identity.command_code != COMMAND_CODE_EXIT_MAINTENANCE &&
        request->identity.command_code != COMMAND_CODE_SOFTWARE_RESET) {
        return TR2_ERROR_UNSUPPORTED;
    }

    /* Platform capabilities are explicit and optional. An unavailable seam
       must not reserve a transaction or fabricate a business result. */
    if (request->identity.command_code == COMMAND_CODE_SELFTEST &&
        (runtime->deps.selftest_executor == NULL ||
         runtime->deps.selftest_executor->run_standard == NULL)) {
        return TR2_ERROR_NOT_AVAILABLE;
    }
    if (request->identity.command_code == COMMAND_CODE_SOFTWARE_RESET &&
        !platform_reset_trigger_is_valid(runtime->deps.reset_trigger)) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    memset(out_admission, 0, sizeof(*out_admission));
    memset(out_entry, 0, sizeof(*out_entry));

    operation_result = command_engine_admit(&runtime->command_engine,
                                            request,
                                            out_admission);
    if (operation_result != TR2_OK) return operation_result;
    *out_entry = out_admission->entry;

    if (out_admission->kind != COMMAND_ADMISSION_NEW) {
        return refresh_b5(runtime);
    }

    switch (request->identity.command_code) {
    case COMMAND_CODE_SELFTEST:
        operation_result = command_selftest_execute(
            &runtime->command_engine,
            &runtime->selftest_service,
            runtime->deps.selftest_executor,
            request->transaction_id,
            terminal_timestamp,
            out_entry);
        if (operation_result == TR2_OK) {
            status_result = refresh_runtime_status(runtime);
        }
        break;
    case COMMAND_CODE_ACKNOWLEDGE_FAULT:
        operation_result = command_acknowledge_fault_execute(
            &runtime->command_engine, &runtime->diagnostic_service,
            request->transaction_id, terminal_timestamp, out_entry);
        break;
    case COMMAND_CODE_REFRESH_INDICATORS:
        refresh_source.context = runtime;
        refresh_source.collect = collect_runtime_system_state;
        operation_result = command_refresh_indicators_execute(
            &runtime->command_engine,
            &runtime->diagnostic_service,
            &runtime->system_state_aggregator,
            &refresh_source,
            request->transaction_id,
            terminal_timestamp,
            &runtime->diagnostic_snapshot,
            &runtime->system_state_snapshot,
            out_entry);
        if (operation_result == TR2_OK) {
            status_result = project_runtime_status_images(runtime);
        }
        break;
    case COMMAND_CODE_ENTER_MAINTENANCE:
        operation_result = command_enter_maintenance_execute(
            &runtime->command_engine, &runtime->maintenance_service,
            campaign_service_acquisition_running(&runtime->campaign_service),
            request->transaction_id, terminal_timestamp, out_entry);
        break;
    case COMMAND_CODE_EXIT_MAINTENANCE:
        operation_result = command_exit_maintenance_execute(
            &runtime->command_engine, &runtime->maintenance_service,
            request->transaction_id, terminal_timestamp, out_entry);
        break;
    case COMMAND_CODE_SOFTWARE_RESET:
        operation_result = command_software_reset_execute(
            &runtime->command_engine,
            &runtime->boot_intent_store,
            runtime->deps.reset_trigger,
            campaign_service_acquisition_running(&runtime->campaign_service),
            selftest_service_running(&runtime->selftest_service),
            request->transaction_id,
            terminal_timestamp,
            out_entry);
        break;
    default:
        return TR2_ERROR_UNSUPPORTED;
    }

    b5_result = refresh_b5(runtime);
    if (operation_result != TR2_OK) return operation_result;
    if (status_result != TR2_OK) return status_result;
    return b5_result;
}

Tr2Result system_runtime_drive_acquisition_step(
    SystemRuntime *runtime,
    CampaignAcquisitionStep *out_step)
{
    Tr2Result operation_result;
    Tr2Result supervision_result;
    Tr2Result b3_result;

    if (runtime == NULL || out_step == NULL) return TR2_ERROR_INVALID_ARGUMENT;
    if (!runtime->initialized || !runtime->system_ready_for_modbus ||
        !runtime->fg_runtime_available) return TR2_ERROR_INVALID_STATE;

    operation_result = campaign_service_drive_acquisition_step(&runtime->campaign_service, out_step);
    if (out_step->kind != CAMPAIGN_ACQUISITION_STEP_WINDOW_COMPLETED ||
        out_step->storage_result != TR2_OK) return operation_result;

    supervision_result = campaign_service_publish_supervision_step(&runtime->supervision_service, out_step);
    b3_result = supervision_result == TR2_OK ? refresh_b3(runtime) : supervision_result;
    if (operation_result != TR2_OK) return operation_result;
    return b3_result;
}

bool system_runtime_b1_image(const SystemRuntime *runtime, ModbusBlock1Image *out_image)
{
    if (runtime == NULL || out_image == NULL || !runtime->initialized ||
        !runtime->system_ready_for_modbus || !runtime->b1_image_available ||
        !runtime->system_state_snapshot_available ||
        runtime->b1_image.source_generation != runtime->system_state_snapshot.generation) {
        return false;
    }
    *out_image = runtime->b1_image;
    return true;
}

bool system_runtime_b3_image(const SystemRuntime *runtime, ModbusBlock3Image *out_image)
{
    SupervisionSnapshot snapshot;

    if (runtime == NULL || out_image == NULL || !runtime->initialized ||
        !runtime->system_ready_for_modbus || !runtime->fg_runtime_available ||
        !runtime->b3_image_available ||
        !supervision_service_snapshot(&runtime->supervision_service, &snapshot) ||
        runtime->b3_image.source_calculation_sequence != snapshot.calculation_sequence) {
        return false;
    }

    *out_image = runtime->b3_image;
    return true;
}

bool system_runtime_b7_image(const SystemRuntime *runtime, ModbusBlock7Image *out_image)
{
    if (runtime == NULL || out_image == NULL || !runtime->initialized ||
        !runtime->system_ready_for_modbus || !runtime->b7_image_available ||
        !runtime->system_state_snapshot_available ||
        runtime->b7_image.source_generation != runtime->diagnostic_snapshot.generation) {
        return false;
    }
    *out_image = runtime->b7_image;
    return true;
}

#undef TR2_B1_ACQUISITION_RUNNING
#undef TR2_B1_ACQUISITION_STOPPED
#undef TR2_B1_STORAGE_AVAILABLE
#undef TR2_B6_INVENTORY_STRUCTURE_VERSION