set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4q_result
p r4_result
if r4q_result.stage != 1 || r4q_result.error != 0
 quit 4
end
set r4_trigger=0x52340009

detach
quit
