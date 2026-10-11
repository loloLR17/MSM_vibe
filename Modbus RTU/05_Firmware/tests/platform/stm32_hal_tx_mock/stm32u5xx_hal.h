#ifndef TR2_TX_TEST_HAL_H
#define TR2_TX_TEST_HAL_H
#include <stdint.h>
#include <stdbool.h>
typedef enum { HAL_OK, HAL_ERROR, HAL_BUSY, HAL_TIMEOUT } HAL_StatusTypeDef;
typedef enum { GPIO_PIN_RESET, GPIO_PIN_SET } GPIO_PinState;
typedef struct { unsigned unused; } GPIO_TypeDef;
typedef struct { uint32_t Pin,Mode,Pull,Speed,Alternate; } GPIO_InitTypeDef;
typedef struct { uint32_t BaudRate,WordLength,StopBits,Parity,Mode,HwFlowCtl,OneBitSampling,ClockPrescaler; } UART_InitTypeDef;
typedef struct { void *Instance; UART_InitTypeDef Init; struct { uint32_t AdvFeatureInit; } AdvancedInit; } UART_HandleTypeDef;
typedef struct { void *Instance; struct { uint32_t Prescaler,CounterMode,Period,AutoReloadPreload; } Init; } TIM_HandleTypeDef;
typedef struct { uint32_t APB1CLKDivider; } RCC_ClkInitTypeDef;
extern GPIO_TypeDef mock_gpio;
#define GPIOG (&mock_gpio)
#define GPIO_PIN_4 0x10u
#define GPIO_PIN_7 0x80u
#define GPIO_PIN_8 0x100u
#define GPIO_AF8_LPUART1 8u
#define GPIO_MODE_AF_PP 2u
#define GPIO_NOPULL 0u
#define GPIO_SPEED_FREQ_LOW 0u
#define LPUART1 ((void *)1)
#define TIM6 ((void *)2)
#define LPUART1_IRQn 1
#define TIM6_IRQn 2
#define UART_WORDLENGTH_9B 1u
#define UART_STOPBITS_1 1u
#define UART_PARITY_EVEN 1u
#define UART_MODE_TX_RX 3u
#define UART_HWCONTROL_NONE 0u
#define UART_ONE_BIT_SAMPLE_DISABLE 0u
#define UART_PRESCALER_DIV1 0u
#define UART_ADVFEATURE_NO_INIT 0u
#define UART_TXFIFO_THRESHOLD_1_8 0u
#define UART_RXFIFO_THRESHOLD_1_8 0u
#define UART_FLAG_RXNE 1u
#define UART_FLAG_TC 2u
#define UART_FLAG_BUSY 4u
#define HAL_UART_ERROR_ORE 1u
#define HAL_UART_ERROR_FE 2u
#define HAL_UART_ERROR_PE 4u
#define HAL_UART_ERROR_NE 8u
#define RCC_HCLK_DIV1 1u
#define TIM_IT_UPDATE 1u
#define TIM_FLAG_UPDATE 1u
#define TIM_COUNTERMODE_UP 0u
#define TIM_AUTORELOAD_PRELOAD_DISABLE 0u
#define __HAL_RCC_TIM6_CLK_ENABLE() ((void)0)
#define __HAL_RCC_GPIOG_CLK_ENABLE() ((void)0)
#define __HAL_RCC_LPUART1_CLK_ENABLE() ((void)0)
extern uint32_t mock_period,mock_primask;
extern bool mock_timer_on,mock_rx_pending,mock_uart_busy;
#define __HAL_TIM_DISABLE_IT(t,f) ((void)(t),(void)(f))
#define __HAL_TIM_DISABLE(t) ((void)(t),mock_timer_on=false)
#define __HAL_TIM_SET_COUNTER(t,n) ((void)(t),(void)(n))
#define __HAL_TIM_CLEAR_FLAG(t,f) ((void)(t),(void)(f))
#define __HAL_TIM_SET_AUTORELOAD(t,n) ((void)(t),mock_period=(n)+1u)
#define __HAL_TIM_ENABLE_IT(t,f) ((void)(t),(void)(f))
#define __HAL_TIM_ENABLE(t) ((void)(t),mock_timer_on=true)
#define __HAL_UART_GET_FLAG(t,f) ((void)(t), (f)==UART_FLAG_BUSY ? mock_uart_busy : mock_rx_pending)
static inline uint32_t __get_PRIMASK(void) { return mock_primask; }
static inline void __disable_irq(void) { mock_primask=1; }
static inline void __enable_irq(void) { mock_primask=0; }
void HAL_GPIO_Init(GPIO_TypeDef *,const GPIO_InitTypeDef *);
void HAL_GPIO_WritePin(GPIO_TypeDef *,uint16_t,GPIO_PinState);
HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *);
HAL_StatusTypeDef HAL_UARTEx_SetTxFifoThreshold(UART_HandleTypeDef *,uint32_t);
HAL_StatusTypeDef HAL_UARTEx_SetRxFifoThreshold(UART_HandleTypeDef *,uint32_t);
HAL_StatusTypeDef HAL_UARTEx_DisableFifoMode(UART_HandleTypeDef *);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *,uint8_t *,uint16_t);
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *,const uint8_t *,uint16_t);
HAL_StatusTypeDef HAL_UART_AbortReceive(UART_HandleTypeDef *);
HAL_StatusTypeDef HAL_UART_AbortTransmit(UART_HandleTypeDef *);
uint32_t HAL_UART_GetError(UART_HandleTypeDef *);
void HAL_UART_IRQHandler(UART_HandleTypeDef *);
void HAL_TIM_IRQHandler(TIM_HandleTypeDef *);
HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *);
void HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef *,uint32_t *);
uint32_t HAL_RCC_GetPCLK1Freq(void);
void HAL_NVIC_SetPriority(int,uint32_t,uint32_t);
void HAL_NVIC_EnableIRQ(int);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *);
#endif
