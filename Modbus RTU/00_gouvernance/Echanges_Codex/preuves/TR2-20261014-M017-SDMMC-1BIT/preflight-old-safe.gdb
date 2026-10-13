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
x/wx 0x42020400
x/wx 0x420C0400
x/wx 0x420C0404

detach
quit
