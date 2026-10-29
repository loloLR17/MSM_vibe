#include "stm32u5xx_hal.h"
#include "sd_spi.h"
extern uint64_t r3_now(void *);
typedef struct {uint32_t valid,length,status,error,rx_left,tx_left,sr,cr1,cr2,cfg1,cfg2;} SpiFault;
SpiFault r3_spi_fault;
static uint32_t active_length,preclose_seen,preclose_error;
static SPI_HandleTypeDef spi;
static void select_card(void *unused,bool on){(void)unused;HAL_GPIO_WritePin(GPIOF,GPIO_PIN_12,on?GPIO_PIN_RESET:GPIO_PIN_SET);}
/* Qualified HAL observer before it clears status. No trace inside RX loop. */
void r3_sd_preclose(SPI_HandleTypeDef *h)
{
 if(h!=&spi)return;
 preclose_seen++;
 if(h->RxXferCount||h->TxXferCount||h->ErrorCode||(h->Instance->SR&SPI_SR_OVR)){
  preclose_error=1;if(!r3_spi_fault.valid)r3_spi_fault=(SpiFault){1,active_length,0,h->ErrorCode,h->RxXferCount,h->TxXferCount,h->Instance->SR,h->Instance->CR1,h->Instance->CR2,h->Instance->CFG1,h->Instance->CFG2};
 }
}
static int exchange(void *unused,const uint8_t *tx,uint8_t *rx,uint16_t n)
{
 (void)unused;uint32_t seen=preclose_seen;active_length=n;
 HAL_StatusTypeDef result=HAL_SPI_TransmitReceive(&spi,(uint8_t *)tx,rx,n,n<=128?2u:30u);
 bool failed=result!=HAL_OK||spi.ErrorCode||spi.RxXferCount||spi.TxXferCount||preclose_error||preclose_seen!=seen+1u;
 if(failed&&!r3_spi_fault.valid)r3_spi_fault=(SpiFault){1,n,result,spi.ErrorCode,spi.RxXferCount,spi.TxXferCount,SPI2->SR,SPI2->CR1,SPI2->CR2,SPI2->CFG1,SPI2->CFG2};
 return failed?-1:0;
}
static int frequency(bool fast)
{
 spi.Init.BaudRatePrescaler=fast?SPI_BAUDRATEPRESCALER_4:SPI_BAUDRATEPRESCALER_64;
 if(HAL_SPI_Init(&spi)!=HAL_OK)return -1;
 return 0;
}
static int setup(void)
{
 GPIO_InitTypeDef g={0};RCC_PeriphCLKInitTypeDef p={0};
 __HAL_RCC_GPIOB_CLK_ENABLE();__HAL_RCC_GPIOD_CLK_ENABLE();__HAL_RCC_GPIOC_CLK_ENABLE();__HAL_RCC_GPIOF_CLK_ENABLE();
 /* CS is latched HIGH before making PF12 an output; no JTAG/SWO or old DAT drive. */
 HAL_GPIO_WritePin(GPIOF,GPIO_PIN_12,GPIO_PIN_SET);g.Pin=GPIO_PIN_12;g.Mode=GPIO_MODE_OUTPUT_PP;g.Pull=GPIO_PULLUP;g.Speed=GPIO_SPEED_FREQ_LOW;HAL_GPIO_Init(GPIOF,&g);
 DBGMCU->CR&=~DBGMCU_CR_TRACE_IOEN;
 g.Pin=GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_14|GPIO_PIN_15;g.Mode=GPIO_MODE_INPUT;g.Pull=GPIO_NOPULL;HAL_GPIO_Init(GPIOB,&g);
 g.Pin=GPIO_PIN_6|GPIO_PIN_7;g.Mode=GPIO_MODE_ANALOG;HAL_GPIO_Init(GPIOD,&g);
 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);g.Pin=GPIO_PIN_9;g.Mode=GPIO_MODE_OUTPUT_PP;HAL_GPIO_Init(GPIOC,&g);
 p.PeriphClockSelection=RCC_PERIPHCLK_SPI2;p.Spi2ClockSelection=RCC_SPI2CLKSOURCE_HSI;
 if(HAL_RCCEx_PeriphCLKConfig(&p)!=HAL_OK||HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI2)!=16000000u)return -1;
 __HAL_RCC_SPI2_CLK_ENABLE();g.Pin=GPIO_PIN_13;g.Mode=GPIO_MODE_AF_PP;g.Pull=GPIO_NOPULL;g.Speed=GPIO_SPEED_FREQ_HIGH;g.Alternate=GPIO_AF5_SPI2;HAL_GPIO_Init(GPIOB,&g);
 g.Pin=GPIO_PIN_3|GPIO_PIN_4;HAL_GPIO_Init(GPIOD,&g);
 spi.Instance=SPI2;spi.Init.Mode=SPI_MODE_MASTER;spi.Init.Direction=SPI_DIRECTION_2LINES;spi.Init.DataSize=SPI_DATASIZE_8BIT;
 spi.Init.CLKPolarity=SPI_POLARITY_LOW;spi.Init.CLKPhase=SPI_PHASE_1EDGE;spi.Init.NSS=SPI_NSS_SOFT;spi.Init.FirstBit=SPI_FIRSTBIT_MSB;
 spi.Init.TIMode=SPI_TIMODE_DISABLE;spi.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;spi.Init.CRCPolynomial=7;
 spi.Init.NSSPMode=SPI_NSS_PULSE_DISABLE;spi.Init.NSSPolarity=SPI_NSS_POLARITY_LOW;spi.Init.FifoThreshold=SPI_FIFO_THRESHOLD_01DATA;
 spi.Init.MasterSSIdleness=SPI_MASTER_SS_IDLENESS_00CYCLE;spi.Init.MasterInterDataIdleness=SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
 spi.Init.MasterReceiverAutoSusp=SPI_MASTER_RX_AUTOSUSP_DISABLE;spi.Init.MasterKeepIOState=SPI_MASTER_KEEP_IO_STATE_ENABLE;
 spi.Init.IOSwap=SPI_IO_SWAP_DISABLE;spi.Init.ReadyMasterManagement=SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;spi.Init.ReadyPolarity=SPI_RDY_POLARITY_HIGH;
 return frequency(false);
}

static bool prepared;
int r3_sd_prepare(void){int r=setup();prepared=!r;return r;}
int r3_sd_init(SdSpi *card)
{
 if(!prepared)return SD_IO;
 int r=sd_spi_init(card,(SdSpiIo){NULL,exchange,select_card,r3_now});
 if(!r&&frequency(true))r=SD_IO;
 return r;
}
void r3_sd_deselect(void){select_card(NULL,false);}
