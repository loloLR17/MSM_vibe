#ifndef TR2_PERSISTENCE_COMMAND_JOURNAL_STORE_H
#define TR2_PERSISTENCE_COMMAND_JOURNAL_STORE_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/persistence/command_journal.h"
#include "tr2/persistence/command_journal_record.h"
#include "tr2/persistence/persistent_storage_core.h"

#define TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS 2u
#define TR2_COMMAND_JOURNAL_STORE_MAX_TRANSACTION_ID UINT16_MAX
#define TR2_COMMAND_JOURNAL_STORE_STORAGE_SIZE \
    ((uint32_t)TR2_COMMAND_JOURNAL_STORE_MAX_TRANSACTION_ID * \
     TR2_COMMAND_JOURNAL_STORE_REDUNDANT_SLOTS * \
     TR2_COMMAND_JOURNAL_RECORD_SIZE)

typedef enum {
    COMMAND_JOURNAL_RECOVERY_VALID = 0,
    COMMAND_JOURNAL_RECOVERY_EMPTY,
    COMMAND_JOURNAL_RECOVERY_CORRUPTED,
    COMMAND_JOURNAL_RECOVERY_UNAVAILABLE,
    COMMAND_JOURNAL_RECOVERY_UNSUPPORTED
} CommandJournalRecoveryStatus;

typedef struct {
    CommandJournalRecoveryStatus status;
    uint32_t known_transaction_count;
    uint32_t next_completion_order;
} CommandJournalRecoveryResult;

typedef struct {
    PersistentStorageCore *storage;
    uint16_t max_transaction_id;
    bool initialized;
    bool recovery_required;
    uint32_t next_completion_order;
    CommandJournal journal;
} CommandJournalStore;

Tr2Result command_journal_store_init(CommandJournalStore *store,
                                     PersistentStorageCore *storage,
                                     uint16_t max_transaction_id);

bool command_journal_store_is_initialized(const CommandJournalStore *store);
bool command_journal_store_recovery_required(const CommandJournalStore *store);
CommandJournal *command_journal_store_journal(CommandJournalStore *store);

Tr2Result command_journal_store_set_recovery_context(
    CommandJournalStore *store,
    uint16_t transaction_id,
    const CommandRecoveryContext *recovery_context,
    CommandJournalEntry *entry);

Tr2Result command_journal_store_recover(CommandJournalStore *store,
                                        CommandJournalRecoveryResult *result);

#endif
