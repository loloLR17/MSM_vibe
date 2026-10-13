set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p bench_stage
p bench_trigger
p/x hsd2.Instance->CLKCR
p/x GPIOB->IDR
p/x GPIOB->MODER
p/x GPIOB->AFR
p/x GPIOB->PUPDR
p/x DBGMCU->CR
p/x PWR->UCPDR
p/x hsd2.ErrorCode
x/2wx 0xE000ED28

detach
quit
