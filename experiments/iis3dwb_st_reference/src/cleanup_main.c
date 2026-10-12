/* M015-M safety-only exit after trace-capacity refusal. No acquisition. */
#include "platform.h"
BenchResult bench_result;
void SysTick_Handler(void){HAL_IncTick();}
int main(void)
{
  stmdev_ctx_t ctx={0};BenchClock clock={0};uint8_t ctrl1=255,ctrl4=255,ctrl10=255;
  if(HAL_Init()!=HAL_OK || platform_init(&ctx,&clock)) {
    bench_result.result=BENCH_IO;for(;;)__WFI();
  }
  bench_ready=1;
  while(bench_command!=0x4D31354BU)__WFI();
  bench_ready=0;bench_command=0;bench_result.stage=5;
  int failed=iis3dwb_xl_data_rate_set(&ctx,IIS3DWB_XL_ODR_OFF)!=0;
  if(iis3dwb_fifo_mode_set(&ctx,IIS3DWB_BYPASS_MODE))failed=1;
  if(iis3dwb_timestamp_set(&ctx,PROPERTY_DISABLE))failed=1;
  if(iis3dwb_read_reg(&ctx,IIS3DWB_CTRL1_XL,&ctrl1,1))failed=1;
  if(iis3dwb_read_reg(&ctx,IIS3DWB_FIFO_CTRL4,&ctrl4,1))failed=1;
  if(iis3dwb_read_reg(&ctx,IIS3DWB_CTRL10_C,&ctrl10,1))failed=1;
  if((ctrl1&0xE0U) || (ctrl4&7U) || (ctrl10&0x20U))failed=1;
  bench_result.controls[0]=ctrl1;bench_result.controls[1]=ctrl4;
  bench_result.controls[2]=ctrl10;
  bench_result.result=bench_result.cleanup_result=failed?BENCH_IO:BENCH_OK;
  bench_result.stage=6;
  for(;;)__WFI();
}
