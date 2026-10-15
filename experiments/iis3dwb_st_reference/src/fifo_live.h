#ifndef FIFO_LIVE_H
#define FIFO_LIVE_H
#include "fifo_window.h"
#include <stdbool.h>
#include <stddef.h>
/* Callback borrows one wholly checked raw burst. No partial burst is published.
 * It must consume/copy before returning, never block or access FRAM/SD. */
typedef int (*FifoLiveConsume)(void *,const uint8_t *,uint32_t,uint64_t);
typedef struct {
 stmdev_ctx_t ctx; FifoWindow parser;
 uint8_t words[128u*7u];
 uint64_t run_id,total_xyz,total_ts,last_ms,last_data_ms;
 uint32_t config_id,config_generation,max_gap_ms;
 /* Explicit first-data policy, defaults to max_gap_ms. The board may select a
  * bounded startup allowance after checking sensor turn-on specifications;
  * polling-gap, status/overrun and steady-state no-data guards stay unchanged. */
 uint32_t first_data_timeout_ms;
 uint16_t axes,full_scale;
 void (*cleanup_begin)(void);
 FifoLiveConsume consume;void *consumer;
 uint32_t error,stop_error;
 bool initialized,running,stopped;
} FifoLive;
int fifo_live_init(FifoLive *,const stmdev_ctx_t *,uint32_t,FifoLiveConsume,void *);
int fifo_live_start(FifoLive *,uint64_t,uint32_t,uint32_t,uint16_t,uint16_t,uint64_t);
int fifo_live_stop(FifoLive *);
int fifo_live_observe(FifoLive *);
int fifo_live_poll(FifoLive *,uint64_t);
#endif
