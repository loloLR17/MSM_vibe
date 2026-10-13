#include "tr2_fifo_admission.h"
#include <assert.h>
int main(void){
 BenchResult good={.stage=6,.result=0,.cleanup_result=0,.count=4608,.xyz_count=4096,.timestamp_count=512,.who=0x7B,.cleanup_controls={0x0C,0x80,0}};
 assert(tr2_fifo_admit(&good,0,0));
 assert(!tr2_fifo_admit(0,0,0));assert(!tr2_fifo_admit(&good,1,0));assert(!tr2_fifo_admit(&good,0,1));
 for(unsigned i=0;i<11;i++){
  BenchResult bad=good;
  switch(i){case 0:bad.stage=4;break;case 1:bad.result=BENCH_IO;break;case 2:bad.cleanup_result=BENCH_IO;break;case 3:bad.count--;break;case 4:bad.xyz_count--;break;case 5:bad.timestamp_count--;break;case 6:bad.flags=0x40;break;case 7:bad.who=0;break;case 8:bad.cleanup_controls[0]=0xAC;break;case 9:bad.cleanup_controls[1]=0x86;break;default:bad.cleanup_controls[2]=0x20;}
  assert(!tr2_fifo_admit(&bad,0,0));
 }
 return 0;
}
