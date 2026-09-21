#ifndef TR2_PERSISTENCE_COMMAND_JOURNAL_BOUNDED_ADMISSION_H
#define TR2_PERSISTENCE_COMMAND_JOURNAL_BOUNDED_ADMISSION_H

#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/command/command.h"
#include "tr2/persistence/command_journal_bounded_slot.h"
#include "tr2/persistence/persistent_storage_core.h"

typedef enum {
    COMMAND_JOURNAL_BOUNDED_ADMISSION_RETRY_EXISTING = 0,
    COMMAND_JOURNAL_BOUNDED_ADMISSION_COLLISION_EXISTING,
    COMMAND_JOURNAL_BOUNDED_ADMISSION_ADMIT_EMPTY,
    COMMAND_JOURNAL_BOUNDED_ADMISSION_EVICT_COMPLETED,
    COMMAND_JOURNAL_BOUNDED_ADMISSION_NO_CAPACITY
} CommandJournalBoundedAdmissionKind;

typedef struct {
    CommandJournalBoundedAdmissionKind kind;
    bool has_logical_slot;
    size_t logical_slot;
    bool has_current;
    CommandJournalBoundedSlotSelection current;
} CommandJournalBoundedAdmissionPlan;

/*
 * Produce a read-only admission decision for one B5 request identity.
 *
 * The persistent image is expected to have been qualified by bounded recovery.
 * The planner never writes, commits, increments counters or assigns a new
 * admission_order.
 */
Tr2Result command_journal_bounded_admission_plan(
    const PersistentStorageCore *storage,
    uint16_t transaction_id,
    const CommandRequestIdentity *request_identity,
    CommandJournalBoundedAdmissionPlan *plan);

#endif
