#ifndef TR2_LIVE_RUNTIME_H
#define TR2_LIVE_RUNTIME_H
#include "tr2/application/live_modbus_server.h"
/* One foreground owner. UART ISR only queues bytes/completes TX. The board
 * drain hook owns sensor time/FIFO; service NEVER admits commands or enters R2.
 * Bind this service through the existing work callback/context, not a global.
 * Opt-in composition, not an assertion of complete protocol2 product support. */
typedef struct {
 LiveModbusServer *server;Tr2CooperativeWork *work;
 void *context;Tr2Result (*drain)(void *);
 uint64_t next_publish_ms,last_ms;uint32_t service_calls,publications,polls,owner_calls;
 uint64_t last_drain_us;uint32_t drain_period_us,drain_calls;
 uint16_t poll_quantum;bool seen,servicing,main_busy,drain_seen;
} LiveRuntime;
Tr2Result live_runtime_init(LiveRuntime *,LiveModbusServer *,Tr2CooperativeWork *,
                           void *,Tr2Result (*drain)(void *),uint16_t poll_quantum);
/* Opt-in pacing of the costly sensor callback only. UART and deadline checks
 * still run at every checkpoint. Configure only stopped; 0 preserves R4U.
 * Maximum one quarter of the existing work limit, never a relaxed budget. */
Tr2Result live_runtime_set_drain_period(LiveRuntime *,uint32_t period_us);
Tr2Result live_runtime_service(LiveRuntime *);
/* At most one immutable intake OR durable R2 phase per foreground step. */
Tr2Result live_runtime_step(LiveRuntime *);
#endif
