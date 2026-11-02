/* TR2-001 only. Snapshot after producer STOP, before any cable reconnection.
 * Startup excludes this section from BSS zeroing. A valid capture survives
 * subsequent boot errors; a new deliberate USER start invalidates it first. */
#include "retained_guard.h"
typedef struct {
 char source_sha[40];Result result;SdGroup writer;CaptureQueue queue;
 R3Hash source_hash;uint8_t source_sha256[32],header[512],partial_frame[512];
 uint32_t frame_count,frame_seq,first_output,clock_fault;
 FifoEventTime mapper;Bracket last_clock_bracket;
 uint32_t budget_mask,maximum_cycles[4];Record budget_record;
 uint32_t sensor_only;
} RetainedPayload;
typedef struct {RetainedGuard guard;RetainedPayload payload;} RetainedSnapshot;
RetainedSnapshot r3_retained __attribute__((section(".tr2_retained"),used,aligned(8)));
uint32_t r3_retained_available;
extern uint32_t __tr2_retained_end__;
static void retain_capture(void)
{
 r3_retained.guard.magic=0;__atomic_thread_fence(__ATOMIC_SEQ_CST);
 RetainedPayload *p=&r3_retained.payload;
 memcpy(p->source_sha,R3_CODE_SHA,40);p->result=r3_result;p->writer=r3_writer;p->queue=q;
 p->source_hash=source_hash;memcpy(p->source_sha256,r3_source_sha,32);
 memcpy(p->header,r3_header,512);memcpy(p->partial_frame,frame,512);
 p->frame_count=durable_frame_count;p->frame_seq=durable_frame_seq;
 p->first_output=durable_first_output;p->clock_fault=r3_clock_fault;
 p->mapper=mapper;p->last_clock_bracket=r3_last_clock_bracket;
 p->budget_mask=r3_budget_mask;p->budget_record=r3_budget_record;
 p->sensor_only=R3_TR2_001_SENSOR_ONLY;
 for(unsigned i=0;i<4;i++)p->maximum_cycles[i]=r3_distributions[i].max;
 retained_seal(&r3_retained.guard,p,sizeof *p);__DSB();r3_retained_available=1;
}
