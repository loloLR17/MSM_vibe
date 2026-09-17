#ifndef TR2_PERSISTENCE_COMMAND_JOURNAL_BOUNDED_RECORD_H
#define TR2_PERSISTENCE_COMMAND_JOURNAL_BOUNDED_RECORD_H

#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/persistence/command_journal.h"

#define TR2_COMMAND_JOURNAL_BOUNDED_RECORD_FORMAT_VERSION 3u
#define TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE 70u

typedef struct {
    uint32_t generation;
    uint32_t admission_order;
    CommandJournalEntry entry;
} CommandJournalBoundedRecord;

Tr2Result tr2_command_journal_bounded_record_encode(const CommandJournalBoundedRecord *record_value,
                                                    uint8_t *record,
                                                    size_t record_size);
Tr2Result tr2_command_journal_bounded_record_decode(const uint8_t *record,
                                                    size_t record_size,
                                                    CommandJournalBoundedRecord *record_value);

#endif
