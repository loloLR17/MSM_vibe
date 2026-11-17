#ifndef SHIELD_BENCH_GATES_H
#define SHIELD_BENCH_GATES_H
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#define FRAM_BYTES 262144u
#define SCRATCH_OFFSET (FRAM_BYTES-64u)
#define WRITE_ARM 0x53414645u
static inline bool bringup_uid_allowed(const uint32_t u[3])
{return u && (u[0]==0x001B0046u||u[0]==0x004A003Du)&&u[1]==0x38425018u&&u[2]==0x20333157u;}
static inline bool bench_card(const uint8_t cid[16],uint32_t sectors)
{
 static const uint8_t expected[16]={0x03,0x53,0x44,0x53,0x44,0x33,0x32,0x47,0x85,0x8b,0xf0,0x92,0x3c,0x01,0x92,0x7b};
 return cid && sectors==62333952u && memcmp(cid,expected,16)==0;
}
static inline bool bench_fram_write(uint32_t arm,bool exported,bool identified,uint32_t offset,uint32_t n)
{return arm==WRITE_ARM && exported && identified && offset==SCRATCH_OFFSET && n==64u;}
#endif
