/* R5-AFIX only: exact monotone R4V final snapshot; no retirement/RENEW.
 * Reuses the qualified UART/FIFO/R2 harness. WAIT until explicit trigger.
 * One genuine START5/10/run10, STOP5/11; no invented B3 authority. */
#include "tr2/application/live_runtime.h"
#define R4U_INITIAL_SHA 0x70,0xe2,0x73,0x97,0x2a,0x83,0xe6,0x12,0x83,0xa6,0xe5,0x72,0xdb,0x31,0x87,0xaf,0x00,0x2f,0xd9,0x7b,0xa8,0xde,0x63,0xf8,0xc7,0x09,0xd3,0xe3,0x0a,0xf7,0x35,0x87
#define R4U_INITIAL_JOURNAL 62
#define R4U_INITIAL_ROOT 75
#define R4U_INITIAL_RUN 9
#define R4U_INITIAL_STOP 9
#define R4U_FINAL_RUN 10
#define R4U_FINAL_STOP 11
#define R4U_EFFECT(code,point,result) ((void)0)
#define R4U_PORT(p) ((void)0)
#define R4U_BOUNDED_INIT() ((void)0)
static void r5afix_prepared(void);
#define R4U_PREPARED() r5afix_prepared()
#include "stm32_r4u_runtime.c"
static void r5afix_prepared(void)
{
 Tr2Result result=live_runtime_set_drain_period(&runtime,250);
 if(result!=TR2_OK){r4u_result.error=9;work.error=result;}
}
