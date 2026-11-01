#ifndef TR2_RETAINED_GUARD_H
#define TR2_RETAINED_GUARD_H
#include "archive_format.h"
/* Volatile RAM only: no survival across power loss is promised. Publish magic
 * last, reject partial writes, wrong layout and any damaged payload. */
typedef struct {volatile uint32_t magic;uint32_t version,bytes,crc;} RetainedGuard;
#define RETAINED_MAGIC 0x31525452u
static inline bool retained_valid(const RetainedGuard *g,const void *p,uint32_t n)
{return g->magic==RETAINED_MAGIC&&g->version==1&&g->bytes==n&&g->crc==durable_crc(p,n);}
static inline void retained_seal(RetainedGuard *g,const void *p,uint32_t n)
{g->version=1;g->bytes=n;g->crc=durable_crc(p,n);__atomic_thread_fence(__ATOMIC_SEQ_CST);g->magic=RETAINED_MAGIC;}
#endif
