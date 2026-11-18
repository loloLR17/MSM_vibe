/* NON-PRODUCTION isolated Click bench. No SPI, media or persistent identity. */
#include "stm32u5xx_hal.h"
#include "stm32_serial_transport.h"
#include "stm32_rs485_rx_gpio.h"
#include "stm32_modbus_diagnostic.h"
#include "stm32_irq_observer.h"
#include "policy.h"
#include <string.h>
typedef struct {uint32_t cycles,type,value;} Trace;
typedef struct {uint32_t cycles,state,idr,odr;} DeTrace;
volatile struct {
    uint32_t version,stage,mode,error,uid[3],core_hz,credits,rx_count,trace_count,de_count,frames,tx_count;
    uint8_t rx[1024];Trace trace[2048];DeTrace de[128];
} click_probe;
volatile uint32_t click_command,click_arm;
static SerialTransport hardware,transport;
static Stm32ModbusDiagnostic diagnostic;
static uint8_t frame[256],reply[26];static size_t frame_length;
void __real_HAL_GPIO_WritePin(GPIO_TypeDef *,uint16_t,GPIO_PinState);
void __wrap_HAL_GPIO_WritePin(GPIO_TypeDef *port,uint16_t pin,GPIO_PinState state)
{
    __real_HAL_GPIO_WritePin(port,pin,state);
    if(port==GPIOG && pin==GPIO_PIN_4 && click_probe.de_count<128){
        uint32_t i=click_probe.de_count;click_probe.de[i]=(DeTrace){DWT->CYCCNT,state,GPIOG->IDR,GPIOG->ODR};click_probe.de_count=i+1;
    }
}
void SysTick_Handler(void){HAL_IncTick();}
void LPUART1_IRQHandler(void){stm32_serial_transport_irq_handler();}
void TIM6_IRQHandler(void){stm32_serial_transport_tim6_irq_handler();}
void HAL_MspInit(void){stm32_rs485_rx_gpio_init();}
void Error_Handler(void){click_probe.error=99;HAL_GPIO_WritePin(GPIOG,GPIO_PIN_4,GPIO_PIN_RESET);for(;;)__WFI();}
static bool observed_poll(void *ctx,SerialTransportEvent *e)
{
    (void)ctx;bool ok=hardware.poll_event(hardware.context,e);
    if(ok && e->type!=SERIAL_TRANSPORT_EVENT_NONE){
        uint32_t i=click_probe.trace_count;
        if(i<2048){click_probe.trace[i]=(Trace){DWT->CYCCNT,e->type,e->type==SERIAL_TRANSPORT_EVENT_BYTE?e->byte:e->error};click_probe.trace_count=i+1;}
        else {click_probe.error=10;click_probe.credits=0;}
        if(e->type==SERIAL_TRANSPORT_EVENT_BYTE){uint32_t n=click_probe.rx_count;if(n<1024){click_probe.rx[n]=e->byte;click_probe.rx_count=n+1;}else{click_probe.error=11;click_probe.credits=0;}}
        if(e->type==SERIAL_TRANSPORT_EVENT_ERROR){click_probe.error=12;click_probe.credits=0;}
    }
    return ok;
}
static Tr2Result observed_tx(void *ctx,const uint8_t *data,size_t n)
{
    (void)ctx;
    if(click_probe.error || click_probe.mode<2 || !click_probe.credits)return TR2_ERROR_UNAVAILABLE;
    --click_probe.credits;
    Tr2Result r=hardware.transmit(hardware.context,data,n);
    if(r==TR2_OK)++click_probe.tx_count;else click_probe.error=13;
    return r;
}
static Tr2Result observed_start(void *ctx){(void)ctx;return hardware.start_receive(hardware.context);}
static ModbusPduServerOutcome readonly_pdu(void *ctx,const uint8_t *in,size_t n,uint8_t *out,size_t cap)
{
    (void)ctx;
    if(n && in[0]==MODBUS_PDU_FC_READ_HOLDING_REGISTERS)
        return modbus_pdu_server_process(&diagnostic.rtu.pdu_context,in,n,out,cap);
    if(n && cap>=2){out[0]=in[0]|0x80;out[1]=1;return (ModbusPduServerOutcome){TR2_OK,2};}
    return (ModbusPduServerOutcome){TR2_ERROR_INVALID_ARGUMENT,0};
}
int main(void)
{
    uint32_t uid[3]={HAL_GetUIDw0(),HAL_GetUIDw1(),HAL_GetUIDw2()};
    click_probe.version=1;for(unsigned i=0;i<3;i++)click_probe.uid[i]=uid[i];
    if(uid[0]!=CLICK_UID0 || uid[1]!=0x38425018u || uid[2]!=0x20333157u){click_probe.error=1;for(;;)__WFI();}
    HAL_Init();
    if(HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY)!=HAL_OK || HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)!=HAL_OK)Error_Handler();
    RCC_OscInitTypeDef osc={0};osc.OscillatorType=RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_MSI;
    osc.HSIState=RCC_HSI_ON;osc.HSICalibrationValue=RCC_HSICALIBRATION_DEFAULT;
    osc.MSIState=RCC_MSI_ON;osc.MSIClockRange=RCC_MSIRANGE_4;osc.MSICalibrationValue=RCC_MSICALIBRATION_DEFAULT;
    osc.PLL.PLLState=RCC_PLL_ON;osc.PLL.PLLSource=RCC_PLLSOURCE_MSI;osc.PLL.PLLMBOOST=RCC_PLLMBOOST_DIV1;
    osc.PLL.PLLM=1;osc.PLL.PLLN=80;osc.PLL.PLLP=2;osc.PLL.PLLQ=2;osc.PLL.PLLR=2;osc.PLL.PLLRGE=RCC_PLLVCIRANGE_0;
    if(HAL_RCC_OscConfig(&osc)!=HAL_OK)Error_Handler();
    RCC_ClkInitTypeDef clk={0};clk.ClockType=RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2|RCC_CLOCKTYPE_PCLK3;
    clk.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;clk.AHBCLKDivider=RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider=RCC_HCLK_DIV1;clk.APB2CLKDivider=RCC_HCLK_DIV1;clk.APB3CLKDivider=RCC_HCLK_DIV1;
    if(HAL_RCC_ClockConfig(&clk,FLASH_LATENCY_4)!=HAL_OK)Error_Handler();
    RCC_PeriphCLKInitTypeDef pc={0};pc.PeriphClockSelection=RCC_PERIPHCLK_LPUART1;pc.Lpuart1ClockSelection=RCC_LPUART1CLKSOURCE_HSI;
    if(HAL_RCCEx_PeriphCLKConfig(&pc)!=HAL_OK)Error_Handler();
    SystemCoreClockUpdate();click_probe.core_hz=SystemCoreClock;
    CoreDebug->DEMCR|=CoreDebug_DEMCR_TRCENA_Msk;DWT->CYCCNT=0;DWT->CTRL|=DWT_CTRL_CYCCNTENA_Msk;
    if(stm32_serial_transport_init(&hardware)!=TR2_OK)Error_Handler();
    transport=(SerialTransport){NULL,observed_start,observed_tx,observed_poll};
    if(stm32_modbus_diagnostic_init(&diagnostic,&transport,1)!=TR2_OK)Error_Handler();
    diagnostic.rtu.process_pdu=readonly_pdu;
    if(modbus_rtu_server_runtime_start(&diagnostic.rtu)!=TR2_OK)Error_Handler();
    click_probe.stage=1;
    for(;;){
        if(click_command){
            uint32_t cmd=click_command;click_command=0;uint32_t arm=click_arm;click_arm=0;
            if(!click_probe.error && click_mode_allowed(cmd,arm,(GPIOG->ODR&GPIO_PIN_4)==0)){
                click_probe.mode=cmd;click_probe.credits=cmd>=2?16:0;frame_length=0;modbus_rtu_receiver_init(&diagnostic.rtu.receiver);
            }else click_probe.error=2;
        }
        if(click_probe.mode==3){
            if(stm32_modbus_diagnostic_poll_once(&diagnostic)!=TR2_OK)click_probe.error=14;
        }else{
            SerialTransportEvent event;
            if(!observed_poll(NULL,&event)){click_probe.error=15;click_probe.credits=0;continue;}
            if(event.type==SERIAL_TRANSPORT_EVENT_BYTE){if(frame_length<sizeof(frame))frame[frame_length++]=event.byte;else{click_probe.error=16;click_probe.credits=0;}}
            if(event.type==SERIAL_TRANSPORT_EVENT_SILENCE_T3_5){
                ++click_probe.frames;
                if(click_probe.mode==2){size_t n=click_raw_reply(frame,frame_length,uid,reply,sizeof(reply));if(n)(void)observed_tx(NULL,reply,n);}
                frame_length=0;
            }
            if(event.type==SERIAL_TRANSPORT_EVENT_ERROR)frame_length=0;
        }
        /* Poll continuously: no debugger output or blocking work in this loop. */
    }
}
