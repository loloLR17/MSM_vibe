/* HOST MODEL of metadata only; not evidence of physical acquisition effects. */
#define main old_main
#include "test_r4m_maintenance.c"
#undef main
int main(int argc,char **argv)
{
 assert(argc==3);Bench *b=calloc(1,sizeof(*b));assert(b);FILE *f=fopen(argv[1],"rb");assert(f);
 assert(fread(b->bytes,1,262144,f)==262144&&!fclose(f));b->owner=true;b->budget=SIZE_MAX;boot(b);
 assert(live_command_engine_boot(&b->engine)==TR2_OK&&b->store.generation==46&&b->store.epoch.current==5);
 b->guard.phase=3;b->guard.epoch_floor=4;b->guard.run_floor=7;
 LiveCommandRequest q={0};q.epoch=5;q.command.transaction_id=5;q.command.identity.command_code=13;
 run(b,&q);assert(b->starts==1&&b->run==8&&b->running);
 q.command.transaction_id=6;q.command.identity.command_code=14;run(b,&q);assert(!b->running&&b->store.generation==54&&b->live.generation==67);
 f=fopen(argv[2],"wb");assert(f&&fwrite(b->bytes,1,262144,f)==262144&&!fclose(f));
 puts("SIMULATED METADATA ONLY: journal54/root67/epoch5/run8/config3, START and STOP completed4/0; real effects still require target observation");free(b);return 0;
}
