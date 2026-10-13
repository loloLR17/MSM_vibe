set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if bench_ready != 1 || cooperative.state != 1 || cooperative_arm || cooperative_start_tick
 quit 8
end
set var bench_command=0x4D313556

detach
quit
