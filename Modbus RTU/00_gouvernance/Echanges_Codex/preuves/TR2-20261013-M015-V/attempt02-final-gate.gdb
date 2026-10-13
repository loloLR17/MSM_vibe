set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if cooperative.state != 4 || tr2_fifo_state != 3 || cooperative_fram_state != 3 || cooperative.result || cooperative.cleanup_result || tr2_production_poll_errors || g_serial.uart.ErrorCode || g_serial.tx_fault || tr2_irq_observer.events_dropped || tr2_irq_observer.error_callback || tr2_irq_observer.tx_fail_safe
 quit 9
end

detach
quit
