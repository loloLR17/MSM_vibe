#include "platform.h"
BenchResult bench_result;
void SysTick_Handler(void){HAL_IncTick();}
int main(void)
{
  stmdev_ctx_t ctx={0};BenchClock clock={0};
  if(HAL_Init()!=HAL_OK) {bench_result.result=BENCH_IO;for(;;)__WFI();}
  if(platform_init(&ctx,&clock)) {bench_result.result=BENCH_IO;for(;;)__WFI();}
  bench_ready=1;
  /* Inert after boot until an explicitly authorized, one-shot bench trigger. */
  while(bench_command!=0x4D31354BU)__WFI();
  bench_ready=0;bench_command=0;
  (void)bench_run(&ctx,&clock,BENCH_FIFO,BENCH_STATUS10,&bench_result);
  for(;;)__WFI();
}
