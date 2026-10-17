/* R4V: timed, independent FIFO/FRAM/UART model; actual R2 barriers unchanged. */
#define TR2_LIVE_MODBUS_MODEL_LIBRARY 1
#include "test_live_modbus_server.c"
#include "tr2/application/live_runtime.h"
static LiveRuntime rt;
static unsigned drain_count;
static Tr2Result drain_paced(void *ctx)
{
 Model *m=ctx;drain_count++;
 if(m->emergency&&m->us>=m->emergency_at)return TR2_ERROR_UNAVAILABLE;
 assert(live_runtime_step(&rt)==TR2_ERROR_INVALID_STATE);
 if(m->acquisition.fifo.running) {
  if(m->last_fifo_us&&m->us-m->last_fifo_us>m->max_fifo_gap)m->max_fifo_gap=m->us-m->last_fifo_us;
  m->last_fifo_us=m->us;m->polls++;uint64_t before=m->acquisition.fifo.total_xyz;
  if(live_fifo_acquisition_poll(&m->acquisition)!=TR2_OK)return TR2_ERROR_UNAVAILABLE;
  advance_model(m,12+5*(m->acquisition.fifo.total_xyz-before));
 }
 return TR2_OK;
}
static Tr2Result service_paced(void *ctx)
{Tr2Result r=live_runtime_service(&rt);advance_model(ctx,32);return r;}
static void scenario(uint32_t period)
{
 Model *m=live_model();drain_count=0;
 assert(live_runtime_init(&rt,&server,&m->work,m,drain_paced,8)==TR2_OK);
 m->work.service=service_paced;
 assert(live_runtime_set_drain_period(&rt,251)==TR2_ERROR_INVALID_ARGUMENT);
 assert(live_runtime_set_drain_period(&rt,period)==TR2_OK);
 /* Deterministic boundary oracle: early callback skipped, due callback runs. */
 assert(live_runtime_service(&rt)==TR2_OK);unsigned calls=drain_count;
 if(period) {
  /* model_us explicitly advances 4us per read; observe period-1. */
  advance_model(m,period-5);assert(live_runtime_service(&rt)==TR2_OK&&drain_count==calls);
  advance_model(m,1);assert(live_runtime_service(&rt)==TR2_OK&&drain_count==calls+1);
 }
 capture(m,1,1,13,0);
 for(unsigned i=0;i<12&&(!metric.sequence||m->b->engine.has_active);i++)assert(live_runtime_step(&rt)==TR2_OK);
 assert(metric.running&&metric.has_value&&!m->b->engine.has_active);
 assert(live_runtime_set_drain_period(&rt,0)==TR2_ERROR_INVALID_STATE);
 size_t before=m->b->f->physical.written;unsigned run=(unsigned)m->acquisition.fifo.run_id;
 capture(m,1,1,13,0);assert(live_runtime_step(&rt)==TR2_OK&&m->b->f->physical.written==before);
 capture(m,1,1,14,0);assert(live_runtime_step(&rt)==TR2_OK&&m->b->engine.result==27&&m->b->f->physical.written==before);
 /* Missing I/O budget is a refusal, not a delayed accepted physical effect. */
 m->acquisition.budget=NULL;capture(m,1,2,14,0);
 assert(live_runtime_step(&rt)==TR2_OK&&m->b->engine.status==5&&m->b->engine.result==7);
 assert(m->acquisition.fifo.running&&m->b->f->physical.written==before&&!m->b->engine.has_active);
 m->acquisition.budget=bounded_admission;
 capture(m,1,3,14,0);for(unsigned i=0;i<12&&(m->b->engine.has_active||metric.running);i++)assert(live_runtime_step(&rt)==TR2_OK);
 assert(!metric.running&&!m->acquisition.fifo.running&&m->b->engine.status==4&&!m->b->engine.has_active);
 assert((unsigned)m->acquisition.fifo.run_id==run);
 assert(!m->overrun&&!m->work.deadline_missed&&m->max_fifo_gap<=5000&&m->work.max_work_us<=1000&&m->work.max_service_us<=2500);
 printf("SIMULATION pacing%u: service%u drain%u work%llu serviceMax%llu gap%llu XYZ%llu RTU%u; real R2 retry/collision/STOPbudget refusal/no late effect: PASS\n",period,rt.service_calls,drain_count,(unsigned long long)m->work.max_work_us,(unsigned long long)m->work.max_service_us,(unsigned long long)m->max_fifo_gap,(unsigned long long)m->acquisition.fifo.total_xyz,m->rtu_replies);
 model_free(m);
}
static void fault_case(bool storage)
{
 Model *m=live_model();assert(live_runtime_init(&rt,&server,&m->work,m,drain_paced,8)==TR2_OK);
 m->work.service=service_paced;assert(live_runtime_set_drain_period(&rt,250)==TR2_OK);
 capture(m,1,1,13,0);for(unsigned i=0;i<12&&(!metric.has_value||m->b->engine.has_active);i++)assert(live_runtime_step(&rt)==TR2_OK);
 assert(metric.running&&metric.has_value);
 if(storage){m->b->f->physical.budget=0;capture(m,1,2,14,0);assert(live_runtime_step(&rt)!=TR2_OK);}
 else {m->emergency=true;m->emergency_at=m->us+500;unsigned i=0;while(live_runtime_step(&rt)==TR2_OK)assert(++i<100);assert(m->stopped_at-m->emergency_at<1000);}
 assert(!m->acquisition.fifo.running&&!m->work.depth&&metric.error);
 assert(!(server.images[server.published].b3.registers[1]&6));
 uint16_t b3[48];ModbusBlock3Image view;
 assert(live_metrology_project_b3(&metric,m->us/1000,&view)==TR2_ERROR_INVALID_STATE);memcpy(b3,view.registers,sizeof(b3));assert(!(b3[1]&6));
 puts(storage?"Paced FRAM failure: stopped, invalid, no fabricated receipt PASS":"Paced service failure: safety stop, invalid B3, no FIFO wait PASS");model_free(m);
}
static void stopped_snapshot(void)
{
 Model *m=live_model();assert(live_runtime_init(&rt,&server,&m->work,m,drain_paced,8)==TR2_OK);m->work.service=service_paced;
 capture(m,1,1,13,0);for(unsigned i=0;i<12&&(!metric.has_value||m->b->engine.has_active);i++)assert(live_runtime_step(&rt)==TR2_OK);
 /* Exact effect boundary: publish a current running snapshot, stop the real
  * model port, then serve again before the periodic 10ms publication. */
 rt.next_publish_ms=0;assert(live_runtime_service(&rt)==TR2_OK);
 assert(server.images[server.published].metric.running);
 LiveAcquisitionPort port=live_fifo_acquisition_port(&m->acquisition);assert(port.stop(port.context)==TR2_OK);
 assert(m->us/1000<rt.next_publish_ms);
 assert(live_runtime_service(&rt)==TR2_OK);
 assert(!server.images[server.published].metric.running&&!(server.images[server.published].b3.registers[1]&4));
 puts("STOP effect immediately withdraws cached FRESH before next periodic publication: PASS");model_free(m);
}
int main(void){scenario(0);scenario(250);fault_case(true);fault_case(false);stopped_snapshot();return 0;}
