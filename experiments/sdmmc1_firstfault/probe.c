#include "trace.h"
#include <string.h>
#ifndef FF_PROFILE
#error FF_PROFILE must be explicit
#endif
volatile FF_Result ff_result;
uint8_t ff_data512[512] __attribute__((aligned(4)));
uint8_t ff_data8192[8192] __attribute__((aligned(4)));
static void snapshot(volatile FF_Snapshot *s, SDMMC_TypeDef *sd) {
  s->phase=ff_result.stage; s->tick=HAL_GetTick(); s->cycles=DWT->CYCCNT;
  s->ipsr=__get_IPSR(); s->sta=sd->STA; s->respcmd=sd->RESPCMD;
  s->resp1=sd->RESP1; s->arg=sd->ARG; s->cmd=sd->CMD;
  s->dctrl=sd->DCTRL; s->dlen=sd->DLEN; s->dcount=sd->DCOUNT;
  s->clkcr=sd->CLKCR; s->mask=sd->MASK; s->hal_error=hsd1.ErrorCode;
  s->hal_state=hsd1.State; s->hal_context=hsd1.Context;
}
void ff_capture(SDMMC_TypeDef *sd, uint32_t error, uint32_t line, uint32_t kind) {
  if (ff_result.first_error.valid) return;
  snapshot(&ff_result.first_error,sd);
  ff_result.first_error.error=error; ff_result.first_error.source_line=line;
  ff_result.first_error.kind=kind;
  __DMB(); ff_result.first_error.valid=1;
}
void ff_r1_preclear(SDMMC_TypeDef *sd, uint32_t expected_cmd, uint32_t line) {
  if (sd->RESPCMD != expected_cmd)
    ff_capture(sd,SDMMC_ERROR_CMD_CRC_FAIL,line,1);
  else if (sd->RESP1 & SDMMC_OCR_ERRORBITS)
    ff_capture(sd,sd->RESP1 & SDMMC_OCR_ERRORBITS,line,3);
}
void ff_r6_preclear(SDMMC_TypeDef *sd, uint32_t line) {
  uint32_t bits=sd->RESP1 & (SDMMC_R6_GENERAL_UNKNOWN_ERROR |
      SDMMC_R6_ILLEGAL_CMD | SDMMC_R6_COM_CRC_FAILED);
  if(bits)ff_capture(sd,bits,line,6);
}
static void gpio_snapshot(uint32_t slot) {
  (void)slot;
  ff_result.gpio_c[0]=GPIOC->MODER; ff_result.gpio_c[1]=GPIOC->AFR[1];
  ff_result.gpio_c[2]=GPIOC->PUPDR; ff_result.gpio_c[3]=GPIOC->OSPEEDR;
  ff_result.gpio_c[4]=GPIOC->IDR;
  ff_result.gpio_d[0]=GPIOD->MODER; ff_result.gpio_d[1]=GPIOD->AFR[0];
  ff_result.gpio_d[2]=GPIOD->PUPDR; ff_result.gpio_d[3]=GPIOD->OSPEEDR;
  ff_result.gpio_d[4]=GPIOD->IDR;
  ff_result.rcc[0]=RCC->CR; ff_result.rcc[1]=RCC->CCIPR1;
  ff_result.rcc[2]=RCC->CCIPR2; ff_result.core_clock=SystemCoreClock;
  ff_result.primask=__get_PRIMASK(); ff_result.basepri=__get_BASEPRI();
  for (uint32_t i=0;i<4;i++)ff_result.nvic_iser[i]=NVIC->ISER[i];
  ff_result.nvic_ipr_sd=NVIC->IPR[SDMMC1_IRQn];
  ff_result.nvic_ipr_tim=NVIC->IPR[TIM17_IRQn];
  ff_result.nvic_ipr_usb=NVIC->IPR[OTG_FS_IRQn];
}
void ff_prepare(void) {
  ff_result.magic=FF_MAGIC; ff_result.profile=FF_PROFILE;
  for(uint32_t i=0;i<3;i++)ff_result.uid[i]=((volatile uint32_t *)UID_BASE)[i];
  if(ff_result.uid[0]!=0x001B0046U || ff_result.uid[1]!=0x38425018U ||
     ff_result.uid[2]!=0x20333157U) {
    ff_result.terminal_reason=1; ff_result.stage=FF_FAILED; return;
  }
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT=0; DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  ff_result.stage=FF_WAIT;
}
static volatile FF_Phase *begin(uint32_t phase) {
  ff_result.stage=phase;
  volatile FF_Phase *p=&ff_result.phases[ff_result.phase_count++];
  p->phase=phase; p->error_before=hsd1.ErrorCode;
  p->tick_start=HAL_GetTick(); p->cycles_start=DWT->CYCCNT;
  /* SDMMC clock is not enabled yet before HAL_SD_Init/MSP. */
  if(phase!=FF_INIT)snapshot(&p->before,SDMMC1);
  return p;
}
static int end(volatile FF_Phase *p,uint32_t result,int expected) {
  p->result=result; p->error_after=hsd1.ErrorCode;
  p->cycles_end=DWT->CYCCNT; p->tick_end=HAL_GetTick();
  snapshot(&p->after,SDMMC1);
  if(!expected || hsd1.ErrorCode || ff_result.first_error.valid) {
    if(!ff_result.first_error.valid)
      ff_capture(SDMMC1,hsd1.ErrorCode,p->phase,4);
    ff_result.terminal_reason=2; ff_result.stage=FF_FAILED; return 0;
  }
  return 1;
}
static int status(uint32_t phase) {
  volatile FF_Phase *p=begin(phase);
  uint32_t state=HAL_SD_GetCardState(&hsd1);
  if(phase==FF_INITIAL_STATUS)ff_result.initial_state=state;
  return end(p,state,state==HAL_SD_CARD_TRANSFER);
}
static uint32_t compare(const uint8_t *b,uint32_t size) {
  uint32_t mismatches=0;
  for(uint32_t i=0;i<size;i++) {
    uint8_t expected=(uint8_t)(i*89U+(i>>8)*17U+(i>>16)*31U+0xB7U);
    if(b[i]!=expected) {
      if(mismatches==0)ff_result.first_mismatch=i;
      mismatches++;
    }
  }
  return mismatches;
}
void ff_run(void) {
  if(ff_result.stage!=FF_WAIT || ff_result.run_count || ff_result.trigger!=FF_TRIGGER)return;
  ff_result.trigger=0; ff_result.run_count=1;
  ff_result.first_mismatch=0xFFFFFFFFU;
  hsd1.Instance=SDMMC1;
  hsd1.Init.ClockEdge=SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave=SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide=SDMMC_BUS_WIDE_1B;
  hsd1.Init.HardwareFlowControl=SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv=2;
  volatile FF_Phase *p=begin(FF_INIT);
  HAL_StatusTypeDef result=HAL_SD_Init(&hsd1);
  gpio_snapshot(0);
  if(!end(p,result,result==HAL_OK))return;
  if(SystemCoreClock!=160000000U || __get_IPSR()!=0U || __get_PRIMASK()!=0U ||
     (GPIOC->MODER & 0x03FF0000U)!=0x02AA0000U ||
     (GPIOC->PUPDR & 0x03FF0000U)!=0x00550000U ||
     (GPIOC->OSPEEDR & 0x03FF0000U)!=0x02AA0000U ||
     (GPIOC->AFR[1] & 0x000FFFFFU)!=0x000CCCCCU ||
     (GPIOD->MODER & 0x30U)!=0x20U || (GPIOD->PUPDR & 0x30U)!=0x10U ||
     (GPIOD->OSPEEDR & 0x30U)!=0x20U || (GPIOD->AFR[0] & 0xF00U)!=0xC00U ||
     NVIC_GetEnableIRQ(OTG_FS_IRQn)) {
    ff_result.terminal_reason=8;ff_result.stage=FF_FAILED;return;
  }
  for(uint32_t i=0;i<4;i++)ff_result.cid[i]=hsd1.CID[i];
  ff_result.sectors=hsd1.SdCard.LogBlockNbr;
  ff_result.sector_bytes=hsd1.SdCard.LogBlockSize;
  if(hsd1.CID[0]!=0x03534453U || hsd1.CID[1]!=0x44333247U ||
     hsd1.CID[2]!=0x858BF092U || hsd1.CID[3]!=0x3C01927AU ||
     ff_result.sectors!=62333952U || ff_result.sector_bytes!=512U) {
    ff_result.terminal_reason=3; ff_result.stage=FF_FAILED; return;
  }
  p=begin(FF_WIDTH);
  result=HAL_SD_ConfigWideBusOperation(&hsd1,
      FF_PROFILE==1 ? SDMMC_BUS_WIDE_1B : SDMMC_BUS_WIDE_4B);
  if(!end(p,result,result==HAL_OK))return;
  if(SDMMC1->CLKCR!=(FF_PROFILE==1?0x0002U:0x4002U) ||
     (RCC->CCIPR1 & RCC_CCIPR1_ICLKSEL) ||
     (RCC->CCIPR2 & RCC_CCIPR2_SDMMCSEL) ||
     ((RCC->CR & (RCC_CR_HSI48ON|RCC_CR_HSI48RDY)) != (RCC_CR_HSI48ON|RCC_CR_HSI48RDY))) {
    ff_result.terminal_reason=4; ff_result.stage=FF_FAILED;return;
  }
  HAL_SD_CardStatusTypeDef ssr={0};p=begin(FF_SSR);
  result=HAL_SD_GetCardStatus(&hsd1,&ssr);ff_result.ssr_width=ssr.DataBusWidth;
  if(!end(p,result,result==HAL_OK))return;
  if(ssr.DataBusWidth!=(FF_PROFILE==1?0U:2U)) {
    ff_result.terminal_reason=5;ff_result.stage=FF_FAILED;return;
  }
  if(!status(FF_INITIAL_STATUS))return;
  memset(ff_data512,0xA5,sizeof ff_data512);p=begin(FF_READ512);
  result=HAL_SD_ReadBlocks(&hsd1,ff_data512,506304,1,1000);
  if(!end(p,result,result==HAL_OK))return;
  ff_result.mismatch512=compare(ff_data512,sizeof ff_data512);
  if(ff_result.mismatch512){ff_result.terminal_reason=6;ff_result.stage=FF_FAILED;return;}
  if(!status(FF_STATUS512))return;
  memset(ff_data8192,0xA5,sizeof ff_data8192);p=begin(FF_READ8192);
  result=HAL_SD_ReadBlocks(&hsd1,ff_data8192,506304,16,1000);
  if(!end(p,result,result==HAL_OK))return;
  ff_result.mismatch8192=compare(ff_data8192,sizeof ff_data8192);
  if(ff_result.mismatch8192){ff_result.terminal_reason=7;ff_result.stage=FF_FAILED;return;}
  if(!status(FF_STATUS8192))return;
  ff_result.stage=FF_PASSED;
}
