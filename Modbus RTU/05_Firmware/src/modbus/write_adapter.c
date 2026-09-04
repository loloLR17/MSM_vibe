#include <stddef.h>
#include <string.h>

#include "tr2/modbus/codec.h"
#include "tr2/modbus/write_adapter.h"

#define TR2_B2_PREPARED_TIME_MSW_ADDRESS UINT16_C(2008)
#define TR2_B2_PREPARED_TIME_LSW_ADDRESS UINT16_C(2009)

#define TR2_B4_PREPARED_CONFIG_ID_MSW_ADDRESS UINT16_C(4002)
#define TR2_B4_PREPARED_CONFIG_ID_LSW_ADDRESS UINT16_C(4003)
#define TR2_B4_PREPARED_CONFIG_CRC_MSW_ADDRESS UINT16_C(4008)
#define TR2_B4_PREPARED_CONFIG_CRC_LSW_ADDRESS UINT16_C(4009)

static void b4_snapshot_or_zero(const ConfigurationStagingService *service,
                                PreparedConfiguration *prepared)
{
    if (!configuration_staging_snapshot(service, prepared)) {
        memset(prepared, 0, sizeof(*prepared));
    }
}

static void b4_write_u32_word(uint16_t address,
                              uint16_t value,
                              uint16_t msw_address,
                              uint16_t lsw_address,
                              uint32_t *target)
{
    uint16_t msw;
    uint16_t lsw;

    modbus_codec_u32_to_msw_lsw(*target, &msw, &lsw);
    if (address == msw_address) {
        msw = value;
    } else if (address == lsw_address) {
        lsw = value;
    }
    *target = modbus_codec_u32_from_msw_lsw(msw, lsw);
}

static Tr2Result b4_write_payload_register(ConfigurationPayload *payload,
                                           uint16_t address,
                                           uint16_t value)
{
    if (address == UINT16_C(4016)) {
        payload->sampling_frequency_hz = value;
    } else if (address == UINT16_C(4018)) {
        payload->axes_enable_mask = value;
    } else if (address == UINT16_C(4019)) {
        payload->full_scale_code = value;
    } else if (address == UINT16_C(4020)) {
        payload->acquisition_mode = value;
    } else if (address == UINT16_C(4021)) {
        payload->window_size_samples = value;
    } else if (address == UINT16_C(4022)) {
        payload->indicator_period_ms = value;
    } else if (address == UINT16_C(4023) || address == UINT16_C(4024)) {
        b4_write_u32_word(address, value, UINT16_C(4023), UINT16_C(4024),
                          &payload->campaign_duration_s);
    } else if (address == UINT16_C(4025)) {
        payload->storage_mode = value;
    } else if (address == UINT16_C(4026) || address == UINT16_C(4027)) {
        b4_write_u32_word(address, value, UINT16_C(4026), UINT16_C(4027),
                          &payload->storage_limit_mb);
    } else if (address == UINT16_C(4040)) {
        payload->supervision_enable_mask = value;
    } else if (address == UINT16_C(4041)) {
        payload->rms_warn_threshold_mg = value;
    } else if (address == UINT16_C(4042)) {
        payload->rms_alarm_threshold_mg = value;
    } else if (address == UINT16_C(4043)) {
        payload->peak_warn_threshold_mg = value;
    } else if (address == UINT16_C(4044)) {
        payload->peak_alarm_threshold_mg = value;
    } else if (address == UINT16_C(4045)) {
        payload->threshold_hysteresis_mg = value;
    } else if (address == UINT16_C(4046)) {
        payload->alarm_hold_time_ms = value;
    } else if (address == UINT16_C(4056) || address == UINT16_C(4057)) {
        b4_write_u32_word(address, value, UINT16_C(4056), UINT16_C(4057),
                          &payload->campaign_context_id);
    } else if (address == UINT16_C(4058) || address == UINT16_C(4059)) {
        b4_write_u32_word(address, value, UINT16_C(4058), UINT16_C(4059),
                          &payload->mission_id);
    } else if (address >= UINT16_C(4060) && address <= UINT16_C(4075)) {
        const uint16_t index = (uint16_t)(address - UINT16_C(4060));
        payload->campaign_label[(size_t)index * 2u] = (char)(uint8_t)(value >> 8u);
        payload->campaign_label[((size_t)index * 2u) + 1u] = (char)(uint8_t)value;
    } else if (address >= UINT16_C(4076) && address <= UINT16_C(4091)) {
        const uint16_t index = (uint16_t)(address - UINT16_C(4076));
        payload->mission_label[(size_t)index * 2u] = (char)(uint8_t)(value >> 8u);
        payload->mission_label[((size_t)index * 2u) + 1u] = (char)(uint8_t)value;
    } else if (address == UINT16_C(4092)) {
        payload->operating_mode_code = value;
    } else if (address == UINT16_C(4093)) {
        payload->navigation_zone_code = value;
    } else if (address == UINT16_C(4094)) {
        payload->load_state_code = value;
    } else if (address == UINT16_C(4095)) {
        payload->sea_state_code = value;
    } else {
        return TR2_ERROR_INTERNAL;
    }

    return TR2_OK;
}

