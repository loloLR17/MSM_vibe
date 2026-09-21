#ifndef TR2_PERSISTENCE_COMMAND_JOURNAL_BOUNDED_WRITER_H
#define TR2_PERSISTENCE_COMMAND_JOURNAL_BOUNDED_WRITER_H

#include <stddef.h>

#include "tr2/common/result.h"
#include "tr2/persistence/command_journal_bounded_record.h"
#include "tr2/persistence/command_journal_bounded_slot.h"
#include "tr2/persistence/persistent_storage_core.h"

/*
 * Persist a first V3 record into an EMPTY logical slot.
 *
 * The caller provides the complete logical record, including a non-zero
 * admission_order. The writer owns only the physical A/B write policy:
 * copy 0, generation 1, then commit.
 */
Tr2Result command_journal_bounded_writer_admit_empty(
    PersistentStorageCore *storage,
    size_t logical_slot,
    const CommandJournalBoundedRecord *record);

/*
 * Persist a replacement record into a currently VALID logical slot.
 *
 * current must be the authoritative selection for logical_slot. The writer
 * writes the opposite copy with generation + 1 and commits it. The supplied
 * replacement must preserve the current admission_order.
 */
Tr2Result command_journal_bounded_writer_mutate(
    PersistentStorageCore *storage,
    size_t logical_slot,
    const CommandJournalBoundedSlotSelection *current,
    const CommandJournalBoundedRecord *replacement);

/*
 * Re-admit a new logical transaction into a currently VALID COMPLETED slot.
 *
 * Unlike mutate(), this operation intentionally permits a new admission_order
 * because the previous terminal transaction is being evicted. Physical A/B
 * continuity is preserved: opposite copy, generation + 1, then commit.
 */
Tr2Result command_journal_bounded_writer_readmit_completed(
    PersistentStorageCore *storage,
    size_t logical_slot,
    const CommandJournalBoundedSlotSelection *current,
    const CommandJournalBoundedRecord *replacement);

#endif
