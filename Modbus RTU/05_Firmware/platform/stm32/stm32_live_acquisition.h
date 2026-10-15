#ifndef STM32_LIVE_ACQUISITION_H
#define STM32_LIVE_ACQUISITION_H
#include "stm32u5xx_hal.h"
#include "live_fifo_acquisition.h"
typedef struct {
 LiveFifoAcquisition acquisition;
 uint32_t previous_tick;uint64_t elapsed_ms;
} Stm32LiveAcquisition;
/* Explicit single-owner board attachment; does not START or touch FRAM/SD.
 * Uses the qualified SPI3 transport and its positive UID/clock/CS guards. */
Tr2Result stm32_live_acquisition_attach(Stm32LiveAcquisition *,SPI_HandleTypeDef *,
 uint32_t,FifoLiveConsume,void *);
Tr2Result stm32_live_acquisition_poll(Stm32LiveAcquisition *);
#endif
