#ifndef TR2_PERSISTENCE_SYSTEM_PERSISTENT_LAYOUT_H
#define TR2_PERSISTENCE_SYSTEM_PERSISTENT_LAYOUT_H

#include "tr2/persistence/configuration_store.h"
#include "tr2/persistence/time_history_record.h"
#include "tr2/persistence/campaign_repository_store.h"
#include "tr2/persistence/campaign_data_store_persistent.h"
#include "tr2/persistence/command_journal_bounded_store.h"
#include "tr2/persistence/diagnostic_history_record.h"

/* Preserve the historical data reservation and every existing persistent offset. */
#define TR2_TIME_HISTORY_STORAGE_OFFSET ((uint32_t)TR2_CONFIGURATION_STORE_STORAGE_SIZE)
#define TR2_CAMPAIGN_REPOSITORY_STORAGE_OFFSET \
    (TR2_TIME_HISTORY_STORAGE_OFFSET + (uint32_t)TR2_TIME_HISTORY_RECORD_SIZE)
#define TR2_CAMPAIGN_DATA_STORAGE_OFFSET \
    (TR2_CAMPAIGN_REPOSITORY_STORAGE_OFFSET + \
     (uint32_t)TR2_CAMPAIGN_REPOSITORY_STORAGE_SIZE)
#define TR2_COMMAND_JOURNAL_STORAGE_OFFSET \
    (TR2_CAMPAIGN_DATA_STORAGE_OFFSET + (uint32_t)TR2_CAMPAIGN_DATA_STORAGE_SIZE)
#define TR2_DIAGNOSTIC_HISTORY_STORAGE_OFFSET \
    (TR2_COMMAND_JOURNAL_STORAGE_OFFSET + \
     (uint32_t)TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE)
#define TR2_BOOT_INTENT_STORAGE_OFFSET \
    (TR2_DIAGNOSTIC_HISTORY_STORAGE_OFFSET + \
     (uint32_t)TR2_DIAGNOSTIC_HISTORY_RECORD_SIZE + \
     (uint32_t)TR2_DIAGNOSTIC_SELFTEST_RECORD_SIZE)

#endif
