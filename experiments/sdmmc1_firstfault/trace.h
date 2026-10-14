#ifndef FIRSTFAULT_TRACE_H
#define FIRSTFAULT_TRACE_H
#include "stm32u5xx_hal.h"
#define FF_MAGIC 0x46303133U
#define FF_TRIGGER 0x52454144U
enum { FF_WAIT=1, FF_INIT=2, FF_WIDTH=3, FF_SSR=4, FF_INITIAL_STATUS=5,
       FF_READ512=6, FF_STATUS512=7, FF_READ8192=8, FF_STATUS8192=9,
       FF_FAILED=99, FF_PASSED=100 };
typedef struct {
  uint32_t valid, phase, source_line, kind, error, tick, cycles, ipsr;
  uint32_t sta, respcmd, resp1, arg, cmd, dctrl, dlen, dcount, clkcr, mask;
  uint32_t hal_error, hal_state, hal_context;
} FF_Snapshot;
typedef struct {
  uint32_t phase, result, error_before, error_after, tick_start, tick_end;
  uint32_t cycles_start, cycles_end;
  FF_Snapshot before, after;
} FF_Phase;
typedef struct {
  uint32_t magic, profile, stage, trigger, run_count, terminal_reason;
  uint32_t uid[3], cid[4], sectors, sector_bytes, ssr_width, initial_state;
  uint32_t mismatch512, mismatch8192, first_mismatch;
  uint32_t gpio_c[5], gpio_d[5], rcc[3], core_clock, primask, basepri;
  uint32_t nvic_iser[4], nvic_ipr_sd, nvic_ipr_tim, nvic_ipr_usb;
  FF_Snapshot first_error;
  uint32_t phase_count;
  FF_Phase phases[10];
} FF_Result;
extern volatile FF_Result ff_result;
extern SD_HandleTypeDef hsd1;
void ff_capture(SDMMC_TypeDef *sd, uint32_t error, uint32_t line, uint32_t kind);
void ff_r1_preclear(SDMMC_TypeDef *sd, uint32_t expected_cmd, uint32_t line);
void ff_r6_preclear(SDMMC_TypeDef *sd, uint32_t line);
static inline void ff_data_preclear(SD_HandleTypeDef *hsd, uint32_t line) {
  uint32_t flags=hsd->Instance->STA;
  if (flags & (SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT |
               SDMMC_FLAG_RXOVERR | SDMMC_FLAG_TXUNDERR))
    ff_capture(hsd->Instance, flags, line, 2U);
}
void ff_prepare(void);
void ff_run(void);
#endif
