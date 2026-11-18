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
if click_probe.mode != 1 || click_probe.error || click_probe.rx_count != 14 || click_probe.tx_count || g_serial.tx_busy || g_serial.tx_fault || (GPIOG->ODR & 16)
 disconnect
 quit 9
end
set var click_arm=0xC118A55A
set var click_command=2

detach
quit
