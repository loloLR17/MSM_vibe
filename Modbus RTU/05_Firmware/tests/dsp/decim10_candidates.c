/* Test-only arithmetic candidates: identical raw history, no product integration. */
#include "decim10_candidates_coefficients.h"
void d10_dot_f32(const int16_t *history, unsigned head, float acc[3])
{
    for(unsigned a=0;a<3;a++)acc[a]=0;
    for(unsigned k=0;k<107;k++) {
        for(unsigned a=0;a<3;a++)acc[a]+=(float)history[head*3+a]*d10_candidate_f32[k];
        head=head?head-1:106;
    }
}
void d10_dot_q31(const int16_t *history, unsigned head, int64_t acc[3])
{
    for(unsigned a=0;a<3;a++)acc[a]=0;
    for(unsigned k=0;k<107;k++) {
        for(unsigned a=0;a<3;a++)acc[a]+=(int64_t)history[head*3+a]*d10_candidate_q31[k];
        head=head?head-1:106;
    }
}
