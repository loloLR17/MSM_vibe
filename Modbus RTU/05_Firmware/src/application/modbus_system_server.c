#include <string.h>

#include "tr2/application/modbus_system_server.h"

static Tr2Result refresh_context_selected(void *context, bool include_commands,
                                           bool request_context)
{
    ModbusSystemServer *server = context;
    SystemRuntime *runtime = server->binding.runtime;
    ModbusPduServerContext *pdu = &server->rtu.pdu_context;
    ModbusReadSources *sources = &pdu->read_sources;

    memset(pdu, 0, sizeof(*pdu));
    if (!system_runtime_is_ready_for_modbus(runtime)) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    /* Refresh authoritative dynamic status before projecting a fresh response.
     * Preserve explicit withdrawal: an unavailable B1 must not be resurrected. */
    if (runtime->b1_image_available && server->binding.refresh_status != NULL) {
        Tr2Result status = server->binding.refresh_status(runtime);
        if (status != TR2_OK) return status;
    }
    sources->b0_image = server->binding.identity;
    if (time_service_get_snapshot(&runtime->time_service, &server->time) == TR2_OK) {
        sources->time = &server->time;
        pdu->time_service = &runtime->time_service;
    }
    if (system_runtime_b1_image(runtime, &server->b1)) {
        sources->b1_image = &server->b1;
    }
    if (system_runtime_b3_image(runtime, &server->b3)) {
        sources->b3_image = &server->b3;
    }
    if (system_runtime_b4_image(runtime, &server->b4)) {
        sources->b4_image = &server->b4;
    }
    if (server->binding.configuration_workflow != NULL) {
        ConfigurationWorkflow *workflow = server->binding.configuration_workflow;
        PreparedConfiguration prepared;
        ActiveConfigurationSnapshot active;
        ModbusBlock4ProjectionSource projection = {0};
        bool has_prepared = configuration_staging_snapshot(workflow->staging, &prepared);
        bool has_active = configuration_service_active_snapshot(
            &runtime->configuration_service, &active);

        if (runtime->b4_image_available) {
            projection.config_structure_version = runtime->b4_image.registers[0];
            projection.config_capabilities_mask = runtime->b4_image.registers[1];
            projection.config_error_code = runtime->b4_image.registers[7];
        }
        projection.prepared = has_prepared ? &prepared : NULL;
        projection.active = has_active ? &active : NULL;
        projection.config_state = (uint16_t)configuration_workflow_state(workflow);
        if (modbus_project_b4(&projection, &server->b4) != TR2_OK) {
            return TR2_ERROR_INTERNAL;
        }
        sources->b4_image = &server->b4;
        pdu->configuration_staging = workflow->staging;
        pdu->configuration_workflow = workflow;
    }
    if (runtime->command_runtime_available && include_commands) {
        CommandSnapshot snapshot;
        ModbusBlock5ProjectionSource projection = {&runtime->command_mailbox, &snapshot};

        Tr2Result snapshot_result = server->binding.command_snapshot != NULL
            ? server->binding.command_snapshot(server->binding.command_submit_context, &snapshot)
            : command_engine_snapshot(&runtime->command_engine, &snapshot);
        if (snapshot_result != TR2_OK ||
            modbus_project_b5(&projection, &server->b5) != TR2_OK) {
            ++server->command_snapshot_failures;
            /* Initialization/start remain fail-fast. At request time, the
             * freshly cleared context exposes no stale B5 or write authority:
             * the existing read/write adapters return exception 04. */
            if (!request_context) return TR2_ERROR_INTERNAL;
        } else {
            sources->b5_image = &server->b5;
            /* Never acknowledge a B5 write whose submission would be discarded. */
            if (server->binding.command_submit != NULL) {
                pdu->command_mailbox = &runtime->command_mailbox;
                pdu->command_submit = server->binding.command_submit;
                pdu->command_submit_context = server->binding.command_submit_context;
            }
        }
    }
    if (runtime->b6_image_available &&
        campaign_inventory_service_is_initialized(&runtime->campaign_inventory_service)) {
        /* B6 writes update the same authoritative view subsequently read. */
        sources->b6_image = &runtime->b6_image;
        pdu->b6_image = &runtime->b6_image;
        pdu->campaign_inventory = &runtime->campaign_inventory_service;
    }
    if (system_runtime_b7_image(runtime, &server->b7)) {
        sources->b7_image = &server->b7;
    }
    return TR2_OK;
}

