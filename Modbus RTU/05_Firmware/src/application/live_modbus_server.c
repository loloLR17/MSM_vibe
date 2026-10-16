#include "tr2/application/live_modbus_server.h"
#include "tr2/modbus/write_adapter.h"
#include "tr2/modbus/b4_configuration_codec.h"
#include <string.h>
#include <limits.h>

static uint16_t be(const uint8_t *b) { return (uint16_t)((uint16_t)b[0]<<8|b[1]); }
static void put(uint8_t *b,uint16_t n) { b[0]=(uint8_t)(n>>8);b[1]=(uint8_t)n; }
static void inc(uint32_t *n) { if(*n<UINT32_MAX)(*n)++; }
static ModbusPduServerOutcome outcome(Tr2Result r,size_t n)
{ ModbusPduServerOutcome o={r,n};return o; }
static ModbusPduServerOutcome exception(uint8_t fc,uint8_t code,uint8_t *out)
{ out[0]=fc|128u;out[1]=code;return outcome(TR2_OK,2); }
Tr2Result live_modbus_server_init(LiveModbusServer *s,LiveCommandEngine *e,
 LiveMetrology *m,SerialTransport *t,uint8_t unit,const ModbusBlock0Image *b0,
 uint64_t (*now)(void *),void *time_context)
{
 ModbusPduServerContext pdu={0};
 if(!s||!e||!m||!b0||!now||m->running||m->has_value||!e->store||!e->store->configuration||b0->registers[6]!=2u)
  return TR2_ERROR_INVALID_ARGUMENT;
 memset(s,0,sizeof(*s));s->engine=e;s->metrology=m;s->identity=*b0;
 s->now_ms=now;s->time_context=time_context;
 configuration_staging_init(&s->staging);
 Tr2Result r=modbus_rtu_server_runtime_init(&s->rtu,t,unit,&pdu);
 if(r!=TR2_OK)return r;
 s->rtu.process_context=s;s->rtu.process_pdu=live_modbus_server_process_pdu;return TR2_OK;
}
Tr2Result live_modbus_server_publish(LiveModbusServer *s,uint64_t now)
{
 if(!s||!s->engine||s->responding)return TR2_ERROR_INVALID_STATE;
 unsigned next=s->published^1u;LiveModbusImage *i=&s->images[next];
 memset(i,0,sizeof(*i));
 i->metric=*s->metrology;i->metric.safe_stop=NULL;i->metric.stop_context=NULL;
 /* Before the first run, configuration authority comes from recovered R1;
  * no acquisition or sample clock is fabricated by exposing enabled/idle. */
 if(!i->metric.has_value&&!i->metric.running&&!i->metric.error&&s->engine->booted) {
  i->metric.boot=false;
  i->metric.authority=live_configuration_active(s->engine->store->configuration,&i->metric.active);
 }
 Tr2Result metric=live_metrology_project_b3(&i->metric,now,&i->b3);
 Tr2Result r=live_configuration_project_b4(s->engine->store->configuration,
  s->staging.has_prepared?&s->staging.prepared:NULL,&i->b4);
 if(r!=TR2_OK)return r;
 r=live_command_engine_project_b5(s->engine,i->b5);if(r!=TR2_OK)return r;
 if(i->metric.has_value) {
  if(!s->value_seen||i->metric.value.value_monotonic_ms!=s->value_ms) {
   s->value_seen=true;s->value_ms=i->metric.value.value_monotonic_ms;s->value_run=i->metric.run_id;
  }
  i->run_id=s->value_run;i->config_id=i->metric.value.configuration.config_id;
  i->config_generation=i->metric.value.configuration.generation;
 }
 s->published=next;s->image_ready=true;
 /* Even invalid/held/error images must be published. Owner handles the fault. */
 return metric;
}
static bool in_range(uint16_t a,uint16_t n,unsigned base,unsigned count)
{ return a>=base&&(unsigned)a<base+count&&n<=base+count-a; }
static unsigned choose(LiveModbusServer *s)
{
 unsigned first=TR2_LIVE_INGRESS_SLOTS;
 for(unsigned i=0;i<TR2_LIVE_INGRESS_SLOTS;i++)
  if(s->queue[i].used&&(first==TR2_LIVE_INGRESS_SLOTS||s->queue[i].order<s->queue[first].order))first=i;
 unsigned stop=TR2_LIVE_INGRESS_SLOTS-1u;
 /* No reordering across a collision key. Priority only for a running, idle R2.
  * Requests against a nonterminal are refused by R2, never saved for retry. */
 if(first<stop&&s->queue[stop].used&&!s->engine->has_active&&s->engine->state==LIVE_RUNNING) {
  bool collision=false;
  for(unsigned i=0;i<stop;i++)if(s->queue[i].used&&
   s->queue[i].epoch==s->queue[stop].epoch&&s->queue[i].mailbox[1]==s->queue[stop].mailbox[1])collision=true;
  if(!collision)first=stop;
 }
 return first;
}
Tr2Result live_modbus_server_owner_step(LiveModbusServer *s)
{
 if(!s||!s->engine||s->owner_busy||s->responding)return TR2_ERROR_INVALID_STATE;
 Tr2CooperativeWork *w=s->engine->store->configuration->live->work;
 if(w&&(w->depth||w->servicing))return TR2_ERROR_INVALID_STATE;
 s->owner_busy=true;inc(&s->owner_calls);Tr2Result r=TR2_OK;
 if(s->time_fault) {
  live_metrology_invalidate(s->metrology,false,false);
  r=live_command_engine_fault(s->engine);s->owner_busy=false;return r;
 }
 unsigned at=choose(s);
 if(s->engine->has_active&&!s->engine->indeterminate&&(s->step_due||at==TR2_LIVE_INGRESS_SLOTS)) {
  s->step_due=false;r=live_command_engine_step(s->engine);
 } else if(at<TR2_LIVE_INGRESS_SLOTS) {
  LiveModbusCapture capture=s->queue[at];s->queue[at].used=false;
  if(capture.mailbox[0]==1u&&(capture.mailbox[7]&1u)) {
   s->engine->prepared_present=false;
   if(capture.prepared_present)r=live_command_engine_prepare(s->engine,&capture.prepared);
  }
  if(r==TR2_OK) {
   s->engine->mailbox_epoch=capture.epoch;
   r=live_command_engine_write_b5(s->engine,5000,capture.mailbox,8);
  }
  s->step_due=true;
 }
 s->owner_busy=false;return r;
}
static uint8_t capture_b5(LiveModbusServer *s,uint16_t a,uint16_t n,const uint16_t *v)
{
 uint16_t m[8];uint32_t epoch=s->mailbox_epoch;memcpy(m,s->mailbox,sizeof(m));
 for(unsigned i=0;i<n;i++) {
  unsigned addr=a+i;
  if(addr<=5007u)m[addr-5000u]=v[i];
  else if(addr==5020u)epoch=(epoch&65535u)|((uint32_t)v[i]<<16);
  else epoch=(epoch&0xFFFF0000u)|v[i];
 }
 /* Same control precedence as R2. CLEAR is RAM-only and does not cancel an
  * immutable captured request. Invalid flags/CANCEL go to the real R2 handler. */
 bool clear=!(m[7]&0xFFF8u)&&(m[7]&4u);
 bool capture=!clear&&(m[7]&0xFFFFu)!=0;
 if(capture) {
  unsigned at=TR2_LIVE_INGRESS_SLOTS;
  bool stop=m[0]==14u&&m[7]==1u;
  unsigned begin=stop?TR2_LIVE_INGRESS_SLOTS-1u:0;
  unsigned end=stop?TR2_LIVE_INGRESS_SLOTS:TR2_LIVE_INGRESS_SLOTS-1u;
  for(unsigned i=begin;i<end;i++)if(!s->queue[i].used){at=i;break;}
  if(at==TR2_LIVE_INGRESS_SLOTS||s->capture_order==UINT64_MAX) {inc(&s->queue_full);return 4;}
  LiveModbusCapture *c=&s->queue[at];memset(c,0,sizeof(*c));
  memcpy(c->mailbox,m,sizeof(m));c->epoch=epoch;c->used=true;c->order=++s->capture_order;
  c->prepared_present=configuration_staging_snapshot(&s->staging,&c->prepared);inc(&s->captured);
 }
 if(clear) {memset(s->mailbox,0,sizeof(s->mailbox));s->mailbox_epoch=0;}
 else {m[7]=0;memcpy(s->mailbox,m,sizeof(m));s->mailbox_epoch=epoch;}
 return 0;
}
static ModbusPduServerOutcome process(LiveModbusServer *s,const uint8_t *q,size_t n,uint8_t *o,size_t cap)
{
 uint16_t a=0,count=0;const uint16_t *words=NULL;uint16_t values[123];
 if(q[0]!=3&&q[0]!=16)return exception(q[0],1,o);
 if((q[0]==3&&n!=5)||(q[0]==16&&n<6))return exception(q[0],3,o);
 a=be(q+1);count=be(q+3);
 if(!count||count>(q[0]==3?125u:123u))return exception(q[0],3,o);
 if(q[0]==3) {
  if(cap<2u+2u*count)return outcome(TR2_ERROR_INVALID_ARGUMENT,0);
  if(!s->image_ready)return exception(q[0],4,o);
  const LiveModbusImage *image=&s->images[s->published];unsigned offset=0;
  ModbusBlock3Image current;
  ModbusBlock4Image prepared_view;
  if(in_range(a,count,0,21)){words=s->identity.registers;offset=a;}
  else if(in_range(a,count,3000,48)) {
   /* Read-time age/freshness from a private immutable RAM copy. No producer,
    * sensor, stop or FRAM callback is reachable here, including clock faults. */
   LiveMetrology view=image->metric;uint64_t now=s->now_ms(s->time_context);
   if(s->read_seen&&now<s->last_read_ms)s->time_fault=true;
   s->read_seen=true;s->last_read_ms=now;
   if(s->time_fault)live_metrology_invalidate(&view,false,false);
   if(live_metrology_project_b3(&view,now,&current)!=TR2_OK)s->time_fault|=
    now<image->metric.last_read_ms||(image->metric.has_value&&now<image->metric.value.value_monotonic_ms);
   words=current.registers;offset=a-3000u;
  }
  else if(in_range(a,count,4000,176)) {
   prepared_view=image->b4;
   if(s->staging.has_prepared) {
    const PreparedConfiguration *p=&s->staging.prepared;
    prepared_view.registers[2]=(uint16_t)(p->config_id>>16);prepared_view.registers[3]=(uint16_t)p->config_id;
    prepared_view.registers[8]=(uint16_t)(p->supplied_crc>>16);prepared_view.registers[9]=(uint16_t)p->supplied_crc;
    tr2_b4_serialize_prepared_payload(&p->payload,prepared_view.registers+16);
   }
   words=prepared_view.registers;offset=a-4000u;
  }
  else if(in_range(a,count,5000,29)){words=image->b5;offset=a-5000u;}
  else return exception(q[0],2,o); /* No synthetic B1/B2/B6/B7 authority. */
  o[0]=3;o[1]=(uint8_t)(2u*count);
  for(unsigned i=0;i<count;i++) {
   uint16_t word=words[offset+i];
   if(a>=5000u&&a<=5028u) {
    unsigned j=offset+i;
    if(j<8u)word=s->mailbox[j];
    else if(j==20u)word=(uint16_t)(s->mailbox_epoch>>16);
    else if(j==21u)word=(uint16_t)s->mailbox_epoch;
   }
   put(o+2+2*i,word);
  }
  return outcome(TR2_OK,2u+2u*count);
 }
 if(q[5]!=2u*count||n!=6u+2u*count||cap<5)return exception(q[0],3,o);
 for(unsigned i=0;i<count;i++)values[i]=be(q+6+2*i);
 uint8_t error=0;
 if(in_range(a,count,5000,8)||in_range(a,count,5020,2))error=capture_b5(s,a,count,values);
 else if(a>=4000u&&a<=4175u) {
  ConfigurationStagingService candidate=s->staging;
  ModbusWriteOutcome w=modbus_write_adapter_write_b4(&candidate,a,values,count);
  if(w.access_result!=MODBUS_ACCESS_OK)error=2;
  else if(w.operation_result!=TR2_OK)error=4;
  else s->staging=candidate;
 } else error=2;
 if(error)return exception(q[0],error,o);
 o[0]=16;put(o+1,a);put(o+3,count);return outcome(TR2_OK,5);
}
ModbusPduServerOutcome live_modbus_server_process_pdu(void *ctx,const uint8_t *q,size_t n,uint8_t *o,size_t cap)
{
 LiveModbusServer *s=ctx;
 if(!s||!q||!o||!n||n>253||cap<2||s->responding)return outcome(TR2_ERROR_INVALID_ARGUMENT,0);
 s->responding=true;ModbusPduServerOutcome r=process(s,q,n,o,cap);s->responding=false;return r;
}
Tr2Result live_modbus_server_poll_once(LiveModbusServer *s)
{
 if(!s)return TR2_ERROR_INVALID_ARGUMENT;
 Tr2Result r=modbus_rtu_server_runtime_poll_once(&s->rtu);
 if(r!=TR2_OK)inc(&s->transport_errors);
 return r;
}
