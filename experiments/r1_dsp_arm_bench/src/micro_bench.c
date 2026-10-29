#include "micro_bench.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>
_Static_assert(sizeof(R1CycleStats)==32,"cycle schema");
_Static_assert(sizeof(R1MicroCase)==128,"case schema");
_Static_assert(sizeof(R1MicroResult)==1136,"result schema");
uint32_t r1_block_cycles[R1_BENCH_CASES][R1_BENCH_BLOCKS];
uint32_t r1_emit_cycles[R1_BENCH_CASES][R1_BENCH_MAX_EMIT];
uint32_t r1_overhead_cycles[R1_BENCH_BLOCKS];
static uint32_t sorted[R1_BENCH_MAX_EMIT];
static FirDecimator10 state;
static FirDecimator10Input input[R1_BENCH_MAX_BURST];
static int compare(const void *a,const void *b)
{
    uint32_t x=*(const uint32_t *)a,y=*(const uint32_t *)b;
    return (x>y)-(x<y);
}
bool r1_cycle_statistics(const uint32_t *data,size_t n,R1CycleStats *s)
{
    if(!data||!s||!n||n>R1_BENCH_MAX_EMIT)return false;
    memset(s,0,sizeof(*s));s->count=(uint32_t)n;
    for(size_t i=0;i<n;i++){sorted[i]=data[i];s->sum+=data[i];}
    qsort(sorted,n,sizeof(*sorted),compare);
    s->minimum=sorted[0];s->maximum=sorted[n-1];
    s->p95=sorted[(n*95u+99u)/100u-1u];s->p99=sorted[(n*99u+99u)/100u-1u];return true;
}
static uint32_t hash_word(uint32_t h,uint32_t x)
{ for(unsigned i=0;i<4;i++){h=(h^(x&255u))*16777619u;x>>=8;}return h; }
static void prepare(uint32_t first,unsigned n)
{
    for(unsigned i=0;i<n;i++) {
        uint32_t index=first+i;
        input[i]=(FirDecimator10Input){.source_index=index,.time_us=(uint64_t)index*75u/2u,.valid=true,.time_valid=true};
        for(unsigned a=0;a<3;a++) {
            uint32_t bits=(index*1103515245u+(a+1u)*12345u)^(index>>3u);
            input[i].xyz[a]=(int16_t)((int32_t)(bits%24001u)-12000);
        }
    }
}
/* A bypass is NOT an FIR result: one raw endpoint out of ten, separate case ID.
 * Shared validation/dispatch/publication path; incremental comparison includes
 * the real FIR API/history/quality overhead, not only an ideal MAC loop. */
