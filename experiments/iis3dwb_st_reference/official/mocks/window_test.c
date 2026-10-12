#include "platform.h"
#include "port.h"
#include "spi_diagnostic.h"
#include "fifo_window.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
extern BenchResult bench_result;
extern FifoWindow fifo_window;
static uint8_t regs[128];
static uint32_t ticks,records,bytes,snapshots,cleanup_active,produced,xyz,ts;
static unsigned fault;
static uint32_t tick(void){return ticks;}
static void delay(uint32_t n){ticks+=n;}
static uint32_t stop(void){return 0;}
static void cleanup(void){cleanup_active=1;}
static int room(unsigned n)
{
 if(bench_log_refusal(records,bytes,snapshots,n,cleanup_active))return -1;
 records++;bytes+=n;snapshots+=2;return 0;
}
static uint8_t tag(unsigned type,unsigned ctr)
{
 unsigned b=(type<<3)|(ctr<<1),p=b;p^=p>>4;p^=p>>2;p^=p>>1;
 return (uint8_t)(b|(p&1));
}
static int32_t read_reg(void *h,uint8_t r,uint8_t *d,uint16_t n)
{
 (void)h;if(room(n))return -1;
 if(fault==1 && r==120)return -1;
 if(r==18 && (regs[18]&1))regs[18]=4;
 if(r==58){d[0]=40;d[1]=fault==2?0xC1:0x81;return 0;}
 if(r==120){
  for(unsigned i=0;i<n;i+=7){memset(d+i,0,7);
   if(produced%9==8){d[i]=tag(4,0);ts+=24;d[i+1]=(uint8_t)ts;d[i+2]=(uint8_t)(ts>>8);}
   else{d[i]=tag(2,xyz%4);d[i+1]=(uint8_t)xyz;d[i+2]=(uint8_t)(xyz>>8);xyz++;}
   if(fault==3 && produced==10)d[i]^=1;
   produced++;
  }
  if(fault==4)regs[9]=0x14;
  return 0;
 }
 memcpy(d,regs+r,n);return 0;
}
static int32_t write_reg(void *h,uint8_t r,const uint8_t *d,uint16_t n)
{
 (void)h;if(room(n) || (fault==5 && cleanup_active))return -1;
 memcpy(regs+r,d,n);return 0;
}
int platform_init(stmdev_ctx_t *ctx,BenchClock *clock)
{*ctx=(stmdev_ctx_t){.read_reg=read_reg,.write_reg=write_reg};*clock=(BenchClock){tick,delay,stop,cleanup};return 0;}
int main(int argc,char **argv)
{
 assert(argc==2);fault=(unsigned)(argv[1][0]-'0');regs[15]=123;regs[18]=4;
 if(fault==6)records=1012;
 official_prepare();official_run();assert(cleanup_active && bench_result.stage==6);
 if(fault==0 || fault==5){assert(bench_result.result==0 && bench_result.xyz_count==BENCH_WINDOW_XYZ);}
 else if(fault==1 || fault==6)assert(bench_result.result==BENCH_IO);
 else if(fault==2)assert(bench_result.result==BENCH_OVERFLOW);
 else if(fault==3)assert(bench_result.result==BENCH_TAG && bench_result.count==128);
 else if(fault==4)assert(bench_result.result==BENCH_CONFIG && bench_result.count==128);
 if(fault==5)assert(bench_result.cleanup_result==BENCH_IO);
 else{assert(bench_result.cleanup_result==0);assert(!(regs[16]&0xE0) && !(regs[10]&7) && !(regs[25]&0x20));}
 printf("official setup/window/cleanup scenario %u passed (target %u)\n",fault,BENCH_WINDOW_XYZ);
 return 0;
}
