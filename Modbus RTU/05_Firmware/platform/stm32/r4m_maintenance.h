#ifndef R4M_MAINTENANCE_H
#define R4M_MAINTENANCE_H
#include "tr2/application/live_command_engine.h"
/* Isolated qualification bench only, never automatic product recovery.
 * Every boot starts CLOSED; no UART/normal command ingress exists in the probe.
 * The complete archived STARTED remains unresolved outside the new context. */
typedef struct {
 uint32_t epoch_floor;
 uint64_t run_floor;
 uint8_t intent[128];
 unsigned phase; /* 0 CLOSED, 1 CAPTURED, 2 RETIRING, 3 OPEN after genuine durable RENEW */
} R4mMaintenance;
Tr2Result r4m_capture(R4mMaintenance *,const LiveCommandEngine *);
/* Explicit reviewed bridge resume only: caller must verify full snapshot SHA
 * and authenticated archived intent. Never a generic boot recovery. */
Tr2Result r4m_capture_bridge(R4mMaintenance *,const LiveCommandEngine *,const uint8_t archived_intent[128]);
Tr2Result r4m_isolate_journal(R4mMaintenance *,LiveCommandEngine *);
bool r4m_authorize(const R4mMaintenance *,const LiveCommandEngine *,const LiveCommandRequest *);
Tr2Result r4m_open_after_renew(R4mMaintenance *,const LiveCommandEngine *);

/* R4P exact epoch2/run2 snapshot only, caller also verifies full FRAM SHA. */
Tr2Result r4p_capture(R4mMaintenance *,const LiveCommandEngine *);
#endif
