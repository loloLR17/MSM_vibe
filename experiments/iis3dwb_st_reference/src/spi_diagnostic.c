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
uint32_t bench_log_refusal(uint32_t traces, uint32_t bytes, uint32_t snapshots,
                          uint32_t requested, uint32_t cleanup)
{
  uint32_t limit_t=cleanup?1024U:1012U;
  uint32_t limit_b=cleanup?65536U:65504U;
  uint32_t limit_s=cleanup?2048U:2024U;
  return (traces>=limit_t?1U:0U) |
    (bytes>limit_b || requested>limit_b-bytes?2U:0U) |
    (snapshots>limit_s || limit_s-snapshots<2U?4U:0U);
}
