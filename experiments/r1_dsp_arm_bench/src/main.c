/* NON-PRODUCTION TR2-002 only. NO GPIO, SPI, UART, media or FRAM initialization.
 * Genuine CubeU5 startup/vector table. RAM-triggered once; inspect after DONE.
 */
#include "stm32u5xx_hal.h"
#include "micro_bench.h"
R1MicroResult r1_result;
volatile uint32_t r1_command,r1_irq_count;
extern uint32_t _estack,_ebss;
void SystemClock_Config(void);
void HAL_MspInit(void){__HAL_RCC_PWR_CLK_ENABLE();}
void SysTick_Handler(void){HAL_IncTick();r1_irq_count++;}
void Error_Handler(void){r1_result.error=r1_result.error?r1_result.error:1;r1_result.stage=99;for(;;)__WFI();}
static uint32_t cycles(void *unused){(void)unused;__DSB();__ISB();return DWT->CYCCNT;}
static uint32_t ticks(void *unused){(void)unused;return HAL_GetTick();}
static uint32_t irqs(void *unused){(void)unused;return r1_irq_count;}
static void paint_stack(void)
{
    uintptr_t end=(__get_MSP()-256u)&~(uintptr_t)3u,begin=(uintptr_t)&_estack-2048u;
    if(end<=begin||begin<(uintptr_t)&_ebss+512u){r1_result.error=8;Error_Handler();}
    r1_result.stack_begin=(uint32_t)begin;r1_result.stack_end=(uint32_t)end;
    for(volatile uint32_t *p=(uint32_t *)begin;(uintptr_t)p<end;p++)*p=0xA53CC35Au;
}
static void scan_stack(void)
{
    uintptr_t start=r1_result.stack_begin,end=r1_result.stack_end,p=start;
    while(p<end&&*(volatile uint32_t *)p==0xA53CC35Au)p+=4;
    r1_result.stack_untouched=(uint32_t)(p-start);
    r1_result.stack_deepest=p<end?(uint32_t)((uintptr_t)&_estack-p):0;
    if(p==start){r1_result.error=9;Error_Handler();} /* no intact floor marker */
}
/* Temporary hardware breakpoint belongs HERE, never in the timed benchmark. */
__attribute__((noinline)) void r1_bench_finished(void){__DSB();__ISB();}
int main(void)
{
    r1_result.version=1;r1_result.stage=1;
    r1_result.uid[0]=HAL_GetUIDw0();r1_result.uid[1]=HAL_GetUIDw1();r1_result.uid[2]=HAL_GetUIDw2();
    if(r1_result.uid[0]!=0x001B0046u||r1_result.uid[1]!=0x38425018u||r1_result.uid[2]!=0x20333157u){r1_result.error=2;Error_Handler();}
    if(HAL_Init()!=HAL_OK){r1_result.error=3;Error_Handler();}
    HAL_PWREx_DisableUCPDDeadBattery();
    if(HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY)!=HAL_OK){r1_result.error=4;Error_Handler();}
    SystemClock_Config();r1_result.clock_hz=HAL_RCC_GetHCLKFreq();
    if(r1_result.clock_hz!=160000000u){r1_result.error=5;Error_Handler();}
    CoreDebug->DEMCR|=CoreDebug_DEMCR_TRCENA_Msk;
    if(DWT->CTRL&DWT_CTRL_NOCYCCNT_Msk){r1_result.error=6;Error_Handler();}
    DWT->CYCCNT=0;DWT->CTRL|=DWT_CTRL_CYCCNTENA_Msk;
    r1_result.primask=__get_PRIMASK();r1_result.basepri=__get_BASEPRI();r1_result.faultmask=__get_FAULTMASK();
    if(r1_result.primask||r1_result.basepri||r1_result.faultmask){r1_result.error=7;Error_Handler();}
    r1_result.demcr=CoreDebug->DEMCR;r1_result.dwt_ctrl=DWT->CTRL;r1_result.flash_acr=FLASH->ACR;
    paint_stack();r1_result.stage=2;
    while(r1_command!=R1_BENCH_TRIGGER){if(r1_command){r1_result.error=10;Error_Handler();}__WFI();}
    r1_command=0;r1_result.stage=3;
    Tr2Result r=r1_micro_run(&r1_result,cycles,NULL,ticks,NULL,irqs,NULL);
    r1_result.systick_count=r1_irq_count;scan_stack();
    if(r!=TR2_OK){r1_result.stage=99;}else r1_result.stage=4;
    r1_bench_finished();
    for(;;)__WFI();
}
