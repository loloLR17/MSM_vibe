#include "capture_queue.h"
#include <string.h>
void capture_queue_init(CaptureQueue *q){memset(q,0,sizeof *q);}
bool capture_queue_push(CaptureQueue *q,const uint8_t *b,uint32_t n)
{
 if(!q||!b||n>R3_QUEUE_CAPACITY-q->count){if(q)q->overflows++;return false;}
 for(uint32_t i=0;i<n;i++){q->data[q->head]=b[i];q->head=(q->head+1)%R3_QUEUE_CAPACITY;}
 q->count+=n;if(q->count>q->peak)q->peak=q->count;return true;
}
const uint8_t *capture_queue_acquire(CaptureQueue *q,bool final,uint8_t scratch[512])
{
 if(!q||q->borrowed||!q->count||q->tail%512||(!final&&q->count<512))return NULL;
 q->borrowed=q->count<512?q->count:512;
 if(q->borrowed==512)return q->data+q->tail;
 memcpy(scratch,q->data+q->tail,q->borrowed);memset(scratch+q->borrowed,255,512-q->borrowed);return scratch;
}
bool capture_queue_commit(CaptureQueue *q)
{
 if(!q||!q->borrowed||q->borrowed>q->count)return false;
 q->tail=(q->tail+q->borrowed)%R3_QUEUE_CAPACITY;q->count-=q->borrowed;q->borrowed=0;return true;
}
