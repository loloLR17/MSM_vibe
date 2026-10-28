#ifndef TR2_LIVE_DYNAMIC_H
#define TR2_LIVE_DYNAMIC_H
#include "tr2/domain/supervision/indicator_calculator.h"
/* Experimental AC1 channel, never a reinterpretation of normative B3.
 * DC estimator: first-order low-pass, fc=2Hz at Fs=26667Hz, Q16 micro-g.
 * The complementary high-pass is updated for EVERY XYZ, including window gaps.
 * Warmup=26667 samples. Measurement windows retain the configured N/P grid. */
#define TR2_AC1_CUTOFF_MILLIHZ 2000u
#define TR2_AC1_WARMUP_SAMPLES 26667u
#define TR2_AC1_ALPHA_Q30 505863LL

typedef struct {
    ActiveConfigurationSnapshot config;
    AcquisitionWindow window;
    VibrationIndicators value;
    int64_t dc_q16[3];
    int32_t value_dc_ug[3];
    uint64_t samples, run_id, value_ms, next_window_ms, first_ms;
    uint32_t sequence;
    bool enabled, running, valid, has_value, initialized, window_active, grid_ready;
} LiveDynamic;
void live_dynamic_start(LiveDynamic *,uint64_t,const ActiveConfigurationSnapshot *);
void live_dynamic_stop(LiveDynamic *);
void live_dynamic_invalidate(LiveDynamic *);
Tr2Result live_dynamic_sample(LiveDynamic *,const int16_t [3],uint64_t);
#endif
