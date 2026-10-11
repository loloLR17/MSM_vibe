#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "stm32u5xx_hal.h"
#include "stm32_serial_transport.h"
#include "stm32_modbus_diagnostic.h"
#include "tr2/application/modbus_system_server.h"

GPIO_TypeDef mock_gpio;
uint32_t mock_period, mock_primask;
bool mock_timer_on, mock_rx_pending, mock_uart_busy;
static UART_HandleTypeDef *uart;
static TIM_HandleTypeDef *timer;
static uint8_t *rx;
static bool de, receive_active;
static unsigned sends, releases, aborts;
static HAL_StatusTypeDef tx_result, rx_result, abort_rx_result;
static uint8_t sent[256];
static size_t sent_size;
static SerialTransport transport;
static ModbusSystemServer server;
static SystemRuntime runtime_fixture;
static ModbusBlock0Image identity;

void HAL_GPIO_Init(GPIO_TypeDef *p, const GPIO_InitTypeDef *g)
{ assert(p == GPIOG && g->Pin == (GPIO_PIN_7 | GPIO_PIN_8)); }
void HAL_GPIO_WritePin(GPIO_TypeDef *p, uint16_t pin, GPIO_PinState level)
{
    assert(p == GPIOG && pin == GPIO_PIN_4);
    if (level == GPIO_PIN_SET) { assert(!receive_active); de = true; }
    else { de = false; ++releases; }
}
HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *u) { uart=u; return HAL_OK; }
HAL_StatusTypeDef HAL_UARTEx_SetTxFifoThreshold(UART_HandleTypeDef *u,uint32_t n)
{ (void)u; (void)n; return HAL_OK; }
HAL_StatusTypeDef HAL_UARTEx_SetRxFifoThreshold(UART_HandleTypeDef *u,uint32_t n)
{ (void)u; (void)n; return HAL_OK; }
HAL_StatusTypeDef HAL_UARTEx_DisableFifoMode(UART_HandleTypeDef *u)
{ (void)u; return HAL_OK; }
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *u,uint8_t *b,uint16_t n)
{ assert(u==uart && n==1 && !de); rx=b; receive_active=rx_result==HAL_OK; return rx_result; }
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *u,const uint8_t *b,uint16_t n)
{
    assert(u==uart && de && !receive_active && mock_timer_on && mock_period==50000);
    assert(mock_primask==1); ++sends; memcpy(sent,b,n); sent_size=n; return tx_result;
}
HAL_StatusTypeDef HAL_UART_AbortReceive(UART_HandleTypeDef *u)
{ assert(u==uart); receive_active=false; mock_rx_pending=false; return abort_rx_result; }
HAL_StatusTypeDef HAL_UART_AbortTransmit(UART_HandleTypeDef *u)
{ assert(u==uart && !de); ++aborts; return HAL_OK; }
uint32_t HAL_UART_GetError(UART_HandleTypeDef *u) { (void)u; return HAL_UART_ERROR_FE; }
void HAL_UART_IRQHandler(UART_HandleTypeDef *u) { (void)u; /* TXE is NOT TC. */ }
void HAL_TIM_IRQHandler(TIM_HandleTypeDef *t) { HAL_TIM_PeriodElapsedCallback(t); }
HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *t) { timer=t; return HAL_OK; }
void HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef *c,uint32_t *l)
{ c->APB1CLKDivider=RCC_HCLK_DIV1; *l=0; }
uint32_t HAL_RCC_GetPCLK1Freq(void) { return 16000000; }
void HAL_NVIC_SetPriority(int i,uint32_t a,uint32_t b) { (void)i; (void)a; (void)b; }
void HAL_NVIC_EnableIRQ(int i) { (void)i; }

