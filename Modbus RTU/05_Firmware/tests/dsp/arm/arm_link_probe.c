/* LINK CHECK ONLY. No vector table, HAL, sensor, DWT access or flash recipe.
 * Never flashed/executed in R1-DSP-A. Separate physical contract required. */
#include "tr2/domain/acquisition/fir_decimator10.h"
#ifdef D10_BASELINE
volatile unsigned d10_probe_marker;
void Reset_Handler(void) { for(;;)++d10_probe_marker; }
#else
FirDecimator10 d10_probe_state;
FirDecimator10Input d10_probe_input;
FirDecimator10Output d10_probe_output;
volatile unsigned d10_probe_marker;
#ifdef D10_CANDIDATES
void d10_dot_f32(const int16_t *,unsigned,float [3]);
void d10_dot_q31(const int16_t *,unsigned,int64_t [3]);
float d10_probe_float[3];int64_t d10_probe_q31[3];
#endif
void Reset_Handler(void)
{
    fir_decimator10_init(&d10_probe_state);
    d10_probe_marker=(unsigned)fir_decimator10_start(&d10_probe_state,1,0,(FirDecimator10Clock){80000,3,false});
    d10_probe_input.valid=true;
    for(unsigned i=0;i<1000;i++) {
        d10_probe_input.source_index=i;
        size_t consumed,written;
        d10_probe_marker+=(unsigned)fir_decimator10_process(&d10_probe_state,&d10_probe_input,1,&d10_probe_output,1,&consumed,&written);
#ifdef D10_CANDIDATES
        d10_dot_f32(&d10_probe_state.history[0][0],0,d10_probe_float);
        d10_dot_q31(&d10_probe_state.history[0][0],0,d10_probe_q31);
#endif
    }
    fir_decimator10_stop(&d10_probe_state);
    fir_decimator10_invalidate(&d10_probe_state,TR2_D10_SOURCE_FAULT);
    for(;;)++d10_probe_marker;
}
#endif
