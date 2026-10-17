#include "tr2/application/live_runtime.h"
#include <string.h>
static void inc(uint32_t *n){if(*n<UINT32_MAX)(*n)++;}
Tr2Result live_runtime_init(LiveRuntime *r,LiveModbusServer *s,Tr2CooperativeWork *w,
 void *ctx,Tr2Result (*drain)(void *),uint16_t quantum)
{
 if(!r||!s||!w||!drain||!s->engine||!s->now_ms||!quantum||quantum>64||w->depth||w->servicing)
  return TR2_ERROR_INVALID_ARGUMENT;
 memset(r,0,sizeof(*r));r->server=s;r->work=w;r->context=ctx;r->drain=drain;r->poll_quantum=quantum;return TR2_OK;
}
Tr2Result live_runtime_set_drain_period(LiveRuntime *r,uint32_t us)
{
 if(!r||!r->server||!r->work||!r->work->now_us||!r->server->metrology||!r->server->engine)return TR2_ERROR_INVALID_ARGUMENT;
 if(r->main_busy||r->servicing||r->work->depth||r->work->servicing||
    r->server->metrology->running||r->server->engine->has_active)return TR2_ERROR_INVALID_STATE;
 if(us>r->work->work_limit_us/4u)return TR2_ERROR_INVALID_ARGUMENT;
 r->drain_period_us=us;r->drain_seen=false;return TR2_OK;
}
Tr2Result live_runtime_service(LiveRuntime *r)
{
 if(!r||!r->server||!r->drain||r->servicing)return TR2_ERROR_INVALID_STATE;
 r->servicing=true;inc(&r->service_calls);
 Tr2Result result=TR2_OK;bool due=true;uint64_t drain_at=0;
 if(r->drain_period_us) {
  drain_at=r->work->now_us(r->work->context);
  if(r->drain_seen&&drain_at<r->last_drain_us)result=TR2_ERROR_INVALID_STATE;
  due=!r->drain_seen||drain_at-r->last_drain_us>=r->drain_period_us;
 }
 if(result==TR2_OK&&due) {
  r->last_drain_us=drain_at;r->drain_seen=true;inc(&r->drain_calls);
  result=r->drain(r->context);
 }
 if(result==TR2_OK) {
  uint64_t now=r->server->now_ms(r->server->time_context);
  if(r->seen&&now<r->last_ms)result=TR2_ERROR_INVALID_STATE;
  else {
   r->seen=true;r->last_ms=now;
   if(!r->server->image_ready||now>=r->next_publish_ms) {
    result=live_modbus_server_publish(r->server,now);inc(&r->publications);
    r->next_publish_ms=now>UINT64_MAX-10?UINT64_MAX:now+10;
   }
  }
 }
 for(unsigned i=0;i<r->poll_quantum&&result==TR2_OK;i++) {
  result=live_modbus_server_poll_once(r->server);inc(&r->polls);
 }
 r->servicing=false;return result;
}
Tr2Result live_runtime_step(LiveRuntime *r)
{
 if(!r||!r->server||!r->work||r->main_busy||r->servicing||r->work->depth||r->work->servicing)
  return TR2_ERROR_INVALID_STATE;
 r->main_busy=true;Tr2Result result=tr2_cooperative_enter(r->work);
 if(result==TR2_OK)result=tr2_cooperative_leave(r->work);
 if(result==TR2_OK){inc(&r->owner_calls);result=live_modbus_server_owner_step(r->server);}
 if(result!=TR2_OK) {
  /* A foreground/service deadline failure also withdraws the retained B3.
   * R2's fault path stops the port without requiring a successful FRAM write.
   * Preserve the original error; never manufacture a terminal receipt. */
  (void)live_command_engine_fault(r->server->engine);
  (void)live_modbus_server_publish(r->server,r->server->now_ms(r->server->time_context));
 }
 r->main_busy=false;return result;
}
