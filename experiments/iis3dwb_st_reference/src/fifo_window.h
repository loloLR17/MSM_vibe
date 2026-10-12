#ifndef FIFO_WINDOW_H
#define FIFO_WINDOW_H
#include "bench.h"
#define WINDOW_MAX_WORDS 4736U
/* Sensor stays streaming; this RAM capacity is not the sensor FIFO depth. */
typedef struct {
 uint32_t target_xyz, received, xyz, timestamps, polls, bursts;
 uint32_t first_tick, last_tick, max_level, last_level, flags_or, reason;
 uint32_t previous_counter, counter_valid, previous_ts, ts_valid, since_ts;
} FifoWindow;
int fifo_window_controls(const uint8_t controls[14], uint8_t ctrl10);
int fifo_window_run(stmdev_ctx_t *, const BenchClock *, uint32_t target,
                    uint8_t *words, uint32_t capacity_words, FifoWindow *);
#endif
