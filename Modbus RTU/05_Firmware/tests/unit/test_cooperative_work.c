#define TR2_LIVE_R2_TEST_LIBRARY 1
#include "test_live_command_engine.c"
#include "tr2/common/cooperative_work.h"
#include <time.h>

typedef struct {uint64_t us;unsigned calls,stops;bool fail,long_service;} WorkModel;
static uint64_t work_now(void *ctx){WorkModel *m=ctx;return m->us++;}
static Tr2Result work_service(void *ctx)
{WorkModel *m=ctx;m->calls++;m->us+=m->long_service?2000:1;return m->fail?TR2_ERROR_UNAVAILABLE:TR2_OK;}
static void work_stop(void *ctx){((WorkModel *)ctx)->stops++;}
static void init_work(Tr2CooperativeWork *w,WorkModel *m)
{memset(m,0,sizeof(*m));assert(tr2_cooperative_init(w,m,work_now,work_service,work_stop,32,1000,1000)==TR2_OK);}
static void callback_failures(void)
{
 Tr2CooperativeWork w;WorkModel m;uint8_t data[64]={0},expected[32],actual[32];
 init_work(&w,&m);assert(tr2_cooperative_enter(&w)==TR2_OK);m.us+=1001;
 assert(tr2_cooperative_checkpoint(&w)!=TR2_OK&&w.deadline_missed&&m.stops==1);
 assert(tr2_cooperative_leave(&w)!=TR2_OK&&!w.depth);
 assert(tr2_cooperative_enter(&w)!=TR2_OK); /* never silently clear a sticky error */
 init_work(&w,&m);m.long_service=true;assert(tr2_cooperative_enter(&w)!=TR2_OK&&m.stops==1);
 init_work(&w,&m);assert(tr2_cooperative_enter(&w)==TR2_OK);m.us=0;
 assert(tr2_cooperative_checkpoint(&w)!=TR2_OK&&m.stops==1);assert(tr2_cooperative_leave(&w)!=TR2_OK&&!w.depth);
 init_work(&w,&m);m.fail=true;assert(tr2_sha256_bytes_cooperative(data,sizeof(data),actual,&w)!=TR2_OK&&m.stops);
 init_work(&w,&m);assert(tr2_sha256_bytes(data,sizeof(data),expected)==TR2_OK);
 assert(tr2_sha256_bytes_cooperative(data,sizeof(data),actual,&w)==TR2_OK&&!memcmp(expected,actual,32));
 w.servicing=true;assert(tr2_sha256_bytes_cooperative(data,sizeof(data),actual,&w)==TR2_ERROR_INVALID_STATE);
 puts("Cooperative callback failure, deadline/reversal, sticky abort and reentry, unchanged SHA: PASS");
}
static uint64_t cpu_now(void *ctx)
{(void)ctx;return (uint64_t)clock()*1000000u/CLOCKS_PER_SEC;}
static Tr2Result cpu_service(void *ctx){((WorkModel *)ctx)->calls++;return TR2_OK;}
static void host_cpu_measurement(void)
{
 Bench *b=bench();Tr2CooperativeWork w;WorkModel m={0};
 assert(tr2_cooperative_init(&w,&m,cpu_now,cpu_service,work_stop,32,1000000,1000000)==TR2_OK);
 assert(transactional_image_media_bind_work(&b->f->historical,&w)==TR2_OK);
 assert(transactional_image_media_bind_work(&b->f->live,&w)==TR2_OK);
 LiveCommandRequest q=request(1,1,13);run_command(b,&q);q=request(1,2,14);run_command(b,&q);
 printf("HOST_CPU_ONLY,work_max_us=%llu,callback_max_us=%llu,checkpoints=%llu,clock=C_clock_process_CPU\n",
  (unsigned long long)w.max_work_us,(unsigned long long)w.max_service_us,(unsigned long long)w.checkpoints);
 destroy(b);
}
int main(void)
{
 callback_failures();host_cpu_measurement();
 for(unsigned stage=0;stage<10;stage++) {
  Tr2CooperativeWork w;WorkModel m;init_work(&w,&m);
  cut_campaign_with_work(stage,&w);assert(m.calls>1000&&!w.depth&&!m.stops);
 }
 puts("Original 1561 R2 power-loss oracles reused with cooperative physical chunks/CRC/SHA, all phases: PASS");
 return 0;
}
