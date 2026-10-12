#include "platform.h"
#include "port.h"
extern BenchResult bench_result;
void SysTick_Handler(void){HAL_IncTick();}
int main(void)
{
  if(HAL_Init()!=HAL_OK){for(;;)__WFI();}
  official_prepare();if(bench_result.result){for(;;)__WFI();}
  bench_ready=1;
  while(bench_command!=0x4D31354BU)__WFI();
  bench_ready=0;bench_command=0;official_run();
  for(;;)__WFI();
}
