#include "fifo_window.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint8_t regs[128],words[WINDOW_MAX_WORDS*7];
static uint32_t now,reads,fail_at,produced,xyz,ts,burst,mode,stop_now;
static FifoWindow result;
static uint32_t tick(void){return now++;}
static void delay(uint32_t n){now+=n;}
static uint32_t stop(void){return stop_now;}
static uint8_t tag(unsigned type,unsigned ctr)
{
 unsigned v=(type<<3)|(ctr<<1),n=v;
 n^=n>>4;n^=n>>2;n^=n>>1;return (uint8_t)(v|(n&1));
}
static int32_t read_reg(void *h,uint8_t r,uint8_t *d,uint16_t n)
{
 (void)h;reads++;if(reads==fail_at)return -1;
 if(r==58){
  d[0]=mode==1?0:40;d[1]=(uint8_t)(0x81|(mode==2?0x40:0));
  if(mode==3){d[0]=0;d[1]=2;}
  if(mode==4 && burst)d[1]|=0x40;
  if(mode==5){d[0]=0;d[1]=0;}
  if(mode==6)now+=600;
  return 0;
 }
 if(r==120){
  assert(n<=896 && n%7==0);burst++;
  for(unsigned i=0;i<n;i+=7){
   memset(d+i,0,7);unsigned is_ts=(produced%9)==8 || mode==7;
   if(is_ts){d[i]=tag(4,0);ts+=24;d[i+1]=(uint8_t)ts;d[i+2]=(uint8_t)(ts>>8);}
   else{d[i]=tag(2,xyz%4);d[i+1]=(uint8_t)xyz;d[i+2]=(uint8_t)(xyz>>8);xyz++;}
   if(mode==8 && produced==10)d[i]^=1;
   if(mode==9 && produced==10)d[i]=tag(2,3);
   if(mode==10 && produced==10)d[i]=tag(1,0);
   if(mode==11 && produced==10){d[i+1]=0xFF;d[i+2]=0x7F;}
   if(mode==12 && produced==17){d[i+1]=24;d[i+2]=0;}
   if(mode==13 && produced==18)d[i]=tag(4,0);
   produced++;
  }
  if(mode==14)regs[9]=0x14;
  return 0;
 }
 assert(r+n<=128);memcpy(d,regs+r,n);return 0;
}
static stmdev_ctx_t ctx={.read_reg=read_reg};
static BenchClock clock_ops={.tick=tick,.delay=delay,.stop=stop};
static void reset(void)
{
 memset(regs,0,sizeof(regs));regs[8]=1;regs[9]=10;regs[10]=0x86;
 regs[15]=0x7B;regs[16]=0xAC;regs[18]=0x44;regs[25]=0x20;
 memset(words,0xA5,sizeof(words));now=reads=fail_at=produced=xyz=ts=burst=mode=stop_now=0;
}
static int run(uint32_t target){return fifo_window_run(&ctx,&clock_ops,target,words,WINDOW_MAX_WORDS,&result);}
int main(void)
{
 reset();assert(run(512)==0);assert(result.xyz==512 && result.timestamps==63 && result.received==575 && result.bursts>=5);
 unsigned nominal_reads=reads;assert(words[result.received*7]==0xA5);
 reset();assert(run(4096)==0);assert(result.xyz==4096 && result.timestamps==511 && result.received==4607 && result.bursts>=36);assert(words[result.received*7]==0xA5);
 reset();assert(run(513)==BENCH_CAPACITY);
 reset();assert(fifo_window_run(&ctx,&clock_ops,512,words,511,&result)==BENCH_CAPACITY);
 reset();assert(fifo_window_run(&ctx,&clock_ops,512,words,WINDOW_MAX_WORDS+1,&result)==BENCH_CAPACITY);
 reset();regs[17]=0xFF;regs[24]=0xE2;assert(run(512)==0); /* reserved 11/18 */
 for(unsigned a=7;a<=25;a++){
  if((a>10 && a<15) || a==17 || a==24)continue;
  reset();regs[a]^=1;assert(run(512)==BENCH_CONFIG);
 }
 reset();mode=2;assert(run(512)==BENCH_OVERFLOW && !result.received);
 reset();mode=3;assert(run(512)==BENCH_OVERFLOW && !result.received);
 reset();mode=4;assert(run(512)==BENCH_OVERFLOW && result.received==128);
 reset();mode=5;assert(run(512)==BENCH_TIMEOUT);assert(!result.received);
 reset();mode=6;assert(run(512)==BENCH_TIMEOUT);
 reset();stop_now=1;assert(run(512)==BENCH_STOP);
 reset();mode=7;assert(run(512)!=0);assert(!result.xyz);
 for(unsigned m=8;m<=13;m++){reset();mode=m;assert(run(512)==BENCH_TAG);assert(result.received==128);assert(result.reason);}
 reset();mode=14;assert(run(512)==BENCH_CONFIG && result.received==128);
 for(unsigned f=1;f<=nominal_reads;f++){reset();fail_at=f;assert(run(512)==BENCH_IO);assert(result.received<=576);}
 reset();now=0xFFFFFF00U;assert(run(512)==0); /* tick wrap */
 printf("window success512/4096, guards, received-buffer retention, %u IO fault positions passed\n",nominal_reads);
 return 0;
}
