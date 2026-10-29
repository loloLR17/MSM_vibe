#ifndef FIFO_EVENT_TIME_H
#define FIFO_EVENT_TIME_H
#include "fifo_live.h"
#include "tr2/application/live_metrology.h"
/* Opt-in DEC8 provider. Requires timestamp BEFORE the XYZ of the same TAG_CNT.
 * Waits for the next actual anchor, then interpolates only inside that bracket.
 * DEC8 anchor steps must stay within [23,25] sensor ticks (24 observed;
 * one tick quantization allowance). This gate never generates sample instants.
 * No nominal ODR, dequeue time, extrapolation, UTC, media or allocation. */
typedef Tr2Result (*FifoTimedSink)(void *,uint64_t,const int16_t [3],uint64_t);
typedef struct {
 FifoLive *fifo;LiveMetrology *metric;
 FifoTimedSink sink;void *sink_context;
 uint64_t base_us,span_us,span_ticks,ticks,anchor_ticks,input_index,emitted;
 uint64_t max_residual_us,last_event_us,first_event_us;
 uint32_t raw,anchor_raw,groups,calibration_raw;
 uint64_t calibration_us,mapping_cycles,metrology_cycles;
 uint8_t counter,count;
 int16_t xyz[8][3];
 bool initialized,anchor,fault;
} FifoEventTime;
Tr2Result fifo_event_time_init(FifoEventTime *,FifoLive *,LiveMetrology *,
 uint32_t,uint64_t,uint64_t,uint64_t);
/* Optional isolated sink uses the SAME validated DEC8 mapper, preserving exact
 * microseconds/index. Exclusive of LiveMetrology, default constructor unchanged.
 * Sink failure closes the mapper; caller must invalidate its DSP and stop FIFO. */
Tr2Result fifo_event_time_init_sink(FifoEventTime *,FifoLive *,FifoTimedSink,void *,
 uint32_t,uint64_t,uint64_t,uint64_t);
/* MCU clock bracketing of a live counter read. A violated empirical envelope
 * fails closed; it does not silently move an old window or claim UTC. */
Tr2Result fifo_event_time_check(FifoEventTime *,uint32_t,uint64_t,uint64_t,uint64_t);
/* Optional continuous sensor/MCU clock discipline. Checks the SAME residual
 * limit before correction; measured slope plus measured phase error steers
 * the next interval, continuously pivoting at the last FIFO anchor. Published
 * event times never change. No UTC or nominal ODR is introduced. */
Tr2Result fifo_event_time_track(FifoEventTime *,uint32_t,uint64_t,uint64_t,uint64_t);
int fifo_event_time_consume(void *,const uint8_t *,uint32_t,uint64_t);
#endif
