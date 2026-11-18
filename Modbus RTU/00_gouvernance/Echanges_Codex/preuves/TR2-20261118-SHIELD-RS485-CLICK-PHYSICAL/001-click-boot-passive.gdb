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
p click_probe
p click_command
p click_arm
p/x GPIOG->MODER
p/x GPIOG->ODR
p/x GPIOG->IDR
p/x GPIOG->AFR
p/x PWR->SVMCR
p/x PWR->SVMSR
p/x RCC->CCIPR3
p g_serial.tx_busy
p g_serial.tx_fault
p g_serial.uart.ErrorCode
p/x g_serial.uart.Instance->BRR
p/x g_serial.uart.Instance->CR1
p/x g_serial.uart.Instance->ISR
p tr2_irq_observer.init
x/3wx 0xE000E100
x/3wx 0xE000E200
x/2wx 0xE000ED28
if click_probe.stage != 1 || click_probe.error || click_probe.mode || click_probe.tx_count || click_command || click_arm || (GPIOG->ODR & 16)
 disconnect
 quit 6
end

detach
quit
