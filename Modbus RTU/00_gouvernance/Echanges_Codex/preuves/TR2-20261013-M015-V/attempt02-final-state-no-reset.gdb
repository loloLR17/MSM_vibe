set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
printf "FINAL stage=%u result=%u errors=%u gen=%u coop=%u fifo=%u fram=%u crc_before=%u crc_after=%u UART=%u rx=%u tx=%u dropped=%u failsafe=%u head=%u tail=%u busy=%u fault=%u log_refused=%u preclose_overflow=%u core=%u\n", tr2_production_stage,tr2_production_result,tr2_production_poll_errors,production_runtime.recovery.generation,cooperative.state,tr2_fifo_state,cooperative_fram_state,cooperative_fram_crc_before,cooperative_fram_crc_after,g_serial.uart.ErrorCode,tr2_irq_observer.rx_callback,tr2_irq_observer.tx_callback,tr2_irq_observer.events_dropped,tr2_irq_observer.tx_fail_safe,g_serial.event_head,g_serial.event_tail,g_serial.tx_busy,tr2_fifo_fault,bench_log_refused,bench_preclose_overflow,SystemCoreClock
p production_runtime.application.runtime.configuration_service.recovery_status
x/3wx 0x0BFA0700
x/3wx 0xE000E100
x/2wx 0xE000ED28
x/wx 0x46020CE8
x/6wx 0x46002000
p cooperative.cleanup
if tr2_production_stage!=2 || tr2_production_result || tr2_production_poll_errors || production_runtime.recovery.generation!=12 || cooperative.state!=4 || tr2_fifo_state!=3 || cooperative_fram_state!=3 || cooperative_fram_crc_before!=cooperative_fram_crc_after || g_serial.uart.ErrorCode || tr2_irq_observer.events_dropped || tr2_irq_observer.tx_fail_safe || g_serial.event_head!=g_serial.event_tail || g_serial.tx_busy || tr2_fifo_fault || bench_log_refused || bench_preclose_overflow
 quit 11
end

detach
quit