/* Initial binding/start still validate the complete context. */
static Tr2Result refresh_context(void *context)
{
    return refresh_context_selected(context, true, false);
}

static bool request_targets_b5(const ModbusRtuAduView *request)
{
    if (request == NULL || request->pdu_length < 5u) return false;
    const uint8_t *pdu = request->pdu;
    const uint16_t first = (uint16_t)((uint16_t)pdu[1] << 8u | pdu[2]);
    const uint16_t count = (uint16_t)((uint16_t)pdu[3] << 8u | pdu[4]);
    if (pdu[0] == MODBUS_PDU_FC_READ_HOLDING_REGISTERS) {
        if (request->pdu_length != 5u || count == 0u || count > 125u ||
            modbus_register_model_validate_read(first, count) != MODBUS_ACCESS_OK)
            return false;
    } else if (pdu[0] == MODBUS_PDU_FC_WRITE_MULTIPLE_REGISTERS) {
        if (count == 0u || count > 123u || request->pdu_length < 6u ||
            pdu[5] != (size_t)count * 2u ||
            request->pdu_length != 6u + (size_t)count * 2u ||
            modbus_register_model_validate_write(first, count) != MODBUS_ACCESS_OK)
            return false;
    } else {
        return false;
    }
    const ModbusRegisterDescriptor *descriptor = modbus_register_model_find(first);
    return descriptor != NULL && descriptor->block == MODBUS_BLOCK_5;
}

static Tr2Result refresh_request(void *context, const ModbusRtuAduView *request)
{
    ModbusSystemServer *server = context;
    /* Only a mapped, structurally valid B5 request can need its journal.
     * The PDU server retains all address/function/value validation and performs
     * no B5 mailbox mutation when the synchronous I/O authority is withdrawn. */
    const bool targets_b5 = request_targets_b5(request);
    const bool include_commands = targets_b5 &&
        (server->binding.command_io_available == NULL ||
         server->binding.command_io_available(server->binding.command_io_context));
    if (targets_b5 && !include_commands) ++server->command_io_denied_requests;
    return refresh_context_selected(context, include_commands, true);
}

Tr2Result modbus_system_server_init(ModbusSystemServer *server,
                                    const ModbusSystemServerBinding *binding)
{
    ModbusPduServerContext pdu = {0};
    Tr2Result result;

    if (server == NULL || binding == NULL || binding->runtime == NULL ||
        !serial_transport_is_valid(binding->transport) ||
        !modbus_rtu_server_runtime_unit_id_is_valid(binding->unit_id) ||
        (binding->configuration_workflow != NULL &&
         (binding->configuration_workflow->staging == NULL ||
          binding->configuration_workflow->integrity.compute_prepared_crc == NULL ||
          binding->configuration_workflow->activation.commit == NULL))) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memset(server, 0, sizeof(*server));
    if (!system_runtime_is_ready_for_modbus(binding->runtime)) {
        return TR2_ERROR_NOT_AVAILABLE;
    }
    server->binding = *binding;
    result = modbus_rtu_server_runtime_init(&server->rtu, binding->transport,
                                           binding->unit_id, &pdu);
    if (result == TR2_OK) {
        result = refresh_context(server);
    }
    server->rtu.prepare_context = server;
    server->rtu.prepare = NULL;
    server->rtu.prepare_request = refresh_request;
    server->initialized = result == TR2_OK;
    return result;
}

Tr2Result modbus_system_server_start(ModbusSystemServer *server)
{
    Tr2Result result;

    if (server == NULL || !server->initialized || server->started) {
        return TR2_ERROR_INVALID_STATE;
    }
    result = refresh_context(server);
    if (result == TR2_OK) {
        result = modbus_rtu_server_runtime_start(&server->rtu);
    }
    server->started = result == TR2_OK;
    return result;
}

Tr2Result modbus_system_server_poll_once(ModbusSystemServer *server)
{
    Tr2Result result;

    if (server == NULL || !server->initialized || !server->started) {
        return TR2_ERROR_INVALID_STATE;
    }
    result = system_runtime_is_ready_for_modbus(server->binding.runtime)
                 ? TR2_OK : TR2_ERROR_NOT_AVAILABLE;
    if (result != TR2_OK) {
        /* Explicit restart is required after application readiness is lost. */
        modbus_rtu_receiver_init(&server->rtu.receiver);
        server->started = false;
        return result;
    }
    return modbus_rtu_server_runtime_poll_once(&server->rtu);
}
