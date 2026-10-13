#ifndef STM32_SDMMC_TEST_HAL_H
#define STM32_SDMMC_TEST_HAL_H
#include <stdint.h>
typedef enum {HAL_OK=0,HAL_ERROR=1} HAL_StatusTypeDef;
typedef uint32_t HAL_SD_CardStateTypeDef;
#define HAL_SD_CARD_READY 1u
#define HAL_SD_CARD_TRANSFER 4u
#define HAL_SD_CARD_PROGRAMMING 7u
#define HAL_SD_CARD_DISCONNECTED 8u
#define HAL_SD_CARD_ERROR 255u
typedef struct {uint32_t ErrorCode;} SD_HandleTypeDef;
typedef struct {uint32_t LogBlockNbr,LogBlockSize;} HAL_SD_CardInfoTypeDef;
uint32_t HAL_GetTick(void);
HAL_SD_CardStateTypeDef HAL_SD_GetCardState(SD_HandleTypeDef *);
HAL_StatusTypeDef HAL_SD_GetCardInfo(SD_HandleTypeDef *,HAL_SD_CardInfoTypeDef *);
HAL_StatusTypeDef HAL_SD_ReadBlocks(SD_HandleTypeDef *,uint8_t *,uint32_t,uint32_t,uint32_t);
HAL_StatusTypeDef HAL_SD_WriteBlocks(SD_HandleTypeDef *,const uint8_t *,uint32_t,uint32_t,uint32_t);
#endif
