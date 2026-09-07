#ifndef TR2_PERSISTENCE_DIAGNOSTIC_HISTORY_RECORD_H
#define TR2_PERSISTENCE_DIAGNOSTIC_HISTORY_RECORD_H

#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/diagnostic/diagnostic.h"

#define TR2_DIAGNOSTIC_HISTORY_RECORD_FORMAT_VERSION UINT16_C(1)
#define TR2_DIAGNOSTIC_HISTORY_RECORD_SIZE 20u
#define TR2_DIAGNOSTIC_SELFTEST_RECORD_FORMAT_VERSION UINT16_C(1)
#define TR2_DIAGNOSTIC_SELFTEST_RECORD_SIZE 20u

Tr2Result tr2_diagnostic_history_record_encode(const DiagnosticLastFault *last_fault,
                                               uint8_t *record,
                                               size_t record_size);

Tr2Result tr2_diagnostic_history_record_decode(const uint8_t *record,
                                               size_t record_size,
                                               DiagnosticLastFault *last_fault);

Tr2Result tr2_diagnostic_selftest_record_encode(const DiagnosticSelfTestFacts *selftest,
                                                uint8_t *record,
                                                size_t record_size);

Tr2Result tr2_diagnostic_selftest_record_decode(const uint8_t *record,
                                                size_t record_size,
                                                DiagnosticSelfTestFacts *selftest);

#endif
