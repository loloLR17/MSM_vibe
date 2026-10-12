#ifndef IIS3DWB_REFERENCE_BENCH_H
#define IIS3DWB_REFERENCE_BENCH_H
#include "iis3dwb_reg.h"
#include <stdint.h>
#define BENCH_WORDS 512U
enum { BENCH_OK, BENCH_IO, BENCH_WHO, BENCH_RESET_TIMEOUT,
       BENCH_TIMEOUT, BENCH_STOP, BENCH_CONFIG, BENCH_OVERFLOW,
       BENCH_ST_LEVEL, BENCH_TAG, BENCH_CAPACITY };
typedef struct {
  uint32_t (*tick)(void);
  void (*delay)(uint32_t);
  uint32_t (*stop)(void);
} BenchClock;
typedef struct {
  uint32_t stage, result, cleanup_result, count, xyz_count, timestamp_count;
  uint32_t who, raw_level, st_level, flags, status10, st_batch;
  uint8_t controls[14]; /* 07..0A then 0F..18 */
  iis3dwb_fifo_out_raw_t words[BENCH_WORDS];
  int16_t xyz[64][3];
  int16_t temperature;
} BenchResult;
int bench_run(stmdev_ctx_t *ctx, const BenchClock *clock,
              unsigned fifo, unsigned status10, BenchResult *out);
#endif
