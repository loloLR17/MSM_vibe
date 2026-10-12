#ifndef BENCH_SPI_DIAGNOSTIC_H
#define BENCH_SPI_DIAGNOSTIC_H
#include <stdint.h>
#define PRECLOSE_RECORDS 2048U
typedef struct {
  uint32_t trace_index, phase, begin_cycles, end_cycles, tick, requested;
  uint32_t state, sr, cr1, cr2, cfg1, cfg2, error_before_close;
  uint32_t rx_size, rx_remaining, tx_remaining, rx_offset, cs, offset_valid;
} BenchPreclose;
extern BenchPreclose bench_preclose[PRECLOSE_RECORDS];
extern volatile uint32_t bench_preclose_count, bench_preclose_overflow;
/* Pure bounds calculation shared by the passive observer and its host tests. */
uint32_t bench_rx_offset(uintptr_t base, uintptr_t current, uint32_t requested,
                         uint32_t *valid);
int bench_rx_complete(const BenchPreclose *r, uint32_t trace_index, uint32_t requested);
#endif
