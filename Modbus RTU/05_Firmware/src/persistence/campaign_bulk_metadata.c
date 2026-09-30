#include "tr2/persistence/campaign_bulk_metadata.h"

#include <string.h>

#define CRC_OFFSET 508u
#define STATE_OFFSET 20u
#define DATA_BASE_OFFSET 24u
#define DURABLE_PREFIX_OFFSET 32u

typedef enum {
    RECORD_EMPTY = 0,
    RECORD_VALID,
    RECORD_UNSUPPORTED,
    RECORD_BAD,
    RECORD_IO
} RecordState;

typedef struct {
    RecordState state;
    CampaignBulkDescriptor descriptor;
} RecordInfo;

static void put_u16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v >> 8u);
    p[1] = (uint8_t)v;
}

static void put_u32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24u);
    p[1] = (uint8_t)(v >> 16u);
    p[2] = (uint8_t)(v >> 8u);
    p[3] = (uint8_t)v;
}

static void put_u64(uint8_t *p, uint64_t v)
{
    put_u32(p, (uint32_t)(v >> 32u));
    put_u32(p + 4u, (uint32_t)v);
}

static uint16_t get_u16(const uint8_t *p)
{
    return (uint16_t)(((uint16_t)p[0] << 8u) | p[1]);
}

static uint32_t get_u32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24u) |
           ((uint32_t)p[1] << 16u) |
           ((uint32_t)p[2] << 8u) |
           p[3];
}

static uint64_t get_u64(const uint8_t *p)
{
    return ((uint64_t)get_u32(p) << 32u) | get_u32(p + 4u);
}

static uint32_t crc32_bytes(const uint8_t *p, size_t n)
{
    uint32_t crc = UINT32_C(0xFFFFFFFF);
    size_t i;

    for (i = 0u; i < n; ++i) {
        uint8_t bit;
        crc ^= p[i];
        for (bit = 0u; bit < 8u; ++bit) {
            crc = (crc & 1u) != 0u
                      ? (crc >> 1u) ^ UINT32_C(0xEDB88320)
                      : crc >> 1u;
        }
    }
    return crc ^ UINT32_C(0xFFFFFFFF);
}

static bool uniform(const uint8_t *p, uint8_t value)
{
    size_t i;
    for (i = 0u; i < TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE; ++i) {
        if (p[i] != value) {
            return false;
        }
    }
    return true;
}

static bool descriptor_fields_valid(const CampaignBulkDescriptor *d)
{
    return d->generation != 0u &&
           d->generation != UINT64_MAX &&
           d->campaign_id != TR2_CAMPAIGN_ID_INVALID &&
           (d->state == CAMPAIGN_BULK_METADATA_STATE_OPEN ||
            d->state == CAMPAIGN_BULK_METADATA_STATE_FINISHED) &&
           d->durable_prefix_bytes <= UINT64_MAX - d->data_base;
}

