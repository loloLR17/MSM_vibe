#ifndef TR2_APPLICATION_SELFTEST_SERVICE_H
#define TR2_APPLICATION_SELFTEST_SERVICE_H

#include <stdbool.h>

#include "tr2/application/diagnostic_service.h"
#include "tr2/common/result.h"
#include "tr2/persistence/diagnostic_history_store.h"

typedef struct {
    DiagnosticService *diagnostic_service;
    DiagnosticHistoryStore *history_store;
    bool initialized;
    bool running;
} SelfTestService;

Tr2Result selftest_service_init(SelfTestService *service,
                                DiagnosticService *diagnostic_service,
                                DiagnosticHistoryStore *history_store);

bool selftest_service_is_initialized(const SelfTestService *service);
bool selftest_service_running(const SelfTestService *service);

Tr2Result selftest_service_recover(SelfTestService *service,
                                   DiagnosticHistoryRecoveryStatus *out_status);

Tr2Result selftest_service_begin_standard(SelfTestService *service);

Tr2Result selftest_service_complete(SelfTestService *service,
                                    bool passed,
                                    uint16_t result_code,
                                    uint16_t detail);

#endif
