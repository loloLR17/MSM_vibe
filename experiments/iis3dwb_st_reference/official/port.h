#ifndef OFFICIAL_PORT_H
#define OFFICIAL_PORT_H
#include <stdint.h>
#include "iis3dwb_reg.h"
extern uint8_t official_bus;
#define SENSOR_BUS official_bus
int32_t official_read(void *,uint8_t,uint8_t *,uint16_t);
int32_t official_write(void *,uint8_t,const uint8_t *,uint16_t);
void official_init(void);
void official_delay(uint32_t);
void official_console(uint8_t *,uint16_t);
int32_t official_status10(const stmdev_ctx_t *,iis3dwb_fifo_status_t *);
void official_prepare(void);
void official_run(void);
#endif
