#include "endurance_clock.h"
#include "archive_format.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
int main(void){
 uint64_t elapsed=0;uint32_t c=0xfffffff0u,tick=0xfffffff0u;
 /* Accelerated24h, sampling10ms: many DWT32 and a SysTick32 wrap. */
 for(uint64_t i=1;i<=8640000;i++){
  uint32_t next=c+1600000u,next_tick=tick+10;
  assert(durable_clock_gap_ok((uint32_t)(next-c),(uint32_t)(next_tick-tick)));
  elapsed=durable_elapsed_cycles(elapsed,next,c);c=next;tick=next_tick;
  assert(elapsed==i*1600000u);
 }
 assert(elapsed/160u==86400000000ull);
 assert(!durable_clock_gap_ok(160000u,54001u)); /* two lost DWT wraps */
 assert(!durable_clock_gap_ok(160000u,100u)); /* bad cross-counter agreement */
 uint64_t native=30600ull*28000,outputs=(native+9)/10,records=(outputs+77)/78;
 assert(native<UINT32_MAX&&outputs*6<UINT32_MAX&&records*512<UINT32_MAX);
 assert(records*512+1024<640ull*1024*1024);
 assert(30600ull*4000<UINT32_MAX&&30600000000ull>UINT32_MAX);
 uint8_t header[512]={0};durable_put64(header+168,30600000000ull);
 uint64_t got=0;for(unsigned i=0;i<8;i++)got|=(uint64_t)header[168+i]<<(i*8);assert(got==30600000000ull);
 puts("u64 8h/24h, DWT/tick wraps, lost-wrap refusal and capacity arithmetic PASS (MODEL)");
 return 0;
}
