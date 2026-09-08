#include "tr2/modbus/write_adapter.h"

#include "tr2/modbus/codec.h"

#define TR2_B6_SELECTED_CAMPAIGN_INDEX_ADDRESS UINT16_C(6003)
#define TR2_B6_INVENTORY_STRUCTURE_VERSION UINT16_C(1)

ModbusWriteOutcome modbus_write_adapter_write_b6(CampaignInventoryService *inventory_service,
                                                 ModbusBlock6Image *image,
                                                 uint16_t start_address,
                                                 const uint16_t *values,
                                                 uint16_t quantity)
{
    ModbusWriteOutcome outcome = { MODBUS_ACCESS_OK, TR2_OK };
    CampaignInventoryViewSnapshot snapshot;
    ModbusBlock6ProjectionSource source;
    ModbusBlock6Image candidate;

    if (inventory_service == NULL || image == NULL || values == NULL) {
        outcome.operation_result = TR2_ERROR_INVALID_ARGUMENT;
        return outcome;
    }

    outcome.access_result = modbus_register_model_validate_write(start_address, quantity);
    if (outcome.access_result != MODBUS_ACCESS_OK) {
        return outcome;
    }

    if (start_address != TR2_B6_SELECTED_CAMPAIGN_INDEX_ADDRESS || quantity != 1u) {
        outcome.access_result = MODBUS_ACCESS_ILLEGAL_ADDRESS;
        return outcome;
    }

    outcome.operation_result = campaign_inventory_service_select(inventory_service,
                                                                 values[0]);
    if (outcome.operation_result != TR2_OK) {
        return outcome;
    }

    outcome.operation_result = campaign_inventory_service_snapshot(inventory_service,
                                                                   &snapshot);
    if (outcome.operation_result != TR2_OK) {
        return outcome;
    }

    source.inventory_snapshot = &snapshot;
    source.inventory_structure_version = image->registers[0] != 0u
                                             ? image->registers[0]
                                             : TR2_B6_INVENTORY_STRUCTURE_VERSION;
    source.storage_used_mb = modbus_codec_u32_from_msw_lsw(image->registers[5],
                                                           image->registers[6]);
    source.storage_free_mb = modbus_codec_u32_from_msw_lsw(image->registers[7],
                                                           image->registers[8]);
    source.storage_health_status = image->registers[9];

    outcome.operation_result = modbus_project_b6(&source, &candidate);
    if (outcome.operation_result != TR2_OK) {
        return outcome;
    }

    *image = candidate;
    return outcome;
}

#undef TR2_B6_SELECTED_CAMPAIGN_INDEX_ADDRESS
#undef TR2_B6_INVENTORY_STRUCTURE_VERSION
