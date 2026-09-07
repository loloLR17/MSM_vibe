#include "tr2/modbus/projection.h"

#include <limits.h>
#include <string.h>

#include "tr2/modbus/codec.h"

#define TR2_B6_CAMPAIGN_STATE_RUNNING UINT16_C(2)
#define TR2_B6_CAMPAIGN_STATE_FINISHED UINT16_C(3)
#define TR2_B6_CAMPAIGN_STATE_ERROR UINT16_C(4)
#define TR2_B6_CAMPAIGN_STATE_PARTIAL UINT16_C(5)
#define TR2_B6_DATA_INTEGRITY_UNKNOWN UINT16_C(0)
#define TR2_B6_DATA_INTEGRITY_OK UINT16_C(1)
#define TR2_B6_DATA_INTEGRITY_CORRUPTED UINT16_C(2)
#define TR2_B6_DATA_INTEGRITY_PARTIAL UINT16_C(3)
#define TR2_BYTES_PER_MB UINT64_C(1000000)

static Tr2Result campaign_count_to_register(size_t count, uint16_t *output)
{
    if (output == NULL || count > UINT16_MAX) {
        return TR2_ERROR_INVALID_STATE;
    }
    *output = (uint16_t)count;
    return TR2_OK;
}

static Tr2Result data_size_mb(const CampaignMetadata *metadata, uint32_t *output)
{
    uint64_t megabytes;

    if (metadata == NULL || output == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    megabytes = metadata->durable_data_size_bytes / TR2_BYTES_PER_MB;
    if (megabytes > UINT32_MAX) {
        return TR2_ERROR_INVALID_STATE;
    }

    *output = (uint32_t)megabytes;
    return TR2_OK;
}

static uint16_t campaign_state(const CampaignMetadata *metadata)
{
    if (metadata->data_integrity == CAMPAIGN_DATA_INTEGRITY_PARTIAL) {
        return TR2_B6_CAMPAIGN_STATE_PARTIAL;
    }
    if (metadata->data_integrity == CAMPAIGN_DATA_INTEGRITY_CORRUPTED) {
        return TR2_B6_CAMPAIGN_STATE_ERROR;
    }
    return metadata->lifecycle_state == CAMPAIGN_LIFECYCLE_OPEN
               ? TR2_B6_CAMPAIGN_STATE_RUNNING
               : TR2_B6_CAMPAIGN_STATE_FINISHED;
}

static uint16_t data_integrity_status(CampaignDataIntegrity integrity)
{
    switch (integrity) {
    case CAMPAIGN_DATA_INTEGRITY_COMPLETE:
        return TR2_B6_DATA_INTEGRITY_OK;
    case CAMPAIGN_DATA_INTEGRITY_PARTIAL:
        return TR2_B6_DATA_INTEGRITY_PARTIAL;
    case CAMPAIGN_DATA_INTEGRITY_CORRUPTED:
        return TR2_B6_DATA_INTEGRITY_CORRUPTED;
    case CAMPAIGN_DATA_INTEGRITY_UNKNOWN:
    default:
        return TR2_B6_DATA_INTEGRITY_UNKNOWN;
    }
}

Tr2Result modbus_project_b6(const ModbusBlock6ProjectionSource *source,
                            ModbusBlock6Image *output)
{
    ModbusBlock6Image candidate = { { 0u }, 0u };
    const CampaignInventoryViewSnapshot *snapshot;
    const CampaignMetadata *metadata;
    uint32_t selected_data_size_mb;
    Tr2Result result;

    if (source == NULL || source->inventory_snapshot == NULL || output == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (source->storage_health_status > UINT16_C(3)) {
        return TR2_ERROR_INVALID_STATE;
    }

    snapshot = source->inventory_snapshot;
    result = campaign_count_to_register(snapshot->inventory.total_campaign_count,
                                        &candidate.registers[1]);
    if (result != TR2_OK) {
        return result;
    }
    result = campaign_count_to_register(snapshot->inventory.valid_campaign_count,
                                        &candidate.registers[2]);
    if (result != TR2_OK) {
        return result;
    }

    candidate.registers[0] = source->inventory_structure_version;
    candidate.registers[3] = snapshot->selected_campaign_index;
    candidate.registers[4] = snapshot->selected_campaign_valid ? UINT16_C(1) : UINT16_C(0);
    modbus_codec_u32_to_msw_lsw(source->storage_used_mb,
                                &candidate.registers[5],
                                &candidate.registers[6]);
    modbus_codec_u32_to_msw_lsw(source->storage_free_mb,
                                &candidate.registers[7],
                                &candidate.registers[8]);
    candidate.registers[9] = source->storage_health_status;

    if (!snapshot->selected_campaign_valid) {
        candidate.source_generation = snapshot->generation;
        *output = candidate;
        return TR2_OK;
    }

    metadata = &snapshot->selected_campaign;
    if (metadata->campaign_id == TR2_CAMPAIGN_ID_INVALID) {
        return TR2_ERROR_INVALID_STATE;
    }

    result = data_size_mb(metadata, &selected_data_size_mb);
    if (result != TR2_OK) {
        return result;
    }

    modbus_codec_u32_to_msw_lsw(metadata->campaign_id,
                                &candidate.registers[12],
                                &candidate.registers[13]);
    modbus_codec_u32_to_msw_lsw(metadata->mission_id,
                                &candidate.registers[14],
                                &candidate.registers[15]);
    if (metadata->start_timestamp.available) {
        modbus_codec_u32_to_msw_lsw(metadata->start_timestamp.value,
                                    &candidate.registers[16],
                                    &candidate.registers[17]);
    }
    if (metadata->end_timestamp.available) {
        modbus_codec_u32_to_msw_lsw(metadata->end_timestamp.value,
                                    &candidate.registers[18],
                                    &candidate.registers[19]);
    }
    candidate.registers[20] = campaign_state(metadata);
    if (metadata->duration.available) {
        modbus_codec_u32_to_msw_lsw(metadata->duration.seconds,
                                    &candidate.registers[21],
                                    &candidate.registers[22]);
    }
    modbus_codec_u32_to_msw_lsw(selected_data_size_mb,
                                &candidate.registers[23],
                                &candidate.registers[24]);

    if (!modbus_codec_ascii_fixed_encode(metadata->campaign_label,
                                         TR2_CAMPAIGN_LABEL_LENGTH,
                                         &candidate.registers[25],
                                         16u) ||
        !modbus_codec_ascii_fixed_encode(metadata->mission_label,
                                         TR2_CAMPAIGN_LABEL_LENGTH,
                                         &candidate.registers[41],
                                         16u)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    candidate.registers[57] = data_integrity_status(metadata->data_integrity);
    candidate.source_generation = snapshot->generation;
    *output = candidate;
    return TR2_OK;
}

#undef TR2_B6_CAMPAIGN_STATE_RUNNING
#undef TR2_B6_CAMPAIGN_STATE_FINISHED
#undef TR2_B6_CAMPAIGN_STATE_ERROR
#undef TR2_B6_CAMPAIGN_STATE_PARTIAL
#undef TR2_B6_DATA_INTEGRITY_UNKNOWN
#undef TR2_B6_DATA_INTEGRITY_OK
#undef TR2_B6_DATA_INTEGRITY_CORRUPTED
#undef TR2_B6_DATA_INTEGRITY_PARTIAL
#undef TR2_BYTES_PER_MB
