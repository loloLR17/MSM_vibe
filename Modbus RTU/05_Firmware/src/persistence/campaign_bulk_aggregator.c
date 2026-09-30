#include "tr2/persistence/campaign_bulk_aggregator.h"

#include <string.h>

static Tr2Result write_buffer(CampaignBulkAggregator *aggregator, size_t size)
{
    Tr2Result result;

    result = campaign_bulk_media_write(aggregator->media,
                                       aggregator->write_offset,
                                       aggregator->buffer,
                                       size);
    if (result != TR2_OK) {
        aggregator->faulted = true;
        return result;
    }

    aggregator->write_offset += (uint64_t)size;
    aggregator->buffered_bytes = 0u;
    return TR2_OK;
}

Tr2Result campaign_bulk_aggregator_init(CampaignBulkAggregator *aggregator,
                                        CampaignBulkMedia *media,
                                        uint8_t *buffer,
                                        size_t buffer_capacity,
                                        uint64_t initial_offset)
{
    uint64_t capacity = 0u;
    Tr2Result result;

    if (aggregator == NULL || media == NULL || buffer == NULL ||
        buffer_capacity == 0u) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    result = campaign_bulk_media_capacity(media, &capacity);
    if (result != TR2_OK) {
        return result;
    }
    if (initial_offset > capacity ||
        (uint64_t)buffer_capacity > capacity - initial_offset) {
        return TR2_ERROR_NOT_AVAILABLE;
    }

    memset(aggregator, 0, sizeof(*aggregator));
    aggregator->media = media;
    aggregator->buffer = buffer;
    aggregator->buffer_capacity = buffer_capacity;
    aggregator->write_offset = initial_offset;
    aggregator->initialized = true;
    return TR2_OK;
}

Tr2Result campaign_bulk_aggregator_append(CampaignBulkAggregator *aggregator,
                                          const uint8_t *data,
                                          size_t size)
{
    size_t consumed = 0u;

    if (aggregator == NULL || !aggregator->initialized || aggregator->faulted) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (data == NULL && size != 0u) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    while (consumed < size) {
        size_t free_bytes = aggregator->buffer_capacity -
                            aggregator->buffered_bytes;
        size_t copy_size = size - consumed;
        Tr2Result result;

        if (copy_size > free_bytes) {
            copy_size = free_bytes;
        }

        memcpy(&aggregator->buffer[aggregator->buffered_bytes],
               &data[consumed],
               copy_size);
        aggregator->buffered_bytes += copy_size;
        consumed += copy_size;

        if (aggregator->buffered_bytes == aggregator->buffer_capacity) {
            result = write_buffer(aggregator, aggregator->buffer_capacity);
            if (result != TR2_OK) {
                return result;
            }
        }
    }

    return TR2_OK;
}

Tr2Result campaign_bulk_aggregator_flush(CampaignBulkAggregator *aggregator)
{
    if (aggregator == NULL || !aggregator->initialized || aggregator->faulted) {
        return TR2_ERROR_INVALID_STATE;
    }
    if (aggregator->buffered_bytes == 0u) {
        return TR2_OK;
    }

    return write_buffer(aggregator, aggregator->buffered_bytes);
}

size_t campaign_bulk_aggregator_buffered_bytes(
    const CampaignBulkAggregator *aggregator)
{
    return aggregator != NULL && aggregator->initialized
               ? aggregator->buffered_bytes
               : 0u;
}

uint64_t campaign_bulk_aggregator_write_offset(
    const CampaignBulkAggregator *aggregator)
{
    return aggregator != NULL && aggregator->initialized
               ? aggregator->write_offset
               : 0u;
}

bool campaign_bulk_aggregator_is_faulted(
    const CampaignBulkAggregator *aggregator)
{
    return aggregator != NULL && aggregator->initialized &&
           aggregator->faulted;
}
