set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p cooperative.state
p cooperative_modbus_count
p tr2_irq_observer
if tr2_production_stage != 2 || tr2_production_result != 0 || cooperative.state != 1 || bench_ready != 1 || cooperative_arm || cooperative_start_tick || cooperative_modbus_count != 1
 quit 4
end
if g_serial.tx_busy || g_serial.tx_fault || g_serial.uart.ErrorCode || g_serial.event_head != g_serial.event_tail || tr2_irq_observer.events_dropped || tr2_irq_observer.errors_polled
 quit 5
end
set var bench_command=0x4D313556

detach
quit
