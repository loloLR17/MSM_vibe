#ifndef TR2_PERSISTENCE_DIAGNOSTIC_HISTORY_STORE_H
#define TR2_PERSISTENCE_DIAGNOSTIC_HISTORY_STORE_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/diagnostic/diagnostic.h"
#include "tr2/persistence/diagnostic_history_record.h"
#include "tr2/persistence/persistent_storage_core.h"

typedef enum {
    DIAGNOSTIC_HISTORY_RECOVERY_VALID = 0,
    DIAGNOSTIC_HISTORY_RECOVERY_EMPTY,
    DIAGNOSTIC_HISTORY_RECOVERY_CORRUPTED,
    DIAGNOSTIC_HISTORY_RECOVERY_UNAVAILABLE,
    DIAGNOSTIC_HISTORY_RECOVERY_UNSUPPORTED
} DiagnosticHistoryRecoveryStatus;

typedef struct {
    DiagnosticHistoryRecoveryStatus status;
    DiagnosticLastFault last_fault;
} DiagnosticHistoryRecoveryResult;

typedef struct {
    PersistentStorageCore *storage;
    uint32_t storage_offset;
    bool initialized;
    bool recovery_required;
} DiagnosticHistoryStore;

Tr2Result diagnostic_history_store_init(DiagnosticHistoryStore *store,
                                        PersistentStorageCore *storage,
                                        uint32_t storage_offset);

bool diagnostic_history_store_is_initialized(const DiagnosticHistoryStore *store);

bool diagnostic_history_store_recovery_required(const DiagnosticHistoryStore *store);

Tr2Result diagnostic_history_store_commit_last_fault(DiagnosticHistoryStore *store,
                                                     const DiagnosticLastFault *last_fault);

Tr2Result diagnostic_history_store_recover(const DiagnosticHistoryStore *store,
                                           DiagnosticHistoryRecoveryResult *result);

#endif
