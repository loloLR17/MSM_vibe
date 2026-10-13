#ifndef TR2_FIFO_ADMISSION_H
#define TR2_FIFO_ADMISSION_H
#include "bench.h"
enum {TR2_FIFO_INVALID,TR2_FIFO_ARMED,TR2_FIFO_CAPTURING,TR2_FIFO_READY};
static inline int tr2_fifo_admit(const BenchResult *r,uint32_t refused,uint32_t overflow)
{
 return r && r->stage==6 && r->result==BENCH_OK && r->cleanup_result==BENCH_OK &&
 r->xyz_count==4096 && r->timestamp_count==512 && r->count==4608 &&
 !(r->flags&0x68) && r->who==0x7B && !refused && !overflow &&
 !(r->cleanup_controls[0]&0xE0) && !(r->cleanup_controls[1]&7) &&
 !(r->cleanup_controls[2]&0x20);
}
#endif
