#ifndef DURABLE_FAT_H
#define DURABLE_FAT_H
#include "sd_writer.h"
typedef struct {uint32_t mounted,formatted,error,io_error,io_uncertain,io_writes,first_lba,sectors,run,fs_type,cluster_sectors;uint64_t session;} DurableStatus;
extern DurableStatus durable_status;
int durable_mount(SdSpi *);
int durable_format(void);
int durable_prepare(uint32_t,uint32_t,uint64_t,const uint8_t [512]);
int durable_verify_last(const uint8_t [32]);
int durable_sector(uint32_t,const uint8_t [512]);
#endif