ModbusWriteOutcome modbus_write_adapter_write_b2(TimeService *time_service,
                                                 uint16_t start_address,
                                                 const uint16_t *values,
                                                 uint16_t quantity)
{
    ModbusWriteOutcome outcome = { MODBUS_ACCESS_OK, TR2_OK };
    bool prepared_time_available = false;
    Tr2CivilTimestamp prepared_time = 0u;
    uint16_t msw = 0u;
    uint16_t lsw = 0u;
    uint32_t offset;

    if (time_service == NULL || values == NULL) {
        outcome.operation_result = TR2_ERROR_INVALID_ARGUMENT;
        return outcome;
    }

    outcome.access_result = modbus_register_model_validate_write(start_address, quantity);
    if (outcome.access_result != MODBUS_ACCESS_OK) {
        return outcome;
    }

    outcome.operation_result = time_service_get_prepared_time(time_service,
                                                              &prepared_time_available,
                                                              &prepared_time);
    if (outcome.operation_result != TR2_OK) {
        return outcome;
    }

    if (!prepared_time_available) {
        prepared_time = 0u;
    }
    modbus_codec_u32_to_msw_lsw(prepared_time, &msw, &lsw);

    for (offset = 0u; offset < (uint32_t)quantity; ++offset) {
        const uint16_t address = (uint16_t)((uint32_t)start_address + offset);
        if (address == TR2_B2_PREPARED_TIME_MSW_ADDRESS) {
            msw = values[offset];
        } else if (address == TR2_B2_PREPARED_TIME_LSW_ADDRESS) {
            lsw = values[offset];
        } else {
            outcome.operation_result = TR2_ERROR_INTERNAL;
            return outcome;
        }
    }

    outcome.operation_result = time_service_prepare_time(
        time_service,
        modbus_codec_u32_from_msw_lsw(msw, lsw));
    return outcome;
}

ModbusWriteOutcome modbus_write_adapter_write_b4(ConfigurationStagingService *staging_service,
                                                 uint16_t start_address,
                                                 const uint16_t *values,
                                                 uint16_t quantity)
{
    ModbusWriteOutcome outcome = { MODBUS_ACCESS_OK, TR2_OK };
    PreparedConfiguration candidate;
    uint32_t offset;

    if (staging_service == NULL || values == NULL) {
        outcome.operation_result = TR2_ERROR_INVALID_ARGUMENT;
        return outcome;
    }

    outcome.access_result = modbus_register_model_validate_write(start_address, quantity);
    if (outcome.access_result != MODBUS_ACCESS_OK) {
        return outcome;
    }

    b4_snapshot_or_zero(staging_service, &candidate);

    for (offset = 0u; offset < (uint32_t)quantity; ++offset) {
        const uint16_t address = (uint16_t)((uint32_t)start_address + offset);
        const uint16_t value = values[offset];

        if (address == TR2_B4_PREPARED_CONFIG_ID_MSW_ADDRESS ||
            address == TR2_B4_PREPARED_CONFIG_ID_LSW_ADDRESS) {
            b4_write_u32_word(address, value,
                              TR2_B4_PREPARED_CONFIG_ID_MSW_ADDRESS,
                              TR2_B4_PREPARED_CONFIG_ID_LSW_ADDRESS,
                              &candidate.config_id);
        } else if (address == TR2_B4_PREPARED_CONFIG_CRC_MSW_ADDRESS ||
                   address == TR2_B4_PREPARED_CONFIG_CRC_LSW_ADDRESS) {
            b4_write_u32_word(address, value,
                              TR2_B4_PREPARED_CONFIG_CRC_MSW_ADDRESS,
                              TR2_B4_PREPARED_CONFIG_CRC_LSW_ADDRESS,
                              &candidate.supplied_crc);
        } else {
            outcome.operation_result = b4_write_payload_register(&candidate.payload,
                                                                 address,
                                                                 value);
            if (outcome.operation_result != TR2_OK) {
                return outcome;
            }
        }
    }

    if (start_address == TR2_B4_PREPARED_CONFIG_ID_MSW_ADDRESS ||
        start_address == TR2_B4_PREPARED_CONFIG_ID_LSW_ADDRESS) {
        configuration_staging_set_config_id(staging_service, candidate.config_id);
    } else if (start_address == TR2_B4_PREPARED_CONFIG_CRC_MSW_ADDRESS ||
               start_address == TR2_B4_PREPARED_CONFIG_CRC_LSW_ADDRESS) {
        configuration_staging_set_supplied_crc(staging_service, candidate.supplied_crc);
    } else {
        configuration_staging_replace_payload(staging_service, &candidate.payload);
    }

    return outcome;
}

#undef TR2_B2_PREPARED_TIME_MSW_ADDRESS
#undef TR2_B2_PREPARED_TIME_LSW_ADDRESS
#undef TR2_B4_PREPARED_CONFIG_ID_MSW_ADDRESS
#undef TR2_B4_PREPARED_CONFIG_ID_LSW_ADDRESS
#undef TR2_B4_PREPARED_CONFIG_CRC_MSW_ADDRESS
#undef TR2_B4_PREPARED_CONFIG_CRC_LSW_ADDRESS
