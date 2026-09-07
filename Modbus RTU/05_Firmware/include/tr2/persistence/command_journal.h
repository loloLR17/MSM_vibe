#ifndef TR2_PERSISTENCE_COMMAND_JOURNAL_H
#define TR2_PERSISTENCE_COMMAND_JOURNAL_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/command/command.h"

typedef enum {
    COMMAND_RECOVERY_CONTEXT_NONE = 0,
    COMMAND_RECOVERY_CONTEXT_CONFIGURATION = 1,
    COMMAND_RECOVERY_CONTEXT_TIME_SYNC = 2,
    COMMAND_RECOVERY_CONTEXT_START_CAMPAIGN = 3,
    COMMAND_RECOVERY_CONTEXT_STOP_CAMPAIGN = 4,
    COMMAND_RECOVERY_CONTEXT_BOOT_INTENT = 5
} CommandRecoveryContextKind;

typedef struct {
    CommandRecoveryContextKind kind;
    uint32_t value1;
    uint32_t value2;
    uint32_t value3;
} CommandRecoveryContext;

typedef struct {
    uint16_t transaction_id;
    CommandRequestIdentity request_identity;
    CommandLifecycleState lifecycle;
    bool has_recovery_context;
    CommandRecoveryContext recovery_context;
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
    Tr2Result (*set_recovery_context)(void *context,
                                     uint16_t transaction_id,
                                     const CommandRecoveryContext *recovery_context,
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

bool command_recovery_context_is_valid(const CommandRecoveryContext *context);
bool command_journal_entry_is_consistent(const CommandJournalEntry *entry);

#endif
