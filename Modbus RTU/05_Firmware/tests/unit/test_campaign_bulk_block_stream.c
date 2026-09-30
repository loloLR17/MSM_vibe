#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/campaign_bulk_block_stream.h"

typedef struct {
    uint8_t bytes[512];
    Tr2Result capacity_result;
    Tr2Result read_result;
    Tr2Result write_result;
    uint32_t read_calls;
    uint32_t write_calls;
} FakeMedia;

static Tr2Result fake_capacity(void *context, uint64_t *capacity)
{
    FakeMedia *fake = context;
    if (fake->capacity_result != TR2_OK) return fake->capacity_result;
    *capacity = sizeof(fake->bytes);
    return TR2_OK;
}

static Tr2Result fake_read(void *context,uint64_t offset,void *buffer,size_t size)
{
    FakeMedia *fake = context;
    fake->read_calls += 1u;
    if (fake->read_result != TR2_OK) return fake->read_result;
    if (offset > sizeof(fake->bytes) ||
        size > sizeof(fake->bytes) - (size_t)offset) return TR2_ERROR_STORAGE;
    memcpy(buffer, &fake->bytes[(size_t)offset], size);
    return TR2_OK;
}

static Tr2Result fake_write(void *context,uint64_t offset,const void *buffer,size_t size)
{
    FakeMedia *fake = context;
    fake->write_calls += 1u;
    if (fake->write_result != TR2_OK) return fake->write_result;
    if (offset > sizeof(fake->bytes) ||
        size > sizeof(fake->bytes) - (size_t)offset) return TR2_ERROR_STORAGE;
    memcpy(&fake->bytes[(size_t)offset], buffer, size);
    return TR2_OK;
}

static Tr2Result fake_sync(void *context)
{
    (void)context;
    return TR2_OK;
}

static CampaignBulkMedia make_media(FakeMedia *fake)
{
    CampaignBulkMedia media = {
        fake, fake_capacity, fake_read, fake_write, fake_sync
    };
    return media;
}

static void fill(uint8_t *bytes, size_t size, uint8_t seed)
{
    size_t i;
    for (i = 0u; i < size; ++i) bytes[i] = (uint8_t)(seed + i);
}

static void test_writer_reader_round_trip_and_continuity(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    CampaignBulkBlockWriter writer;
    CampaignBulkBlockReader reader;
    uint8_t p0[16];
    uint8_t p1[32];
    uint8_t scratch[128];
    CampaignBulkBlockInfo info;
    const uint8_t *payload;
    uint64_t after_first;

    fill(p0, sizeof(p0), 1u);
    fill(p1, sizeof(p1), 33u);

    assert(campaign_bulk_block_writer_init(&writer, &media, 7u, 64u, 0u) == TR2_OK);
    assert(campaign_bulk_block_writer_append(&writer, p0, sizeof(p0),
                                             scratch, sizeof(scratch)) == TR2_OK);
    after_first = campaign_bulk_block_writer_next_offset(&writer);
    assert(campaign_bulk_block_writer_next_index(&writer) == 1u);
    assert(campaign_bulk_block_writer_append(&writer, p1, sizeof(p1),
                                             scratch, sizeof(scratch)) == TR2_OK);
    assert(campaign_bulk_block_writer_next_index(&writer) == 2u);
    assert(fake.write_calls == 2u);

    assert(campaign_bulk_block_reader_init(&reader, &media, 7u, 64u, 0u) == TR2_OK);
    assert(campaign_bulk_block_reader_next(&reader, scratch, sizeof(scratch),
                                           &info, &payload) == TR2_OK);
    assert(info.block_index == 0u);
    assert(info.payload_size == sizeof(p0));
    assert(memcmp(payload, p0, sizeof(p0)) == 0);
    assert(campaign_bulk_block_reader_next_offset(&reader) == after_first);

    assert(campaign_bulk_block_reader_next(&reader, scratch, sizeof(scratch),
                                           &info, &payload) == TR2_OK);
    assert(info.block_index == 1u);
    assert(info.payload_size == sizeof(p1));
    assert(memcmp(payload, p1, sizeof(p1)) == 0);
    assert(campaign_bulk_block_reader_next_offset(&reader) ==
           campaign_bulk_block_writer_next_offset(&writer));
    assert(campaign_bulk_block_reader_next_index(&reader) == 2u);
}

static void test_writer_capacity_failure_does_not_advance(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    CampaignBulkBlockWriter writer;
    uint8_t payload[32] = { 0 };
    uint8_t scratch[128];
    uint64_t offset;

    assert(campaign_bulk_block_writer_init(&writer, &media, 3u, 480u, 5u) == TR2_OK);
    offset = campaign_bulk_block_writer_next_offset(&writer);
    assert(campaign_bulk_block_writer_append(&writer, payload, sizeof(payload),
                                             scratch, sizeof(scratch)) ==
           TR2_ERROR_NOT_AVAILABLE);
    assert(campaign_bulk_block_writer_next_offset(&writer) == offset);
    assert(campaign_bulk_block_writer_next_index(&writer) == 5u);
    assert(fake.write_calls == 0u);
    assert(!campaign_bulk_block_writer_is_faulted(&writer));
}

