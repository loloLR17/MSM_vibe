#ifndef DURABLE_ENDURANCE_CLOCK_H
#define DURABLE_ENDURANCE_CLOCK_H
#include <stdint.h>
#include <stdbool.h>
/* Valid if sampled at least once per DWT period (~26.84s at160MHz).
 * r3_now is called each poll and on every 1ms SysTick wake while idle. */
static inline uint64_t durable_elapsed_cycles(uint64_t elapsed,uint32_t current,uint32_t previous)
{return elapsed+(uint32_t)(current-previous);}
/* SysTick is not an independent duration reference. It detects an ambiguous
 * DWT observation gap/lost wraps while IRQs remain enabled (capture invariant).
 * Four milliseconds cover sampling/IRQ phase, not a relaxed FIFO deadline. */
static inline bool durable_clock_gap_ok(uint32_t delta_cycles,uint32_t delta_ticks)
{
 uint64_t cycles_us=delta_cycles/160u,tick_us=(uint64_t)delta_ticks*1000u;
 uint64_t difference=cycles_us>tick_us?cycles_us-tick_us:tick_us-cycles_us;
 return delta_ticks<25000u&&difference<=4000u;
}
#endif