static void init_transport(void)
{
    de=false; receive_active=false; sends=releases=aborts=0;
    mock_primask=0; mock_rx_pending=false; mock_uart_busy=false; mock_timer_on=false;
    tx_result=rx_result=abort_rx_result=HAL_OK;
    assert(stm32_serial_transport_init(&transport)==TR2_OK && !de);
}
static void init(void)
{
    ModbusSystemServerBinding binding={0};
    init_transport();
    memset(&identity,0,sizeof(identity)); identity.registers[0]=0x1234;
    /* Simulated ready runtime/B0, only in host tests, never board provisioning. */
    memset(&runtime_fixture,0,sizeof(runtime_fixture));
    runtime_fixture.initialized=true; runtime_fixture.system_ready_for_modbus=true;
    binding.runtime=&runtime_fixture; binding.transport=&transport;
    binding.unit_id=17; binding.identity=&identity;
    assert(modbus_system_server_init(&server,&binding)==TR2_OK);
    assert(modbus_system_server_start(&server)==TR2_OK && receive_active);
}
static void byte(uint8_t b)
{
    assert(receive_active); *rx=b; HAL_UART_RxCpltCallback(uart);
    assert(modbus_system_server_poll_once(&server)==TR2_OK);
}
static Tr2Result request(uint8_t unit, uint8_t fc, bool corrupt, bool interrupted)
{
    const uint8_t pdu[]={fc,0,0,0,1}; uint8_t adu[256]; size_t n=0;
    assert(modbus_rtu_adu_encode(unit,pdu,sizeof(pdu),adu,sizeof(adu),&n)==MODBUS_RTU_CODEC_OK);
    if (corrupt) adu[n-1]^=1;
    for (size_t i=0;i<n;++i) byte(adu[i]);
    assert(!de && mock_period==750);
    stm32_serial_transport_tim6_irq_handler();
    assert(modbus_system_server_poll_once(&server)==TR2_OK);
    assert(!de && mock_period==1000);
    if (interrupted) byte(0xaa);
    stm32_serial_transport_tim6_irq_handler();
    if (interrupted) {
        assert(modbus_system_server_poll_once(&server)==TR2_OK);
        stm32_serial_transport_tim6_irq_handler();
    }
    return modbus_system_server_poll_once(&server);
}
int main(void)
{
    ModbusRtuAduView response;
    const uint8_t probe[]={1,2,3};
    init();
    assert(transport.transmit(transport.context,probe,sizeof(probe))==TR2_ERROR_UNAVAILABLE);
    assert(!de && sends==0); /* No unsolicited TX, even in qualified build. */
    assert(request(18,3,false,false)==TR2_OK && sends==0 && !de);
    assert(request(0,3,false,false)==TR2_OK && sends==0 && !de);
    assert(request(0,0x10,false,false)==TR2_OK && sends==0 && !de);
    assert(request(17,3,true,false)==TR2_OK && sends==0 && !de);
    assert(request(17,3,false,true)==TR2_OK && sends==0 && !de);
#if !TR2_STM32_RS485_TX_QUALIFIED
    assert(request(17,3,false,false)==TR2_ERROR_UNAVAILABLE);
    assert(sends==0 && !de && receive_active);
    (void)response;
#else
    assert(request(17,3,false,false)==TR2_OK && sends==1 && de);
    assert(modbus_rtu_adu_decode(sent,sent_size,&response)==MODBUS_RTU_CODEC_OK);
    assert(response.unit_id==17 && response.pdu_length==4);
    assert(response.pdu[0]==3 && response.pdu[1]==2 && response.pdu[2]==0x12 && response.pdu[3]==0x34);
    assert(transport.transmit(transport.context,probe,sizeof(probe))==TR2_ERROR_UNAVAILABLE);
    HAL_UART_RxCpltCallback(uart); /* Late echo callback is suppressed. */
    assert(modbus_system_server_poll_once(&server)==TR2_OK && sends==1);
    stm32_serial_transport_irq_handler(); assert(de); /* Buffer empty isn't line complete. */
    HAL_UART_TxCpltCallback(uart);
    assert(!de && receive_active && mock_period==1750);
    assert(transport.transmit(transport.context,probe,sizeof(probe))==TR2_ERROR_UNAVAILABLE);
    stm32_serial_transport_tim6_irq_handler();
    assert(request(17,6,false,false)==TR2_OK && sends==2 && de);
    assert(modbus_rtu_adu_decode(sent,sent_size,&response)==MODBUS_RTU_CODEC_OK);
    assert(response.pdu[0]==0x86 && response.pdu[1]==1);
    HAL_UART_TxCpltCallback(uart); assert(!de && receive_active);
    stm32_serial_transport_tim6_irq_handler();

    init(); rx_result=HAL_ERROR;
    *rx=17; HAL_UART_RxCpltCallback(uart);
    assert(!de && !receive_active && aborts==1);
    assert(modbus_system_server_poll_once(&server)==TR2_OK);
    assert(modbus_system_server_poll_once(&server)==TR2_OK);
    init(); mock_rx_pending=true;
    assert(request(17,3,false,false)==TR2_ERROR_UNAVAILABLE && !de && sends==0);
    init(); mock_uart_busy=true;
    assert(request(17,3,false,false)==TR2_ERROR_UNAVAILABLE && !de && sends==0);
    init(); mock_primask=1;
    assert(request(17,3,false,false)==TR2_OK && mock_primask==1);
    HAL_UART_TxCpltCallback(uart); mock_primask=0;
    init();
    /* A newer byte after an old T3.5 forbids a delayed response. */
    *rx=17; HAL_UART_RxCpltCallback(uart);
    stm32_serial_transport_tim6_irq_handler();
    stm32_serial_transport_tim6_irq_handler();
    *rx=17; HAL_UART_RxCpltCallback(uart);
    SerialTransportEvent event;
    for (unsigned i=0;i<3;++i) assert(transport.poll_event(transport.context,&event));
    assert(event.type==SERIAL_TRANSPORT_EVENT_SILENCE_T3_5);
    assert(transport.transmit(transport.context,probe,sizeof(probe))==TR2_ERROR_UNAVAILABLE && !de);
    init();
    /* Saturated RX queue must produce an error, never a response. */
    for (unsigned i=0;i<70;++i) { *rx=17; HAL_UART_RxCpltCallback(uart); }
    assert(transport.poll_event(transport.context,&event));
    assert(event.type==SERIAL_TRANSPORT_EVENT_ERROR && event.error==SERIAL_TRANSPORT_ERROR_OVERRUN);
    assert(transport.transmit(transport.context,probe,sizeof(probe))==TR2_ERROR_UNAVAILABLE && !de);

    init(); tx_result=HAL_ERROR;
    assert(request(17,3,false,false)==TR2_ERROR_UNAVAILABLE);
    assert(!de && receive_active && aborts==1);
    assert(modbus_system_server_poll_once(&server)==TR2_OK); /* Error event */
    tx_result=HAL_OK;
    assert(request(17,3,false,false)==TR2_ERROR_UNAVAILABLE && sends==1); /* Latched fault, no retry */

    init(); assert(request(17,3,false,false)==TR2_OK && de);
    stm32_serial_transport_tim6_irq_handler(); /* 50 ms watchdog, no polling dependency */
    assert(!de && receive_active && aborts==1);
    HAL_UART_TxCpltCallback(uart); assert(!de); /* Late completion cannot re-enable */
    assert(modbus_system_server_poll_once(&server)==TR2_OK);
    assert(request(17,3,false,false)==TR2_ERROR_UNAVAILABLE && sends==1);

    init(); assert(request(17,3,false,false)==TR2_OK);
    HAL_UART_ErrorCallback(uart); assert(!de && receive_active && aborts==1);
    init(); abort_rx_result=HAL_ERROR;
    assert(request(17,3,false,false)==TR2_ERROR_UNAVAILABLE && !de && sends==0);
    init(); assert(request(17,3,false,false)==TR2_OK);
    rx_result=HAL_ERROR; HAL_UART_TxCpltCallback(uart);
    assert(!de && aborts==1 && !receive_active);
    init(); HAL_UART_ErrorCallback(uart); assert(!de && receive_active);
    assert(modbus_system_server_poll_once(&server)==TR2_OK);
    assert(request(17,3,false,false)==TR2_OK && de); /* Recover ordinary RX error */
    HAL_UART_TxCpltCallback(uart);
    init(); runtime_fixture.system_ready_for_modbus=false;
    assert(modbus_system_server_poll_once(&server)==TR2_ERROR_NOT_AVAILABLE);
    assert(!de && sends==0 && !server.started);
#endif
    assert(mock_primask==0 && !de);
    /* M013: actual diagnostic + RTU/PDU + STM32 transport, no ready runtime. */
    {
        Stm32ModbusDiagnostic diagnostic;
        const uint8_t adu[]={1,3,3,0xE8,0,1,4,0x7A};
        init_transport();
        assert(stm32_modbus_diagnostic_init(&diagnostic,&transport,1)==TR2_OK);
        assert(modbus_rtu_server_runtime_start(&diagnostic.rtu)==TR2_OK);
        for (size_t i=0;i<sizeof(adu);++i) {
            *rx=adu[i]; HAL_UART_RxCpltCallback(uart);
            assert(modbus_rtu_server_runtime_poll_once(&diagnostic.rtu)==TR2_OK);
            assert(!de && sends==0);
        }
        stm32_serial_transport_tim6_irq_handler();
        assert(modbus_rtu_server_runtime_poll_once(&diagnostic.rtu)==TR2_OK);
        assert(!de && sends==0);
        stm32_serial_transport_tim6_irq_handler();
#if TR2_STM32_RS485_TX_QUALIFIED
        const uint8_t expected[]={1,3,2,0,0,0xB8,0x44};
        assert(modbus_rtu_server_runtime_poll_once(&diagnostic.rtu)==TR2_OK);
        assert(de && sends==1 && sent_size==sizeof(expected));
        assert(memcmp(sent,expected,sizeof(expected))==0);
        stm32_serial_transport_irq_handler(); assert(de);
        HAL_UART_TxCpltCallback(uart);
        assert(!de && receive_active && mock_period==1750);
#else
        assert(modbus_rtu_server_runtime_poll_once(&diagnostic.rtu)==TR2_ERROR_UNAVAILABLE);
        assert(!de && sends==0 && receive_active);
#endif
        assert(diagnostic.rtu.pdu_context.command_mailbox==NULL);
    }
    return 0;
}
