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
if gpio_stage != 2 || gpio_phase != 5 || gpio_command != 0
 quit 4
end
set gpio_command=0x47310006

detach
quit
