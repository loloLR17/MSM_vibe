#include "tr2/domain/acquisition/fir_decimator10.h"
#include "fir_decimator10_coefficients.h"
#include <limits.h>
#include <string.h>

/* The GENERATED l1 bound covers every possible int16 history, not just tones.
 * Integer products and their partial sums fit int32, including all -32768.
 * No float ABI, implementation-defined signed shift or signed-overflow MAC. */
_Static_assert(TR2_D10_COEFF_L1 * 32768LL <= INT32_MAX, "D10 accumulator bound");
static bool rail(int16_t x) { return x == INT16_MIN || x == INT16_MAX; }
static void counter(FirDecimator10 *s, uint64_t *n)
{
    if (*n == UINT64_MAX) s->counter_saturated = true;
    else ++*n;
}
void fir_decimator10_init(FirDecimator10 *s)
{
    if (s) { memset(s, 0, sizeof(*s)); s->initialized = true; }
}
Tr2Result fir_decimator10_start(FirDecimator10 *s, uint64_t id,
                              uint64_t origin, FirDecimator10Clock clock)
{
    if (!s || !s->initialized || !id || !clock.source_hz_num ||
        !clock.source_hz_den || clock.source_hz_den > UINT64_MAX / 10u ||
        origin == UINT64_MAX) return TR2_ERROR_INVALID_ARGUMENT;
    if (s->active || id <= s->segment_id) return TR2_ERROR_INVALID_STATE;
    fir_decimator10_init(s);
    s->segment_id = id; s->next_source_index = origin;
    s->clock = clock; s->active = true;
    return TR2_OK;
}
void fir_decimator10_stop(FirDecimator10 *s)
{
    if (s && s->active) { s->active = false; s->ended = TR2_D10_STOP; }
}
void fir_decimator10_invalidate(FirDecimator10 *s, FirDecimator10End why)
{
    if (s && s->active) {
        s->active = false;
        s->ended = (why >= TR2_D10_SOURCE_FAULT && why <= TR2_D10_OVERFLOW)
                 ? why : TR2_D10_SOURCE_FAULT;
    }
}
static int32_t nearest_even_q15(int32_t acc)
{
    uint32_t magnitude = acc < 0 ? 0u - (uint32_t)acc : (uint32_t)acc;
    uint32_t whole = magnitude >> 15u, rem = magnitude & 32767u;
    if (rem > 16384u || (rem == 16384u && (whole & 1u))) ++whole;
    return acc < 0 ? -(int32_t)whole : (int32_t)whole;
}
Tr2Result fir_decimator10_push(FirDecimator10 *s, const FirDecimator10Input *in,
                             FirDecimator10Output *out, bool *produced)
{
    if (produced) *produced = false;
    if (!s || !in || !out || !produced) return TR2_ERROR_INVALID_ARGUMENT;
    if (!s->initialized || !s->active) return TR2_ERROR_INVALID_STATE;
    FirDecimator10End fault = TR2_D10_NONE;
    if (!in->valid) fault = TR2_D10_SOURCE_FAULT;
    else if (in->source_index != s->next_source_index) fault = TR2_D10_INDEX_GAP;
    else if (in->source_index == UINT64_MAX || s->accepted == UINT64_MAX ||
             (s->phase == 0 && s->emitted == UINT64_MAX)) fault = TR2_D10_OVERFLOW;
    else if (s->accepted && (in->time_valid != s->time_valid ||
             (in->time_valid && in->time_us <= s->last_time_us))) fault = TR2_D10_TIME_FAULT;
    if (fault != TR2_D10_NONE) {
        fir_decimator10_invalidate(s, fault); return TR2_ERROR_INVALID_STATE;
    }
    bool input_rail = false;
    for (unsigned a = 0; a < 3; ++a) {
        if (rail(s->history[s->head][a])) --s->rail_count[a];
        s->history[s->head][a] = in->xyz[a];
        if (rail(in->xyz[a])) { ++s->rail_count[a]; input_rail = true; }
    }
    if (input_rail) counter(s, &s->source_rail_samples);
    s->last_time_us = in->time_us; s->time_valid = in->time_valid;
    ++s->accepted; ++s->next_source_index;
    if (s->phase == 0) {
        memset(out, 0, sizeof(*out));
        out->segment_id = s->segment_id; out->source_index = in->source_index;
        out->output_index = s->emitted; out->causal_time_us = in->time_valid ? in->time_us : 0;
        if (s->accepted < TR2_D10_TAPS) out->flags |= TR2_D10_WARMUP;
        if (!in->time_valid) out->flags |= TR2_D10_TIME_UNKNOWN;
        int32_t acc[3] = {0, 0, 0};
        unsigned at = s->head;
        for (unsigned k = 0; k < TR2_D10_TAPS; ++k) {
            for (unsigned a = 0; a < 3; ++a)
                acc[a] += (int32_t)s->history[at][a] * tr2_d10_q15[k];
            at = at ? at - 1u : TR2_D10_TAPS - 1u;
        }
        for (unsigned a = 0; a < 3; ++a) {
            if (s->rail_count[a]) out->source_rail_axes |= (uint8_t)(1u << a);
            int32_t q = nearest_even_q15(acc[a]);
            if (q > INT16_MAX) { q = INT16_MAX; out->clipped_axes |= (uint8_t)(1u << a); }
            if (q < INT16_MIN) { q = INT16_MIN; out->clipped_axes |= (uint8_t)(1u << a); }
            out->xyz[a] = (int16_t)q;
        }
        if (out->source_rail_axes) out->flags |= TR2_D10_SOURCE_RAIL;
        if (out->clipped_axes) { out->flags |= TR2_D10_OUTPUT_CLIP; counter(s, &s->clipped_outputs); }
        if (s->counter_saturated) out->flags |= TR2_D10_COUNTER_SATURATED;
        ++s->emitted; *produced = true;
    }
    s->head = (uint16_t)((s->head + 1u) % TR2_D10_TAPS);
    s->phase = (uint8_t)((s->phase + 1u) % TR2_D10_FACTOR);
    return TR2_OK;
}
Tr2Result fir_decimator10_process(FirDecimator10 *s,
    const FirDecimator10Input *in, size_t count,
    FirDecimator10Output *out, size_t capacity, size_t *consumed, size_t *written)
{
    if (consumed) *consumed = 0;
    if (written) *written = 0;
    if (!s || (!in && count) || (!out && capacity) || !consumed || !written)
        return TR2_ERROR_INVALID_ARGUMENT;
    if (!s->initialized || !s->active) return TR2_ERROR_INVALID_STATE;
    for (size_t i = 0; i < count; ++i) {
        if (s->phase == 0 && *written == capacity) return TR2_ERROR_NOT_AVAILABLE;
        FirDecimator10Output value; bool ready;
        Tr2Result r = fir_decimator10_push(s, &in[i], &value, &ready);
        if (r != TR2_OK) return r;
        ++*consumed;
        if (ready) out[(*written)++] = value;
    }
    return TR2_OK;
}
