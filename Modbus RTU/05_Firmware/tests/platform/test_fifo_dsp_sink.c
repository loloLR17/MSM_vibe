#include "fifo_event_time.h"
#include "tr2/domain/acquisition/fir_decimator10.h"
#include <assert.h>
#include <stdio.h>
static unsigned parity(unsigned n){unsigned p=0;while(n){p^=n&1;n>>=1;}return p;}
static void anchor(uint8_t b[7],uint32_t ticks)
{b[0]=32;b[0]|=(uint8_t)parity(b[0]);for(unsigned i=0;i<4;i++)b[i+1]=(uint8_t)(ticks>>(8*i));b[5]=0;b[6]=10;}
static void xyz(uint8_t b[7],unsigned i)
{b[0]=(uint8_t)(16+2*(i%4));b[0]|=(uint8_t)parity(b[0]);b[1]=0xe8;b[2]=3;b[3]=b[4]=b[5]=b[6]=0;}
typedef struct{FirDecimator10 d;uint64_t count,fail_at,last_us,outputs;} Sink;
static Tr2Result consume(void *ctx,uint64_t index,const int16_t value[3],uint64_t us)
{
 Sink *s=ctx;if(index==s->fail_at)return TR2_ERROR_UNAVAILABLE;
 assert(index==s->count&&value[0]==1000&&value[1]==0&&value[2]==0);
 assert(us==5000000u+index*75u/2u); /* exact us, including fractional milliseconds */
 FirDecimator10Input p={.source_index=index,.time_us=us,.valid=true,.time_valid=true};
 for(unsigned a=0;a<3;a++)p.xyz[a]=value[a];
 FirDecimator10Output out;bool ready;
 assert(fir_decimator10_push(&s->d,&p,&out,&ready)==TR2_OK);
 if(ready){assert(out.causal_time_us==us&&out.source_index==index);s->outputs++;}
 s->last_us=us;s->count++;return TR2_OK;
}
int main(void)
{
 FifoLive f={0};FifoEventTime t;Sink s={.fail_at=UINT64_MAX};uint8_t b[7];
 fir_decimator10_init(&s.d);assert(fir_decimator10_start(&s.d,1,0,(FirDecimator10Clock){80000,3,false})==TR2_OK);
 assert(fifo_event_time_init_sink(&t,&f,consume,&s,0xfffffff0u,5000000,1000000,80000)==TR2_OK);
 assert(t.metric==NULL&&t.sink==consume);
 for(unsigned i=0;i<=256;i++) {
  if(i%8==0){anchor(b,0xfffffff0u+(i/8)*24);assert(!fifo_event_time_consume(&t,b,1,i));}
  if(i<256){xyz(b,i);assert(!fifo_event_time_consume(&t,b,1,i));}
 }
 assert(s.count==256&&s.outputs==26&&s.last_us%1000!=0);
 fir_decimator10_stop(&s.d);
 assert(fir_decimator10_start(&s.d,2,0,(FirDecimator10Clock){80000,3,false})==TR2_OK);s.count=s.outputs=0;s.fail_at=3;
 assert(fifo_event_time_init_sink(&t,&f,consume,&s,100,5000000,1000000,80000)==TR2_OK);
 anchor(b,100);assert(!fifo_event_time_consume(&t,b,1,0));
 for(unsigned j=0;j<8;j++){xyz(b,j);assert(!fifo_event_time_consume(&t,b,1,j));}
 anchor(b,124);assert(fifo_event_time_consume(&t,b,1,8)==-1&&t.fault&&s.count==3);
 fir_decimator10_invalidate(&s.d,TR2_D10_SOURCE_FAULT);assert(!s.d.active);
 assert(fifo_live_set_poll_limit(&f,64)==BENCH_CONFIG);f.initialized=true;
 assert(!fifo_live_set_poll_limit(&f,64)&&f.poll_word_limit==64);
 assert(fifo_live_set_poll_limit(&f,0)==BENCH_CONFIG&&f.poll_word_limit==64);
 f.running=true;assert(fifo_live_set_poll_limit(&f,32)==BENCH_CONFIG&&f.poll_word_limit==64);
 puts("Validated DEC8 sink preserves us/index/rollover, no second FIFO reader; failures stop prefix; poll cap guarded");
 return 0;
}
