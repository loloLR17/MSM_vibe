/* HOST COUNTEREXAMPLE ONLY. RAM fixture, no hardware driver or authorization.
 * We deliberately apply the generic isolation mechanism beyond the exact old
 * capture guards to test whether it meets R4S's no-counter-reset condition. */
#define main r4m_fixture_entry
#include "test_r4m_maintenance.c"
#undef main
int main(int argc,char **argv)
{
 assert(argc==2);FILE *f=fopen(argv[1],"rb");assert(f);Bench *b=calloc(1,sizeof(*b));assert(b);
 assert(fread(b->bytes,1,262144,f)==262144&&fgetc(f)==EOF);fclose(f);b->owner=true;b->budget=SIZE_MAX;boot(b);
 assert(live_command_engine_boot(&b->engine)==TR2_OK&&b->engine.indeterminate);
 R4mMaintenance rejected;assert(r4m_capture(&rejected,&b->engine)!=TR2_OK&&r4p_capture(&rejected,&b->engine)!=TR2_OK);
 uint64_t before=b->store.generation;assert(before==26&&b->store.epoch.current==4&&b->store.effect.run_id==5);
 /* Pure model of the old retirement effect, NOT a valid capture grant. */
 b->guard.epoch_floor=b->store.epoch.current;b->guard.run_floor=b->store.effect.run_id;
 memcpy(b->guard.intent,b->config.live->candidate+TR2_LIVE_EFFECT_OFFSET,128);b->guard.phase=1;
 assert(r4m_isolate_journal(&b->guard,&b->engine)==TR2_OK);R4mMaintenance retiring=b->guard;
 boot(b);assert(live_command_engine_boot(&b->engine)==TR2_OK);uint64_t bridge=b->store.generation;
 b->guard=retiring;LiveCommandRequest renew={0};renew.epoch=4;renew.command.transaction_id=65535;renew.command.identity.command_code=12;
 run(b,&renew);assert(r4m_open_after_renew(&b->guard,&b->engine)==TR2_OK);
 uint64_t after=b->store.generation;
 printf("{\"journal_before\":%llu,\"journal_bridge\":%llu,\"journal_after_renew\":%llu,\"run_floor\":%llu,\"epoch_after_renew\":%u,\"old_guards_refuse\":true,\"target_access\":false}\n",(unsigned long long)before,(unsigned long long)bridge,(unsigned long long)after,(unsigned long long)b->store.effect.run_id,b->store.epoch.current);
 free(b);return after<before?7:0;
}
