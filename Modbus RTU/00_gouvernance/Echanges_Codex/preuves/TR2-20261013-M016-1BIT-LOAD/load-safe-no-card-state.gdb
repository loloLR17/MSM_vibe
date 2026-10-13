set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p load_stage
p load_trigger
p load_error
p/x GPIOB->MODER
p/x GPIOB->OTYPER
p/x hsd2.Instance->POWER
p/x hsd2.Instance->CLKCR
p hsd2.Init
p SystemCoreClock
p/x DWT->CTRL
p DWT->CYCCNT
x/2wx 0xE000ED28
if load_stage != 1 || load_trigger != 0 || load_error != 0 || (GPIOB->MODER & 0xc00003c0) != 0 || hsd2.Instance->POWER != 0
 quit 4
end

detach
quit
