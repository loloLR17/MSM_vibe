#include "tr2/common/cooperative_work.h"
#include <string.h>
Tr2Result tr2_cooperative_init(Tr2CooperativeWork *w,void *ctx,uint64_t (*now)(void *),
 Tr2Result (*service)(void *),void (*stop)(void *),uint16_t q,uint32_t limit,uint32_t service_limit)
{
 if(!w||!now||!service||!stop||!q||q>256||!limit||!service_limit)return TR2_ERROR_INVALID_ARGUMENT;
 memset(w,0,sizeof(*w));w->context=ctx;w->now_us=now;w->service=service;w->safety_stop=stop;
 w->quantum_bytes=q;w->work_limit_us=limit;w->service_limit_us=service_limit;return TR2_OK;
}
Tr2Result tr2_cooperative_status(const Tr2CooperativeWork *w)
{return !w?TR2_OK:w->servicing?TR2_ERROR_INVALID_STATE:w->error;}
static Tr2Result failed(Tr2CooperativeWork *w,Tr2Result error)
{
 w->error=error;w->servicing=true;w->safety_stop(w->context);w->servicing=false;return error;
}
Tr2Result tr2_cooperative_checkpoint(Tr2CooperativeWork *w)
{
 uint64_t now,end,elapsed;Tr2Result result;
 if(!w)return TR2_OK;
 result=tr2_cooperative_status(w);if(result!=TR2_OK)return result;
 now=w->now_us(w->context);
 if(w->seen) {
  if(now<w->last_us)return failed(w,TR2_ERROR_INVALID_STATE);
  elapsed=now-w->last_us;
  if(elapsed>w->max_work_us)w->max_work_us=elapsed;
  if(elapsed>w->work_limit_us){w->deadline_missed=true;return failed(w,TR2_ERROR_UNAVAILABLE);}
 }
 w->servicing=true;result=w->service(w->context);w->servicing=false;
 end=w->now_us(w->context);
 if(end<now)return failed(w,TR2_ERROR_INVALID_STATE);
 if(end-now>w->max_service_us)w->max_service_us=end-now;
 if(result!=TR2_OK)return failed(w,result);
 if(end-now>w->service_limit_us){w->deadline_missed=true;return failed(w,TR2_ERROR_UNAVAILABLE);}
 if(w->checkpoints<UINT64_MAX)w->checkpoints++;
 w->seen=true;w->last_us=end;return TR2_OK;
}
Tr2Result tr2_cooperative_equal(Tr2CooperativeWork *w,const void *data,uint8_t value,size_t n,bool *equal)
{
 const uint8_t *p=data;size_t q=w?w->quantum_bytes:n;
 if(!equal||(!data&&n))return TR2_ERROR_INVALID_ARGUMENT;
 *equal=false;
 while(n) {
  size_t take=n<q?n:q;
  Tr2Result r=tr2_cooperative_checkpoint(w);if(r!=TR2_OK)return r;
  for(size_t i=0;i<take;i++)if(p[i]!=value)return TR2_OK;
  p+=take;n-=take;
 }
 *equal=true;return tr2_cooperative_checkpoint(w);
}

Tr2Result tr2_cooperative_enter(Tr2CooperativeWork *w)
{
 if(!w)return TR2_OK;
 Tr2Result r=tr2_cooperative_status(w);if(r!=TR2_OK)return r;
 if(w->depth==UINT16_MAX)return TR2_ERROR_INVALID_STATE;
 if(!w->depth)w->seen=false;
 r=tr2_cooperative_checkpoint(w);if(r==TR2_OK)w->depth++;return r;
}
Tr2Result tr2_cooperative_leave(Tr2CooperativeWork *w)
{
 if(!w)return TR2_OK;
 if(w->servicing||!w->depth)return TR2_ERROR_INVALID_STATE;
 Tr2Result r=tr2_cooperative_checkpoint(w);w->depth--;return r;
}
