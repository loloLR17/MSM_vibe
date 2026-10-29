#ifndef TR2_FIR_DECIMATOR10_H
#define TR2_FIR_DECIMATOR10_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "tr2/common/result.h"

/* Experimental R1 /10, version 1. Raw counts, NOT mg and NOT AC1/B3.
 * Foreground only. Caller owns the instance. No allocation, media or ISR work.
 * Each new segment emits at relative source indices 0,10,...; no STOP tail.
 * Timestamp is the supplied causal endpoint, never an invented UTC or the
 * filter centre (group delay 53 source samples). Initial zero padding is flagged.
 */
#define TR2_D10_VERSION 1u
#define TR2_D10_TAPS 107u
#define TR2_D10_FACTOR 10u
#define TR2_D10_DELAY 53u
#define TR2_D10_WARMUP 1u
#define TR2_D10_SOURCE_RAIL 2u
#define TR2_D10_OUTPUT_CLIP 4u
#define TR2_D10_TIME_UNKNOWN 8u
#define TR2_D10_COUNTER_SATURATED 16u

typedef enum {
    TR2_D10_NONE = 0, TR2_D10_STOP, TR2_D10_SOURCE_FAULT,
    TR2_D10_INDEX_GAP, TR2_D10_TIME_FAULT, TR2_D10_OVERFLOW
} FirDecimator10End;
typedef struct {
    uint64_t source_hz_num, source_hz_den;
    bool measured; /* false means nominal metadata, never a measured rate */
} FirDecimator10Clock;
typedef struct {
    uint64_t source_index, time_us; /* index and unwrapped time from authority */
    int16_t xyz[3];
    bool valid, time_valid;
} FirDecimator10Input;
typedef struct {
    uint64_t segment_id, source_index, output_index, causal_time_us;
    int16_t xyz[3];
    uint8_t flags, source_rail_axes, clipped_axes;
} FirDecimator10Output;
typedef struct {
    int16_t history[TR2_D10_TAPS][3];
    uint16_t rail_count[3], head;
    uint8_t phase;
    uint64_t segment_id, next_source_index, accepted, emitted, last_time_us;
    uint64_t source_rail_samples, clipped_outputs;
    FirDecimator10Clock clock; /* output rate = num/(10*den), checked at start */
    FirDecimator10End ended;
    bool initialized, active, time_valid, counter_saturated;
} FirDecimator10;

void fir_decimator10_init(FirDecimator10 *s);
/* Requires init, an inactive instance, fresh strictly increasing segment ID,
 * nonzero rational frequency, denominator <= UINT64_MAX/10. Clears FIR/phase.
 * origin may be nonzero; phase is relative to this explicit segment origin. */
Tr2Result fir_decimator10_start(FirDecimator10 *s, uint64_t segment_id,
                              uint64_t origin, FirDecimator10Clock clock);
void fir_decimator10_stop(FirDecimator10 *s);
void fir_decimator10_invalidate(FirDecimator10 *s, FirDecimator10End reason);
/* A bad source/index/time closes the segment before accepting that sample.
 * If time_valid is selected on the first input, require strictly increasing
 * unwrapped microseconds throughout. Availability changes require segmentation.
 * Caller must unwrap sensor timestamps BEFORE this interface; raw wrap is a fault.
 * Index UINT64_MAX is refused explicitly (no next-index wrap).
 */
Tr2Result fir_decimator10_push(FirDecimator10 *s, const FirDecimator10Input *in,
                             FirDecimator10Output *out, bool *produced);
/* Nonoverlapping arrays. Prefix counts are exact even on failure. Insufficient
 * output capacity returns NOT_AVAILABLE BEFORE consuming the next emitting
 * input; caller can resume from consumed with the SAME input, without a gap.
 * No implicit padding, drop, interpolation or automatic restart. */
Tr2Result fir_decimator10_process(FirDecimator10 *s,
    const FirDecimator10Input *in, size_t count,
    FirDecimator10Output *out, size_t capacity, size_t *consumed, size_t *written);
#endif
