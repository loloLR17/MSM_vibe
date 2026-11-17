#include "gates.h"
#include <assert.h>
int main(void)
{
 uint32_t u[3]={0x001B0046,0x38425018,0x20333157};assert(bringup_uid_allowed(u));u[0]=0x004A003D;assert(bringup_uid_allowed(u));u[0]++;assert(!bringup_uid_allowed(u));assert(!bringup_uid_allowed(0));
 uint8_t id[16]={3,0x53,0x44,0x53,0x44,0x33,0x32,0x47,0x85,0x8b,0xf0,0x92,0x3c,1,0x92,0x7b};assert(bench_card(id,62333952));
 for(unsigned i=0;i<16;i++){id[i]^=1;assert(!bench_card(id,62333952));id[i]^=1;}
 assert(!bench_card(id,1000215186u));assert(!bench_card(0,62333952));
 assert(bench_fram_write(WRITE_ARM,true,true,SCRATCH_OFFSET,64));
 assert(!bench_fram_write(0,true,true,SCRATCH_OFFSET,64));assert(!bench_fram_write(WRITE_ARM,false,true,SCRATCH_OFFSET,64));assert(!bench_fram_write(WRITE_ARM,true,false,SCRATCH_OFFSET,64));assert(!bench_fram_write(WRITE_ARM,true,true,0,64));assert(!bench_fram_write(WRITE_ARM,true,true,SCRATCH_OFFSET,65));return 0;
}
