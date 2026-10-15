/* Actual R2/store phases on the existing memory transport; not MCU timing. */
#define TR2_LIVE_R2_TEST_LIBRARY 1
#include "test_live_command_engine.c"
#undef main
#include <time.h>
static void measured(Bench *b,LiveCommandRequest q,const char *name)
{
    for(unsigned phase=0;phase<4;phase++) {
        size_t r=b->f->physical.read_bytes,w=b->f->physical.written;
        clock_t at=clock();
        Tr2Result result=phase?live_command_engine_step(&b->engine):live_command_engine_submit(&b->engine,&q);
        double ms=1000.0*(double)(clock()-at)/CLOCKS_PER_SEC;
        assert(result==TR2_OK);
        printf("IO_COST,%s,%u,%zu,%zu,%.3f,%u\n",name,phase,
            b->f->physical.read_bytes-r,b->f->physical.written-w,ms,b->acquisition.running?1:0);
    }
    assert(b->engine.status==4&&!b->engine.has_active);
}
int main(void)
{
    Bench *b=bench();
    puts("IO_COST,command,phase,read_bytes,write_bytes,host_cpu_ms,running_after");
    measured(b,request(1,1,13),"START");
    measured(b,request(1,65535,12),"RENEW_LIVE");
    measured(b,request(2,2,14),"STOP");
    {PreparedConfiguration p=prepared(vector_2_prepared,3);
     assert(live_command_engine_prepare(&b->engine,&p)==TR2_OK);}
    measured(b,request(2,3,1),"APPLY_IDLE");
    destroy(b);return 0;
}
