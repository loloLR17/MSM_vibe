#include <stddef.h>

#include "tr2/persistence/command_journal.h"

bool command_recovery_context_is_valid(const CommandRecoveryContext *context)
{
    if (context == NULL) {
        return false;
    }

    return context->kind >= COMMAND_RECOVERY_CONTEXT_CONFIGURATION &&
           context->kind <= COMMAND_RECOVERY_CONTEXT_BOOT_INTENT;
}

bool command_journal_entry_is_consistent(const CommandJournalEntry *entry)
{
    if (entry == NULL || !command_transaction_id_is_valid(entry->transaction_id)) {
        return false;
    }

    if (entry->has_recovery_context) {
        if (!command_recovery_context_is_valid(&entry->recovery_context)) {
            return false;
        }
    } else if (entry->recovery_context.kind != COMMAND_RECOVERY_CONTEXT_NONE ||
               entry->recovery_context.value1 != 0u ||
               entry->recovery_context.value2 != 0u ||
               entry->recovery_context.value3 != 0u) {
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