static Tr2Result consume(const FirDecimator10Input *p,bool fir,
                         FirDecimator10Output *out,bool *ready,uint64_t index)
{
    if(!p->valid||p->source_index!=index||!p->time_valid||p->time_us!=index*75u/2u)return TR2_ERROR_INVALID_STATE;
    if(fir)return fir_decimator10_push(&state,p,out,ready);
    *ready=index%10u==0;
    if(*ready) {
        memset(out,0,sizeof(*out));out->segment_id=1;out->source_index=index;out->output_index=index/10u;
        out->causal_time_us=p->time_us;memcpy(out->xyz,p->xyz,sizeof(out->xyz));
    }
    return TR2_OK;
}
Tr2Result r1_micro_run(R1MicroResult *result,R1ReadCounter cycle,void *cc,
                      R1ReadCounter tick,void *tc,R1ReadCounter irq,void *ic)
{
    if(!result||!cycle||!tick)return TR2_ERROR_INVALID_ARGUMENT;
    result->complete_magic=0;result->error=0;
    const unsigned bursts[4]={1,8,60,128};
    for(unsigned i=0;i<R1_BENCH_BLOCKS;i++) {
        uint32_t start=cycle(cc);r1_overhead_cycles[i]=cycle(cc)-start;
    }
    if(!r1_cycle_statistics(r1_overhead_cycles,R1_BENCH_BLOCKS,&result->overhead))return TR2_ERROR_INTERNAL;
    for(unsigned pair=0;pair<4;pair++)for(unsigned order=0;order<2;order++) {
        unsigned enabled=order^(pair&1u),slot=pair*2u+enabled;
        R1MicroCase *c=&result->cases[slot];memset(c,0,sizeof(*c));
        c->fir_enabled=enabled;c->burst=bursts[pair];c->blocks=R1_BENCH_BLOCKS;
        c->input_hash=c->output_hash=2166136261u;
        fir_decimator10_init(&state);
        if(fir_decimator10_start(&state,1,0,(FirDecimator10Clock){80000,3,false})!=TR2_OK)return TR2_ERROR_INTERNAL;
        uint32_t start_tick=tick(tc),start_cycle=cycle(cc);
        c->irq_before=irq?irq(ic):0;
        for(unsigned block=0;block<R1_BENCH_BLOCKS;block++) {
            if((uint32_t)(tick(tc)-start_tick)>5000u){c->error=1;goto failed;}
            prepare(c->sources,c->burst); /* outside block measurement */
            uint32_t begin=cycle(cc);
            for(unsigned i=0;i<c->burst;i++) {
                bool expected=(c->sources%10u)==0,ready=false;
                FirDecimator10Output out;
                uint32_t emission_begin=expected?cycle(cc):0;
                Tr2Result rc=consume(&input[i],enabled!=0,&out,&ready,c->sources);
                if(expected) {
                    uint32_t delta=cycle(cc)-emission_begin;
                    if(c->outputs>=R1_BENCH_MAX_EMIT){c->error=2;goto failed;}
                    r1_emit_cycles[slot][c->outputs]=delta;
                }
                if(rc!=TR2_OK||ready!=expected){c->error=3;goto failed;}
                if(ready) {
                    if(out.source_index!=c->sources||out.output_index!=c->outputs||out.causal_time_us!=input[i].time_us){c->error=4;goto failed;}
                    if(enabled) {
                        if(out.flags&TR2_D10_WARMUP)c->warmup_outputs++;
                        if(out.flags&TR2_D10_OUTPUT_CLIP)c->clipped_outputs++;
                        if(out.flags&TR2_D10_SOURCE_RAIL)c->source_rail_outputs++;
                    }
                    for(unsigned a=0;a<3;a++)c->output_hash=hash_word(c->output_hash,(uint16_t)out.xyz[a]);
                    c->outputs++;
                }
                c->sources++;
            }
            r1_block_cycles[slot][block]=cycle(cc)-begin;
            for(unsigned i=0;i<c->burst;i++)for(unsigned a=0;a<3;a++)
                c->input_hash=hash_word(c->input_hash,(uint16_t)input[i].xyz[a]);
        }
        c->elapsed_cycles=cycle(cc)-start_cycle;c->elapsed_ms=tick(tc)-start_tick;c->irq_after=irq?irq(ic):0;
        fir_decimator10_stop(&state);c->final_active=state.active?1u:0u;
        if(c->sources!=c->burst*c->blocks||c->outputs!=(c->sources+9u)/10u||
           (enabled&&(state.emitted!=c->outputs||c->warmup_outputs!=11u||c->clipped_outputs||c->source_rail_outputs))||c->final_active||
           !r1_cycle_statistics(r1_block_cycles[slot],c->blocks,&c->block)||
           !r1_cycle_statistics(r1_emit_cycles[slot],c->outputs,&c->emission)) {c->error=5;goto failed;}
        continue;
failed:
        fir_decimator10_invalidate(&state,TR2_D10_SOURCE_FAULT);result->error=100u+slot;return TR2_ERROR_INVALID_STATE;
    }
    for(unsigned pair=0;pair<4;pair++) {
        R1MicroCase *a=&result->cases[2u*pair],*b=a+1;
        if(a->input_hash!=b->input_hash||a->sources!=b->sources||a->outputs!=b->outputs){result->error=200u+pair;return TR2_ERROR_CORRUPTED;}
    }
    result->complete_magic=R1_BENCH_COMPLETE;
    return TR2_OK;
}
