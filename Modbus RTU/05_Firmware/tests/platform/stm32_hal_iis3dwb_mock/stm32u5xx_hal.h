#ifndef TR2_TEST_IIS3DWB_HAL_H
#define TR2_TEST_IIS3DWB_HAL_H
#include <stdint.h>
typedef struct { uint32_t unused; } SPI_HandleTypeDef;
typedef struct { uint32_t unused; } GPIO_TypeDef;
typedef enum {HAL_OK=0, HAL_ERROR=1, HAL_BUSY=2, HAL_TIMEOUT=3} HAL_StatusTypeDef;
typedef enum {GPIO_PIN_RESET=0, GPIO_PIN_SET=1} GPIO_PinState;
typedef struct {uint32_t CTRL, CYCCNT;} DWT_Type;
typedef struct {uint32_t DEMCR;} CoreDebug_Type;
extern DWT_Type test_dwt;
extern CoreDebug_Type test_core_debug;
#define DWT (&test_dwt)
#define CoreDebug (&test_core_debug)
#define DWT_CTRL_NOCYCCNT_Msk (1U<<25)
#define DWT_CTRL_CYCCNTENA_Msk 1U
#define CoreDebug_DEMCR_TRCENA_Msk (1U<<24)
#define RCC_PERIPHCLK_SPI3 1U
void HAL_GPIO_WritePin(GPIO_TypeDef*,uint16_t,GPIO_PinState);
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef*,uint8_t*,uint16_t,uint32_t);
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef*,uint8_t*,uint16_t,uint32_t);
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef*,const uint8_t*,uint8_t*,uint16_t,uint32_t);
void HAL_Delay(uint32_t);
uint32_t HAL_GetTick(void);
uint32_t HAL_RCC_GetHCLKFreq(void);
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t);
#endif
