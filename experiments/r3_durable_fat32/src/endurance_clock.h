#ifndef DURABLE_ENDURANCE_CLOCK_H
#define DURABLE_ENDURANCE_CLOCK_H
#include <stdint.h>
/* Valid if sampled at least once per DWT period (~26.84s at160MHz).
 * r3_now is called each poll and on every 1ms SysTick wake while idle. */
static inline uint64_t durable_elapsed_cycles(uint64_t elapsed,uint32_t current,uint32_t previous)
{return elapsed+(uint32_t)(current-previous);}
#endif
