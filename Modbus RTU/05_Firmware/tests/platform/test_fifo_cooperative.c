#include "fifo_cooperative.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
static uint8_t regs[128],words[36864*7];static uint32_t now,sequence,xyz,timestamp,mode,io_count,fail_io,stopped,cleanups;
static uint32_t tick(void){return now;}static void delay(uint32_t n){now+=n;}static uint32_t stop(void){return stopped;}static void cleanup(void){cleanups++;}
static uint8_t tag(uint32_t kind,uint32_t n){uint8_t v=(kind<<3)|(n<<1);return v|(__builtin_parity(v)&1);}
static int32_t read_reg(void *h,uint8_t reg,uint8_t *data,uint16_t n){
 (void)h;if(++io_count==fail_io)return -1;
 if(reg==58){data[0]=44;data[1]=1;
  if(mode==1){data[0]=data[1]=0;}if(mode==2)data[1]=0x41;
  if(mode==3 && xyz>128)data[1]=0x69;
  return 0;
 }
 if(reg==120){
  assert(n<=896 && n%7==0);
  for(unsigned i=0;i<n;i+=7){memset(data+i,0,7);
   if(sequence%9==0){data[i]=tag(4,0);timestamp+=24;memcpy(data+i+1,&timestamp,4);}
   else{data[i]=tag(2,xyz%4);uint16_t v=xyz%1000;memcpy(data+i+1,&v,2);xyz++;}
   if(mode==4 && sequence==10)data[i]^=1;
   if(mode==5 && sequence==10)data[i]=tag(2,3);
   if(mode==6 && sequence==10)data[i]=tag(1,0);
   if(mode==7 && sequence==10){data[i+1]=255;data[i+2]=127;}
   if(mode==10 && sequence==18){uint32_t old=24;memcpy(data+i+1,&old,4);}
   if(mode==11 && sequence==10)data[i]=tag(4,0);
   sequence++;
  }
  if(mode==8)regs[9]=0x14;
  return 0;
 }
 assert(reg+n<=128);memcpy(data,regs+reg,n);return 0;
}
static int32_t write_reg(void *h,uint8_t reg,const uint8_t *data,uint16_t n){
 (void)h;if(++io_count==fail_io)return -1;
 if(reg==18 && n==1 && data[0]&1){memset(regs,0,sizeof(regs));regs[15]=0x7B;regs[18]=4;return 0;}
 if(mode==9 && cleanups && reg==16)return -1;
 memcpy(regs+reg,data,n);return 0;
}
static stmdev_ctx_t ctx={.read_reg=read_reg,.write_reg=write_reg};static BenchClock clock={tick,delay,stop,cleanup};static FifoCooperative service;
static void reset(uint32_t target){memset(regs,0,sizeof(regs));regs[15]=0x7B;regs[18]=4;now=sequence=xyz=timestamp=mode=io_count=fail_io=stopped=cleanups=0;assert(!fifo_cooperative_init(&service,&ctx,&clock,words,36864,target));}
static void prepare(void){for(unsigned i=0;i<40 && service.state==COOP_CONFIGURING;i++){now++;fifo_cooperative_poll(&service);}assert(service.state==COOP_ARMED);assert(!(regs[16]&0xE0));}
static void finish(void){for(unsigned i=0;i<5000 && service.state!=COOP_READY && service.state!=COOP_INVALID;i++){now++;fifo_cooperative_poll(&service);}assert(service.state==COOP_READY || service.state==COOP_INVALID);assert(cleanups==1);}
int main(void){
 for(uint32_t target=4096;target<=32768;target*=2){reset(target);prepare();assert(!fifo_cooperative_start(&service));assert(fifo_cooperative_start(&service)!=0);finish();assert(service.state==COOP_READY);assert(service.window.xyz==target && service.window.timestamps==target/8);assert(!(regs[16]&0xE0) && !(regs[10]&7) && !(regs[25]&0x20));}
 for(unsigned m=1;m<=11;m++){reset(4096);prepare();mode=m;assert(!fifo_cooperative_start(&service));finish();assert(service.state==COOP_INVALID);}
 for(unsigned pos=1;pos<=30;pos++){reset(4096);fail_io=pos;for(unsigned i=0;i<1000 && service.state!=COOP_INVALID;i++){now++;if(service.state==COOP_ARMED)assert(!fifo_cooperative_start(&service));fifo_cooperative_poll(&service);}assert(service.state==COOP_INVALID && cleanups==1);}
 reset(4096);prepare();stopped=1;assert(!fifo_cooperative_start(&service));finish();assert(service.result==BENCH_STOP);
 reset(4096);prepare();assert(!fifo_cooperative_start(&service));while(service.state!=COOP_CAPTURING){now++;fifo_cooperative_poll(&service);}now+=500;finish();assert(service.result==BENCH_TIMEOUT);
 reset(4096);service.window.target_xyz=123;assert(fifo_cooperative_start(&service)!=0);
 puts("four V1 windows, cooperative scheduling, errors/cleanup and30 IO failure positions passed");return 0;
}
