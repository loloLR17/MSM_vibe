#ifndef R2_PATTERN_H
#define R2_PATTERN_H
#include <stdint.h>
#include <stddef.h>
/* Version1 deterministic byte stream; interpreted as interleaved int16 XYZ.
 * Transport boundaries may split XYZ. No physical acceleration claim. */
static inline void r2_pattern(uint32_t offset,uint32_t seed,uint8_t *out,size_t n)
{
 for(size_t i=0;i<n;i++){
  uint32_t at=offset+(uint32_t)i,x=(at/4u)^seed;
  x^=x>>16;x*=0x7feb352du;x^=x>>15;x*=0x846ca68bu;x^=x>>16;
  out[i]=(uint8_t)(x>>(8u*(at%4u)));
 }
}
#endif
