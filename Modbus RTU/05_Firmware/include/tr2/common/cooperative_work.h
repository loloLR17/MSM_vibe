#ifndef TR2_COOPERATIVE_WORK_H
#define TR2_COOPERATIVE_WORK_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "tr2/common/result.h"
/* Scoped, single-thread/non-reentrant cooperative execution. No global hook,
 * allocation, RTOS, persistent format or implied physical qualification. */
typedef struct {
    void *context;
    uint64_t (*now_us)(void *);
    Tr2Result (*service)(void *);
    void (*safety_stop)(void *);
    uint64_t last_us,max_work_us,max_service_us,checkpoints;
    uint32_t work_limit_us,service_limit_us;
    uint16_t quantum_bytes,depth;
    Tr2Result error;
    bool servicing,seen,deadline_missed;
} Tr2CooperativeWork;
Tr2Result tr2_cooperative_init(Tr2CooperativeWork *,void *,uint64_t (*)(void *),
    Tr2Result (*)(void *),void (*)(void *),uint16_t quantum,
    uint32_t work_limit_us,uint32_t service_limit_us);
Tr2Result tr2_cooperative_enter(Tr2CooperativeWork *);
Tr2Result tr2_cooperative_leave(Tr2CooperativeWork *);
Tr2Result tr2_cooperative_checkpoint(Tr2CooperativeWork *);
/* Status includes prohibition of reentrant mutation from a service callback. */
Tr2Result tr2_cooperative_status(const Tr2CooperativeWork *);
Tr2Result tr2_cooperative_equal(Tr2CooperativeWork *,const void *,uint8_t,size_t,bool *);
#endif
