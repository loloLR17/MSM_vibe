/* Isolated R4R first increment: only raw evidence, never a fabricated B3 time.
 * Volatile diagnostic identity UINT64_MAX-1 does not enter R2 or NVM. */
#define stm32_r4_benchmark_run unused_r4p_benchmark_entry
#include "stm32_r4_bench.c"
#undef stm32_r4_benchmark_run

typedef struct {uint64_t before_us,after_us;uint32_t raw,index;} ClockBracket;
volatile struct {
 uint32_t stage,error,words,anchors,counter_mismatch,reserved_nonzero;
 uint32_t min_delta,max_delta,raw_words,brackets,fine,first_ts,last_ts;
 uint64_t first_index,last_index,begin_us,end_us,max_poll_us;
} r4r_result;
uint8_t r4r_raw[2048*7];
ClockBracket r4r_brackets[256];
static uint32_t last_anchor;
static uint8_t last_xyz_tag;
static int r_consume(void *ctx,const uint8_t *b,uint32_t n,uint64_t index)
{
 (void)ctx;
 if(!r4_result.first_data_us)r4_result.first_data_us=now_us(NULL)-r4_result.start_us;
 for(uint32_t i=0;i<n;i++,b+=7) {
  if(r4r_result.raw_words<2048)memcpy(r4r_raw+7*r4r_result.raw_words++,b,7);
  r4r_result.words++;
  if((b[0]>>3)==2){index++;last_xyz_tag=b[0];continue;}
  if((b[0]>>3)!=4)return -1;
  uint32_t raw=(uint32_t)b[1]|(uint32_t)b[2]<<8|(uint32_t)b[3]<<16|(uint32_t)b[4]<<24;
  if(((b[0]>>1)&3)!=((last_xyz_tag>>1)&3))r4r_result.counter_mismatch++;
  if(b[5]||b[6]!=10)r4r_result.reserved_nonzero++;
  if(r4r_result.anchors){
   uint32_t delta=raw-last_anchor;
   if(delta<r4r_result.min_delta)r4r_result.min_delta=delta;
   if(delta>r4r_result.max_delta)r4r_result.max_delta=delta;
  }else{r4r_result.first_ts=raw;r4r_result.first_index=index;}
  last_anchor=raw;r4r_result.last_ts=raw;r4r_result.last_index=index;r4r_result.anchors++;
 }
 return 0;
}
__attribute__((noreturn)) void stm32_r4_benchmark_run(SPI_HandleTypeDef *spi1,SPI_HandleTypeDef *spi3)
{
 r4_result.core_hz=HAL_RCC_GetHCLKFreq();r4_result.spi1_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI1)/64u;
 r4_result.spi3_hz=HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3)/4u;
 if(!identity_ok()||r4_result.core_hz!=160000000u||r4_result.spi1_hz!=2500000u||r4_result.spi3_hz!=4000000u||
 stm32_fram_storage_init(&fram,spi1,GPIOD,GPIO_PIN_14,2)!=TR2_OK||stm32_live_acquisition_attach(&owner,spi3,5,r_consume,NULL)!=TR2_OK){r4r_result.error=1;goto end;}
 owner.acquisition.fifo.first_data_timeout_ms=20;
 previous_cycles=DWT->CYCCNT;elapsed_cycles=0;bounded=stm32_fram_storage_bounded_physical(&fram);
 if(tr2_cooperative_init(&work,NULL,now_us,service,stop,32,1000,2500)!=TR2_OK){r4r_result.error=2;goto end;}
 r4r_result.stage=1;
 while(r4_trigger!=0x5234000Au){(void)now_us(NULL);HAL_Delay(1);}
 r4_trigger=0;r4r_result.stage=2;
 if(qualify(5)!=TR2_OK||r4_result.ownership||r4_result.write_bytes){r4r_result.error=3;goto end;}
 uint8_t fine=0;
 if(iis3dwb_odr_cal_reg_get(&owner.acquisition.fifo.ctx,&fine)){r4r_result.error=4;goto end;}
 r4r_result.fine=fine;r4r_result.min_delta=UINT32_MAX;r4r_result.stage=3;
 r4_result.start_us=now_us(NULL);
 if(fifo_live_start(&owner.acquisition.fifo,UINT64_MAX-1,3,3,7,0,HAL_GetTick())){r4r_result.error=5;goto end;}
 r4r_result.begin_us=now_us(NULL);
 uint64_t next_bracket=r4r_result.begin_us;
 while(now_us(NULL)-r4r_result.begin_us<2000000u){
  uint64_t begin=now_us(NULL);
  if(service(NULL)!=TR2_OK){r4r_result.error=6;break;}
  uint64_t duration=now_us(NULL)-begin;if(duration>r4r_result.max_poll_us)r4r_result.max_poll_us=duration;
  if(duration>2500u||r4_result.max_gap_us>5000u){r4r_result.error=8;break;}
  if(now_us(NULL)>=next_bracket&&r4r_result.brackets<256){
   ClockBracket *c=&r4r_brackets[r4r_result.brackets];c->before_us=now_us(NULL);
   if(iis3dwb_timestamp_raw_get(&owner.acquisition.fifo.ctx,&c->raw)){r4r_result.error=7;break;}
   c->after_us=now_us(NULL);c->index=(uint32_t)owner.acquisition.fifo.total_xyz;
   r4r_result.brackets++;next_bracket=c->after_us+10000u;
  }
 }
 r4r_result.end_us=now_us(NULL);stop(NULL);snapshot();
 if(!r4r_result.error&&r4r_result.anchors>100&&r4r_result.brackets>100&&!r4_result.fifo_error&&!r4_result.stop_error)r4r_result.stage=10;
end:
 stop(NULL);r4_result.ownership=0;snapshot();for(;;){(void)now_us(NULL);HAL_Delay(1);}
}
