set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
set load_trigger=0x4C31494E
continue &
shell sleep 1
interrupt
p load_stage
p load_error
p/x load_cid
p load_blocks
p load_clock_hz
p load_clkcr
p load_bus_card

detach
quit
