set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p load_stage
p load_error
p/x load_cid
p load_blocks
p load_clock_hz
p load_clkcr
p load_bus_card
p/x GPIOB->MODER
if load_stage != 2 || load_error != 0 || load_clock_hz != 10000000 || load_bus_card != 0
 quit 4
end

detach
quit
