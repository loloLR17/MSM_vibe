#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tr2/persistence/persistent_storage_core.h"

typedef struct {
    uint8_t bytes[64];
    uint32_t read_calls;
    uint32_t write_calls;
    uint32_t commit_calls;
    Tr2Result read_result;
    Tr2Result write_result;
    Tr2Result commit_result;
} FakeMediaContext;

static Tr2Result fake_read(void *context, uint32_t offset, void *buffer, size_t size)
{
    FakeMediaContext *fake = (FakeMediaContext *)context;

    assert(fake != NULL);
    fake->read_calls += 1u;
    if (fake->read_result != TR2_OK) {
        return fake->read_result;
    }
    if ((size_t)offset + size > sizeof(fake->bytes)) {
        return TR2_ERROR_STORAGE;
    }

    memcpy(buffer, &fake->bytes[offset], size);
    return TR2_OK;
}

static Tr2Result fake_write(void *context, uint32_t offset, const void *buffer, size_t size)
{
    FakeMediaContext *fake = (FakeMediaContext *)context;

    assert(fake != NULL);
    fake->write_calls += 1u;
    if (fake->write_result != TR2_OK) {
        return fake->write_result;
    }
    if ((size_t)offset + size > sizeof(fake->bytes)) {
        return TR2_ERROR_STORAGE;
    }

    memcpy(&fake->bytes[offset], buffer, size);
    return TR2_OK;
}

static Tr2Result fake_commit(void *context)
{
    FakeMediaContext *fake = (FakeMediaContext *)context;

    assert(fake != NULL);
    fake->commit_calls += 1u;
    return fake->commit_result;
}

int main(void)
{
    PersistentStorageCore core = { 0 };
    FakeMediaContext fake = { { 0 }, 0u, 0u, 0u, TR2_OK, TR2_OK, TR2_OK };
    PersistentMedia media = { &fake, fake_read, fake_write, fake_commit };
    PersistentMedia incomplete_media = { &fake, fake_read, fake_write, NULL };
    uint8_t write_data[4] = { 1u, 2u, 3u, 4u };
    uint8_t read_data[4] = { 0 };

    assert(!persistent_storage_core_is_initialized(&core));
    assert(persistent_storage_core_read(&core, 0u, read_data, sizeof(read_data)) == TR2_ERROR_INVALID_STATE);
    assert(persistent_storage_core_write(&core, 0u, write_data, sizeof(write_data)) == TR2_ERROR_INVALID_STATE);
    assert(persistent_storage_core_commit(&core) == TR2_ERROR_INVALID_STATE);

    assert(persistent_storage_core_init(NULL, &media) == TR2_ERROR_INVALID_ARGUMENT);
    assert(persistent_storage_core_init(&core, NULL) == TR2_ERROR_INVALID_ARGUMENT);
    assert(persistent_storage_core_init(&core, &incomplete_media) == TR2_ERROR_INVALID_ARGUMENT);

    assert(persistent_storage_core_init(&core, &media) == TR2_OK);
    assert(persistent_storage_core_is_initialized(&core));

    assert(persistent_storage_core_write(&core, 8u, write_data, sizeof(write_data)) == TR2_OK);
    assert(fake.write_calls == 1u);
    assert(fake.commit_calls == 0u);

    assert(persistent_storage_core_read(&core, 8u, read_data, sizeof(read_data)) == TR2_OK);
    assert(fake.read_calls == 1u);
    assert(memcmp(write_data, read_data, sizeof(write_data)) == 0);
    assert(fake.commit_calls == 0u);

    assert(persistent_storage_core_commit(&core) == TR2_OK);
    assert(fake.commit_calls == 1u);

    assert(persistent_storage_core_write(&core, 0u, NULL, 1u) == TR2_ERROR_INVALID_ARGUMENT);
    assert(persistent_storage_core_read(&core, 0u, NULL, 1u) == TR2_ERROR_INVALID_ARGUMENT);
    assert(persistent_storage_core_write(&core, 0u, NULL, 0u) == TR2_OK);
    assert(persistent_storage_core_read(&core, 0u, NULL, 0u) == TR2_OK);
    assert(fake.write_calls == 1u);
    assert(fake.read_calls == 1u);

    fake.write_result = TR2_ERROR_UNAVAILABLE;
    assert(persistent_storage_core_write(&core, 0u, write_data, sizeof(write_data)) == TR2_ERROR_UNAVAILABLE);
    assert(fake.write_calls == 2u);

    fake.read_result = TR2_ERROR_CORRUPTED;
    assert(persistent_storage_core_read(&core, 0u, read_data, sizeof(read_data)) == TR2_ERROR_CORRUPTED);
    assert(fake.read_calls == 2u);

    fake.commit_result = TR2_ERROR_STORAGE;
    assert(persistent_storage_core_commit(&core) == TR2_ERROR_STORAGE);
    assert(fake.commit_calls == 2u);

    return 0;
}
