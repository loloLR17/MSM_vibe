#include "tr2/persistence/campaign_bulk_block_stream.h"

#include <string.h>

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

static bool extent_fits(uint64_t capacity, uint64_t offset, size_t size)
{
    return offset <= capacity && (uint64_t)size <= capacity - offset;
}

static bool offset_is_aligned(uint64_t offset)
{
    return offset % (uint64_t)TR2_CAMPAIGN_BULK_PHYSICAL_ALIGNMENT == 0u;
}

Tr2Result campaign_bulk_block_physical_extent(size_t encoded_size,
                                               size_t *physical_extent)
{
    size_t remainder;
    size_t padding;

    if (physical_extent == NULL || encoded_size == 0u) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    remainder = encoded_size % TR2_CAMPAIGN_BULK_PHYSICAL_ALIGNMENT;
    if (remainder == 0u) {
        *physical_extent = encoded_size;
        return TR2_OK;
    }

    padding = TR2_CAMPAIGN_BULK_PHYSICAL_ALIGNMENT - remainder;
    if (encoded_size > SIZE_MAX - padding) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    *physical_extent = encoded_size + padding;
    return TR2_OK;
}

Tr2Result campaign_bulk_block_writer_init(
    CampaignBulkBlockWriter *writer,
    CampaignBulkMedia *media,
    CampaignId campaign_id,
    uint64_t initial_offset,
    uint64_t initial_block_index)
{
    uint64_t capacity;
    Tr2Result result;

    if (writer == NULL || media == NULL ||
        campaign_id == TR2_CAMPAIGN_ID_INVALID ||
        initial_block_index == UINT64_MAX ||
        !offset_is_aligned(initial_offset)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = campaign_bulk_media_capacity(media, &capacity);
    if (result != TR2_OK) {
        return result;
    }
    if (initial_offset > capacity) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    memset(writer, 0, sizeof(*writer));
    writer->media = media;
    writer->campaign_id = campaign_id;
    writer->next_offset = initial_offset;
    writer->next_block_index = initial_block_index;
    writer->initialized = true;
    return TR2_OK;
}

Tr2Result campaign_bulk_block_writer_append(
    CampaignBulkBlockWriter *writer,
    const uint8_t *payload,
    size_t payload_size,
    uint8_t *scratch,
    size_t scratch_capacity)
{
    uint64_t capacity;
    size_t encoded_size;
    size_t physical_extent;
    Tr2Result result;

    if (writer == NULL || !writer->initialized || writer->faulted) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (writer->next_block_index == UINT64_MAX) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    result = campaign_bulk_block_encode(writer->campaign_id,
                                        writer->next_block_index,
                                        payload,
                                        payload_size,
                                        scratch,
                                        scratch_capacity,
                                        &encoded_size);
    if (result != TR2_OK) {
        return result;
    }

    result = campaign_bulk_block_physical_extent(encoded_size, &physical_extent);
    if (result != TR2_OK) {
        return result;
    }

    result = campaign_bulk_media_capacity(writer->media, &capacity);
    if (result != TR2_OK) {
        writer->faulted = true;
        return result;
    }
    if (!extent_fits(capacity, writer->next_offset, physical_extent)) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    result = campaign_bulk_media_write(writer->media,
                                       writer->next_offset,
                                       scratch,
                                       encoded_size);
    if (result != TR2_OK) {
        writer->faulted = true;
        return result;
    }

    writer->next_offset += (uint64_t)physical_extent;
    writer->next_block_index += 1u;
    return TR2_OK;
}

uint64_t campaign_bulk_block_writer_next_offset(
    const CampaignBulkBlockWriter *writer)
{
    return writer != NULL && writer->initialized ? writer->next_offset : 0u;
}

uint64_t campaign_bulk_block_writer_next_index(
    const CampaignBulkBlockWriter *writer)
{
    return writer != NULL && writer->initialized ? writer->next_block_index : 0u;
}

bool campaign_bulk_block_writer_is_faulted(
    const CampaignBulkBlockWriter *writer)
{
    return writer != NULL && writer->initialized && writer->faulted;
}

Tr2Result campaign_bulk_block_reader_init(
    CampaignBulkBlockReader *reader,
    CampaignBulkMedia *media,
    CampaignId campaign_id,
    uint64_t initial_offset,
    uint64_t initial_block_index)
{
    uint64_t capacity;
    Tr2Result result;

    if (reader == NULL || media == NULL ||
        campaign_id == TR2_CAMPAIGN_ID_INVALID ||
        initial_block_index == UINT64_MAX ||
        !offset_is_aligned(initial_offset)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = campaign_bulk_media_capacity(media, &capacity);
    if (result != TR2_OK) {
        return result;
    }
    if (initial_offset > capacity) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    memset(reader, 0, sizeof(*reader));
    reader->media = media;
    reader->campaign_id = campaign_id;
    reader->next_offset = initial_offset;
    reader->next_block_index = initial_block_index;
    reader->initialized = true;
    return TR2_OK;
}

Tr2Result campaign_bulk_block_reader_next(
    CampaignBulkBlockReader *reader,
    uint8_t *scratch,
    size_t scratch_capacity,
    CampaignBulkBlockInfo *info,
    const uint8_t **payload)
{
    uint8_t header[TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE];
    uint64_t capacity;
    uint32_t payload_size;
    size_t encoded_size;
    size_t physical_extent;
    CampaignBulkBlockInfo decoded;
    const uint8_t *decoded_payload;
    Tr2Result result;

    if (reader == NULL || !reader->initialized || reader->faulted) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (scratch == NULL || info == NULL || payload == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    memset(info, 0, sizeof(*info));
    *payload = NULL;

    result = campaign_bulk_media_capacity(reader->media, &capacity);
    if (result != TR2_OK) {
        reader->faulted = true;
        return result;
    }
    if (!extent_fits(capacity,
                     reader->next_offset,
                     TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE)) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    result = campaign_bulk_media_read(reader->media,
                                      reader->next_offset,
                                      header,
                                      sizeof(header));
    if (result != TR2_OK) {
        reader->faulted = true;
        return result;
    }

    if (get_u32(&header[0]) != TR2_CAMPAIGN_BULK_BLOCK_MAGIC ||
        get_u16(&header[6]) != TR2_CAMPAIGN_BULK_BLOCK_HEADER_SIZE) {
        return TR2_ERROR_CORRUPTED;
    }
    if (get_u16(&header[4]) != TR2_CAMPAIGN_BULK_BLOCK_VERSION) {
        return TR2_ERROR_UNSUPPORTED;
    }

    payload_size = get_u32(&header[20]);
    result = campaign_bulk_block_encoded_size((size_t)payload_size,
                                              &encoded_size);
    if (result != TR2_OK) {
        return TR2_ERROR_CORRUPTED;
    }
    result = campaign_bulk_block_physical_extent(encoded_size, &physical_extent);
    if (result != TR2_OK) {
        return TR2_ERROR_CORRUPTED;
    }
    if (scratch_capacity < encoded_size) {
        return TR2_ERROR_NOT_AVAILABLE;
    }
    if (!extent_fits(capacity, reader->next_offset, physical_extent)) {
        return TR2_ERROR_CORRUPTED;
    }

    result = campaign_bulk_media_read(reader->media,
                                      reader->next_offset,
                                      scratch,
                                      encoded_size);
    if (result != TR2_OK) {
        reader->faulted = true;
        return result;
    }

    result = campaign_bulk_block_decode(scratch,
                                        encoded_size,
                                        &decoded,
                                        &decoded_payload);
    if (result != TR2_OK) {
        return result;
    }
    if (decoded.campaign_id != reader->campaign_id ||
        decoded.block_index != reader->next_block_index) {
        return TR2_ERROR_CORRUPTED;
    }

    *info = decoded;
    *payload = decoded_payload;
    reader->next_offset += (uint64_t)physical_extent;
    reader->next_block_index += 1u;
    return TR2_OK;
}

uint64_t campaign_bulk_block_reader_next_offset(
    const CampaignBulkBlockReader *reader)
{
    return reader != NULL && reader->initialized ? reader->next_offset : 0u;
}

uint64_t campaign_bulk_block_reader_next_index(
    const CampaignBulkBlockReader *reader)
{
    return reader != NULL && reader->initialized ? reader->next_block_index : 0u;
}

bool campaign_bulk_block_reader_is_faulted(
    const CampaignBulkBlockReader *reader)
{
    return reader != NULL && reader->initialized && reader->faulted;
}
