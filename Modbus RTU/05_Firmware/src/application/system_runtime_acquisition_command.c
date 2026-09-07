#include "tr2/application/system_runtime.h"

#include <string.h>

#include "tr2/application/command_acknowledge_fault.h"
#include "tr2/application/command_maintenance.h"
#include "tr2/application/command_policy.h"
#include "tr2/application/command_start_acquisition.h"
#include "tr2/application/command_stop_acquisition.h"

#define TR2_B6_INVENTORY_STRUCTURE_VERSION UINT16_C(1)

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

    if (operation_result != TR2_OK) {
        return operation_result;
    }
    if (b3_result != TR2_OK) {
        return b3_result;
    }
    if (b5_result != TR2_OK) {
        return b5_result;
    }
    return b6_result;
}

Tr2Result system_runtime_execute_p9_command(
    SystemRuntime *runtime,
    const CommandRequest *request,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandAdmissionResult *out_admission,
    CommandJournalEntry *out_entry)
{
    Tr2Result operation_result;
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
    if (request->identity.command_code != COMMAND_CODE_ACKNOWLEDGE_FAULT &&
        request->identity.command_code != COMMAND_CODE_ENTER_MAINTENANCE &&
        request->identity.command_code != COMMAND_CODE_EXIT_MAINTENANCE) {
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

    switch (request->identity.command_code) {
    case COMMAND_CODE_ACKNOWLEDGE_FAULT:
        operation_result = command_acknowledge_fault_execute(
            &runtime->command_engine,
            &runtime->diagnostic_service,
            request->transaction_id,
            terminal_timestamp,
            out_entry);
        break;
    case COMMAND_CODE_ENTER_MAINTENANCE:
        operation_result = command_enter_maintenance_execute(
            &runtime->command_engine,
            &runtime->maintenance_service,
            campaign_service_acquisition_running(&runtime->campaign_service),
            request->transaction_id,
            terminal_timestamp,
            out_entry);
        break;
    case COMMAND_CODE_EXIT_MAINTENANCE:
        operation_result = command_exit_maintenance_execute(
            &runtime->command_engine,
            &runtime->maintenance_service,
            request->transaction_id,
            terminal_timestamp,
            out_entry);
        break;
    default:
        return TR2_ERROR_UNSUPPORTED;
    }

    b5_result = refresh_b5(runtime);
    if (operation_result != TR2_OK) {
        return operation_result;
    }
    return b5_result;
}

Tr2Result system_runtime_drive_acquisition_step(
    SystemRuntime *runtime,
    CampaignAcquisitionStep *out_step)
{
    Tr2Result operation_result;
    Tr2Result supervision_result;
    Tr2Result b3_result;

    if (runtime == NULL || out_step == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (!runtime->initialized || !runtime->system_ready_for_modbus ||
        !runtime->fg_runtime_available) {
        return TR2_ERROR_INVALID_STATE;
    }

    operation_result = campaign_service_drive_acquisition_step(
        &runtime->campaign_service,
        out_step);

    if (out_step->kind != CAMPAIGN_ACQUISITION_STEP_WINDOW_COMPLETED ||
        out_step->storage_result != TR2_OK) {
        return operation_result;
    }

    supervision_result = campaign_service_publish_supervision_step(
        &runtime->supervision_service,
        out_step);
    if (supervision_result == TR2_OK) {
        b3_result = refresh_b3(runtime);
    } else {
        b3_result = supervision_result;
    }

    if (operation_result != TR2_OK) {
        return operation_result;
    }
    return b3_result;
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

#undef TR2_B6_INVENTORY_STRUCTURE_VERSION
