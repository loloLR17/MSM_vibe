#ifndef TR2_PERSISTENCE_COMMAND_JOURNAL_RECORD_H
#define TR2_PERSISTENCE_COMMAND_JOURNAL_RECORD_H

#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/persistence/command_journal.h"

#define TR2_COMMAND_JOURNAL_RECORD_FORMAT_VERSION 2u
#define TR2_COMMAND_JOURNAL_RECORD_SIZE 66u

typedef struct {
    uint32_t generation;
    CommandJournalEntry entry;
} CommandJournalRecord;

Tr2Result tr2_command_journal_record_encode(const CommandJournalRecord *record_value,
                                            uint8_t *record,
                                            size_t record_size);

Tr2Result tr2_command_journal_record_decode(const uint8_t *record,
                                            size_t record_size,
                                            CommandJournalRecord *record_value);

#endif
