#include <stddef.h>

#include "tr2/persistence/command_journal.h"

bool command_journal_entry_is_consistent(const CommandJournalEntry *entry)
{
    if (entry == NULL || !command_transaction_id_is_valid(entry->transaction_id)) {
        return false;
    }

    switch (entry->lifecycle) {
    case COMMAND_LIFECYCLE_RESERVED:
    case COMMAND_LIFECYCLE_STARTED:
        return !entry->has_final_result &&
               !entry->terminal_timestamp.available &&
               entry->completion_order == 0u;

    case COMMAND_LIFECYCLE_COMPLETED:
        return entry->has_final_result &&
               command_status_is_final(entry->final_result.status) &&
               entry->completion_order != 0u;

    default:
        return false;
    }
}
