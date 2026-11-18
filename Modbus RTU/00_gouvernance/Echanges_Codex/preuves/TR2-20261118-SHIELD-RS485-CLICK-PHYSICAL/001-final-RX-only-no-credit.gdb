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
p click_probe.stage
p click_probe.mode
p click_probe.error
p click_probe.rx_count
p click_probe.tx_count
p click_probe.credits
p click_command
p click_arm
p g_serial.tx_busy
p g_serial.tx_fault
p g_serial.rtu_timer_phase
p/x GPIOG->ODR
p/x GPIOG->IDR
if click_probe.stage != 1 || click_probe.mode != 1 || click_probe.error || click_probe.credits || click_probe.rx_count != 190 || click_probe.tx_count != 16 || click_command || click_arm || g_serial.tx_busy || g_serial.tx_fault || (GPIOG->ODR & 16)
 disconnect
 quit 13
end

detach
quit
