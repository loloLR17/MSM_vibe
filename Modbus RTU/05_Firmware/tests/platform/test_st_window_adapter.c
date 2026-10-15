#include "../../platform/stm32/stm32_iis3dwb_st_window.c"
#include <assert.h>
SPI_TypeDef mock_spi3;GPIO_TypeDef mock_c,mock_d;DWT_TypeDef mock_dwt;CoreDebug_TypeDef mock_core;
static uint32_t core_hz=160000000,spi_hz=16000000,uid0=0x004A003D,io_mode;
uint32_t HAL_GetTick(void){return 0;}void HAL_Delay(uint32_t n){(void)n;}
uint32_t HAL_GetUIDw0(void){return uid0;}uint32_t HAL_GetUIDw1(void){return 0x38425018;}uint32_t HAL_GetUIDw2(void){return 0x20333157;}
uint32_t HAL_RCC_GetHCLKFreq(void){return core_hz;}uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t n){(void)n;return spi_hz;}
void HAL_GPIO_WritePin(GPIO_TypeDef *p,uint32_t pin,uint32_t state){if(state)p->ODR|=pin;else p->ODR&=~pin;}
uint32_t HAL_SPI_GetError(SPI_HandleTypeDef *h){return h->ErrorCode;}
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *h,uint8_t *d,uint16_t n,uint32_t t){(void)d;(void)n;(void)t;h->TxXferCount=0;h->RxXferCount=0;bench_spi_preclose(h);return io_mode==1?3:0;}
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *h,uint8_t *d,uint16_t n,uint32_t t){(void)t;memset(d,0,n);h->RxXferSize=n;h->RxXferCount=io_mode==2?1:0;h->pRxBuffPtr=d+(io_mode==3?n-1:n);h->Instance->SR=io_mode==4?64:0;h->ErrorCode=io_mode==5?4:0;bench_spi_preclose(h);return 0;}
int main(void){
 SPI_HandleTypeDef handle={.Instance=SPI3,.Init={1,1,1},.State=1};stmdev_ctx_t ctx;BenchClock clock;
 sensor_spi=&handle;mock_c.ODR=GPIO_PIN_9;
 assert(platform_init(&ctx,&clock)==0);
 core_hz=16000000;assert(platform_init(&ctx,&clock)!=0);core_hz=160000000;
 spi_hz=160000000;assert(platform_init(&ctx,&clock)!=0);spi_hz=16000000;
 uid0=0x001B0046;assert(platform_init(&ctx,&clock)!=0);uid0=0x004A003D;
 handle.State=2;assert(platform_init(&ctx,&clock)!=0);handle.State=1;
 handle.Init.BaudRatePrescaler=2;assert(platform_init(&ctx,&clock)!=0);handle.Init.BaudRatePrescaler=1;
 handle.Init.CLKPhase=0;assert(platform_init(&ctx,&clock)!=0);handle.Init.CLKPhase=1;
 handle.Init.CLKPolarity=0;assert(platform_init(&ctx,&clock)!=0);handle.Init.CLKPolarity=1;
 mock_c.ODR=0;assert(platform_init(&ctx,&clock)!=0);mock_c.ODR=GPIO_PIN_9;
 mock_dwt.CTRL=DWT_CTRL_NOCYCCNT_Msk;assert(platform_init(&ctx,&clock)!=0);mock_dwt.CTRL=0;
 uint8_t value[7];
 for(io_mode=0;io_mode<=5;io_mode++){
  bench_trace_count=bench_payload_count=bench_preclose_count=bench_preclose_overflow=0;
  handle.ErrorCode=0;mock_spi3.SR=0;
  assert((platform_read(sensor_spi,120,value,7)==0)==(io_mode==0));
  assert(mock_c.ODR&GPIO_PIN_9);
 }
 io_mode=0;bench_trace_count=1012;bench_payload_count=bench_preclose_count=0;
 assert(platform_read(sensor_spi,120,value,7)!=0);assert(bench_trace_count==1012);
 bench_cleanup_mode=1;handle.ErrorCode=0;mock_spi3.SR=0;
 assert(platform_read(sensor_spi,16,value,1)==0);
 return 0;
 assert(stm32_st_transport_attach(&handle,&ctx,&clock)==0);
 assert(stm32_st_transport_attach(&handle,&ctx,&clock)!=0);
}
