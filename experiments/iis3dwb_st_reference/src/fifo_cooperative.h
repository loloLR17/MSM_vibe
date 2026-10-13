#ifndef FIFO_COOPERATIVE_H
#define FIFO_COOPERATIVE_H
#include "fifo_window.h"
enum {COOP_CONFIGURING,COOP_ARMED,COOP_CAPTURING,COOP_CLEANUP,COOP_READY,COOP_INVALID};
typedef struct {
 stmdev_ctx_t ctx;BenchClock clock;FifoWindow window;
 uint8_t *words;uint32_t capacity,state,phase,next_tick,reset_tick,result,cleanup_result;
 uint8_t who,cleanup[3];
} FifoCooperative;
int fifo_cooperative_init(FifoCooperative *,const stmdev_ctx_t *,const BenchClock *,uint8_t *,uint32_t,uint32_t);
int fifo_cooperative_start(FifoCooperative *);
void fifo_cooperative_poll(FifoCooperative *);
#endif
