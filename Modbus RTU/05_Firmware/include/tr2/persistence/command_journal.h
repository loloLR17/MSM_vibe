#ifndef TR2_PERSISTENCE_COMMAND_JOURNAL_H
#define TR2_PERSISTENCE_COMMAND_JOURNAL_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/command/command.h"

typedef struct {
    uint16_t transaction_id;
    CommandRequestIdentity request_identity;
    CommandLifecycleState lifecycle;
    bool has_final_result;
    CommandFinalResult final_result;
    CommandTerminalTimestamp terminal_timestamp;
    uint32_t completion_order;
} CommandJournalEntry;

typedef struct CommandJournal CommandJournal;

struct CommandJournal {
    void *context;
    Tr2Result (*find)(void *context,
                      uint16_t transaction_id,
                      CommandJournalEntry *entry);
    Tr2Result (*reserve)(void *context,
                         const CommandRequest *request,
                         CommandJournalEntry *entry);
    Tr2Result (*mark_started)(void *context,
                              uint16_t transaction_id,
                              CommandJournalEntry *entry);
    Tr2Result (*complete)(void *context,
                          uint16_t transaction_id,
                          const CommandFinalResult *final_result,
                          const CommandTerminalTimestamp *terminal_timestamp,
                          CommandJournalEntry *entry);
    Tr2Result (*latest_completed)(void *context,
                                  CommandJournalEntry *entry);
};

bool command_journal_entry_is_consistent(const CommandJournalEntry *entry);

#endif
