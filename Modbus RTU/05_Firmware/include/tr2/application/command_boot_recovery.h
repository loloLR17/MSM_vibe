#ifndef TR2_APPLICATION_COMMAND_BOOT_RECOVERY_H
#define TR2_APPLICATION_COMMAND_BOOT_RECOVERY_H

#include <stdbool.h>

#include "tr2/application/command_apply_configuration.h"
#include "tr2/application/command_selftest.h"
#include "tr2/application/command_start_acquisition.h"
#include "tr2/application/command_stop_acquisition.h"
#include "tr2/application/command_synchronize_time.h"

typedef enum {
    COMMAND_BOOT_RECOVERY_CLEAN = 0,
    COMMAND_BOOT_RECOVERY_RESERVED_NO_EFFECT,
    COMMAND_BOOT_RECOVERY_STARTED_EFFECT_PROVEN,
    COMMAND_BOOT_RECOVERY_STARTED_ABSENCE_PROVEN,
    COMMAND_BOOT_RECOVERY_STARTED_INDETERMINATE
} CommandBootRecoveryStatus;

typedef struct {
    const ConfigurationService *configuration_service;
    const TimeService *time_service;
    const CampaignRepository *campaign_repository;
} CommandBootRecoveryAuthorities;

typedef struct {
    CommandBootRecoveryStatus status;
    bool has_incomplete_transaction;
    CommandJournalEntry incomplete_transaction;
    bool has_latest_completed;
    CommandJournalEntry latest_completed;
} CommandBootRecoveryResult;

Tr2Result command_boot_recovery_scan(
    CommandJournalStore *journal_store,
    const CommandBootRecoveryAuthorities *authorities,
    CommandBootRecoveryResult *result);

#endif
