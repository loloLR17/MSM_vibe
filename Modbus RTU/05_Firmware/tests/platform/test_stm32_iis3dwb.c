#include "stm32_iis3dwb_capture.h"
#include <assert.h>
#include <limits.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
DWT_Type test_dwt;
CoreDebug_Type test_core_debug;
static uint8_t regs[128], address;
static int16_t xyz[3];
static GPIO_PinState cs = GPIO_PIN_SET;
static HAL_StatusTypeDef tx_error, rx_error;
static uint32_t tick, reads, fail_at;
static bool phase_samples;
static jmp_buf finished;
void HAL_GPIO_WritePin(GPIO_TypeDef *port,uint16_t pin,GPIO_PinState state)
{ assert(port != NULL && pin == 1U);cs=state; }
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *spi,uint8_t *data,uint16_t n,uint32_t timeout)
{
    assert(spi != NULL && cs == GPIO_PIN_RESET && timeout == 10U);
    test_dwt.CYCCNT+=100U;
    if(tx_error!=HAL_OK)return tx_error;
    assert(n==1U || n==2U);address=data[0]&0x7fU;
    if(n==2U)regs[address]=data[1];
    return HAL_OK;
}
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *spi,uint8_t *data,uint16_t n,uint32_t timeout)
{
    assert(spi!=NULL && cs==GPIO_PIN_RESET && timeout==10U);
    test_dwt.CYCCNT+=1000U;reads++;
    if(rx_error!=HAL_OK)return rx_error;
    if(fail_at && reads==fail_at)return HAL_TIMEOUT;
    if(address==0x28U){
        assert(n==6U);
        for(unsigned i=0;i<3U;i++){
            int16_t raw=xyz[i];
            if(phase_samples)raw=(int16_t)(xyz[i]+(regs[0x14U]==1U?12000:regs[0x14U]==2U?-12000:0));
            data[i*2U]=(uint8_t)raw;data[i*2U+1U]=(uint8_t)((uint16_t)raw>>8);
        }
    }else {assert(n==1U);data[0]=regs[address];}
    return HAL_OK;
}
void HAL_Delay(uint32_t ms)
{
    tick+=ms;test_dwt.CYCCNT+=ms*160000U;
    if(tr2_iis3dwb_capture_info.stage==10U)longjmp(finished,1);
}
uint32_t HAL_GetTick(void){return tick;}
uint32_t HAL_RCC_GetHCLKFreq(void){return 160000000U;}
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t which){assert(which==RCC_PERIPHCLK_SPI3);return 160000000U;}
static void defaults(void)
{
    memset(regs,0,sizeof(regs));regs[0x0f]=0x7b;regs[0x12]=4;regs[0x1e]=1;
    tx_error=rx_error=HAL_OK;tick=reads=fail_at=0;phase_samples=false;
    memset(&test_dwt,0,sizeof(test_dwt));
    memset((void*)&tr2_iis3dwb_capture_info,0,sizeof(tr2_iis3dwb_capture_info));
}
int main(void)
{
    Stm32Iis3dwbVibrationSource source;
    SPI_HandleTypeDef spi={0};GPIO_TypeDef gpio={0};
    VibrationSourceConfiguration cfg={26667U,7U,0U};
    Stm32Iis3dwbRawSample raw;
    defaults();assert(stm32_iis3dwb_vibration_source_init(&source,&spi,&gpio,1U)==TR2_OK);
    VibrationSource v=stm32_iis3dwb_vibration_source_interface(&source);
    memset(&raw,0xff,sizeof(raw));assert(stm32_iis3dwb_read_raw(&source,&raw)==TR2_ERROR_INVALID_STATE);assert(!raw.sample.valid);
    const uint8_t fs[4]={0,8,12,4};const int32_t mg[4]={61,122,244,488};
    for(unsigned i=0;i<4U;i++){
        cfg.full_scale_code=(uint16_t)i;assert(v.configure(v.context,&cfg)==TR2_OK);
        assert(regs[0x10]==(0xa0U|fs[i]) && regs[0x12]==0x44U);
        assert(v.start(v.context)==TR2_OK);xyz[0]=1000;xyz[1]=-1000;xyz[2]=0;
        assert(stm32_iis3dwb_read_raw(&source,&raw)==TR2_OK);
        assert(raw.xyz[0]==1000 && raw.xyz[1]==-1000 && raw.status==1U);
        assert(raw.sample.x_mg==mg[i] && raw.sample.y_mg==-mg[i] && raw.sample.valid && !raw.sample.saturated);
        xyz[2]=INT16_MIN;assert(stm32_iis3dwb_read_raw(&source,&raw)==TR2_OK && raw.sample.saturated);
        xyz[2]=INT16_MAX;assert(stm32_iis3dwb_read_raw(&source,&raw)==TR2_OK && raw.sample.saturated);
        assert(v.stop(v.context)==TR2_OK);
    }
    cfg.full_scale_code=0;cfg.axes_enable_mask=1;assert(v.configure(v.context,&cfg)==TR2_OK);assert(v.start(v.context)==TR2_OK);
    xyz[0]=9;xyz[1]=-9;xyz[2]=1000;assert(stm32_iis3dwb_read_raw(&source,&raw)==TR2_OK);
    assert(raw.sample.x_mg==1 && raw.sample.y_mg==0 && raw.sample.z_mg==0);
    VibrationSample stale={1,2,3,true,true};regs[0x1e]=0;
    assert(v.read_sample(v.context,&stale)==TR2_ERROR_NOT_AVAILABLE && !stale.valid && stale.x_mg==0);
    regs[0x1e]=1;rx_error=HAL_TIMEOUT;assert(stm32_iis3dwb_read_raw(&source,&raw)==TR2_ERROR_UNAVAILABLE && !raw.sample.valid && cs==GPIO_PIN_SET);
    rx_error=HAL_ERROR;assert(v.read_sample(v.context,&stale)==TR2_ERROR_INTERNAL && !stale.valid && cs==GPIO_PIN_SET);
    tx_error=HAL_BUSY;assert(v.read_sample(v.context,&stale)==TR2_ERROR_UNAVAILABLE && cs==GPIO_PIN_SET);
    tx_error=rx_error=HAL_OK;assert(v.stop(v.context)==TR2_OK);
    cfg.axes_enable_mask=8;assert(v.configure(v.context,&cfg)==TR2_ERROR_UNSUPPORTED);
    assert(stm32_iis3dwb_write_control(&source,0x75,1)==TR2_ERROR_UNSUPPORTED);
    assert(stm32_iis3dwb_write_control(&source,0x12,1)==TR2_ERROR_UNSUPPORTED);
    assert(stm32_iis3dwb_write_control(&source,0x14,3)==TR2_ERROR_UNSUPPORTED);
    regs[0x0f]=0;assert(stm32_iis3dwb_vibration_source_init(&source,&spi,&gpio,1)==TR2_ERROR_NOT_FOUND);
    /* Exercise actual bounded capture orchestration and sensor restoration. */
    for(unsigned run=0;run<3U;run++){
        defaults();xyz[0]=0;xyz[1]=-700;xyz[2]=8200;phase_samples=true;
        if(run==1U)fail_at=700U; /* midway through capture */
        if(run==2U)regs[0x15]=1U; /* refuse inherited single-axis authority */
        tr2_iis3dwb_capture_trigger=0x4d303135U;
        if(setjmp(finished)==0)stm32_iis3dwb_capture_run(&spi,&gpio,1U);
        assert(tr2_iis3dwb_capture_info.stage==10U && regs[0x10]==0 && regs[0x12]==4 && regs[0x14]==0 && cs==GPIO_PIN_SET);
        assert(tr2_iis3dwb_capture_info.restore_result==TR2_OK);
        if(run==0U){
            assert(tr2_iis3dwb_capture_info.result==TR2_OK && tr2_iis3dwb_capture_info.count==2048U);
            assert(tr2_iis3dwb_capture_samples[512].phase==1 && tr2_iis3dwb_capture_samples[512].raw_x==12000);
            assert(tr2_iis3dwb_capture_samples[1024].raw_x==-12000);
        }else assert(tr2_iis3dwb_capture_info.result!=TR2_OK && tr2_iis3dwb_capture_info.count<2048U);
    }
    puts("IIS3DWB driver/conversion/invalidity, bounded capture and failure restoration: passed");return 0;
}
