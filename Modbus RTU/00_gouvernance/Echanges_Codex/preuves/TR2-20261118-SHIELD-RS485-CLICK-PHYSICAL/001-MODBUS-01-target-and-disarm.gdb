set pagination off
set confirm off
set print elements 24
set remotetimeout 10
target extended-remote localhost:23341
x/3wx 0x0BFA0700
if *(unsigned int *)0xBFA0700 != 0x004A003D || *(unsigned int *)0xBFA0704 != 0x38425018 || *(unsigned int *)0xBFA0708 != 0x20333157
 disconnect
 quit 3
end
p click_probe.mode
p click_probe.error
p click_probe.rx_count
p click_probe.trace_count
p click_probe.de_count
p click_probe.tx_count
p click_probe.credits
p tr2_irq_observer
p g_serial.tx_busy
p g_serial.tx_fault
p g_serial.rtu_timer_phase
p g_serial.uart.ErrorCode
p/x GPIOG->ODR
p/x GPIOG->IDR
x/2wx 0xE000ED28
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-shield-rs485-click-20261118/001-MODBUS-01-probe.bin &click_probe (&click_probe+1)
if click_probe.mode != 3 || click_probe.error || click_probe.rx_count != 190 || click_probe.tx_count != 16 || click_probe.credits != 8 || g_serial.tx_busy || g_serial.tx_fault || tr2_irq_observer.accepted_request != 8 || tr2_irq_observer.crc_valid != 8 || tr2_irq_observer.tx_callback != 16 || tr2_irq_observer.error_callback || tr2_irq_observer.events_dropped || tr2_irq_observer.tx_fail_safe || (GPIOG->ODR & 16)
 disconnect
 quit 12
end
set var click_command=1

detach
quit
