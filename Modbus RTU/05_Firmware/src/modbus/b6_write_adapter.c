#include "tr2/modbus/write_adapter.h"

#include "tr2/modbus/codec.h"

#define TR2_B6_BASE_ADDRESS UINT16_C(6000)
#define TR2_B6_LAST_ADDRESS UINT16_C(6063)
#define TR2_B6_SELECTED_CAMPAIGN_INDEX_ADDRESS UINT16_C(6003)
#define TR2_B6_RESERVED_6A_FIRST UINT16_C(6010)
#define TR2_B6_RESERVED_6A_LAST UINT16_C(6011)
#define TR2_B6_RESERVED_6B_FIRST UINT16_C(6058)
#define TR2_B6_RESERVED_6B_LAST UINT16_C(6063)
#define TR2_B6_INVENTORY_STRUCTURE_VERSION UINT16_C(1)

static bool b6_address_is_reserved(uint16_t address)
{
    return (address >= TR2_B6_RESERVED_6A_FIRST && address <= TR2_B6_RESERVED_6A_LAST) ||
           (address >= TR2_B6_RESERVED_6B_FIRST && address <= TR2_B6_RESERVED_6B_LAST);
}

static ModbusAccessResult b6_validate_write(uint16_t start_address, uint16_t quantity)
{
    uint32_t offset;

    if (quantity == 0u) {
        return MODBUS_ACCESS_ILLEGAL_ADDRESS;
    }

    for (offset = 0u; offset < (uint32_t)quantity; ++offset) {
        const uint32_t address = (uint32_t)start_address + offset;

        if (address < TR2_B6_BASE_ADDRESS || address > TR2_B6_LAST_ADDRESS) {
            return MODBUS_ACCESS_ILLEGAL_ADDRESS;
        }
        if ((uint16_t)address == TR2_B6_SELECTED_CAMPAIGN_INDEX_ADDRESS) {
            continue;
        }
        if (b6_address_is_reserved((uint16_t)address)) {
            return MODBUS_ACCESS_RESERVED;
        }
        return MODBUS_ACCESS_READ_ONLY;
    }

    return MODBUS_ACCESS_OK;
}

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

    outcome.access_result = b6_validate_write(start_address, quantity);
    if (outcome.access_result != MODBUS_ACCESS_OK) {
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

#undef TR2_B6_BASE_ADDRESS
#undef TR2_B6_LAST_ADDRESS
#undef TR2_B6_SELECTED_CAMPAIGN_INDEX_ADDRESS
#undef TR2_B6_RESERVED_6A_FIRST
#undef TR2_B6_RESERVED_6A_LAST
#undef TR2_B6_RESERVED_6B_FIRST
#undef TR2_B6_RESERVED_6B_LAST
#undef TR2_B6_INVENTORY_STRUCTURE_VERSION
