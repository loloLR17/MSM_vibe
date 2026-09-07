#ifndef TR2_PERSISTENCE_CONFIGURATION_STORE_H
#define TR2_PERSISTENCE_CONFIGURATION_STORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/configuration/configuration.h"
#include "tr2/persistence/configuration_record.h"
#include "tr2/persistence/persistent_storage_core.h"

#define TR2_CONFIGURATION_STORE_SLOT_COUNT 2u
#define TR2_CONFIGURATION_STORE_SLOT_SIZE TR2_CONFIGURATION_RECORD_SIZE
#define TR2_CONFIGURATION_STORE_STORAGE_SIZE \
    (TR2_CONFIGURATION_STORE_SLOT_COUNT * TR2_CONFIGURATION_STORE_SLOT_SIZE)

typedef struct {
    PersistentStorageCore *storage;
    bool initialized;
} ConfigurationStore;

Tr2Result configuration_store_init(ConfigurationStore *store,
                                   PersistentStorageCore *storage);

bool configuration_store_is_initialized(const ConfigurationStore *store);

Tr2Result configuration_store_commit(ConfigurationStore *store,
                                     const ActiveConfigurationSnapshot *snapshot);

#endif
