/* R4U composition on independently timed asynchronous FIFO/FRAM/UART model.
 * Reuse real stores and R4Q oracles; no old regression main is executed here. */
#define TR2_LIVE_MODBUS_MODEL_LIBRARY 1
#include "test_live_modbus_server.c"
#include "tr2/application/live_runtime.h"
static LiveRuntime runtime;
static Tr2Result u_drain(void *ctx)
{
 Model *m=ctx;assert(live_runtime_step(&runtime)==TR2_ERROR_INVALID_STATE);
 assert(live_modbus_server_owner_step(&server)==TR2_ERROR_INVALID_STATE);
 if(m->acquisition.fifo.running) {
  if(m->last_fifo_us&&m->us-m->last_fifo_us>m->max_fifo_gap)m->max_fifo_gap=m->us-m->last_fifo_us;
  m->last_fifo_us=m->us;m->polls++;uint64_t before=m->acquisition.fifo.total_xyz;
  if(live_fifo_acquisition_poll(&m->acquisition)!=TR2_OK)return TR2_ERROR_UNAVAILABLE;
  advance_model(m,12+5*(m->acquisition.fifo.total_xyz-before));
 }
 return TR2_OK;
}
static Tr2Result u_service(void *ctx)
{Tr2Result r=live_runtime_service(&runtime);advance_model(ctx,32);return r;}
int main(void)
{
 Model *m=live_model();
 assert(live_runtime_init(&runtime,&server,&m->work,m,u_drain,65)==TR2_ERROR_INVALID_ARGUMENT);
 assert(live_runtime_init(&runtime,&server,&m->work,m,u_drain,8)==TR2_OK);m->work.service=u_service;
 uint8_t q[5]={3,23,112,0,64},out[253];size_t writes=m->b->f->physical.written;
 assert(pdu(m,q,5,out).response_length==2&&out[1]==2); /* opt-in only */
 assert(live_modbus_server_enable_unavailable_inventory(&server)==TR2_OK);
 assert(pdu(m,q,5,out).response_length==130&&out[3]==2&&out[21]==4);
 for(unsigned i=0;i<64;i++)if(i!=0&&i!=9)assert(out[2+2*i]==0&&out[3+2*i]==0);
 uint16_t selected=65535;write_words(m,6003,&selected,1,0);
 assert(pdu(m,q,5,out).response_length==130&&out[8]==255&&out[9]==255&&out[11]==0);
 assert(m->b->f->physical.written==writes);
 for(unsigned base=1000;base<=7000;base+=1000)if(base==1000||base==2000||base==7000) {
  uint8_t rq[5]={3,(uint8_t)(base>>8),(uint8_t)base,0,1};assert(pdu(m,rq,5,out).response_length==2&&out[1]==2);
 }
 server.read_only=true;
 uint16_t denied_epoch[2]={0,1},denied_command[8]={13,1,0,0,0,0,0,1};
 write_words(m,5020,denied_epoch,2,1);write_words(m,5000,denied_command,8,1);
 assert(!server.captured);server.read_only=false;
 capture(m,1,1,13,0); /* deferred true R2, no callback admission */
 for(unsigned n=0;n<10&&(!metric.sequence||m->b->engine.has_active);n++)assert(live_runtime_step(&runtime)==TR2_OK);
 assert(metric.running&&metric.sequence&&m->acquisition.fifo.run_id==1&&!m->b->engine.has_active);
 uint64_t until=m->us+500000;while(m->us<until)assert(live_runtime_step(&runtime)==TR2_OK);assert(live_modbus_server_publish(&server,m->us/1000)==TR2_OK);
 q[1]=11;q[2]=184;q[4]=48;assert(pdu(m,q,5,out).response_length==98&&(out[5]&6)==6);
 /* Retry cannot repeat effect; collision/old epoch kept by the genuine R2. */
 writes=m->b->f->physical.written;capture(m,1,1,13,0);
 for(unsigned i=0;i<4;i++)assert(live_runtime_step(&runtime)==TR2_OK);
 assert(m->acquisition.fifo.run_id==1&&m->b->f->physical.written==writes);
 capture(m,1,1,14,0);assert(live_runtime_step(&runtime)==TR2_OK&&m->b->engine.result==27);
 capture(m,1,2,14,0);
 for(unsigned i=0;i<8&&metric.running;i++)assert(live_runtime_step(&runtime)==TR2_OK);
 assert(!metric.running&&!m->acquisition.fifo.running);
 while(m->b->engine.has_active)assert(live_runtime_step(&runtime)==TR2_OK);
 assert(live_modbus_server_publish(&server,m->us/1000)==TR2_OK);
 assert(pdu(m,q,5,out).response_length==98&&!(out[5]&4));
 assert(!m->work.deadline_missed&&!m->overrun&&m->work.max_work_us<=1000&&m->work.max_service_us<=2500&&m->max_fifo_gap<=5000);
 printf("R4U MODEL: work%llu service%llu gap%llu XYZ%llu RTU%u RXmax%u; B6unavailable/selector exact, B1/2/7exception02, START/STOP/retry/collision/held/no reentry: PASS\n",(unsigned long long)m->work.max_work_us,(unsigned long long)m->work.max_service_us,(unsigned long long)m->max_fifo_gap,(unsigned long long)m->acquisition.fifo.total_xyz,m->rtu_replies,m->max_rx_backlog);
 model_free(m);return 0;
}
