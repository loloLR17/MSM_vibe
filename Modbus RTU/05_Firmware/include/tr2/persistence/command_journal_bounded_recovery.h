#ifndef TR2_PERSISTENCE_COMMAND_JOURNAL_BOUNDED_RECOVERY_H
#define TR2_PERSISTENCE_COMMAND_JOURNAL_BOUNDED_RECOVERY_H

#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/persistence/persistent_storage_core.h"

typedef enum {
    COMMAND_JOURNAL_BOUNDED_RECOVERY_VALID = 0,
    COMMAND_JOURNAL_BOUNDED_RECOVERY_EMPTY,
    COMMAND_JOURNAL_BOUNDED_RECOVERY_CORRUPTED,
    COMMAND_JOURNAL_BOUNDED_RECOVERY_UNAVAILABLE,
    COMMAND_JOURNAL_BOUNDED_RECOVERY_UNSUPPORTED
} CommandJournalBoundedRecoveryStatus;

typedef struct {
    CommandJournalBoundedRecoveryStatus status;
    uint32_t known_transaction_count;
    uint32_t next_admission_order;
    uint32_t next_completion_order;
} CommandJournalBoundedRecoveryResult;

Tr2Result command_journal_bounded_recovery_scan(
    const PersistentStorageCore *storage,
    CommandJournalBoundedRecoveryResult *result);

#endif
