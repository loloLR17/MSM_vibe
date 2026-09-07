#ifndef TR2_PERSISTENCE_BOOT_INTENT_STORE_H
#define TR2_PERSISTENCE_BOOT_INTENT_STORE_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/boot/boot_intent.h"
#include "tr2/persistence/boot_intent_record.h"
#include "tr2/persistence/persistent_storage_core.h"

typedef enum {
    BOOT_INTENT_RECOVERY_VALID = 0,
    BOOT_INTENT_RECOVERY_EMPTY,
    BOOT_INTENT_RECOVERY_CORRUPTED,
    BOOT_INTENT_RECOVERY_UNAVAILABLE,
    BOOT_INTENT_RECOVERY_UNSUPPORTED
} BootIntentRecoveryStatus;

typedef struct {
    BootIntentRecoveryStatus status;
    BootIntent intent;
} BootIntentRecoveryResult;

typedef struct {
    PersistentStorageCore *storage;
    uint32_t storage_offset;
    bool initialized;
    bool recovery_required;
} BootIntentStore;

Tr2Result boot_intent_store_init(BootIntentStore *store,
                                 PersistentStorageCore *storage,
                                 uint32_t storage_offset);
bool boot_intent_store_is_initialized(const BootIntentStore *store);
bool boot_intent_store_recovery_required(const BootIntentStore *store);
Tr2Result boot_intent_store_commit(BootIntentStore *store,
                                   const BootIntent *intent);
Tr2Result boot_intent_store_clear(BootIntentStore *store);
Tr2Result boot_intent_store_recover(const BootIntentStore *store,
                                    BootIntentRecoveryResult *result);

#endif
