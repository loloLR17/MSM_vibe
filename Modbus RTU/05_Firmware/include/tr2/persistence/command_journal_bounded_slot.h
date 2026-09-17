#ifndef TR2_PERSISTENCE_COMMAND_JOURNAL_BOUNDED_SLOT_H
#define TR2_PERSISTENCE_COMMAND_JOURNAL_BOUNDED_SLOT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/persistence/command_journal_bounded_record.h"
#include "tr2/persistence/persistent_storage_core.h"

#define TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT 256u
#define TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COPY_COUNT 2u
#define TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE \
    ((uint32_t)TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COUNT * \
     TR2_COMMAND_JOURNAL_BOUNDED_SLOT_COPY_COUNT * \
     TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE)

typedef enum {
    COMMAND_JOURNAL_BOUNDED_SLOT_EMPTY = 0,
    COMMAND_JOURNAL_BOUNDED_SLOT_VALID,
    COMMAND_JOURNAL_BOUNDED_SLOT_CORRUPTED,
    COMMAND_JOURNAL_BOUNDED_SLOT_UNAVAILABLE,
    COMMAND_JOURNAL_BOUNDED_SLOT_UNSUPPORTED
} CommandJournalBoundedSlotStatus;

typedef struct {
    CommandJournalBoundedSlotStatus status;
    bool has_record;
    size_t current_copy;
    CommandJournalBoundedRecord record;
} CommandJournalBoundedSlotSelection;

Tr2Result command_journal_bounded_slot_offset(size_t logical_slot,
                                              size_t copy_index,
                                              uint32_t *offset);

Tr2Result command_journal_bounded_slot_select(
    const PersistentStorageCore *storage,
    size_t logical_slot,
    CommandJournalBoundedSlotSelection *selection);

#endif
