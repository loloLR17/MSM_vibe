#ifndef DURABLE_SD_GROUP_H
#define DURABLE_SD_GROUP_H
#include "sd_spi.h"
typedef struct {
 SdSpi *card;const uint8_t *data;uint32_t lba,blocks,block,offset,state,error;
 uint64_t begin_us,deadline_us,busy_begin_us,busy_sum_us,complete_us;
 uint32_t max_busy_us,block_busy_us[9];uint16_t crc;bool active,done,uncertain,needs_power_cycle;
} SdGroup;
/* 1..8 immutable contiguous preallocated sectors. One <=128B transfer per step.
 * ACMD23/CMD25, FC per sector, FD stop, busy then separate CMD13. */
int sd_group_begin(SdGroup *,SdSpi *,uint32_t,const uint8_t *,uint32_t);
int sd_group_step(SdGroup *);
#endif
