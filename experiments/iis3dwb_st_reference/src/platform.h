#ifndef ST_REFERENCE_PLATFORM_H
#define ST_REFERENCE_PLATFORM_H
#include "bench.h"
#include "stm32u5xx_hal.h"
#define TRACE_RECORDS 1024U
#define TRACE_BYTES 65536U
typedef struct {
  uint32_t tick, reg, len, write, status, error, rx_left, tx_left, offset, valid;
} BenchTrace;
extern BenchTrace bench_trace[TRACE_RECORDS];
extern uint8_t bench_payload[TRACE_BYTES];
extern volatile uint32_t bench_trace_count, bench_payload_count, bench_stop;
extern volatile uint32_t bench_command, bench_ready, bench_uid[3], bench_core_hz;
int platform_init(stmdev_ctx_t *ctx, BenchClock *clock);
int32_t platform_read(void *handle,uint8_t reg,uint8_t *data,uint16_t len);
int32_t platform_write(void *handle,uint8_t reg,const uint8_t *data,uint16_t len);
#endif
