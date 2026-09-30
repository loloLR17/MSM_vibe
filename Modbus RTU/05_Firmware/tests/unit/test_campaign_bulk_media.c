#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/campaign_bulk_media.h"

typedef struct {
    uint8_t bytes[64];
    Tr2Result capacity_result;
    Tr2Result read_result;
    Tr2Result write_result;
    Tr2Result sync_result;
    uint32_t sync_calls;
} FakeBulkMedia;

static Tr2Result fake_capacity(void *context, uint64_t *capacity_bytes)
{
    FakeBulkMedia *fake = context;
    if (fake->capacity_result != TR2_OK) {
        return fake->capacity_result;
    }
    *capacity_bytes = sizeof(fake->bytes);
    return TR2_OK;
}

static Tr2Result fake_read(void *context, uint64_t offset, void *buffer, size_t size)
{
    FakeBulkMedia *fake = context;
    if (fake->read_result != TR2_OK) {
        return fake->read_result;
    }
    if (offset > sizeof(fake->bytes) || size > sizeof(fake->bytes) - (size_t)offset) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(buffer, &fake->bytes[(size_t)offset], size);
    return TR2_OK;
}

static Tr2Result fake_write(void *context,
                            uint64_t offset,
                            const void *buffer,
                            size_t size)
{
    FakeBulkMedia *fake = context;
    if (fake->write_result != TR2_OK) {
        return fake->write_result;
    }
    if (offset > sizeof(fake->bytes) || size > sizeof(fake->bytes) - (size_t)offset) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(&fake->bytes[(size_t)offset], buffer, size);
    return TR2_OK;
}

static Tr2Result fake_sync(void *context)
{
    FakeBulkMedia *fake = context;
    fake->sync_calls += 1u;
    return fake->sync_result;
}

static CampaignBulkMedia make_media(FakeBulkMedia *fake)
{
    CampaignBulkMedia media;
    media.context = fake;
    media.capacity_bytes = fake_capacity;
    media.read = fake_read;
    media.write = fake_write;
    media.sync = fake_sync;
    return media;
}

static void test_delegates_raw_media_operations(void)
{
    FakeBulkMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    uint8_t source[] = { 1u, 2u, 3u, 4u };
    uint8_t output[sizeof(source)] = { 0 };
    uint64_t capacity = 0u;

    assert(campaign_bulk_media_capacity(&media, &capacity) == TR2_OK);
    assert(capacity == sizeof(fake.bytes));
    assert(campaign_bulk_media_write(&media, 7u, source, sizeof(source)) == TR2_OK);
    assert(campaign_bulk_media_read(&media, 7u, output, sizeof(output)) == TR2_OK);
    assert(memcmp(source, output, sizeof(source)) == 0);
    assert(campaign_bulk_media_sync(&media) == TR2_OK);
    assert(fake.sync_calls == 1u);
}

static void test_zero_length_does_not_touch_media(void)
{
    FakeBulkMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);

    fake.read_result = TR2_ERROR_STORAGE;
    fake.write_result = TR2_ERROR_STORAGE;
    assert(campaign_bulk_media_read(&media, 0u, NULL, 0u) == TR2_OK);
    assert(campaign_bulk_media_write(&media, 0u, NULL, 0u) == TR2_OK);
}

static void test_errors_are_not_hidden(void)
{
    FakeBulkMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    uint8_t byte = 0u;
    uint64_t capacity = 0u;

    fake.capacity_result = TR2_ERROR_UNAVAILABLE;
    assert(campaign_bulk_media_capacity(&media, &capacity) == TR2_ERROR_UNAVAILABLE);

    fake.read_result = TR2_ERROR_STORAGE;
    assert(campaign_bulk_media_read(&media, 0u, &byte, 1u) == TR2_ERROR_STORAGE);

    fake.write_result = TR2_ERROR_STORAGE;
    assert(campaign_bulk_media_write(&media, 0u, &byte, 1u) == TR2_ERROR_STORAGE);

    fake.sync_result = TR2_ERROR_STORAGE;
    assert(campaign_bulk_media_sync(&media) == TR2_ERROR_STORAGE);
}

static void test_rejects_invalid_contract_usage(void)
{
    FakeBulkMedia fake = { 0 };
    CampaignBulkMedia media = make_media(&fake);
    uint8_t byte = 0u;

    assert(campaign_bulk_media_capacity(NULL, NULL) == TR2_ERROR_INVALID_ARGUMENT);
    assert(campaign_bulk_media_read(&media, 0u, NULL, 1u) == TR2_ERROR_INVALID_ARGUMENT);
    assert(campaign_bulk_media_write(&media, UINT64_MAX, &byte, 2u) ==
           TR2_ERROR_INVALID_ARGUMENT);

    media.sync = NULL;
    assert(campaign_bulk_media_sync(&media) == TR2_ERROR_INVALID_ARGUMENT);
}

int main(void)
{
    test_delegates_raw_media_operations();
    test_zero_length_does_not_touch_media();
    test_errors_are_not_hidden();
    test_rejects_invalid_contract_usage();
    return 0;
}
