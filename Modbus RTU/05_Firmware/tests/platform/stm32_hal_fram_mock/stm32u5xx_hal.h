#ifndef TR2_FRAM_MOCK_HAL
#define TR2_FRAM_MOCK_HAL
#include <stdint.h>
typedef struct {unsigned unused;} SPI_HandleTypeDef;
typedef struct {unsigned unused;} GPIO_TypeDef;
typedef enum {HAL_OK=0,HAL_ERROR=1} HAL_StatusTypeDef;
#define GPIO_PIN_SET 1u
#define GPIO_PIN_RESET 0u
void HAL_GPIO_WritePin(GPIO_TypeDef *,uint16_t,uint32_t);
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *,uint8_t *,uint16_t,uint32_t);
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *,uint8_t *,uint16_t,uint32_t);
#endif
