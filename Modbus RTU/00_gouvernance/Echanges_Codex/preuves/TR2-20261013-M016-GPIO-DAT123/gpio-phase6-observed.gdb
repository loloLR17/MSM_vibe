set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p gpio_stage
p gpio_phase
p gpio_command
p/x gpio_output_mask
p gpio_error
p uwTick
p gpio_phase_started_ms
p/x GPIOB->ODR
p/x GPIOB->IDR
p/x GPIOB->MODER
p/x GPIOB->OTYPER
p/x RCC->AHB2ENR1
if gpio_phase != 6 || (GPIOB->ODR & 0x8018) != 0 || (GPIOB->IDR & 0x8018) != 0 || (RCC->AHB2ENR1 & 0x18000000) != 0
 quit 5
end

detach
quit
