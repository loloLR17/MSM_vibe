#ifndef U_MOCK_HAL
#define U_MOCK_HAL
#include <stdint.h>
#include <stddef.h>
typedef struct {uint32_t SR,CR1,CR2,CFG1,CFG2;} SPI_TypeDef;
typedef struct {uint32_t BaudRatePrescaler,CLKPolarity,CLKPhase;} SPI_InitTypeDef;
typedef struct {SPI_TypeDef *Instance;SPI_InitTypeDef Init;uint32_t State,ErrorCode,RxXferSize,RxXferCount,TxXferCount;uint8_t *pRxBuffPtr;} SPI_HandleTypeDef;
typedef struct {uint32_t ODR;} GPIO_TypeDef;
typedef struct {uint32_t CTRL,CYCCNT;} DWT_TypeDef;
typedef struct {uint32_t DEMCR;} CoreDebug_TypeDef;
extern SPI_TypeDef mock_spi3;extern GPIO_TypeDef mock_c,mock_d;extern DWT_TypeDef mock_dwt;extern CoreDebug_TypeDef mock_core;
#define SPI3 (&mock_spi3)
#define GPIOC (&mock_c)
#define GPIOD (&mock_d)
#define DWT (&mock_dwt)
#define CoreDebug (&mock_core)
#define CoreDebug_DEMCR_TRCENA_Msk 1U
#define DWT_CTRL_NOCYCCNT_Msk 2U
#define DWT_CTRL_CYCCNTENA_Msk 1U
#define GPIO_PIN_9 (1U<<9)
#define GPIO_PIN_14 (1U<<14)
#define GPIO_PIN_RESET 0U
#define GPIO_PIN_SET 1U
#define SPI_SR_OVR 64U
#define HAL_SPI_STATE_READY 1U
#define SPI_BAUDRATEPRESCALER_4 1U
#define SPI_POLARITY_HIGH 1U
#define SPI_PHASE_2EDGE 1U
#define RCC_PERIPHCLK_SPI3 1U
#define __WFI() ((void)0)
typedef uint32_t HAL_StatusTypeDef;
#define HAL_OK 0U
uint32_t HAL_GetTick(void);void HAL_Delay(uint32_t);
uint32_t HAL_GetUIDw0(void);uint32_t HAL_GetUIDw1(void);uint32_t HAL_GetUIDw2(void);
uint32_t HAL_RCC_GetHCLKFreq(void);uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t);
void HAL_GPIO_WritePin(GPIO_TypeDef *,uint32_t,uint32_t);
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *,uint8_t *,uint16_t,uint32_t);
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *,uint8_t *,uint16_t,uint32_t);
uint32_t HAL_SPI_GetError(SPI_HandleTypeDef *);
#endif
