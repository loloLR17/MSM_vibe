/* Metadata oracle only, no hardware execution. */
#define main old_main
#include "test_r4m_maintenance.c"
#undef main
int main(int argc,char **argv){
 assert(argc==3);Bench*b=calloc(1,sizeof(*b));assert(b);FILE*f=fopen(argv[1],"rb");assert(f&&fread(b->bytes,1,262144,f)==262144&&!fclose(f));
 b->owner=true;b->budget=SIZE_MAX;boot(b);assert(live_command_engine_boot(&b->engine)==TR2_OK&&b->store.generation==102&&b->live.generation==115&&b->store.epoch.current==5);
 b->guard.phase=3;b->guard.epoch_floor=4;b->guard.run_floor=14;
 LiveCommandRequest q={0};q.epoch=5;q.command.transaction_id=20;q.command.identity.command_code=13;run(b,&q);assert(b->starts==1&&b->running&&b->run==15);
 q.command.transaction_id=21;q.command.identity.command_code=14;run(b,&q);assert(!b->running&&b->store.generation==110&&b->live.generation==123);
 f=fopen(argv[2],"wb");assert(f&&fwrite(b->bytes,1,262144,f)==262144&&!fclose(f));puts("METADATA MODEL ONLY journal110/root123/epoch5/run15/config3/STOP5/21");free(b);return 0;}
