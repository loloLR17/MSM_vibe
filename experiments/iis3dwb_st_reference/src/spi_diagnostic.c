#include "spi_diagnostic.h"
uint32_t bench_rx_offset(uintptr_t base, uintptr_t current, uint32_t requested,
                         uint32_t *valid)
{
  *valid=base!=0 && current>=base && current-base<=requested;
  return *valid?(uint32_t)(current-base):0U;
}
int bench_rx_complete(const BenchPreclose *r, uint32_t trace_index, uint32_t requested)
{
  return r && r->trace_index==trace_index && r->phase==2 &&
    r->requested==requested && r->rx_size==requested && !r->rx_remaining &&
    r->offset_valid && r->rx_offset==requested && !r->cs && !r->error_before_close;
}
