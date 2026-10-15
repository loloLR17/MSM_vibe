#include "stm32_live_acquisition.h"
#include "platform.h"
#include "port.h"
#include <string.h>
extern volatile uint32_t bench_cleanup_mode,bench_log_refused,bench_preclose_overflow;
extern int stm32_st_transport_attach(SPI_HandleTypeDef *,stmdev_ctx_t *,BenchClock *);
extern void stm32_st_transport_next_window(void);
static uint64_t now(void *ctx)
{
 Stm32LiveAcquisition *s=ctx;uint32_t tick=HAL_GetTick();
 uint32_t delta=tick-s->previous_tick;s->previous_tick=tick;
 if(UINT64_MAX-s->elapsed_ms<delta)return UINT64_MAX;
 s->elapsed_ms+=delta;return s->elapsed_ms;
}
Tr2Result stm32_live_acquisition_attach(Stm32LiveAcquisition *s,SPI_HandleTypeDef *spi,
 uint32_t gap,FifoLiveConsume consume,void *consumer)
{
 stmdev_ctx_t ctx;BenchClock clock;Tr2Result result;
 if(!s||!spi)return TR2_ERROR_INVALID_ARGUMENT;
 if(stm32_st_transport_attach(spi,&ctx,&clock))return TR2_ERROR_UNAVAILABLE;
 memset(s,0,sizeof(*s));
 s->previous_tick=HAL_GetTick();s->elapsed_ms=s->previous_tick;
 result=live_fifo_acquisition_init(&s->acquisition,&ctx,now,s,gap,consume,consumer);
 if(result==TR2_OK)s->acquisition.fifo.cleanup_begin=clock.cleanup_begin;
 return result;
}
Tr2Result stm32_live_acquisition_poll(Stm32LiveAcquisition *s)
{
 Tr2Result result;
 if(!s)return TR2_ERROR_INVALID_ARGUMENT;
 if(!s->acquisition.fifo.running)return live_fifo_acquisition_poll(&s->acquisition);
 /* The previous bounded quantum was fully checked. Sticky errors never clear. */
 if(bench_log_refused||bench_preclose_overflow) {
  (void)fifo_live_stop(&s->acquisition.fifo);
  if(s->acquisition.engine)(void)live_command_engine_fault(s->acquisition.engine);
  return TR2_ERROR_UNAVAILABLE;
 }
 stm32_st_transport_next_window();bench_cleanup_mode=0;
 result=live_fifo_acquisition_poll(&s->acquisition);return result;
}
