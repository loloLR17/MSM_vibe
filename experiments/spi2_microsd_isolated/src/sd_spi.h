#ifndef R2_SD_SPI_H
#define R2_SD_SPI_H
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
enum {SD_OK=0,SD_IO=1,SD_TIMEOUT=2,SD_RESPONSE=3,SD_CRC=4,SD_IDENTITY=5,SD_RANGE=6,SD_TOKEN=7,SD_REFUSED=8};
typedef struct {void *context;int (*exchange)(void *,const uint8_t *,uint8_t *,uint16_t);void (*select)(void *,bool);uint64_t (*us)(void *);} SdSpiIo;
typedef struct {uint32_t cmd,arg,r1,extra,elapsed_us;} SdTrace;
typedef struct {
 SdSpiIo io;uint8_t cid[16],csd[16];uint32_t ocr,sectors,error,last_cmd,last_r1,commands;
 uint32_t reads,writes,max_busy_us,last_busy_us,timeouts,crc_errors,begin_lba,limit_lba;
 bool initialized,write_enabled,failed;
 SdTrace trace[256];
} SdSpi;
uint8_t sd_crc7(const uint8_t *,size_t);
uint16_t sd_crc16(const uint8_t *,size_t);
int sd_spi_init(SdSpi *,SdSpiIo);
int sd_spi_read(SdSpi *,uint32_t,uint8_t [512]);
/* Exact test CID/capacity only. Explicit raw test layout includes sector0;
 * application must describe the layout BEFORE selecting this destructive gate. */
int sd_spi_test_gate(SdSpi *,uint32_t,uint32_t);
int sd_spi_write(SdSpi *,uint32_t,const uint8_t [512]);
int sd_spi_quiesce(SdSpi *);
#endif
