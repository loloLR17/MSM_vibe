#ifndef TR2_PERSISTENCE_COMMAND_JOURNAL_BOUNDED_READER_H
#define TR2_PERSISTENCE_COMMAND_JOURNAL_BOUNDED_READER_H

#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/persistence/command_journal_bounded_record.h"
#include "tr2/persistence/persistent_storage_core.h"

typedef Tr2Result (*CommandJournalBoundedRecordVisitor)(
    void *context,
    size_t logical_slot,
    const CommandJournalBoundedRecord *record);

Tr2Result command_journal_bounded_reader_find(
    const PersistentStorageCore *storage,
    uint16_t transaction_id,
    size_t *logical_slot,
    CommandJournalBoundedRecord *record);

Tr2Result command_journal_bounded_reader_visit(
    const PersistentStorageCore *storage,
    CommandJournalBoundedRecordVisitor visitor,
    void *visitor_context);

Tr2Result command_journal_bounded_reader_latest_completed(
    const PersistentStorageCore *storage,
    size_t *logical_slot,
    CommandJournalBoundedRecord *record);

#endif
