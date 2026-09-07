#ifndef TR2_PERSISTENCE_TIME_HISTORY_RECORD_H
#define TR2_PERSISTENCE_TIME_HISTORY_RECORD_H

#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/time/time_recovery.h"

#define TR2_TIME_HISTORY_RECORD_FORMAT_VERSION UINT16_C(1)
#define TR2_TIME_HISTORY_RECORD_SIZE 18u

Tr2Result tr2_time_history_record_encode(const LastSyncHistory *history,
                                         uint8_t *record,
                                         size_t record_size);

Tr2Result tr2_time_history_record_decode(const uint8_t *record,
                                         size_t record_size,
                                         LastSyncHistory *history);

#endif
