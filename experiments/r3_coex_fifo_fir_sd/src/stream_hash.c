#include "stream_hash.h"
/* Reuse the existing audited SHA compressor without changing the core API.
 * Unused one-shot/cooperative entry points are removed by section GC. */
#include "../../../Modbus RTU/05_Firmware/src/common/sha256.c"
void r3_hash_init(R3Hash *s)
{static const uint32_t initial[8]={0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};memset(s,0,sizeof *s);memcpy(s->h,initial,sizeof initial);}
void r3_hash_update(R3Hash *s,const uint8_t *b,size_t n)
{for(size_t i=0;i<n;i++){s->pending[s->used++]=b[i];s->bytes++;if(s->used==64){block(s->h,s->pending);s->used=0;}}}
void r3_hash_finish(const R3Hash *input,uint8_t out[32])
{
 R3Hash s=*input;uint64_t bits=s.bytes*8u;uint8_t pad[128]={0x80};size_t n=s.used<56?56-s.used:120-s.used;
 r3_hash_update(&s,pad,n);for(unsigned i=0;i<8;i++)pad[i]=(uint8_t)(bits>>(56-8*i));r3_hash_update(&s,pad,8);
 for(unsigned i=0;i<32;i++)out[i]=(uint8_t)(s.h[i/4]>>(24-8*(i%4)));
}