static void test_writer_io_failure_faults_without_advancing(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    CampaignBulkBlockWriter writer;
    uint8_t payload[16] = { 0 };
    uint8_t scratch[128];

    assert(campaign_bulk_block_writer_init(&writer, &media, 3u, 32u, 0u) == TR2_OK);
    fake.write_result = TR2_ERROR_STORAGE;
    assert(campaign_bulk_block_writer_append(&writer, payload, sizeof(payload),
                                             scratch, sizeof(scratch)) ==
           TR2_ERROR_STORAGE);
    assert(campaign_bulk_block_writer_next_offset(&writer) == 32u);
    assert(campaign_bulk_block_writer_next_index(&writer) == 0u);
    assert(campaign_bulk_block_writer_is_faulted(&writer));
}

static void test_reader_rejects_wrong_campaign_and_index(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    CampaignBulkBlockWriter writer;
    CampaignBulkBlockReader reader;
    uint8_t payload_in[16] = { 0 };
    uint8_t scratch[128];
    CampaignBulkBlockInfo info;
    const uint8_t *payload_out;

    assert(campaign_bulk_block_writer_init(&writer, &media, 8u, 0u, 4u) == TR2_OK);
    assert(campaign_bulk_block_writer_append(&writer, payload_in, sizeof(payload_in),
                                             scratch, sizeof(scratch)) == TR2_OK);

    assert(campaign_bulk_block_reader_init(&reader, &media, 9u, 0u, 4u) == TR2_OK);
    assert(campaign_bulk_block_reader_next(&reader, scratch, sizeof(scratch),
                                           &info, &payload_out) ==
           TR2_ERROR_CORRUPTED);
    assert(campaign_bulk_block_reader_next_offset(&reader) == 0u);

    assert(campaign_bulk_block_reader_init(&reader, &media, 8u, 0u, 3u) == TR2_OK);
    assert(campaign_bulk_block_reader_next(&reader, scratch, sizeof(scratch),
                                           &info, &payload_out) ==
           TR2_ERROR_CORRUPTED);
    assert(campaign_bulk_block_reader_next_index(&reader) == 3u);
}

static void test_reader_scratch_capacity_is_checked_before_full_read(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    CampaignBulkBlockWriter writer;
    CampaignBulkBlockReader reader;
    uint8_t payload_in[32] = { 0 };
    uint8_t write_scratch[128];
    uint8_t read_scratch[40];
    CampaignBulkBlockInfo info;
    const uint8_t *payload_out;

    assert(campaign_bulk_block_writer_init(&writer, &media, 2u, 0u, 0u) == TR2_OK);
    assert(campaign_bulk_block_writer_append(&writer, payload_in, sizeof(payload_in),
                                             write_scratch, sizeof(write_scratch)) == TR2_OK);
    fake.read_calls = 0u;
    assert(campaign_bulk_block_reader_init(&reader, &media, 2u, 0u, 0u) == TR2_OK);
    assert(campaign_bulk_block_reader_next(&reader,
                                           read_scratch,
                                           sizeof(read_scratch),
                                           &info,
                                           &payload_out) ==
           TR2_ERROR_NOT_AVAILABLE);
    assert(fake.read_calls == 1u);
    assert(campaign_bulk_block_reader_next_offset(&reader) == 0u);
}

static void test_reader_io_failure_faults_without_advancing(void)
{
    FakeMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    CampaignBulkBlockReader reader;
    uint8_t scratch[128];
    CampaignBulkBlockInfo info;
    const uint8_t *payload;

    assert(campaign_bulk_block_reader_init(&reader, &media, 2u, 0u, 0u) == TR2_OK);
    fake.read_result = TR2_ERROR_STORAGE;
    assert(campaign_bulk_block_reader_next(&reader, scratch, sizeof(scratch),
                                           &info, &payload) ==
           TR2_ERROR_STORAGE);
    assert(campaign_bulk_block_reader_next_offset(&reader) == 0u);
    assert(campaign_bulk_block_reader_next_index(&reader) == 0u);
    assert(campaign_bulk_block_reader_is_faulted(&reader));
}

int main(void)
{
    test_writer_reader_round_trip_and_continuity();
    test_writer_capacity_failure_does_not_advance();
    test_writer_io_failure_faults_without_advancing();
    test_reader_rejects_wrong_campaign_and_index();
    test_reader_scratch_capacity_is_checked_before_full_read();
    test_reader_io_failure_faults_without_advancing();
    return 0;
}
