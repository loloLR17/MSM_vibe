#ifndef DURABLE_FORMAT_H
#define DURABLE_FORMAT_H
#include <stdint.h>
#include <stdbool.h>
#define DURABLE_SAMPLES 78u
uint32_t durable_crc(const uint8_t *,uint32_t);
void durable_put32(uint8_t *,uint32_t);
void durable_put64(uint8_t *,uint64_t);
void durable_record(uint8_t [512],uint64_t,uint32_t,uint32_t,uint64_t,uint64_t,uint16_t,uint16_t);
bool durable_record_valid(const uint8_t [512],uint64_t,uint32_t,uint32_t);
#endif
