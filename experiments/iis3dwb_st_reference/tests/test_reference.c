#include "bench.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint8_t regs[128];
static unsigned time_ms, io_count, fail_at, stuck_reset, stopped, overflow;
static unsigned frozen, bad_tag, corrupt, no_ifinc, received, writes;
static BenchResult out;
static uint32_t tick(void){return time_ms++;}
static void delay(uint32_t n){time_ms+=n;}
static uint32_t stop(void){return stopped;}
static int32_t read_reg(void *h,uint8_t r,uint8_t *d,uint16_t n)
{
  (void)h;if(++io_count==fail_at)return -1;
  if(r==IIS3DWB_CTRL3_C && (regs[r]&1) && !stuck_reset)regs[r]=no_ifinc?0:4;
  if(r==IIS3DWB_FIFO_STATUS1){assert(n==2);d[0]=0;d[1]=(uint8_t)(0x81|(overflow?0x60:0));return 0;}
  if(r==IIS3DWB_STATUS_REG){d[0]=frozen?0:3;return 0;}
  if(r==IIS3DWB_OUTX_L_A){for(unsigned i=0;i<n;i++)d[i]=(uint8_t)(received+i);received++;return 0;}
  if(r==IIS3DWB_FIFO_DATA_OUT_TAG){
    assert(n<=896 && n%7==0);
    for(unsigned i=0;i<n;i+=7){d[i]=(uint8_t)((bad_tag?1:2)<<3);for(unsigned j=1;j<7;j++)d[i+j]=(uint8_t)(received+j);received++;}
    if(corrupt)regs[IIS3DWB_FIFO_CTRL3]=0x14;
    return 0;
  }
  assert((unsigned)r+n<=128);memcpy(d,regs+r,n);return 0;
}
static int32_t write_reg(void *h,uint8_t r,const uint8_t *d,uint16_t n)
{(void)h;writes++;if(++io_count==fail_at)return -1;assert((unsigned)r+n<=128);memcpy(regs+r,d,n);return 0;}
static void reset(void)
{
  memset(regs,0,sizeof(regs));regs[IIS3DWB_WHO_AM_I]=IIS3DWB_ID;regs[IIS3DWB_CTRL3_C]=4;
  time_ms=io_count=fail_at=stuck_reset=stopped=overflow=0;
  frozen=bad_tag=corrupt=no_ifinc=received=writes=0;
}
static stmdev_ctx_t ctx={.read_reg=read_reg,.write_reg=write_reg};
static BenchClock clock_ops={tick,delay,stop};
int main(void)
{
  reset();assert(bench_run(&ctx,&clock_ops,2,0,&out)==BENCH_OK);
  assert(out.who==IIS3DWB_ID && !writes && out.count==0);
  reset();assert(bench_run(&ctx,&clock_ops,0,0,&out)==BENCH_OK);
  assert(out.count==64 && out.xyz_count==64 && out.cleanup_result==0 && regs[16]==0x02);
  reset();assert(bench_run(&ctx,&clock_ops,1,0,&out)==BENCH_ST_LEVEL);
  assert(out.st_level==0 && out.raw_level==256 && out.count==0);
  reset();assert(bench_run(&ctx,&clock_ops,1,1,&out)==BENCH_OK);
  assert(out.count==512 && out.xyz_count==512 && received==512 && out.cleanup_result==0);
  assert(out.st_batch==IIS3DWB_XL_NOT_BATCHED && (out.controls[2]&15)==0x0A);
  reset();regs[15]=0;assert(bench_run(&ctx,&clock_ops,0,0,&out)==BENCH_WHO);assert(!writes);
  reset();fail_at=1;assert(bench_run(&ctx,&clock_ops,0,0,&out)==BENCH_IO);assert(!writes);
  reset();stuck_reset=1;assert(bench_run(&ctx,&clock_ops,0,0,&out)==BENCH_RESET_TIMEOUT);
  reset();frozen=1;assert(bench_run(&ctx,&clock_ops,0,0,&out)==BENCH_TIMEOUT);
  reset();stopped=1;assert(bench_run(&ctx,&clock_ops,1,1,&out)==BENCH_STOP);
  reset();overflow=1;assert(bench_run(&ctx,&clock_ops,1,1,&out)==BENCH_OVERFLOW);assert(!out.count);
  reset();bad_tag=1;assert(bench_run(&ctx,&clock_ops,1,1,&out)==BENCH_TAG);assert(out.count==128);
  reset();corrupt=1;assert(bench_run(&ctx,&clock_ops,1,1,&out)==BENCH_CONFIG);assert(out.count==128);
  reset();no_ifinc=1;assert(bench_run(&ctx,&clock_ops,0,0,&out)==BENCH_CONFIG);
  /* Inject a transport error at every position of the nominal initialization. */
  for(unsigned pos=2;pos<30;pos++){reset();fail_at=pos;assert(bench_run(&ctx,&clock_ops,1,1,&out)==BENCH_IO);}
  puts("41 scenarios passed: WHO, official driver, polling/FIFO, two upstream defects, failures and bounds");
  return 0;
}
