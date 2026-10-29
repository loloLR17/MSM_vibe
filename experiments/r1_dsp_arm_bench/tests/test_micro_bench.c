#include "micro_bench.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint32_t cycle(void *ctx){uint32_t *p=ctx;*p+=7;return *p;}
static uint32_t frozen_tick(void *ctx){(void)ctx;return 0;}
static uint32_t timeout_tick(void *ctx){uint32_t *p=ctx;*p+=6000;return *p;}
int main(void)
{
    uint32_t v[]={99,1,7,4,2};R1CycleStats s;
    assert(!r1_cycle_statistics(v,0,&s));assert(r1_cycle_statistics(v,5,&s));
    assert(s.count==5&&s.sum==113&&s.minimum==1&&s.maximum==99&&s.p95==99&&s.p99==99&&v[0]==99);
    static R1MicroResult r;uint32_t clock=UINT32_MAX-1000u;
    assert(r1_micro_run(&r,cycle,&clock,frozen_tick,NULL,NULL,NULL)==TR2_OK);
    assert(r.complete_magic==R1_BENCH_COMPLETE&&r.error==0&&r.overhead.minimum==7);
    for(unsigned i=0;i<R1_BENCH_CASES;i++) {
        R1MicroCase *c=&r.cases[i];assert(c->fir_enabled==i%2&&c->error==0);
        assert(c->sources==c->blocks*c->burst&&c->outputs==(c->sources+9u)/10u);
        assert(c->emission.count==c->outputs&&c->block.count==256);
        assert(c->final_active==0&&c->clipped_outputs==0&&c->source_rail_outputs==0);
        assert(c->warmup_outputs==(i%2?11u:0u));
        assert(c->emission.minimum==7&&c->emission.maximum==7);
        if(i%2)assert(c->input_hash==r.cases[i-1].input_hash);
    }
    /* A failed retry must never retain the previous successful completion. */
    uint32_t tick=0;
    assert(r1_micro_run(&r,cycle,&clock,timeout_tick,&tick,NULL,NULL)!=TR2_OK);
    assert(r.complete_magic==0&&r.error!=0);
    printf("Micro bench control/paired inputs/counts/wrap/stats/deadline PASS; fake cycles only, NOT ARM results. result=%zu\n",sizeof(r));
    return 0;
}
