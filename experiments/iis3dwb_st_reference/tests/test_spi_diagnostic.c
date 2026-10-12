#include "spi_diagnostic.h"
#include <assert.h>
#include <stdio.h>
int main(void)
{
  uint32_t valid;
  assert(bench_rx_offset(1000,1225,896,&valid)==225 && valid);
  assert(bench_rx_offset(1000,1896,896,&valid)==896 && valid);
  assert(bench_rx_offset(1000,1000,896,&valid)==0 && valid);
  assert(bench_rx_offset(1000,999,896,&valid)==0 && !valid);
  assert(bench_rx_offset(1000,1897,896,&valid)==0 && !valid);
  assert(bench_rx_offset(0,0,896,&valid)==0 && !valid);
  /* The observer's independent offset survives the HAL clearing remaining=0. */
  uint32_t remaining=671, offset=bench_rx_offset(1000,1225,896,&valid);
  assert(offset==896-remaining && valid);remaining=0;
  assert(offset==225 && remaining==0);
  BenchPreclose r={.trace_index=3,.phase=2,.requested=896,.rx_size=896,
    .rx_offset=896,.offset_valid=1};
  assert(bench_rx_complete(&r,3,896));
  r.rx_offset=225;assert(!bench_rx_complete(&r,3,896));
  r.rx_offset=896;r.rx_remaining=1;assert(!bench_rx_complete(&r,3,896));
  r.rx_remaining=0;r.cs=1;assert(!bench_rx_complete(&r,3,896));
  r.cs=0;r.error_before_close=256;assert(!bench_rx_complete(&r,3,896));
  r.error_before_close=0;assert(!bench_rx_complete(&r,4,896));
  assert(!bench_rx_complete(0,3,896));
  assert(bench_log_refusal(1012,0,0,1,0)==1);
  assert(bench_log_refusal(1012,0,0,1,1)==0);
  assert(bench_log_refusal(0,65504,0,1,0)==2);
  assert(bench_log_refusal(0,65504,0,1,1)==0);
  assert(bench_log_refusal(0,0,2024,1,0)==4);
  assert(bench_log_refusal(0,0,2024,1,1)==0);
  assert(bench_log_refusal(1024,65536,2048,1,1)==7);
  assert(bench_log_refusal(0,0,0,0xFFFFFFFFU,0)==2);
  uint8_t tx[897],value=0x0A;
  assert(bench_build_frame(0x78,0,896,0,tx,sizeof(tx))==897);
  assert(tx[0]==0xF8 && tx[1]==0 && tx[896]==0);
  assert(bench_build_frame(9,&value,1,1,tx,sizeof(tx))==2 && tx[0]==9 && tx[1]==0x0A);
  assert(!bench_build_frame(0x78,0,897,0,tx,sizeof(tx)));
  assert(!bench_build_frame(0x78,0,896,0,tx,896));
  assert(!bench_build_frame(0x80,&value,1,1,tx,sizeof(tx)));
  assert(!bench_build_frame(9,0,1,1,tx,sizeof(tx)));
  assert(!bench_build_frame(9,&value,0,1,tx,sizeof(tx)));
  puts("14 diagnostic bounds and pre-cleanup preservation scenarios passed");
  return 0;
}
