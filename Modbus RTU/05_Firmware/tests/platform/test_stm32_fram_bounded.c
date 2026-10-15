#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "stm32_fram_storage.h"
static unsigned calls,fail_at,cs,timeout_seen;
void HAL_GPIO_WritePin(GPIO_TypeDef *port,uint16_t pin,uint32_t value)
{(void)port;(void)pin;cs=value;}
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *h,uint8_t *b,uint16_t n,uint32_t timeout)
{(void)h;(void)b;assert(!cs&&n<=64);timeout_seen=timeout;return ++calls==fail_at?HAL_ERROR:HAL_OK;}
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *h,uint8_t *b,uint16_t n,uint32_t timeout)
{(void)h;assert(!cs&&n<=64);timeout_seen=timeout;memset(b,2,n);return ++calls==fail_at?HAL_ERROR:HAL_OK;}
int main(void)
{
 Stm32FramStorage storage;SPI_HandleTypeDef spi;GPIO_TypeDef gpio;uint8_t b[65]={0};
 assert(stm32_fram_storage_init(&storage,&spi,&gpio,1,10)==TR2_OK);
 TransactionalImagePhysicalStorage p=stm32_fram_storage_bounded_physical(&storage);
 for(unsigned i=0;i<=6;i++) {
  calls=0;fail_at=i;
  Tr2Result r=p.write(p.context,122880,b,32);
  assert((i==0)==(r==TR2_OK));assert(cs&&calls<=6&&timeout_seen==2&&storage.timeout_ms==10);
 }
 for(unsigned i=0;i<=2;i++) {
  calls=0;fail_at=i;Tr2Result r=p.read(p.context,122880,b,32);
  assert((i==0)==(r==TR2_OK));assert(cs&&calls<=2&&timeout_seen==2);
 }
 calls=0;assert(p.write(p.context,122880,b,65)==TR2_ERROR_INVALID_ARGUMENT&&!calls);
 fail_at=0;p=stm32_fram_storage_physical(&storage);
 assert(p.read(p.context,0,b,32)==TR2_OK&&timeout_seen==10); /* V1 untouched */
 puts("Actual STM32 FRAM driver on HAL mock: CS released, all6/2 I/O failure positions, no retry, opt-in2ms, V1 timeout unchanged: PASS");
 return 0;
}
