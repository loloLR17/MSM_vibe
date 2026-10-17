/* R5-B only: exact monotone R5AFIX final snapshot; no retirement/RENEW.
 * Reuses the qualified UART/FIFO/R2 harness. WAIT until explicit trigger.
 * One genuine START5/12/run11, STOP5/13; no invented B3 authority. */
#include "tr2/application/live_runtime.h"
#define R4U_INITIAL_SHA 0x4e,0x7f,0xab,0xde,0x5d,0xa1,0x10,0x0c,0xf9,0xae,0x8f,0xaf,0x26,0x2e,0xa0,0x79,0x72,0x78,0x56,0x82,0x6b,0xb2,0x82,0x9c,0x1e,0x99,0xb6,0x8a,0xd6,0x3b,0x84,0xbd
#define R4U_INITIAL_JOURNAL 70
#define R4U_INITIAL_ROOT 83
#define R4U_INITIAL_RUN 10
#define R4U_INITIAL_STOP 11
#define R4U_FINAL_RUN 11
#define R4U_FINAL_STOP 13
#define R4U_EFFECT(code,point,result) ((void)0)
#define R4U_PORT(p) ((void)0)
#define R4U_BOUNDED_INIT() ((void)0)
static void r5b_prepared(void);
#define R4U_PREPARED() r5b_prepared()
#define R4U_STOP_HOLD_US 20000000 /* bounded HELD service observation after command-master closes */
#include "stm32_r4u_runtime.c"
static void r5b_prepared(void)
{
 Tr2Result result=live_runtime_set_drain_period(&runtime,250);
 if(result!=TR2_OK){r4u_result.error=9;work.error=result;}
}
