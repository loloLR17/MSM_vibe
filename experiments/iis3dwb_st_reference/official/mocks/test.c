#include "platform.h"
#include "port.h"
#include "spi_diagnostic.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
extern BenchResult bench_result;
extern uint32_t official_packets;
static uint8_t regs[128];
static uint32_t ticks,calls,records,bytes,snapshots,cleanup_active;
static unsigned fault,produced;
static uint32_t tick(void){return ticks;}
static void delay(uint32_t n){ticks+=n;}
static uint32_t stop(void){return 0;}
static void cleanup(void){cleanup_active=1;}
static int room(unsigned n)
{
 if(bench_log_refusal(records,bytes,snapshots,n,cleanup_active))return -1;
 records++;bytes+=n;snapshots+=2;return 0;
}
static int32_t read_reg(void *h,uint8_t r,uint8_t *d,uint16_t n)
{
 (void)h;if(room(n))return -1;calls++;
 if(fault==1 && calls==4)return -1;
 if(r==18 && (regs[18]&1))regs[18]=4;
 if(r==58){d[0]=fault==4?44:0;d[1]=fault==2?0xC1:0x81;return 0;}
 if(r==120){for(unsigned i=0;i<n;i+=7){unsigned tag=fault==3?4:2;
  unsigned b=(tag<<3)|((produced%4)<<1);unsigned parity=0;
  for(unsigned j=0;j<8;j++)parity^=(b>>j)&1;
  d[i]=(uint8_t)(b|parity);memset(d+i+1,0,6);d[i+1]=(uint8_t)produced++;}return 0;}
 memcpy(d,regs+r,n);return 0;
}
static int32_t write_reg(void *h,uint8_t r,const uint8_t *d,uint16_t n)
{(void)h;if(room(n))return -1;calls++;memcpy(regs+r,d,n);return 0;}
int platform_init(stmdev_ctx_t *ctx,BenchClock *clock)
{*ctx=(stmdev_ctx_t){.read_reg=read_reg,.write_reg=write_reg};*clock=(BenchClock){tick,delay,stop,cleanup};return 0;}
int main(int argc,char **argv)
{
 assert(argc==2);if(!strcmp(argv[1],"io"))fault=1;else if(!strcmp(argv[1],"overflow"))fault=2;else if(!strcmp(argv[1],"timestamp"))fault=3;else if(!strcmp(argv[1],"depth300"))fault=4;
 regs[15]=!strcmp(argv[1],"who")?0:123;regs[18]=4;
 if(!strcmp(argv[1],"capacity"))records=1012;
 official_prepare();official_run();assert(cleanup_active);
 if(!strcmp(argv[1],"capacity")){assert(bench_result.result==1 && bench_result.cleanup_result==0);}
 else if(!strcmp(argv[1],"who")){assert(bench_result.result==2 && !bench_result.count);}
 else if(fault==1){assert(bench_result.result==1 && !bench_result.count);}
 else if(fault==2){assert(bench_result.result==7 && !bench_result.count);}
 else {
#if OFFICIAL_STATUS10
   assert(bench_result.result==0 && bench_result.count==(fault==4?300:256) && official_packets==1);
   assert(bench_result.xyz_count==(fault==3?0:(fault==4?300:256)));
   assert(bench_result.timestamp_count==(fault==3?256:0));
#else
   if(fault==4){assert(bench_result.result==0 && bench_result.count==44 && bench_result.xyz_count==44);}
   else assert(bench_result.result==11 && !bench_result.count);
#endif
 }
 assert(bench_result.cleanup_result==0);
 assert(!(regs[16]&0xE0) && !(regs[10]&7) && !(regs[25]&0x20));
 puts("official function and platform safety scenario passed");return 0;
}
