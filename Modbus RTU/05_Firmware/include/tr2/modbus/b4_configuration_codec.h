#ifndef TR2_MODBUS_B4_CONFIGURATION_CODEC_H
#define TR2_MODBUS_B4_CONFIGURATION_CODEC_H

#include <stddef.h>
#include <stdint.h>

#include "tr2/domain/configuration/configuration.h"

#define TR2_B4_PREPARED_REGISTER_COUNT 84u
#define TR2_B4_ACTIVE_REGISTER_COUNT 76u

void tr2_b4_serialize_prepared_payload(const ConfigurationPayload *payload,
                                       uint16_t registers[TR2_B4_PREPARED_REGISTER_COUNT]);

void tr2_b4_serialize_active_payload(const ConfigurationPayload *payload,
                                     uint16_t registers[TR2_B4_ACTIVE_REGISTER_COUNT]);

uint32_t tr2_b4_crc32_registers(const uint16_t *registers, size_t register_count);

uint32_t tr2_b4_prepared_payload_crc(const ConfigurationPayload *payload);
uint32_t tr2_b4_active_payload_crc(const ConfigurationPayload *payload);

#endif
