#ifndef R3_STREAM_HASH_H
#define R3_STREAM_HASH_H
#include <stdint.h>
#include <stddef.h>
typedef struct {uint32_t h[8];uint8_t pending[64];uint64_t bytes;uint32_t used;} R3Hash;
void r3_hash_init(R3Hash *);
void r3_hash_update(R3Hash *,const uint8_t *,size_t);
void r3_hash_finish(const R3Hash *,uint8_t [32]);
#endif
