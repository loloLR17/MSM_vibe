#include "tr2/domain/acquisition/fir_decimator10.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
static FirDecimator10Clock nominal(void) { return (FirDecimator10Clock){80000,3,false}; }
static FirDecimator10Input point(uint64_t i, int16_t v)
{
    FirDecimator10Input p={.source_index=i,.time_us=i*75u/2u,.xyz={v,v,v},.valid=true,.time_valid=true};
    return p;
}
static void lifecycle(void)
{
    FirDecimator10 s; FirDecimator10Output out; bool ready=true;
    fir_decimator10_init(&s);
    assert(fir_decimator10_start(&s,0,0,nominal())==TR2_ERROR_INVALID_ARGUMENT);
    FirDecimator10Clock bad=nominal(); bad.source_hz_den=UINT64_MAX;
    assert(fir_decimator10_start(&s,1,0,bad)==TR2_ERROR_INVALID_ARGUMENT);
    assert(fir_decimator10_start(&s,1,0,nominal())==TR2_OK);
    assert(fir_decimator10_start(&s,2,0,nominal())==TR2_ERROR_INVALID_STATE&&s.segment_id==1);
    for(uint64_t i=0;i<37;i++) {
        FirDecimator10Input p=point(i,1000);
        assert(fir_decimator10_push(&s,&p,&out,&ready)==TR2_OK);
        assert(ready==(i%10==0));
        if(ready) assert(out.source_index==i&&out.output_index==i/10&&out.segment_id==1&&out.causal_time_us==p.time_us);
    }
    fir_decimator10_stop(&s); assert(s.emitted==4&&s.accepted==37&&s.ended==TR2_D10_STOP);
    FirDecimator10Input p=point(37,0);
    assert(fir_decimator10_push(&s,&p,&out,&ready)==TR2_ERROR_INVALID_STATE&&!ready);
    assert(fir_decimator10_start(&s,1,0,nominal())==TR2_ERROR_INVALID_STATE);
    assert(fir_decimator10_start(&s,2,999,nominal())==TR2_OK);
    p=point(999,0); assert(fir_decimator10_push(&s,&p,&out,&ready)==TR2_OK&&ready);
    assert(out.source_index==999&&out.output_index==0&&(out.flags&TR2_D10_WARMUP));
    for(unsigned i=0;i<3;i++) assert(out.xyz[i]==0); /* no history from prior run */
    p=point(1001,0);assert(fir_decimator10_push(&s,&p,&out,&ready)==TR2_ERROR_INVALID_STATE);
    assert(!s.active&&s.accepted==1&&s.ended==TR2_D10_INDEX_GAP);
}
static void constant_and_rails(void)
{
    const int16_t values[]={0,1,-1,16384,-8192,32767,-32768};
    for(unsigned v=0;v<sizeof(values)/sizeof(values[0]);v++) {
        FirDecimator10 s; fir_decimator10_init(&s); assert(fir_decimator10_start(&s,1,0,nominal())==TR2_OK);
        for(unsigned i=0;i<260;i++) {
            FirDecimator10Input p=point(i,values[v]);FirDecimator10Output out;bool ready;
            assert(fir_decimator10_push(&s,&p,&out,&ready)==TR2_OK);
            if(ready&&i>=106) {
                assert(!(out.flags&TR2_D10_WARMUP)&&!(out.flags&TR2_D10_OUTPUT_CLIP));
                assert(out.xyz[0]==values[v]&&out.xyz[1]==values[v]&&out.xyz[2]==values[v]);
                assert(!!(out.flags&TR2_D10_SOURCE_RAIL)==(values[v]==INT16_MAX||values[v]==INT16_MIN));
            }
        }
    }
    FirDecimator10 s; fir_decimator10_init(&s); assert(fir_decimator10_start(&s,1,0,nominal())==TR2_OK);
    for(unsigned i=0;i<800;i++) {
        FirDecimator10Input p=point(i,i<400?32760:-32760);FirDecimator10Output out;bool ready;
        assert(fir_decimator10_push(&s,&p,&out,&ready)==TR2_OK);
        if(ready&&(out.flags&TR2_D10_OUTPUT_CLIP)) {
            assert(out.clipped_axes==7&&!(out.flags&TR2_D10_SOURCE_RAIL));
            assert(out.xyz[0]==INT16_MAX||out.xyz[0]==INT16_MIN);
        }
    }
    assert(s.clipped_outputs>0&&s.source_rail_samples==0);
}
static void chunks_and_backpressure(void)
{
    FirDecimator10 a,b;FirDecimator10Input in[257];FirDecimator10Output ref[26],out[26];
    fir_decimator10_init(&a);fir_decimator10_init(&b);
    assert(fir_decimator10_start(&a,1,0,nominal())==TR2_OK&&fir_decimator10_start(&b,1,0,nominal())==TR2_OK);
    size_t n=0;
    for(unsigned i=0;i<257;i++) {
        in[i]=point(i,(int16_t)((int32_t)((i*1009u)%65536u)-32768)); bool ready;
        assert(fir_decimator10_push(&a,&in[i],&ref[n],&ready)==TR2_OK);if(ready)n++;
    }
    assert(n==26);
    size_t consumed,written;
    assert(fir_decimator10_process(&b,in,257,NULL,0,&consumed,&written)==TR2_ERROR_NOT_AVAILABLE);
    assert(consumed==0&&written==0&&b.accepted==0);
    assert(fir_decimator10_process(&b,in,257,out,1,&consumed,&written)==TR2_ERROR_NOT_AVAILABLE);
    assert(consumed==10&&written==1&&b.accepted==10);
    size_t c2,w2;
    assert(fir_decimator10_process(&b,in+consumed,257-consumed,out+1,25,&c2,&w2)==TR2_OK);
    assert(consumed+c2==257&&written+w2==26&&memcmp(out,ref,sizeof(out))==0);
    assert(a.accepted==b.accepted&&a.emitted==b.emitted);
}
static void faults_and_clock(void)
{
    FirDecimator10 s;fir_decimator10_init(&s);FirDecimator10Output out;bool ready;
    const int ppm[]={-1000,-50,0,50,1000};
    for(unsigned i=0;i<5;i++) {
        FirDecimator10Clock c={80000u*(uint64_t)(1000000+ppm[i]),3000000,true};
        assert(fir_decimator10_start(&s,i+1,0,c)==TR2_OK);
        assert(s.clock.source_hz_num==c.source_hz_num&&s.clock.source_hz_den*10u==30000000&&s.clock.measured);
        FirDecimator10Input p=point(0,0);
        for(uint64_t k=0;k<1001;k++) {
            p=point(k,0);
            p.time_us=(uint64_t)UINT32_MAX-10u+k*1000000u*c.source_hz_den/c.source_hz_num;
            assert(fir_decimator10_push(&s,&p,&out,&ready)==TR2_OK);
            if(ready) assert(out.causal_time_us==p.time_us&&out.source_index==k&&out.output_index==k/10);
        }
        assert(s.emitted==101);
        p.source_index=1001;p.time_us=65; /* raw rollover is NOT silently unwrapped */
        assert(fir_decimator10_push(&s,&p,&out,&ready)==TR2_ERROR_INVALID_STATE&&s.ended==TR2_D10_TIME_FAULT);
    }
    assert(fir_decimator10_start(&s,6,0,nominal())==TR2_OK);
    FirDecimator10Input p=point(0,0);p.time_valid=false;
    assert(fir_decimator10_push(&s,&p,&out,&ready)==TR2_OK&&(out.flags&TR2_D10_TIME_UNKNOWN)&&out.causal_time_us==0);
    p=point(1,0);assert(fir_decimator10_push(&s,&p,&out,&ready)==TR2_ERROR_INVALID_STATE);
    assert(fir_decimator10_start(&s,7,0,nominal())==TR2_OK);
    p=point(0,0);p.valid=false;assert(fir_decimator10_push(&s,&p,&out,&ready)==TR2_ERROR_INVALID_STATE&&s.accepted==0&&s.ended==TR2_D10_SOURCE_FAULT);
    for(unsigned why=TR2_D10_SOURCE_FAULT;why<=TR2_D10_OVERFLOW;why++) {
        assert(fir_decimator10_start(&s,8+why,0,nominal())==TR2_OK);
        fir_decimator10_invalidate(&s,(FirDecimator10End)why);assert(!s.active&&s.ended==(FirDecimator10End)why);
    }
    assert(fir_decimator10_start(&s,20,UINT64_MAX-1,nominal())==TR2_OK);
    p=point(0,0);p.source_index=UINT64_MAX-1;p.time_valid=false;
    assert(fir_decimator10_push(&s,&p,&out,&ready)==TR2_OK);
    p.source_index=UINT64_MAX;assert(fir_decimator10_push(&s,&p,&out,&ready)==TR2_ERROR_INVALID_STATE&&s.ended==TR2_D10_OVERFLOW);
    assert(fir_decimator10_start(&s,21,0,nominal())==TR2_OK);s.source_rail_samples=UINT64_MAX;
    p=point(0,INT16_MAX);assert(fir_decimator10_push(&s,&p,&out,&ready)==TR2_OK);
    assert(s.source_rail_samples==UINT64_MAX&&(out.flags&TR2_D10_COUNTER_SATURATED));
}
int main(void)
{
    lifecycle();constant_and_rails();chunks_and_backpressure();faults_and_clock();
    printf("D10 lifecycle/segments/DC/rails/ringing/backpressure/time/overflow PASS; state=%zu output=%zu\n",sizeof(FirDecimator10),sizeof(FirDecimator10Output));return 0;
}
