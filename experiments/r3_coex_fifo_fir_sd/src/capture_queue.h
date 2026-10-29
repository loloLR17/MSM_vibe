#ifndef R3_CAPTURE_QUEUE_H
#define R3_CAPTURE_QUEUE_H
#include <stdint.h>
#include <stdbool.h>
#ifndef R3_QUEUE_CAPACITY
#define R3_QUEUE_CAPACITY 65536u
#endif
typedef struct {uint8_t data[R3_QUEUE_CAPACITY];uint32_t head,tail,count,peak,overflows,borrowed;} CaptureQueue;
void capture_queue_init(CaptureQueue *);
bool capture_queue_push(CaptureQueue *,const uint8_t *,uint32_t);
const uint8_t *capture_queue_acquire(CaptureQueue *,bool,uint8_t [512]);
bool capture_queue_commit(CaptureQueue *);
#endif