Tr2Result campaign_bulk_descriptor_encode(
    const CampaignBulkDescriptor *descriptor,
    uint8_t output[TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE])
{
    if (descriptor == NULL || output == NULL ||
        !descriptor_fields_valid(descriptor)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(output, 0, TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE);
    put_u32(&output[0], TR2_CAMPAIGN_BULK_DESCRIPTOR_MAGIC);
    put_u16(&output[4], TR2_CAMPAIGN_BULK_DESCRIPTOR_VERSION);
    put_u16(&output[6], (uint16_t)TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE);
    put_u64(&output[8], descriptor->generation);
    put_u32(&output[16], descriptor->campaign_id);
    put_u16(&output[STATE_OFFSET], (uint16_t)descriptor->state);
    put_u64(&output[DATA_BASE_OFFSET], descriptor->data_base);
    put_u64(&output[DURABLE_PREFIX_OFFSET],
            descriptor->durable_prefix_bytes);
    put_u32(&output[CRC_OFFSET], crc32_bytes(output, CRC_OFFSET));
    return TR2_OK;
}

static RecordState decode_record(const uint8_t record[TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE],
                                 CampaignBulkDescriptor *descriptor)
{
    size_t i;

    memset(descriptor, 0, sizeof(*descriptor));
    if (uniform(record, 0x00u) || uniform(record, 0xFFu)) {
        return RECORD_EMPTY;
    }
    if (get_u32(&record[0]) != TR2_CAMPAIGN_BULK_DESCRIPTOR_MAGIC ||
        get_u16(&record[6]) != TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE) {
        return RECORD_BAD;
    }
    if (get_u16(&record[4]) != TR2_CAMPAIGN_BULK_DESCRIPTOR_VERSION) {
        return RECORD_UNSUPPORTED;
    }
    if (get_u32(&record[CRC_OFFSET]) != crc32_bytes(record, CRC_OFFSET)) {
        return RECORD_BAD;
    }
    if (get_u16(&record[22]) != 0u) {
        return RECORD_BAD;
    }
    for (i = 40u; i < CRC_OFFSET; ++i) {
        if (record[i] != 0u) {
            return RECORD_BAD;
        }
    }

    descriptor->generation = get_u64(&record[8]);
    descriptor->campaign_id = get_u32(&record[16]);
    descriptor->state =
        (CampaignBulkMetadataState)get_u16(&record[STATE_OFFSET]);
    descriptor->data_base = get_u64(&record[DATA_BASE_OFFSET]);
    descriptor->durable_prefix_bytes =
        get_u64(&record[DURABLE_PREFIX_OFFSET]);

    return descriptor_fields_valid(descriptor) ? RECORD_VALID : RECORD_BAD;
}

static RecordState read_record(CampaignBulkMetadata *metadata,
                               uint8_t copy,
                               CampaignBulkDescriptor *descriptor)
{
    uint8_t record[TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE];

    if (campaign_bulk_media_read(metadata->media,
                                 metadata->descriptor_offsets[copy],
                                 record,
                                 sizeof(record)) != TR2_OK) {
        return RECORD_IO;
    }
    return decode_record(record, descriptor);
}

Tr2Result campaign_bulk_metadata_init(CampaignBulkMetadata *metadata,
                                      CampaignBulkMedia *media,
                                      uint64_t descriptor_a_offset,
                                      uint64_t descriptor_b_offset)
{
    uint64_t capacity = 0u;
    Tr2Result result;

    if (metadata == NULL || media == NULL ||
        descriptor_a_offset == descriptor_b_offset) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    result = campaign_bulk_media_capacity(media, &capacity);
    if (result != TR2_OK) {
        return result;
    }
    if (descriptor_a_offset > capacity ||
        TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE > capacity - descriptor_a_offset ||
        descriptor_b_offset > capacity ||
        TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE > capacity - descriptor_b_offset) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    memset(metadata, 0, sizeof(*metadata));
    metadata->media = media;
    metadata->descriptor_offsets[0] = descriptor_a_offset;
    metadata->descriptor_offsets[1] = descriptor_b_offset;
    metadata->initialized = true;
    return TR2_OK;
}

Tr2Result campaign_bulk_metadata_publish(
    CampaignBulkMetadata *metadata,
    const CampaignBulkDescriptor *descriptor)
{
    uint8_t record[TR2_CAMPAIGN_BULK_DESCRIPTOR_SIZE];
    CampaignBulkDescriptor verified;
    uint8_t target;
    RecordState state;
    Tr2Result result;

    if (metadata == NULL || !metadata->initialized ||
        metadata->recovery_required || descriptor == NULL) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (metadata->authority_loaded) {
        if (metadata->generation >= UINT64_MAX - 1u ||
            descriptor->generation != metadata->generation + 1u) {
            return TR2_ERROR_INVALID_ARGUMENT;
        }
        target = (uint8_t)(1u - metadata->active_copy);
    } else {
        if (descriptor->generation != 1u) {
            return TR2_ERROR_INVALID_ARGUMENT;
        }
        target = 0u;
    }

    result = campaign_bulk_descriptor_encode(descriptor, record);
    if (result != TR2_OK) {
        return result;
    }
    result = campaign_bulk_media_write(metadata->media,
                                       metadata->descriptor_offsets[target],
                                       record,
                                       sizeof(record));
    if (result != TR2_OK) {
        metadata->recovery_required = true;
        return result;
    }
    result = campaign_bulk_media_sync(metadata->media);
    if (result != TR2_OK) {
        metadata->recovery_required = true;
        return result;
    }
    state = read_record(metadata, target, &verified);
    if (state != RECORD_VALID ||
        memcmp(&verified, descriptor, sizeof(verified)) != 0) {
        metadata->recovery_required = true;
        return TR2_ERROR_STORAGE;
    }

    metadata->generation = descriptor->generation;
    metadata->active_copy = target;
    metadata->authority_loaded = true;
    return TR2_OK;
}

Tr2Result campaign_bulk_metadata_recover(
    CampaignBulkMetadata *metadata,
    CampaignBulkMetadataRecoveryResult *result)
{
    RecordInfo records[2];
    uint8_t chosen;

    if (metadata == NULL || !metadata->initialized || result == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    memset(result, 0, sizeof(*result));
    records[0].state = read_record(metadata, 0u, &records[0].descriptor);
    records[1].state = read_record(metadata, 1u, &records[1].descriptor);

    if (records[0].state == RECORD_VALID &&
        records[1].state == RECORD_VALID) {
        if (records[0].descriptor.generation ==
            records[1].descriptor.generation) {
            if (memcmp(&records[0].descriptor,
                       &records[1].descriptor,
                       sizeof(CampaignBulkDescriptor)) != 0) {
                result->status = CAMPAIGN_BULK_METADATA_RECOVERY_CORRUPTED;
                return TR2_OK;
            }
            chosen = 0u;
        } else {
            chosen = records[1].descriptor.generation >
                             records[0].descriptor.generation
                         ? 1u
                         : 0u;
        }
    } else if (records[0].state == RECORD_VALID) {
        chosen = 0u;
    } else if (records[1].state == RECORD_VALID) {
        chosen = 1u;
    } else {
        if (records[0].state == RECORD_IO ||
            records[1].state == RECORD_IO) {
            result->status = CAMPAIGN_BULK_METADATA_RECOVERY_UNAVAILABLE;
        } else if (records[0].state == RECORD_UNSUPPORTED ||
                   records[1].state == RECORD_UNSUPPORTED) {
            result->status = CAMPAIGN_BULK_METADATA_RECOVERY_UNSUPPORTED;
        } else if (records[0].state == RECORD_EMPTY &&
                   records[1].state == RECORD_EMPTY) {
            result->status = CAMPAIGN_BULK_METADATA_RECOVERY_EMPTY;
        } else {
            result->status = CAMPAIGN_BULK_METADATA_RECOVERY_CORRUPTED;
        }
        return TR2_OK;
    }

    metadata->generation = records[chosen].descriptor.generation;
    metadata->active_copy = chosen;
    metadata->authority_loaded = true;
    metadata->recovery_required = false;
    result->status = CAMPAIGN_BULK_METADATA_RECOVERY_VALID;
    result->descriptor = records[chosen].descriptor;
    result->active_copy = chosen;
    return TR2_OK;
}

bool campaign_bulk_metadata_recovery_required(
    const CampaignBulkMetadata *metadata)
{
    return metadata != NULL && metadata->initialized &&
           metadata->recovery_required;
}
