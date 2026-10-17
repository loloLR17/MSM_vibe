/* Mission R4V only. Passive, bounded MCU chronology; never edits an R2 record.
 * Baseline and paced profiles require different exact monotone FRAM snapshots. */
#include "tr2/application/live_runtime.h"
#include "tr2/persistence/transactional_image_media.h"
#include <stdint.h>
#ifndef R4V_PACING_US
#error "Select the explicit R4V recipe"
#endif
#if R4V_PACING_US==0
#define R4U_INITIAL_SHA 0x95,0xda,0xb7,0x90,0x9d,0xbb,0xab,0x0a,0x85,0xab,0xf1,0xa3,0xd9,0xf8,0x29,0x2b,0xaf,0xb4,0x8d,0xc9,0x8d,0xe7,0x3f,0x02,0x6c,0xe2,0x78,0x62,0x51,0xb9,0xaf,0x41
#define R4U_INITIAL_JOURNAL 46
#define R4U_INITIAL_ROOT 59
#define R4U_INITIAL_RUN 7
#define R4U_INITIAL_STOP 4
#define R4U_FINAL_RUN 8
#define R4U_FINAL_STOP 6
#else
#define R4U_INITIAL_SHA 0xfc,0xe8,0xe1,0xf9,0x33,0x09,0x89,0xe0,0x2f,0x71,0x39,0xd1,0x0c,0xb0,0x66,0x38,0x3b,0xca,0x1c,0x58,0x44,0x7a,0x00,0xa9,0x85,0x2a,0x8c,0xa6,0xc4,0xb4,0xa6,0x82
#define R4U_INITIAL_JOURNAL 54
#define R4U_INITIAL_ROOT 67
#define R4U_INITIAL_RUN 8
#define R4U_INITIAL_STOP 6
#define R4U_FINAL_RUN 9
#define R4U_FINAL_STOP 9
#endif
static Tr2Result v_observe(void *,LiveAcquisitionObservation *);
static Tr2Result v_step(LiveRuntime *);
static Tr2Result v_service(LiveRuntime *);
static void v_prepared(void),v_io_init(void),v_effect(unsigned,unsigned,unsigned);
#define R4U_EFFECT(code,point,result) v_effect(code,point,result)
#define R4U_PORT(p) ((p).observe=v_observe)
#define R4U_PREPARED() v_prepared()
#define R4U_BOUNDED_INIT() v_io_init()
#define live_runtime_step v_step
#define live_runtime_service v_service
#include "stm32_r4u_runtime.c"
#undef live_runtime_step
#undef live_runtime_service

