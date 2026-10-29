#ifndef R3_SD_WRITER_H
#define R3_SD_WRITER_H
#include "sd_spi.h"
/* Separate cooperative writer; R2 blocking implementation unchanged.
 * One borrowed immutable sector until DONE, CS stays asserted through payload
 * and busy; SPI3 foreground may run between steps without SPI2 clocks. */
typedef struct {
 SdSpi *card;const uint8_t *data;uint32_t state,lba,offset,error;
 uint64_t begin_us,busy_begin_us,complete_us,busy_us;
 uint16_t crc;bool active,done,uncertain;
} SdWriter;
int sd_writer_r3_gate(SdSpi *);
int sd_writer_begin(SdWriter *,SdSpi *,uint32_t,const uint8_t [512]);
/* Exactly one <=128-byte data quantum or bounded command/response per call.
 * Returns0 for pending/DONE, nonzero on first latched failure. No retry. */
int sd_writer_step(SdWriter *);
#endif
