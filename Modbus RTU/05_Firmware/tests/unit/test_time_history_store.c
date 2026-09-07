#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/time_history_store.h"

#define TEST_OFFSET UINT32_C(7)
#define TEST_MEDIA_SIZE (TEST_OFFSET + TR2_TIME_HISTORY_RECORD_SIZE + 4u)

typedef struct {
    uint8_t durable[TEST_MEDIA_SIZE];
    uint8_t staged[TEST_MEDIA_SIZE];
    bool fail_read;
    bool fail_write;
    bool fail_commit;
} TestMedia;

static Tr2Result test_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;

    if (media->fail_read || (size_t)offset + size > TEST_MEDIA_SIZE) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(buffer, &media->durable[offset], size);
    return TR2_OK;
}

static Tr2Result test_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    TestMedia *media = (TestMedia *)context;

    if (media->fail_write || (size_t)offset + size > TEST_MEDIA_SIZE) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(&media->staged[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result test_commit(void *context)
{
    TestMedia *media = (TestMedia *)context;

    if (media->fail_commit) {
        return TR2_ERROR_STORAGE;
    }
    memcpy(media->durable, media->staged, sizeof(media->durable));
    return TR2_OK;
}

static uint32_t crc32_bytes(const uint8_t *bytes, size_t byte_count)
{
    uint32_t crc = UINT32_C(0xFFFFFFFF);
    size_t byte_index;

    for (byte_index = 0u; byte_index < byte_count; ++byte_index) {
        uint8_t bit_index;
        crc ^= (uint32_t)bytes[byte_index];
        for (bit_index = 0u; bit_index < 8u; ++bit_index) {
            if ((crc & UINT32_C(1)) != 0u) {
                crc = (crc >> 1u) ^ UINT32_C(0xEDB88320);
            } else {
                crc >>= 1u;
            }
        }
    }
    return crc ^ UINT32_C(0xFFFFFFFF);
}

static void put_u32_be(uint8_t *output, uint32_t value)
{
    output[0] = (uint8_t)(value >> 24u);
    output[1] = (uint8_t)(value >> 16u);
    output[2] = (uint8_t)(value >> 8u);
    output[3] = (uint8_t)(value & UINT32_C(0xFF));
}

static void reset_media(TestMedia *media, uint8_t fill)
{
    memset(media, fill, sizeof(*media));
    media->fail_read = false;
    media->fail_write = false;
    media->fail_commit = false;
}

int main(void)
{
    TestMedia media;
    PersistentMedia persistent_media;
    PersistentStorageCore storage;
    TimeHistoryStore store = {0};
    TimeHistoryStore invalid_store = {0};
    TimeHistoryRecoveryResult recovery;
    LastSyncHistory valid = time_last_sync_history_valid(UINT32_C(0x12345678), UINT16_C(3));
    LastSyncHistory none = time_last_sync_history_none();
    uint8_t record[TR2_TIME_HISTORY_RECORD_SIZE];
    uint32_t crc;

    reset_media(&media, UINT8_C(0xFF));
    persistent_media.context = &media;
    persistent_media.read = test_read;
    persistent_media.write = test_write;
    persistent_media.commit = test_commit;

    assert(time_history_store_init(NULL, &storage, TEST_OFFSET) == TR2_ERROR_INVALID_ARGUMENT);
    assert(time_history_store_init(&invalid_store, &storage, TEST_OFFSET) == TR2_ERROR_INVALID_ARGUMENT);
    assert(persistent_storage_core_init(&storage, &persistent_media) == TR2_OK);
    assert(time_history_store_init(&store, &storage, TEST_OFFSET) == TR2_OK);
    assert(time_history_store_is_initialized(&store));
    assert(!time_history_store_recovery_required(&store));

    assert(time_history_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == TIME_HISTORY_RECOVERY_EMPTY);
    assert(recovery.history.state == LAST_SYNC_HISTORY_NONE);

    assert(time_history_store_commit(&store, NULL) == TR2_ERROR_INVALID_ARGUMENT);
    assert(time_history_store_commit(&store, &none) == TR2_ERROR_INVALID_ARGUMENT);
    assert(time_history_store_commit(&store, &valid) == TR2_OK);
    assert(time_history_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == TIME_HISTORY_RECOVERY_VALID);
    assert(recovery.history.state == LAST_SYNC_HISTORY_VALID);
    assert(recovery.history.timestamp == valid.timestamp);
    assert(recovery.history.source == valid.source);

    media.durable[TEST_OFFSET + 10u] ^= UINT8_C(0x01);
    assert(time_history_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == TIME_HISTORY_RECOVERY_CORRUPTED);
    assert(recovery.history.state == LAST_SYNC_HISTORY_NONE);

    assert(tr2_time_history_record_encode(&valid, record, sizeof(record)) == TR2_OK);
    record[4] = UINT8_C(0x00);
    record[5] = UINT8_C(0x02);
    crc = crc32_bytes(record, TR2_TIME_HISTORY_RECORD_SIZE - sizeof(uint32_t));
    put_u32_be(&record[TR2_TIME_HISTORY_RECORD_SIZE - sizeof(uint32_t)], crc);
    memcpy(&media.durable[TEST_OFFSET], record, sizeof(record));
    assert(time_history_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == TIME_HISTORY_RECOVERY_UNSUPPORTED);
    assert(recovery.history.state == LAST_SYNC_HISTORY_NONE);

    media.fail_read = true;
    assert(time_history_store_recover(&store, &recovery) == TR2_OK);
    assert(recovery.status == TIME_HISTORY_RECOVERY_UNAVAILABLE);
    assert(recovery.history.state == LAST_SYNC_HISTORY_NONE);
    media.fail_read = false;

    reset_media(&media, UINT8_C(0xFF));
    media.fail_write = true;
    assert(time_history_store_commit(&store, &valid) == TR2_ERROR_STORAGE);
    assert(time_history_store_recovery_required(&store));
    assert(time_history_store_commit(&store, &valid) == TR2_ERROR_INVALID_STATE);
    assert(time_history_store_recover(&store, &recovery) == TR2_ERROR_INVALID_STATE);

    assert(time_history_store_init(&store, &storage, TEST_OFFSET) == TR2_OK);
    media.fail_write = false;
    media.fail_commit = true;
    assert(time_history_store_commit(&store, &valid) == TR2_ERROR_STORAGE);
    assert(time_history_store_recovery_required(&store));

    return 0;
}
