#ifndef TR2_PERSISTENCE_COMMAND_JOURNAL_BOUNDED_STORE_H
#define TR2_PERSISTENCE_COMMAND_JOURNAL_BOUNDED_STORE_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/persistence/command_journal.h"
#include "tr2/persistence/command_journal_bounded_recovery.h"
#include "tr2/persistence/persistent_storage_core.h"

typedef struct {
    PersistentStorageCore *storage;
    bool initialized;
    bool recovery_required;
    uint32_t next_admission_order;
    uint32_t next_completion_order;
    CommandJournal journal;
} CommandJournalBoundedStore;

Tr2Result command_journal_bounded_store_init(
    CommandJournalBoundedStore *store,
    PersistentStorageCore *storage);

bool command_journal_bounded_store_is_initialized(
    const CommandJournalBoundedStore *store);

bool command_journal_bounded_store_recovery_required(
    const CommandJournalBoundedStore *store);

CommandJournal *command_journal_bounded_store_journal(
    CommandJournalBoundedStore *store);

Tr2Result command_journal_bounded_store_recover(
    CommandJournalBoundedStore *store,
    CommandJournalBoundedRecoveryResult *result);

#endif
