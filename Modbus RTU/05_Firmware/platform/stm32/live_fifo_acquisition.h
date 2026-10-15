#ifndef LIVE_FIFO_ACQUISITION_H
#define LIVE_FIFO_ACQUISITION_H
#include "fifo_live.h"
#include "tr2/application/live_metrology.h"
#include "tr2/application/live_command_engine.h"
typedef struct {
 FifoLive fifo;
 void *time_context;uint64_t (*now_ms)(void *);
 void *budget_context;bool (*budget)(void *,uint16_t);
 LiveCommandEngine *engine;
 LiveMetrology *metrology;
 void *sample_time_context;
 Tr2Result (*sample_time)(void *,uint64_t,uint64_t *,bool *,uint64_t *);
} LiveFifoAcquisition;
Tr2Result live_fifo_acquisition_init(LiveFifoAcquisition *,const stmdev_ctx_t *,
 uint64_t (*)(void *),void *,uint32_t,FifoLiveConsume,void *);
LiveAcquisitionPort live_fifo_acquisition_port(LiveFifoAcquisition *);
Tr2Result live_fifo_acquisition_poll(LiveFifoAcquisition *);
/* Optional coherent sink. No default synthetic time mapper: integrator must
 * provide observed per-XYZ monotone/UTC from a qualified timestamp authority. */
Tr2Result live_fifo_acquisition_bind_metrology(LiveFifoAcquisition *,LiveMetrology *,
 Tr2Result (*)(void *,uint64_t,uint64_t *,bool *,uint64_t *),void *);
Tr2Result live_fifo_acquisition_bind_engine(LiveFifoAcquisition *,LiveCommandEngine *);
#endif
