#include "stm32_iis3dwb_fifo_capture.h"
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
DWT_Type test_dwt;
CoreDebug_Type test_core_debug;
static uint8_t regs[128];
static uint32_t enabled, popped;
static unsigned fault;
static bool stopped, failed_once;
static GPIO_PinState cs;
static jmp_buf done;
void HAL_GPIO_WritePin(GPIO_TypeDef *port,uint16_t pin,GPIO_PinState state)
{ assert(port != NULL && pin == 1U); cs=state; }
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *spi,uint8_t *tx,uint16_t n,uint32_t timeout)
{
    assert(spi && cs==GPIO_PIN_RESET && n==2U && timeout==10U);
    test_dwt.CYCCNT+=5000U;regs[tx[0]]=tx[1];
    if(tx[0]==0x0aU){enabled=test_dwt.CYCCNT;popped=0U;stopped=false;}
    return HAL_OK;
}
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *spi,const uint8_t *tx,uint8_t *rx,uint16_t n,uint32_t timeout)
{
    assert(spi && cs==GPIO_PIN_RESET && n>=2U && n<=8U && timeout==10U && (tx[0]&0x80U));
    for(unsigned i=1U;i<n;i++)assert(tx[i]==0U);
    test_dwt.CYCCNT+=n*2100U;rx[0]=0xeeU;
    uint8_t reg=tx[0]&0x7fU;
    if(reg==0x3aU){
        assert(n==3U);uint32_t produced=(uint32_t)(((uint64_t)(test_dwt.CYCCNT-enabled)*26667U)/160000000U);
        uint32_t level=0U,flags=0U;
        if(regs[0x0aU]==1U){
            if(produced>=256U)stopped=true;
            level=(stopped?256U:produced)-popped;
            if(level==256U)flags=0xa000U;
        }else if(regs[0x0aU]==6U){level=produced>511U?511U:produced;flags=level>=256U?0x8000U:0U;if(produced>511U)flags|=0x6800U;}
        uint32_t status=level|flags;rx[1]=(uint8_t)status;rx[2]=(uint8_t)(status>>8U);
    }else if(reg==0x78U){
        assert(n==8U && stopped && popped<256U);
        if(fault==3U && popped==55U && !failed_once){failed_once=true;return HAL_TIMEOUT;}
        uint8_t counter=(uint8_t)(popped&3U);
        if(fault==1U && popped==5U)counter=(counter+1U)&3U;
        uint8_t tag=(uint8_t)(0x10U|(counter<<1U));unsigned parity=0U;
        for(unsigned i=0U;i<8U;i++)parity^=(tag>>i)&1U;
        tag|=(uint8_t)parity;if(fault==2U && popped==7U)tag^=1U;rx[1]=tag;
        for(unsigned i=0U;i<3U;i++){int16_t raw=(int16_t)((int)popped-128+(int)i*4096);rx[2+i*2]=(uint8_t)raw;rx[3+i*2]=(uint8_t)((uint16_t)raw>>8U);}
        popped++;
    }else {assert(n==2U);rx[1]=regs[reg];}
    return HAL_OK;
}
void HAL_Delay(uint32_t ms){test_dwt.CYCCNT+=ms*160000U;if(tr2_fifo_info.stage==10U)longjmp(done,1);}
uint32_t HAL_GetTick(void){return test_dwt.CYCCNT/160000U;}
uint32_t HAL_RCC_GetHCLKFreq(void){return 160000000U;}
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t which){assert(which==RCC_PERIPHCLK_SPI3);return 160000000U;}
int main(void)
{
    SPI_HandleTypeDef spi={0};GPIO_TypeDef gpio={0};
    for(unsigned run=0U;run<5U;run++){
        memset(regs,0,sizeof(regs));regs[0x0f]=0x7bU;regs[0x12]=4U;
        memset((void*)&tr2_fifo_info,0,sizeof(tr2_fifo_info));memset(&test_dwt,0,sizeof(test_dwt));
        fault=run;failed_once=false;stopped=false;popped=0U;cs=GPIO_PIN_SET;
        if(run==4U)regs[0x0a]=1U; /* Must not clear another authority's FIFO. */
        tr2_fifo_trigger=0x4d313542U;
        if(setjmp(done)==0)stm32_iis3dwb_fifo_capture_run(&spi,&gpio,1U);
        assert(tr2_fifo_info.stage==10U && cs==GPIO_PIN_SET && tr2_fifo_info.restore_result==TR2_OK);
        assert(regs[0x10]==0U && regs[0x12]==4U && regs[0x14]==0U && regs[0x0a]==(run==4U?1U:0U));
        if(run==0U){
            assert(tr2_fifo_info.result==TR2_OK && tr2_fifo_info.count==256U && tr2_fifo_info.loss_flags==0U);
            assert(tr2_fifo_info.negative_rejected==1U && (tr2_fifo_info.negative_status&0x4800U));
            assert(tr2_fifo_info.empty_status==0U && tr2_fifo_words[0].raw_x==-128 && tr2_fifo_words[255].raw_z==8319);
        }else {
            assert(tr2_fifo_info.result!=TR2_OK);
            if(run==1U)assert(tr2_fifo_info.gap_events>0U && (tr2_fifo_info.loss_flags&8U));
            if(run==2U)assert(tr2_fifo_info.parity_errors==1U && (tr2_fifo_info.loss_flags&4U));
            if(run==3U)assert(tr2_fifo_info.count==55U && tr2_fifo_info.bus_errors==1U);
            if(run==4U)assert(tr2_fifo_info.count==0U);
        }
    }
    puts("FIFO bounded capture, framing, levels, tags, parity, gaps, overrun rejection and failure restoration: passed");
    return 0;
}
