#ifndef R4T_MAINTENANCE_H
#define R4T_MAINTENANCE_H
#include "r4m_maintenance.h"
/* Exact authenticated snapshot only; NOT generic product recovery.
 * Every boot CLOSED. No implicit continuation after any interrupted barrier. */
typedef struct {R4mMaintenance gate;uint64_t generation_floor;LiveCommandRecord archived;} R4tRetirement;
Tr2Result r4t_capture(R4tRetirement *,const LiveCommandEngine *,const uint8_t[262144]);
/* Atomically retire slot3 and admit a REAL RENEW using the unchanged R2 engine.
 * On failure the token is consumed and the store/frontend are unavailable.
 * No persistent empty bridge, counter seed record, invented receipt or epoch. */
Tr2Result r4t_retire_and_admit(R4tRetirement *,LiveCommandEngine *);
#endif
