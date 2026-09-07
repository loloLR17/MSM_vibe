#ifndef TR2_APPLICATION_COMMAND_SELFTEST_H
#define TR2_APPLICATION_COMMAND_SELFTEST_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/application/command_engine.h"
#include "tr2/application/selftest_service.h"
#include "tr2/common/result.h"

typedef struct {
    bool passed;
    uint16_t result_code;
    uint16_t detail;
} SelfTestExecutionResult;

typedef Tr2Result (*SelfTestRunStandardFn)(void *context,
                                          SelfTestExecutionResult *result);

typedef struct {
    void *context;
    SelfTestRunStandardFn run_standard;
} SelfTestExecutor;

Tr2Result command_selftest_execute(
    CommandEngine *engine,
    SelfTestService *selftest_service,
    const SelfTestExecutor *executor,
    uint16_t transaction_id,
    const CommandTerminalTimestamp *terminal_timestamp,
    CommandJournalEntry *entry);

CommandReconciliationOutcome command_selftest_reconcile(
    const CommandJournalEntry *entry);

#endif