typedef struct {
 uint64_t us,xyz,last_event_us,journal,root,run;
 uint32_t kind,code,tx,status,result,phase,running,sequence,flags;
} R4vEvent;
typedef struct {
 uint64_t begin_us,end_us,service_us,io_us,journal_before,journal_after,root_before,root_after;
 uint32_t code,tx,phase_before,phase_after,status,result,running_before,running_after;
} R4vSpan;
volatile struct {
 uint32_t event_count,span_count,dropped,refused_once;
 uint64_t service_calls,service_us,read_calls,read_bytes,read_us,write_calls,write_bytes,write_us;
 uint64_t capture_us,start_begin_us,start_end_us,stop_begin_us,stop_end_us,held_publish_us,last_xyz_us,start_observed_us,stop_observed_us;
} r4v_result;
volatile R4vEvent r4v_events[96];
volatile R4vSpan r4v_spans[32];
static TransactionalImagePhysicalStorage raw_bounded;
static bool refuse_stop;
static uint32_t seen_capture;
static void event(unsigned kind,unsigned code,unsigned tx,unsigned result)
{
 if(r4v_result.event_count>=96){r4v_result.dropped++;return;}
 volatile R4vEvent *e=&r4v_events[r4v_result.event_count++];
 e->us=now_us(NULL);e->kind=kind;e->code=code;e->tx=tx;e->result=result;
 e->xyz=owner.acquisition.fifo.total_xyz;e->last_event_us=event_time.last_event_us;
 e->journal=store.generation;e->root=live.generation;e->run=owner.acquisition.fifo.run_id;
 e->status=engine.status;e->phase=engine.active.phase;e->running=owner.acquisition.fifo.running;
 e->sequence=metric.sequence;e->flags=server.image_ready?server.images[server.published].b3.registers[1]:0;
}
static Tr2Result v_read(void *ctx,uint32_t at,void *bytes,size_t n)
{
 (void)ctx;uint64_t t=now_us(NULL);Tr2Result r=raw_bounded.read(raw_bounded.context,at,bytes,n);
 r4v_result.read_calls++;r4v_result.read_bytes+=n;r4v_result.read_us+=now_us(NULL)-t;return r;
}
static Tr2Result v_write(void *ctx,uint32_t at,const void *bytes,size_t n)
{
 (void)ctx;uint64_t t=now_us(NULL);Tr2Result r=raw_bounded.write(raw_bounded.context,at,bytes,n);
 r4v_result.write_calls++;r4v_result.write_bytes+=n;r4v_result.write_us+=now_us(NULL)-t;return r;
}
static void v_io_init(void)
{raw_bounded=bounded;bounded=(TransactionalImagePhysicalStorage){NULL,v_read,v_write};}
static bool v_admit(void *ctx,uint16_t code)
{
 if(code==14&&refuse_stop){refuse_stop=false;r4v_result.refused_once++;event(9,14,engine.mailbox[1],7);return false;}
 return admission(ctx,code); /* never manufacture a positive I/O budget */
}
static void v_prepared(void)
{
 memset((void *)&r4v_result,0,sizeof(r4v_result));seen_capture=0;refuse_stop=R4V_PACING_US!=0;
 owner.acquisition.budget=v_admit;
 Tr2Result r=live_runtime_set_drain_period(&runtime,R4V_PACING_US);
 if(r!=TR2_OK){r4u_result.error=9;work.error=r;}
 event(1,0,0,r);
}
static void v_effect(unsigned code,unsigned point,unsigned result)
{
 uint64_t t=now_us(NULL);
 if(code==13) {
  if(point==1){r4v_result.start_begin_us=t;runtime.drain_seen=false;}
  else r4v_result.start_end_us=t;
 } else {
  if(point==1)r4v_result.stop_begin_us=t;else r4v_result.stop_end_us=t;
 }
 event(point==1?3:4,code,engine.active.request.command.transaction_id,result);
}
static Tr2Result v_observe(void *ctx,LiveAcquisitionObservation *out)
{
 Tr2Result result=real_port.observe(ctx,out);
 if(engine.has_active&&engine.active.phase==2) {
  unsigned code=engine.active.request.command.identity.command_code;
  if(code==13)r4v_result.start_observed_us=now_us(NULL);else r4v_result.stop_observed_us=now_us(NULL);
  event(7,code,engine.active.request.command.transaction_id,result);
 }
 return result;
}
static Tr2Result v_service(LiveRuntime *r)
{
 uint64_t t=now_us(NULL);Tr2Result result=live_runtime_service(r);
 if(server.captured!=seen_capture) {
  seen_capture=server.captured;r4v_result.capture_us=now_us(NULL);
  for(unsigned i=0;i<TR2_LIVE_INGRESS_SLOTS;i++)if(server.queue[i].used)
   event(2,server.queue[i].mailbox[0],server.queue[i].mailbox[1],0);
 }
 if(owner.acquisition.fifo.running)r4v_result.last_xyz_us=event_time.last_event_us;
 if(r4v_result.stop_end_us&&!r4v_result.held_publish_us&&server.image_ready&&
    server.images[server.published].b3.registers[0]==1&&
    (server.images[server.published].b3.registers[1]&512)) {
  r4v_result.held_publish_us=now_us(NULL);event(5,14,engine.active.request.command.transaction_id,0);
 }
 r4v_result.service_calls++;r4v_result.service_us+=now_us(NULL)-t;return result;
}
static Tr2Result v_step(LiveRuntime *r)
{
 R4vSpan s={0};bool pending=engine.has_active;
 for(unsigned i=0;i<TR2_LIVE_INGRESS_SLOTS;i++)pending|=server.queue[i].used;
 uint32_t captured=server.captured;
 s.begin_us=now_us(NULL);s.service_us=r4v_result.service_us;s.io_us=r4v_result.read_us+r4v_result.write_us;
 s.journal_before=store.generation;s.root_before=live.generation;s.phase_before=engine.active.phase;
 s.code=engine.active.request.command.identity.command_code;s.tx=engine.active.request.command.transaction_id;
 s.running_before=owner.acquisition.fifo.running;
 Tr2Result result=live_runtime_step(r);
 s.end_us=now_us(NULL);s.service_us=r4v_result.service_us-s.service_us;
 s.io_us=r4v_result.read_us+r4v_result.write_us-s.io_us;
 s.journal_after=store.generation;s.root_after=live.generation;s.phase_after=engine.active.phase;
 s.running_after=owner.acquisition.fifo.running;s.status=engine.status;s.result=engine.result;
 if(!s.code){s.code=engine.has_active?engine.active.request.command.identity.command_code:engine.mailbox[0];s.tx=engine.has_active?engine.active.request.command.transaction_id:engine.mailbox[1];}
 if(pending||server.captured!=captured||s.journal_before!=s.journal_after) {
  if(r4v_result.span_count<32)r4v_spans[r4v_result.span_count++]=s;else r4v_result.dropped++;
  event(6,s.code,s.tx,result);
 }
 return result;
}
