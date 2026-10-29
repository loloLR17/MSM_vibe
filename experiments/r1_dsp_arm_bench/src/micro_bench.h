#ifndef R1_MICRO_BENCH_H
#define R1_MICRO_BENCH_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "tr2/domain/acquisition/fir_decimator10.h"
#define R1_BENCH_CASES 8u
#define R1_BENCH_BLOCKS 256u
#define R1_BENCH_MAX_BURST 128u
#define R1_BENCH_MAX_EMIT 4096u
#define R1_BENCH_TRIGGER 0x52314131u
#define R1_BENCH_COMPLETE 0x4431304Fu
/* Raw inclusive cycles, no subtraction of DWT/read instrumentation overhead.
 * Every public field uses fixed-width integers. No persisted state or B5 IDs. */
typedef struct {
    uint32_t count, minimum, p95, p99, maximum, reserved;
    uint64_t sum;
} R1CycleStats;
typedef struct {
    uint32_t fir_enabled, burst, blocks, sources, outputs, input_hash, output_hash;
    uint32_t error, irq_before, irq_after, elapsed_cycles, elapsed_ms;
    R1CycleStats block, emission;
    uint32_t warmup_outputs, clipped_outputs, source_rail_outputs, final_active;
} R1MicroCase;
typedef struct {
    uint32_t version, stage, error, clock_hz, uid[3], primask, basepri, faultmask;
    uint32_t demcr, dwt_ctrl, flash_acr, systick_count;
    uint32_t stack_begin, stack_end, stack_untouched, stack_deepest;
    uint32_t complete_magic, reserved;
    R1CycleStats overhead;
    R1MicroCase cases[R1_BENCH_CASES];
} R1MicroResult;
typedef uint32_t (*R1ReadCounter)(void *);
extern uint32_t r1_block_cycles[R1_BENCH_CASES][R1_BENCH_BLOCKS];
extern uint32_t r1_emit_cycles[R1_BENCH_CASES][R1_BENCH_MAX_EMIT];
extern uint32_t r1_overhead_cycles[R1_BENCH_BLOCKS];
bool r1_cycle_statistics(const uint32_t *,size_t,R1CycleStats *);
/* Synthetic XYZ and synthetic timestamps: this bench NEVER claims sensor data.
 * Same preparation, validation/publishing path and image, RAM opt-in selects FIR.
 * Excludes preparation/statistics from block timing. SysTick stays enabled.
 * 5-second wall-clock guard per case; IRQ count callback optional for host only. */
Tr2Result r1_micro_run(R1MicroResult *,R1ReadCounter,void *,R1ReadCounter,void *,R1ReadCounter,void *);
#endif
