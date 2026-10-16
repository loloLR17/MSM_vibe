from pathlib import Path
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4Q-live-b3-rtu/Modbus RTU/05_Firmware')
p=r/'include/tr2/modbus/rtu_server_runtime.h';s=p.read_text();s=s.replace('    uint8_t local_unit_id;','''    /* Optional explicit successor PDU composition. NULL retains V1 exactly.
     * Main-loop only, RAM-only; no IRQ dispatch or durable admission here. */
    void *process_context;
    ModbusPduServerOutcome (*process_pdu)(void *, const uint8_t *, size_t,
                                         uint8_t *, size_t);
    uint8_t local_unit_id;''');p.write_text(s)
p=r/'src/modbus/rtu_server_runtime.c';s=p.read_text();old='''    pdu_outcome = modbus_pdu_server_process(&runtime->pdu_context,
                                            request.pdu,
                                            request.pdu_length,
                                            runtime->response_pdu,
                                            sizeof(runtime->response_pdu));''';assert old in s;s=s.replace(old,'''    if (runtime->process_pdu != NULL) {
        pdu_outcome = runtime->process_pdu(runtime->process_context, request.pdu,
            request.pdu_length, runtime->response_pdu, sizeof(runtime->response_pdu));
    } else {
'''+old+'''
    }''');s=s.replace('    runtime->transport = transport;','    runtime->process_context = NULL;\n    runtime->process_pdu = NULL;\n    runtime->transport = transport;');p.write_text(s)
p=r/'include/tr2/application/live_modbus_server.h';assert not p.exists();p.write_text('''#ifndef TR2_LIVE_MODBUS_SERVER_H
#define TR2_LIVE_MODBUS_SERVER_H
#include "tr2/application/live_command_engine.h"
#include "tr2/application/live_metrology.h"
#include "tr2/modbus/rtu_server_runtime.h"

#define TR2_LIVE_INGRESS_SLOTS 4u /* three normal captures, one STOP reserve */
typedef struct {
    ModbusBlock3Image b3;
    ModbusBlock4Image b4;
    uint16_t b5[29];
    uint64_t run_id;
    uint32_t config_id, config_generation;
} LiveModbusImage;
typedef struct {
    uint16_t mailbox[8];
    uint32_t epoch;
    PreparedConfiguration prepared;
    uint64_t order;
    bool used, prepared_present;
} LiveModbusCapture;
/* All methods are called by one main-loop owner, never ISR/threads. During a
 * cooperative R2 checkpoint only poll_once/publish are allowed, NEVER owner_step.
 * No media, sensor, engine admission or metrology callbacks in PDU handling.
 * FC10 acknowledgement means RAM capture, NOT durable admission or success.
 * Unsupported blocks/effects remain unavailable; not a production LIVE server. */
typedef struct {
    LiveCommandEngine *engine;
    LiveMetrology *metrology;
    ModbusRtuServerRuntime rtu;
    ModbusBlock0Image identity;
    LiveModbusImage images[2];
    ConfigurationStagingService staging;
    uint16_t mailbox[8];
    uint32_t mailbox_epoch;
    LiveModbusCapture queue[TR2_LIVE_INGRESS_SLOTS];
    uint64_t capture_order;
    uint32_t captured, queue_full, owner_calls, transport_errors;
    unsigned published;
    bool image_ready, responding, owner_busy, step_due;
} LiveModbusServer;
Tr2Result live_modbus_server_init(LiveModbusServer *, LiveCommandEngine *,
    LiveMetrology *, SerialTransport *, uint8_t unit_id, const ModbusBlock0Image *);
/* Must be refreshed by the owner after FIFO/invalidation and at cooperative
 * service boundaries. now_ms is observed monotone, not a fabricated sample time. */
Tr2Result live_modbus_server_publish(LiveModbusServer *, uint64_t now_ms);
Tr2Result live_modbus_server_poll_once(LiveModbusServer *);
/* At most one capture/admission OR one durable R2 phase. Refuses reentrancy and
 * entry during an existing cooperative scope. STOP reserve cannot be starved by
 * ordinary ingress; it never preempts an already admitted durable operation. */
Tr2Result live_modbus_server_owner_step(LiveModbusServer *);
ModbusPduServerOutcome live_modbus_server_process_pdu(void *,const uint8_t *,size_t,
                                                     uint8_t *,size_t);
#endif
''')
p=r/'src/application/live_modbus_server.c';assert not p.exists();p.write_text('''#include "tr2/application/live_modbus_server.h"
#include "tr2/modbus/write_adapter.h"
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
 LiveMetrology *m,SerialTransport *t,uint8_t unit,const ModbusBlock0Image *b0)
{
 ModbusPduServerContext pdu={0};
 if(!s||!e||!m||!b0||!e->store||!e->store->configuration||b0->registers[6]!=2u)
  return TR2_ERROR_INVALID_ARGUMENT;
 memset(s,0,sizeof(*s));s->engine=e;s->metrology=m;s->identity=*b0;
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
 Tr2Result metric=live_metrology_project_b3(s->metrology,now,&i->b3);
 Tr2Result r=live_configuration_project_b4(s->engine->store->configuration,
  s->staging.has_prepared?&s->staging.prepared:NULL,&i->b4);
 if(r!=TR2_OK)return r;
 r=live_command_engine_project_b5(s->engine,i->b5);if(r!=TR2_OK)return r;
 i->run_id=s->metrology->run_id;
 i->config_id=s->metrology->active.config_id;
 i->config_generation=s->metrology->active.generation;
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
  if(in_range(a,count,0,21)){words=s->identity.registers;offset=a;}
  else if(in_range(a,count,3000,48)){words=image->b3.registers;offset=a-3000u;}
  else if(in_range(a,count,4000,176)){words=image->b4.registers;offset=a-4000u;}
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
''')
p=r/'CMakeLists.txt';s=p.read_text().replace('    src/application/live_metrology.c','    src/application/live_modbus_server.c\n    src/application/live_metrology.c',1);p.write_text(s)
