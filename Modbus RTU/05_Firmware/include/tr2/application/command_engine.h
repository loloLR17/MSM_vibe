#ifndef TR2_APPLICATION_COMMAND_ENGINE_H
#define TR2_APPLICATION_COMMAND_ENGINE_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/command/command.h"
#include "tr2/persistence/command_journal.h"

typedef enum {
    COMMAND_ADMISSION_NEW = 0,
    COMMAND_ADMISSION_RETRY,
    COMMAND_ADMISSION_COLLISION,
    COMMAND_ADMISSION_BUSY
} CommandAdmissionKind;

typedef struct {
    CommandAdmissionKind kind;
    CommandJournalEntry entry;
} CommandAdmissionResult;

typedef struct {
    CommandJournal *journal;
    bool initialized;
    bool has_active_transaction;
    uint16_t active_transaction_id;
} CommandEngine;

Tr2Result command_engine_init(CommandEngine *engine, CommandJournal *journal);
bool command_engine_is_initialized(const CommandEngine *engine);
bool command_engine_has_active_transaction(const CommandEngine *engine);
uint16_t command_engine_active_transaction_id(const CommandEngine *engine);

Tr2Result command_engine_admit(CommandEngine *engine,
                               const CommandRequest *request,
                               CommandAdmissionResult *result);

Tr2Result command_engine_release_active(CommandEngine *engine,
                                        uint16_t transaction_id);

#endif
